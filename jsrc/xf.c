/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos: Files                                                            */

/* File functions accept file number or boxed file name or 1 or 2          */

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#else
#ifdef __GNUC__
#define _GNU_SOURCE
#include <dlfcn.h>
#endif
#include <sys/types.h>
#include <unistd.h>
#include <fts.h>
#endif

#include "j.h"
#include "x.h"

#if !SY_WIN32 && (SYS & SYS_DOS)
#include <dos.h>
#endif

#if (SYS & SYS_UNIX)
#include <stdlib.h>
typedef long long INT64;
static int rmdir2(const char *dir);
#endif

#if SY_WIN32 && !SY_WINCE
#include <direct.h>
#include <io.h>
static int rmdir2(J jt, const wchar_t *dir);
#endif


#if SY_64
static I fsize(F f){fpos_t z;
 RZ(f);
#if SY_WIN32
 _lseeki64(_fileno(f),0,SEEK_END); 
#else
 fseek(f,0L,SEEK_END);
#endif
 fgetpos(f,&z);
 R *(I*)&z;
}
#else
static I fsize(F f){
 RZ(f);
 if(fseek(f,0L,SEEK_END))R -1;
 R ftell(f);
}
#endif

static A jtrdns(J jt,F f){A za,z;I n=1024;size_t r,tr=0;
 GAT(za,LIT,1024,1,0); clearerr(f);
 while(!feof(f) && (r=fread(CAV(za)+tr,sizeof(C),n-tr,f))){
  tr+=r; if(tr==(U)n){RZ(za=ext(0,za));n*=2;}
 }
 if(tr==(U)n)z=za;
 else {GATV(z,LIT,(I)tr,1,0); MC(CAV(z),CAV(za),tr);}
 R z;
}    /* read entire file stream (non-seekable) */

A jtrd(J jt,F f,I j,I n){A z;C*x;I p=0;size_t q=1;
 RZ(f);
 if(0>n){if(j<0) n=-j; else n=fsize(f)-j;}

#if !SY_WINCE
 {INT64 v; v= j+((0>j)?fsize(f):0); fsetpos(f,(fpos_t*)&v);}
#else
 fseek(f,(long)(0>j?1+j:j),0>j?SEEK_END:SEEK_SET);
#endif

 clearerr(f);
 GATV(z,LIT,n,1,0); x=CAV(z);
 while(q&&n>p){
  p+=q=fread(p+x,sizeof(C),(size_t)(n-p),f);
  if(ferror(f))R jerrno();
 }
 R z;
}    /* read file f for n bytes at j */

static B jtwa(J jt,F f,I j,A w){C*x;I n,p=0;size_t q=1;
 RZ(f&&w);
 n=AN(w)*(C4T&AT(w)?4:C2T&AT(w)?2:1); x=CAV(w);

#if !SY_WINCE
 {INT64 v; v= j+((0>j)?fsize(f):0); fsetpos(f,(fpos_t*)&v);}
#else
 fseek(f,(long)(0>j?1+j:j),0>j?SEEK_END:SEEK_SET);
#endif
 
 clearerr(f);
 while(q&&n>p){
  p+=q=fwrite(p+x,sizeof(C),(size_t)(n-p),f); 
  if(ferror(f))R jerrno()?1:0;
 }
 R 1;
}    /* write/append string w to file f at j */


F1(jtjfread){A z;F f;
 F1RANK(0,jtjfread,0);
 RE(f=stdf(w));
 if(f)R 1==(I)f?jgets("\001"):3==(I)f?rdns(stdin):rd(vfn(f),0L,-1L);
 RZ(f=jope(w,FREAD)); z=rd(f,0L,-1L); fclose(f); 
 R z;
}

F2(jtjfwrite){B b;F f;
 F2RANK(RMAX,0,jtjfwrite,0);
 if(BOX&AT(w)){ASSERT(1>=AR(a),EVRANK); ASSERT(!AN(a)||AT(a)&LIT+C2T+C4T,EVDOMAIN);}
 RE(f=stdf(w));
 if(2==(I)f){b=jt->tostdout; jt->tostdout=1; jt->mtyo=MTYOFILE; jpr(a); jt->mtyo=0; jt->tostdout=b; R a;}
 if(4==(I)f){R (U)AN(a)!=fwrite(CAV(a),sizeof(C),AN(a),stdout)?jerrno():a;}
 if(5==(I)f){R (U)AN(a)!=fwrite(CAV(a),sizeof(C),AN(a),stderr)?jerrno():a;}
 if(b=!f)RZ(f=jope(w,FWRITE)) else RE(vfn(f)); 
 wa(f,0L,a); 
 if(b)fclose(f);else fflush(f);
 RNE(mtm);
}

F2(jtjfappend){B b;F f;
 F2RANK(RMAX,0,jtjfappend,0);
 RE(f=stdf(w));
 if(2==(I)f){B b=jt->tostdout; jt->tostdout=1; jpr(a); jt->tostdout=b; R a;}
 ASSERT(!AN(a)||AT(a)&LIT+C2T+C4T,EVDOMAIN);
 ASSERT(1>=AR(a),EVRANK);
 if(b=!f)RZ(f=jope(w,FAPPEND)) else RE(vfn(f));
 wa(f,fsize(f),a);
 if(b)fclose(f);else fflush(f);
 RNE(mtm);
}

F1(jtjfsize){B b;F f;I m;
 F1RANK(0,jtjfsize,0);
 RE(f=stdf(w));
 if(b=!f)RZ(f=jope(w,FREAD)) else RE(vfn(f)); 
 m=fsize(f); 
 if(b)fclose(f);else fflush(f);
 RNE(sc(m));
}

static F jtixf(J jt,A w){F f;
 ASSERT(2<=AN(w),EVLENGTH);
 switch(CTTZNOFLAG(AT(w))){
  default:  ASSERT(0,EVDOMAIN);
  case B01X: ASSERT(0,EVFNUM);
  case BOXX: ASSERT(2==AN(w),EVLENGTH); f=stdf(head(w)); break; 
  case INTX: f=(F)*AV(w); ASSERT(2<(UI)f,EVFNUM);
 }
 R f?vfn(f):f;
}    /* process index file arg for file number; 0 if a file name */

static B jtixin(J jt,A w,I s,I*i,I*n){A in,*wv;I j,k,m,*u,wd;
 if(AT(w)&BOX){wv=AAV(w); wd=(I)w*ARELATIVE(w); RZ(in=vi(WVR(1))); k=AN(in); u=AV(in);}
 else{in=w; k=AN(in)-1; u=1+AV(in);}
 ASSERT(1>=AR(in),EVRANK);
 ASSERT(k&&k<=(n?2:1),EVLENGTH);
 j=u[0]; j=0>j?s+j:j; m=1==k?s-j:u[1];
 ASSERT(0<=j&&(!n||j<s&&j+m<=s&&0<=m),EVINDEX);
 *i=j; if(n)*n=m;
 R 1;
}    /* process index file arg for index and length */

F1(jtjiread){A z=0;B b;F f;I i,n;
 F1RANK(1,jtjiread,0);
 RE(f=ixf(w)); if(b=!f)RZ(f=jope(w,FREAD));
 if(ixin(w,fsize(f),&i,&n))z=rd(f,i,n);
 if(b)fclose(f);else fflush(f);
 R z;
}

F2(jtjiwrite){B b;F f;I i;
 F2RANK(RMAX,1,jtjiwrite,0);
 ASSERT(!AN(a)||AT(a)&LIT+C2T+C4T,EVDOMAIN);
 ASSERT(1>=AR(a),EVRANK);
 RE(f=ixf(w)); if(b=!f)RZ(f=jope(w,FUPDATE));
 if(ixin(w,fsize(f),&i,0L))wa(f,i,a); 
 if(b)fclose(f);else fflush(f);
 RNE(mtm);
}


#if (SYS & SYS_MACINTOSH)

static B setparm(C*v,C*ms,HParamBlockRec mp){I n;
 n=strlen(v);
 ASSERT(n<=NPATH,EVLIMIT); *ms=n; MC(1+ms,v,n);
 mp.fileParam.ioNamePtr=ms;
 mp.fileParam.ioVRefNum=0;
 mp.fileParam.ioDirID  =0;
 R 1;
}

#define DIRF(f,fsub)  \
 B f(J jt,C*v){C ms[256];HParamBlockRec mp; \
  RZ(setparm(v,ms,mp));                     \
  ASSERT(!fsub(&mp,0),EVFACE);              \
  R 1;                                      \
 }

static DIRF(jtmkdir1,PBDirCreate)
static DIRF(jtrmdir1,PBHDelete  )

static B mkdir(C*v){R!mkdir1(v);}
static B rmdir(C*v){R!rmdir1(v);}

#endif


F1(jtjmkdir){A y,z;
 F1RANK(0,jtjmkdir,0);
 ASSERT(AT(w)&BOX,EVDOMAIN);
 RZ(y=str0(vs(AAV0(w))));
#if (SYS & SYS_UNIX)
 R mkdir(CAV(y),0775)?jerrno():one;
#else
 RZ(z=toutf16x(y));
 R _wmkdir(USAV(z))?jerrno():one;
#endif
}

F1(jtjferase){A y,fn;US*s;I h;
 F1RANK(0,jtjferase,0);
 RE(h=fnum(w));
 if(h) y=str0(fname(sc(h))); else y=AAV0(w);
 ASSERT(y,EVFNUM);
 if(h)RZ(jclose(sc(h)));
#if (SYS&SYS_UNIX)
#if 0
 if(access(CAV(y), F_OK )){R !unlink(CAV(y))?one:jerrno();} /* rmdir2 cannot raise error on non-exist file */
 else {R !unlink(CAV(y))||!rmdir(CAV(y))||!rmdir2(CAV(y))?one:jerrno();}
#else
 R !unlink(CAV(y))||!rmdir(CAV(y))?one:jerrno();
#endif
#else
 RZ(fn=toutf16x(y));
 s=USAV(fn);
// #if SY_WIN32 && !SY_WINCE
#if 0
 R !_wunlink(s)||!_wrmdir(s)||!rmdir2(jt, (wchar_t*)s)?one:jerrno();
#else
 R !_wunlink(s)||!_wrmdir(s)?one:jerrno();
#endif
#endif
}    /* erase file or directory */

F1(jtpathcwd){C path[1+NPATH];US wpath[1+NPATH];
 ASSERTMTV(w);
#if SY_WINCE
 &path;&wpath; /* avoid compiler warnings */
 R cstr("\\");
#else
#if (SYS & SYS_UNIX)
 ASSERT(getcwd(path,NPATH),EVFACE);
#else
 ASSERT(_wgetcwd(wpath,NPATH),EVFACE);
 jttoutf8x(jt,path,NPATH,wpath);
#endif
 R cstr(path);
#endif
}

F1(jtpathchdir){A z;
 RZ(w);
 ASSERT(1>=AR(w),EVRANK);
 ASSERT(AN(w),EVLENGTH);
 ASSERT(LIT&AT(w),EVDOMAIN);
#if SY_WINCE
 &z; /* avoid compiler warning */
 ASSERT(0,EVFACE);
#else
#if (SYS & SYS_UNIX)
 ASSERT(!chdir(CAV(w)),EVFACE);
#else
 RZ(z=toutf16x(w));
 _wchdir(USAV(z));
#endif
 R mtv;
#endif
}

#if SY_WINCE
#define _wgetenv(s)		(0)
#endif

F1(jtjgetenv){
 F1RANK(1,jtjgetenv,0);
 ASSERT(LIT&AT(w),EVDOMAIN);
#if (SYS & SYS_UNIX)
 {
  C*s;
  R(s=getenv(CAV(w)))?cstr(s):zero;
 }
#else
 {
  A z; US* us;
  RZ(z=toutf16x(w));
  us=_wgetenv(USAV(z));
  if(!us)R zero;
  GATV(z,C2T,wcslen(us),1,0);
  MC(USAV(z),us,2*wcslen(us));
  R toutf8(z);
 }
#endif
 R zero;
}

F1(jtjgetpid){
 ASSERTMTV(w);
#if SY_WIN32
 R(sc(GetCurrentProcessId()));
#else
 R(sc(getpid()));
#endif
}

#if (SYS & SYS_UNIX)
// #ifdef __GNUC__
#if 0
F1(jtpathdll){Dl_info info;
 ASSERTMTV(w);
 if(dladdr(jtpathdll, &info)){
  R cstr((C*)info.dli_fname);
 } else R cstr((C*)"");
}
#else
F1(jtpathdll){
 ASSERTMTV(w); R cstr((C*)"");
}
#endif
#else
F1(jtpathdll){char p[MAX_PATH]; extern C dllpath[];
 ASSERTMTV(w);
 strcpy(p,dllpath);
 if('\\'==p[strlen(p)-1]) p[strlen(p)-1]=0;
 R cstr(p);
}
#endif

#if (SYS & SYS_UNIX)
int rmdir2(const char *dir)
{
 int ret=0;
 FTS *ftsp=NULL;
 FTSENT *curr;

 char *files[]={ (char *) dir, NULL };

 // FTS_NOCHDIR  - Avoid changing cwd, which could cause unexpected behavior
 //                in multithreaded programs
 // FTS_PHYSICAL - Don't follow symlinks. Prevents deletion of files outside
 //                of the specified directory
 // FTS_XDEV     - Don't cross filesystem boundaries
 ftsp=fts_open(files, FTS_NOCHDIR | FTS_PHYSICAL | FTS_XDEV, NULL);
 if (!ftsp) {
//  fprintf(stderr, "%s: fts_open failed: %s\n", dir, strerror(errno));
  ret=-1;
  goto finish;
 }

 while ((curr=fts_read(ftsp))) {
  switch (curr->fts_info) {
   case FTS_NS:
   case FTS_DNR:
   case FTS_ERR:
//   fprintf(stderr, "%s: fts_read error: %s\n", curr->fts_accpath, strerror(curr->fts_errno));
   break;

   case FTS_DC:
   case FTS_DOT:
   case FTS_NSOK:
   // Not reached unless FTS_LOGICAL, FTS_SEEDOT, or FTS_NOSTAT were
   // passed to fts_open()
   break;

   case FTS_D:
   // Do nothing. Need depth-first search, so directories are deleted
   // in FTS_DP
   break;

   case FTS_DP:
   case FTS_F:
   case FTS_SL:
   case FTS_SLNONE:
   case FTS_DEFAULT:
    if (remove(curr->fts_accpath) < 0) {
//    fprintf(stderr, "%s: Failed to remove: %s\n", curr->fts_path, strerror(errno));
    ret=-1;
   }
   break;
  }
 }

finish:
 if (ftsp) {
  fts_close(ftsp);
 }

 return ret;
}
#endif
#if SY_WIN32 && !SY_WINCE
int rmdir2(J jt, const wchar_t *dir){A z;US*zv;
 SHFILEOPSTRUCTW sh;
 GATV(z,C2T,wcslen(dir)+2,1,0); zv=USAV(z);
 memcpy(zv,dir,wcslen(dir)*sizeof(wchar_t));
 zv[1+wcslen(dir)]=zv[wcslen(dir)]=0;  // doubly null terminated string
 sh.hwnd   = NULL;
 sh.wFunc  = FO_DELETE;
 sh.pFrom  = zv;
 sh.pTo    = NULL;
 sh.fFlags = FOF_NOCONFIRMATION | FOF_SILENT;
 sh.hNameMappings = 0;
 sh.lpszProgressTitle = NULL;
 R SHFileOperationW (&sh);
}
#endif
