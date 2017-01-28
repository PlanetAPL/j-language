/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos: Host Command Facilities                                          */

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#include <io.h>
#include <stdlib.h>
#else
#include <sys/wait.h>
#include <unistd.h>
#endif
#include <stdint.h>

#ifdef _MSC_VER
#define mktemp _mktemp
#define unlink _unlink
#endif

#include "j.h"
#include "x.h"

#if (SYS & SYS_ARCHIMEDES)
#define Wimp_StartTask 0x400DE
extern int os_swi1(I,I);
#endif


#if (SYS & SYS_MACINTOSH)

F1(jthost  ){ASSERT(0,EVDOMAIN);}
F1(jthostne){ASSERT(0,EVDOMAIN);}

#else

F1(jthost){A z;
 F1RANK(1,jthost,0);
 RZ(w=vs(w));
// #if SY_WINCE
#if SY_WINCE || SY_WIN32
 ASSERT(0,EVDOMAIN);
#else
{
 A t;I b;C*fn,*s;F f;I n;
 n=AN(w);
 GATV(t,LIT,n+5+L_tmpnam,1,0); s=CAV(t);
 fn=5+n+s; MC(s,AV(w),n);
 MC(n+s,"   > ",5L);
#ifdef _MSC_VER
 strcpy(fn,"tmp.XXXXXX");
 {A fz; mktemp(fn);
  RZ(fz=toutf16x(t));
  b=!_wsystem(USAV(fz));
 }
#else
 const char*ftmp=getenv("TMPDIR");  /* android always define TMPDIR in jeload */
 strcpy(fn,ftmp?ftmp:(char*)"/tmp");
 strcat(fn,"/tmp.XXXXXX");
 {int fd=mkstemp(fn); close(fd);}
 b=!system(s);
#endif
 if(b){f=fopen(fn,FREAD); z=rd(f,0L,-1L); fclose(f);}
 unlink(fn);
 ASSERT(b&&f,EVFACE);
}
#endif
 R z;
}

F1(jthostne){C*s;
 F1RANK(1,jthostne,0);
 RZ(w=vs(w));
 s=CAV(w);
// #if SY_WINCE
#if SY_WINCE || SY_WIN32
 ASSERT(0,EVNONCE);
#else
 {
  I b;
#ifdef _MSC_VER
  A fz;
  RZ(fz=toutf16x(w));
  b=_wsystem(USAV(fz));
#else
  b=system(s);
#endif
#if !SY_64 && (SYS&SYS_LINUX)
  //Java-jnative-j.so system always returns -1
  if(jt->sm==SMJAVA&&-1==b) b=-1==system("")?0:-1;
#endif
  b=!b;
  ASSERT(b,EVFACE);
 }
#endif
 R mtv;
}

#endif


#if !(SYS & SYS_UNIX)

F1(jthostio){ASSERT(0,EVDOMAIN);}
F1(jtjwait ){ASSERT(0,EVDOMAIN);}

#else

#define CL(f) {close(f[0]);close(f[1]);}

F1(jthostio){C*s;A z;F*pz;int fi[2],fo[2],r;int fii[2],foi[2];
 if(pipe(fi)==-1) ASSERT(0,EVFACE);
 if(pipe(fo)==-1){CL(fi); ASSERT(0,EVFACE);}
 fii[0]=fi[0];fii[1]=fi[1];foi[0]=fo[0];foi[1]=fo[1];
 F1RANK(1,jthostio,0);
 RZ(w=vs(w));
 s=CAV(w); GAT(z,INT,3,1,0); pz=(F*)AV(z);
 if((r=pipe(fii))==-1||pipe(foi)==-1){if(r!=-1)CL(fi); ASSERT(0,EVFACE);}
 if(!((pz[1]=fdopen(fi[0],"r"))&&(pz[2]=fdopen(fo[1],"w")))){
  if(pz[1])fclose(pz[1]); CL(fi);CL(fo);}
 if(!add2(pz[1],pz[2],s)){fclose(pz[1]);fclose(pz[2]);
                               CL(fi);CL(fo);}
 switch(r=fork()){
  case -1:CL(fi);CL(fo);ASSERT(0,EVFACE);
  case 0:close(0);{int i=dup(fo[0]);};close(1);{int i=dup(fi[1]);};CL(fi);CL(fo);
#ifdef ANDROID
         execl("/system/bin/sh","/system/bin/sh","-c",s,NULL); exit(-1);
#else
         execl("/bin/sh","/bin/sh","-c",s,NULL); exit(-1);
#endif
 }close(fo[0]);close(fi[1]);
 add2(NULL,NULL,NULL); pz[0]=(F)(intptr_t)r;
 R z;
}

F1(jtjwait){I k;int s; RE(k=i0(w)); if(-1==waitpid(k,&s,0))jerrno(); R sc(s);}

#endif
