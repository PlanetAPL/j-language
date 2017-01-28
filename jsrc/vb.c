/* Copyright 1990-2011, Jsoftware Inc.  All rights reserved. */
/* License in license.txt.                                   */
/*                                                                         */
/* Verbs: Boolean-Valued                                                   */

#include "j.h"
#include "ve.h"


BPFX( andBB, AND ,BAND, AND, BAND )    
BPFX(  orBB, OR  ,BOR,  OR,  BOR  )    
BPFX(nandBB, NAND,BNAND,NAND,BNAND)
BPFX( norBB, NOR ,BNOR, NOR, BNOR )


F1(jtrazein){R df2(w,box(raze(w)),amp(swap(ds(CEPS)),ds(COPE)));}


static F2(jtebarmat){A ya,yw,z;B b,*zv;C*au,*av,*u,*v,*v0,*wu,*wv;I*as,c,i,k,m,n,r,s,si,sj,t,*ws;
 RZ(a&&w);
 as=AS(a);      av=CAV(a);
 ws=AS(w); v=v0=wv=CAV(w);
 si=as[0]; m=1+ws[0]-si;
 sj=as[1]; n=1+ws[1]-sj;
 t=AT(w); k=bp(t); c=ws[1]; r=k*c; s=k*sj;
 GATV(z,B01,AN(w),2,ws); zv=BAV(z); memset(zv,C0,AN(z));
 if(t&B01+LIT+C2T+C4T+INT+SBT||0==jt->ct&&t&FL+CMPX)
  for(i=0;i<m;++i){
   DO(n, u=av; b=0; DO(si,                         if(b=!!memcmp(u,v,s))break; u+=s; v+=r;); v=v0+=k; zv[i]=!b;);
   zv+=c; v=v0=wv+=r;
 }else{
  GA(ya,t,sj,1,0); au=CAV(ya);
  GA(yw,t,sj,1,0); wu=CAV(yw);
  for(i=0;i<m;++i){
   DO(n, u=av; b=0; DO(si, MC(au,u,s); MC(wu,v,s); if(b=!equ(ya,yw)  )break; u+=s; v+=r;); v=v0+=k; zv[i]=!b;);
   zv+=c; v=v0=wv+=r;
 }}
 R z;
}    /* E. on matrix arguments */

static F2(jtebarvec){A y,z;B*zv;C*av,*wv,*yv;I an,k,n,s,t,wn;
 RZ(a&&w);
 an=AN(a); av=CAV(a); 
 wn=AN(w); wv=CAV(w); n=1+wn-an; 
 t=AT(w); k=bp(t); s=k*an;
 GATV(z,B01,wn,AR(w)?1:0,0); zv=BAV(z); 
 if(an&&wn>an)memset(zv+n,C0,wn-n); else memset(zv,C0,wn);
 if(t&B01+LIT+C2T+C4T+INT+SBT||0==jt->ct&&t&FL+CMPX)DO(n, zv[i]=!memcmp(av,wv,s); wv+=k;)
 else{GA(y,t,an,AR(a),0); yv=CAV(y); DO(n, MC(yv,wv,s); zv[i]=equ(a,y); wv+=k;);}
 R z;
}    /* E. on vector arguments */

/* preparations for ebar                    */
/* return code meaning:                     */
/* >0 is cardinality of range               */
/* <0:                                      */
/* -1: not homogeneous                      */
/* -2: rank is 2                            */
/* -3: rank > 2                             */
/* -4: not discrete type or range too large */

static I jtebarprep(J jt,A a,A w,A*za,A*zw,I*zc){I ar,at,c=0,ca,cw,d=IMAX,da,dw,m,n,t,wr,wt,memlimit;C4 ccaw;
 ar=AR(a); at=AT(a); m=AN(a);
 wr=AR(w); wt=AT(w); n=AN(w);
 ASSERT(ar==wr||!ar&&1==wr,EVRANK);
 if(m&&n&&!HOMO(at,wt))R -1;
 if(m&&n)RE(t=maxtype(at,wt)) else t=m?at:n?wt:B01;
 if(TYPESNE(t,at))RZ(a=cvt(t,a));
 if(TYPESNE(t,wt))RZ(w=cvt(t,w));
 *za=a; *zw=w;
 // The inputs have been converted to common type
 if(1<wr)R 2==wr?-2:-3;
 memlimit = MIN(4*n,(I)((jt->mmax-100)/sizeof(I)));  // maximum size we will allow our d to reach.  Used only for I type.
  // 4*the size of the search area seems big enough; but not more than what a single memory allocation supports.  The size
  // is measured in Is.  The 100 is to account for memory-manager overhead
 switch(CTTZNOFLAG(t)){
  // calculate the number of distinct values in the range of the two operands.
  // for strings, we just assume the worst (all codes)
  // for ints, actually look at the data to get the range (min and #values+1).  If there is
  // an error getting the range (da or dw==0), leave d==IMAX.  If the min is > 0, and
  // the range can be extended to cover 0..d-1 without exceeding the bound on d, do so to make
  // the SUB0 and SUB1 expressions into EBLOOP simpler
  // We allocate an array for each result in range, so we have to get c and d right
  case INTX: irange(m,AV(a),&ca,&da); if(da)irange(n,AV(w),&cw,&dw); 
            if(da&&dw){c=MIN(ca,cw); d=MAX(ca+da,cw+dw)-c;} // This may make d overflow (if c<0), but we catch that at exit
            if(0<c&&c+d<=memlimit){d+=c;} break;  // Extend lower bound to 0 if that doesn't make d too big
  case SBTX: irange(m,AV(a),&ca,&da); if(da)irange(n,AV(w),&cw,&dw); 
            if(da&&dw){c=MIN(ca,cw); d=MAX(ca+da,cw+dw)-c;} // This may make d overflow (if c<0), but we catch that at exit
            if(0<c&&c+d<=memlimit){d+=c;} break;  // Extend lower bound to 0 if that doesn't make d too big
  case C2TX: d=65536; break;
  case C4TX: c4range(m,C4AV(a),&ccaw,&da); ca=ccaw; if(da){c4range(n,C4AV(w),&ccaw,&dw); cw=ccaw; }
            if(da&&dw){c=MIN((C4)ca,(C4)cw); d=MAX(((C4)ca)+da,((C4)cw)+dw)-(C4)c;} // This may make d overflow (if c<0), but we catch that at exit
            if(0<c&&c+d<=memlimit){d+=c;} break;  // Extend lower bound to 0 if that doesn't make d too big
  case LITX: d=256;   break;
  case B01X: d=2;     break;
 }
 *zc=c;  // Now that we know c, return it
 // if the range of integers is too big, revert to simple search.
 // Also revert for continuous type.  But always use fast search for character/boolean types
 R t&B01+LIT+C2T||t&INT+SBT+C4T&&0<d&&d<=memlimit ? d : -4;
}

#define EBLOOP(T,SUB0,SUB1,ZFUNC)  \
   {T*u=(T*)av,*v=(T*)wv;                                            \
    DO(m, yv[SUB0]=m-i;);                                            \
    while(k< p){for(i=0;i<m&&u[i]==v[k+i];++i); ZFUNC; k+=yv[SUB1];} \
    if   (k==p){for(i=0;i<m&&u[i]==v[k+i];++i); ZFUNC;             } \
   }

F2(jtebar){PROLOG(0065);A y,z;B*zv;C*av,*wv;I c,d,i,k=0,m,n,p,*yv;
 RZ(a&&w);
 RE(d=ebarprep(a,w,&a,&w,&c));
 av=CAV(a); m=AN(a);
 wv=CAV(w); n=AN(w); p=n-m;
 switch(d){
  case -1: R reshape(shape(w),zero);
  case -2: R ebarmat(a,w);
  case -3: R df2(shape(a),w,cut(amp(a,ds(CMATCH)),num[3]));
  case -4: R ebarvec(a,w);
 }
 GATV(z,B01,n,AR(w),0); zv=BAV(z); memset(zv,C0,n);
 GATV(y,INT,d,1,    0); yv= AV(y); DO(d, yv[i]=1+m;); 
 switch(CTTZ(AT(w))){
  case INTX: if(c)EBLOOP(I, u[i]-c,v[k+m]-c, zv[k]=i==m) 
            else EBLOOP(I, u[i],  v[k+m],   zv[k]=i==m); break;
  case SBTX: if(c)EBLOOP(SB,u[i]-c,v[k+m]-c, zv[k]=i==m) 
            else EBLOOP(SB,u[i],  v[k+m],   zv[k]=i==m); break;
  case C2TX:      EBLOOP(US,u[i],  v[k+m],   zv[k]=i==m); break;
  case C4TX: if(c)EBLOOP(C4,u[i]-c,v[k+m]-c, zv[k]=i==m) 
            else EBLOOP(C4,u[i],  v[k+m],   zv[k]=i==m); break;
  default:       EBLOOP(UC,u[i],  v[k+m],   zv[k]=i==m);
 }
 EPILOG(z);
}    /* Daniel M. Sunday, CACM 1990 8, 132-142 */


F2(jti1ebar){A y;C*av,*wv;I c,d,i,k=0,m,n,p,*yv;
 RZ(a&&w);
 RE(d=ebarprep(a,w,&a,&w,&c));
 av=CAV(a); m=AN(a);
 wv=CAV(w); n=AN(w); p=n-m;
 switch(d){
  case -1: R sc(n);
  case -4: R indexof(ebarvec(a,w),one);
 }
 GATV(y,INT,d,1,0); yv=AV(y); DO(d, yv[i]=1+m;); 
 switch(CTTZ(AT(w))){
  case INTX: if(c)EBLOOP(I, u[i]-c,v[k+m]-c, if(i==m)R sc(k)) 
            else EBLOOP(I, u[i],  v[k+m],   if(i==m)R sc(k)); break;
  case SBTX: if(c)EBLOOP(SB,u[i]-c,v[k+m]-c, if(i==m)R sc(k)) 
            else EBLOOP(SB,u[i],  v[k+m],   if(i==m)R sc(k)); break;
  case C2TX:      EBLOOP(US,u[i],  v[k+m],   if(i==m)R sc(k)); break;
  case C4TX: if(c)EBLOOP(C4,u[i]-c,v[k+m]-c, if(i==m)R sc(k)) 
            else EBLOOP(C4,u[i],  v[k+m],   if(i==m)R sc(k)); break;
  default:       EBLOOP(UC,u[i],  v[k+m],   if(i==m)R sc(k));
 }
 R sc(n);
}    /* a (E. i. 1:) w where a and w are atoms or lists */

F2(jtsumebar){A y;C*av,*wv;I c,d,i,k=0,m,n,p,*yv,z=0;
 RZ(a&&w);
 RE(d=ebarprep(a,w,&a,&w,&c));
 av=CAV(a); m=AN(a);
 wv=CAV(w); n=AN(w); p=n-m;
 switch(d){
  case -1: R zero;
  case -4: R aslash(CPLUS,ebarvec(a,w));
 }
 GATV(y,INT,d,1,0); yv=AV(y); DO(d, yv[i]=1+m;); 
 switch(CTTZ(AT(w))){
  case INTX: if(c)EBLOOP(I, u[i]-c,v[k+m]-c, if(i==m)++z) 
            else EBLOOP(I, u[i],  v[k+m],   if(i==m)++z); break;
  case SBTX: if(c)EBLOOP(SB,u[i]-c,v[k+m]-c, if(i==m)++z) 
            else EBLOOP(SB,u[i],  v[k+m],   if(i==m)++z); break;
  case C2TX:      EBLOOP(US,u[i],  v[k+m],   if(i==m)++z); break;
  case C4TX: if(c)EBLOOP(C4,u[i]-c,v[k+m]-c, if(i==m)++z) 
            else EBLOOP(C4,u[i],  v[k+m],   if(i==m)++z); break;
  default:       EBLOOP(UC,u[i],  v[k+m],   if(i==m)++z);
 }
 R sc(z);
}    /* a ([: +/ E.) w where a and w are atoms or lists */

F2(jtanyebar){A y;C*av,*wv;I c,d,i,k=0,m,n,p,*yv;
 RZ(a&&w);
 RE(d=ebarprep(a,w,&a,&w,&c));
 av=CAV(a); m=AN(a);
 wv=CAV(w); n=AN(w); p=n-m;
 switch(d){
  case -1: R zero;
  case -4: R aslash(CPLUSDOT,ebarvec(a,w));
 }
 GATV(y,INT,d,1,0); yv=AV(y); DO(d, yv[i]=1+m;); 
 switch(CTTZ(AT(w))){
  case INTX: if(c)EBLOOP(I, u[i]-c,v[k+m]-c, if(i==m)R one) 
            else EBLOOP(I, u[i],  v[k+m],   if(i==m)R one); break;
  case SBTX: if(c)EBLOOP(SB,u[i]-c,v[k+m]-c, if(i==m)R one) 
            else EBLOOP(SB,u[i],  v[k+m],   if(i==m)R one); break;
  case C2TX:      EBLOOP(US,u[i],  v[k+m],   if(i==m)R one); break;
  case C4TX: if(c)EBLOOP(C4,u[i]-c,v[k+m]-c, if(i==m)R one) 
            else EBLOOP(C4,u[i],  v[k+m],   if(i==m)R one); break;
  default:       EBLOOP(UC,u[i],  v[k+m],   if(i==m)R one);
 }
 R zero;
}    /* a ([: +./ E.) w where a and w are atoms or lists */

#define IFB1  \
 {if(zu==zv){I m=zu-AV(z); RZ(z=ext(0,z)); zv=m+AV(z); zu=AN(z)+AV(z);} *zv++=k;}

F2(jtifbebar){A y,z;C*av,*wv;I c,d,i,k=0,m,n,p,*yv,*zu,*zv;
 RZ(a&&w);
 RE(d=ebarprep(a,w,&a,&w,&c));
 av=CAV(a); m=AN(a);
 wv=CAV(w); n=AN(w); p=n-m;
 switch(d){
  case -1: R mtv;
  case -4: R icap(ebarvec(a,w));
 }
 GATV(z,INT,MAX(22,n/128),1,0); zv=AV(z); zu=zv+AN(z);
 GATV(y,INT,d,1,0); yv=AV(y); DO(d, yv[i]=1+m;); 
 switch(CTTZ(AT(w))){
  case INTX: if(c)EBLOOP(I, u[i]-c,v[k+m]-c, if(i==m)IFB1)
            else EBLOOP(I, u[i],  v[k+m],   if(i==m)IFB1); break;
  case SBTX: if(c)EBLOOP(SB,u[i]-c,v[k+m]-c, if(i==m)IFB1)
            else EBLOOP(SB,u[i],  v[k+m],   if(i==m)IFB1); break;
  case C2TX:      EBLOOP(US,u[i],  v[k+m],   if(i==m)IFB1); break;
  case C4TX: if(c)EBLOOP(C4,u[i]-c,v[k+m]-c, if(i==m)IFB1)
            else EBLOOP(C4,u[i],  v[k+m],   if(i==m)IFB1); break;
  default:       EBLOOP(UC,u[i],  v[k+m],   if(i==m)IFB1);
 }
 AN(z)=*AS(z)=zv-AV(z);
 R z;
}    /* a ([: I. E.) w where a and w are atoms or lists */
