/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Words: Word Formation                                                   */

#include "j.h"
#include "w.h"

#define SS              0    /* space                           */
#define SX              1    /* other                           */
#define SA              2    /* alphanumeric                    */
#define SN              3    /* N                               */
#define SNB             4    /* NB                              */
#define SNZ             5    /* NB.                             */
#define S9              6    /* numeric                         */
#define SQ              7    /* quote                           */
#define SQQ             8    /* even quotes                     */
#define SZ              9    /* trailing comment                */

#define EI              1    /* emit (b,i-1); b=.i              */
#define EN              2    /* b=.i                            */

typedef struct {C new,effect;} ST;

static ST state[10][9]={
/*SS */ {{SX,EN},{SS,0 },{SA,EN},{SN,EN},{SA,EN},{S9,EN},{SX,EN},{SX,EN},{SQ,EN}},
/*SX */ {{SX,EI},{SS,EI},{SA,EI},{SN,EI},{SA,EI},{S9,EI},{SX,0 },{SX,0 },{SQ,EI}},
/*SA */ {{SX,EI},{SS,EI},{SA,0 },{SA,0 },{SA,0 },{SA,0 },{SX,0 },{SX,0 },{SQ,EI}},
/*SN */ {{SX,EI},{SS,EI},{SA,0 },{SA,0 },{SNB,0},{SA,0 },{SX,0 },{SX,0 },{SQ,EI}},
/*SNB*/ {{SX,EI},{SS,EI},{SA,0 },{SA,0 },{SA,0 },{SA,0 },{SNZ,0},{SX,0 },{SQ,EI}},
/*SNZ*/ {{SZ,0 },{SZ,0 },{SZ,0 },{SZ,0 },{SZ,0 },{SZ,0 },{SX,0 },{SX,0 },{SZ,0 }},
/*S9 */ {{SX,EI},{SS,EI},{S9,0 },{S9,0 },{S9,0 },{S9,0 },{S9,0 },{SX,0 },{SQ,EI}},
/*SQ */ {{SQ,0 },{SQ,0 },{SQ,0 },{SQ,0 },{SQ,0 },{SQ,0 },{SQ,0 },{SQ,0 },{SQQ,0}},
/*SQQ*/ {{SX,EI},{SS,EI},{SA,EI},{SN,EI},{SA,EI},{S9,EI},{SX,EI},{SX,EI},{SQ,0 }},
/*SZ */ {{SZ,0 },{SZ,0 },{SZ,0 },{SZ,0 },{SZ,0 },{SZ,0 },{SZ,0 },{SZ,0 },{SZ,0 }}
};
/*         CX      CS      CA      CN      CB      C9      CD      CC      CQ   */

// w points to a string
// result is word index & length; z is (# words),(i0,l0),(i1,l1),...
// (# words) is negated if the last word is NB.
F1(jtwordil){A z;C e,nv,s,t=0;I b,i,m,n,*x,xb,xe;ST p;UC*v;
 RZ(w);
 nv=0; s=SS;   // set not creating numeric constant; init state (after space)
 n=AN(w); v=UAV(w); GATV(z,INT,1+n+n,1,0); x=1+AV(z);  // get count of characters n and address v;
  // allocate absolute worst-case output area (each char is 1 word, plus 1 for count); point x to output indexes
 for(i=0;i<n;++i){   // run the state machine
  p=state[s][wtype[v[i]]]; e=p.effect;    // go to next state
  if(e==EI){  // if 'emit'...
   t&=s==S9;   // was previous state S9? (means we were building a number)
   if(t){if(!nv){nv=1; xb=b;} xe=i;}   // if so, b must be set; if this is first number, remember starting index.  In any case remember presumptive ending index
   else{if(nv){nv=0; *x++=xb; *x++=xe-xb;} *x++=b; *x++=i-b;}  // Not S9.  If a numeric constant was in progress, it ended on the word BEFORE
    // the one ending now.  In that case, emit the numeric constant.  Then in any case emit the (nonnumeric) ending here.
  }
  s=p.new;   // get next state
  if(e){b=i; t=s==S9;}   // If we should move the chains, remember the new starting position, and whether we are processing a numeric
 }
 if(s==SQ){jsignal3(EVOPENQ,w,b); R 0;}  // error if open quote
 t&=s==S9;    // finish outing the last word as above
 if(t){*x++=xb=nv?xb:b; *x++=n-xb;}
 else{if(nv){*x++=xb; *x++=xe-xb;} if(s!=SS){*x++=b; *x++=n-b;}}  // if final state was 'spaces', there is nothing to put out
 m=x-AV(z); *AV(z)=s==SZ||s==SNZ?-(m>>1):(m>>1);  // Calculate & install count; if last field is NB., make count negative
 R z;
}    /* word index & length; z is (# words),(i0,l0),(i1,l1),... */

/* locals in wordil:                                            */
/* b:  beginning index of current word                          */
/* e:  current effect                                           */
/* i:  index of current character being scanned                 */
/* m:  2 * actual number of words                               */
/* n:  length of input string w                                 */
/* nv: 1 iff numeric constant vector being built                */
/* p:  state table entry per current state & character          */
/* s:  current state                                            */
/* t:  1 iff current state is S9                                */
/* v:  ptr to input string                                      */
/* x:  ptr to current element of z being computed               */
/* xb: beginning index of current numeric vector                */
/* xe: end index of current numeric vector                      */
/* z:  result; maximum of n words                               */

F1(jtwords){A t,*x,z;C*s;I k,n,*y;
 F1RANK(1,jtwords,0);
 RZ(w=vs(w));
 RZ(t=wordil(w));
 s=CAV(w); y=AV(t); n=*y++; n=0>n?-n:n;
 GATV(z,BOX,n,1,0); x=AAV(z);
 DO(n, k=*y++; RZ(*x++=str(*y++,s+k)););
 R z;
}


static A jtconstr(J jt,I n,C*s){A z;C b,c,p,*t,*x;I m=0;
 p=0; t=s; DO(n-2, c=*++t; b=c==CQUOTE; if(!b||p)m++;    p=b&&!p;);
 if(0==m)R aqq; else if(1==m&&(z=chr[(UC)s[1]]))R z;
 GATV(z,LIT,m,1!=m,0); x=CAV(z);
 p=0; t=s; DO(n-2, c=*++t; b=c==CQUOTE; if(!b||p)*x++=c; p=b&&!p;);
 R z;
}

#define TNAME(i)    (NAME&AT(v[i]))
#define TASGN(i)    (ASGN&AT(v[i]))
//#define TVERB(i,c)  (y=v[i], c      ==ID(y))  // true if this word is the given verb
#define TVERB(i,c)  (v[i]==ds(c))  // true if this word is the given verb
#define TRBRACE(i)  TVERB(i,CRBRACE)  // true if given word is }
#define TAIA(i,j)   (TASGN(1) && TNAME(j) && AN(v[i])==AN(v[j]) && TNAME(i) && \
                        !memcmp(NAV(v[i])->s,NAV(v[j])->s,AN(v[i])))
  // true if the two words are names, word 1 is assignment, and the names are equal

// Convert text sentence to a sequence of words to be the queue for parsing
// a holds the result of wordil, which is an integer list of word index & length: (# words),(i0,l0),(i1,l1)...
// w holds the string text of the sentence
// env is the environment for which this is being parsed: 0=tacit translator, 1=keyboard/immex with no locals, 2=for explicit defn
A jtenqueue(J jt,A a,A w,I env){A*v,*x,y,z;B b;C d,e,p,*s,*wi;I i,n,*u,wl;UC c;
 RZ(a&&w);
 s=CAV(w); u=AV(a); n=*u++; n=0>n?-(1+n):n;  // point s to start of string; set u as running pointer pointer in a; fetch # words;
    // if negative (meaning last word is NB.), discard the NB. from the count; step u to point to first (i0,l0) pair
 GATV(z,BOX,n,1,0); x=v=AAV(z);   //  allocate list of words; set running word pointer x, and static
   // beginning-of-list pointer v, to start of list of output pointers
 for(i=0;i<n;i++,x++){  // for each word
  wi=s+*u++; wl=*u++; c=e=*wi; p=ctype[(UC)c]; b=0;   // wi=first char, wl=length, c=e=first char, p=type of first char, b='no inflections'
  if(1<wl){d=*(wi+wl-1); if(b=p!=C9&&d==CESC1||d==CESC2)e=spellin(wl,wi);}  // if word has >1 character, starts with nonnumeric, and ends with inflection, convert to pseudocharacter
  if(128>c&&(y=ds(e))){
   // If first char is ASCII, see if the form including inflections is a primitive;
   // if so, that is the word to put into the queue.  No need to copy it
   // Since the address of the shared primitive block is used, we can use that to compare against to identify the primitive later
   // We keep track of We keep track of whether }~ was found.  If } starts the sentence, this will compare garbage, but
   // without risk of program check
   // If the word is an assignment, use the appropriate assignment block, depending on the previous word and the environment
   // In environment 0 (tacit translator), leave as simple assignment
   if(env!=0 && AT(y)&ASGN) {
    if(env==1 || (i && AT(x[-1])&NAME && (NAV(x[-1])->flag&(NMLOC|NMILOC)))){y=ds(CGASGN);}   // sentence is NOT for explicit definition, or preceding word is a locative.  Convert to a global assignment.  This will make the display show =:
    if(i&& AT(x[-1])&NAME){y= y==ds(CGASGN)?asgngloname:asgnlocsimp;}  // if ASGN preceded by NAME, flag it thus, by switching to the block with the ASGNTONAME flag set
   }
   if(AT(y)&NAME&&(NAV(y)->flag&NMDOT)){RZ(y=ca(y)); if(env==2)AT(y)|=NAMEBYVALUE;}  // The inflected names are the old-fashioned x. y. etc.  They must be cloned lest we modify the shared copy
   *x=y;   // install the value
  } else if(e==CFCONS){RZ(*x=FCONS(connum(wl-1,wi)))  // if the inflected form says [_]0-9:, create word for that
  } else switch(b?0:p){    // otherwise, it's not a primitive, but data, either numeric, string, or name.  Check first character, but fail if inflected form, which must be invalid
   default: jsignal3(EVSPELL,w,wi-s); R 0;   // bad first character or inflection
   case C9: RZ(*x=connum(wl,wi));   break;   // starts with numeric, create numeric constant
   case CQ: RZ(*x=constr(wl,wi));   break;   // start with ', make string constant
   case CA: ASSERTN(vnm(wl,wi),EVILNAME,nfs(wl,wi)); RZ(*x=nfs(wl,wi)); if((env==2)&&(NAV(*x)->flag&NMDOT)){AT(*x)|=NAMEBYVALUE;}  // starts with alphabetic, make it a name, error if invalid name
    // If the name is a call-by-value name (x y u. etc), we mark it as BYVALUE if it is slated for execution in an explicit definition
  }

  // Mark the word as not-inplaceable.  Since all allocations start life marked in-placeable, we get into trouble if words in
  // a explicit definition are left that way, because the sentences may get reexecuted and any constant marked inplaceable could
  // be modified by each use.  The trouble happens only if the definition is not assigned (if it's assigned all the usecounts
  // are incremented).  But just do it for all words in sentences
  ACIPNO(*x);  // mark the stored word not inplaceable
 }

 // We have created the queue.  Here we check for special in-place forms.  If we find one, we
 // replace the verb/adverb with a version that performs in-place operation.

 // The game here is to make tests that fail as quickly as possible, so that this overhead
 // falls lightly on most sentences
 if (6 <= n) {
  if(TRBRACE(3) && TVERB(n-2,CLAMIN) && TNAME(0) && TASGN(1) && TNAME(2)){A p,*yv,z1;I c,j,k,m;  // if we match }~, can't match this
   // abc=: pqr}x,...y,:z  (must be odd # words, but that's not worth checking)
   // Verify the form is present: alternating names and commas
   b=1; m=(n-3)>>1;   // number of nouns x..z
   j=4;      DO(m,   if(!TNAME(j)       ){b=0; break;} j+=2;);  // Check all names
   j=5; if(b)DO(m-2, if(!TVERB(j,CCOMMA)){b=0; break;} j+=2;);  // Check inbetween, except after last 2 names 
   if(b){  // if the form is right...
    // Synthesize a new argument to replace the list of arrays, and a new sentence to execute on this argument
    // The sentence is abc =: CCASEV (new argument)
    // The new argument is (the list of names), then
    // (_3) the name pqr, (_2) the position if any of abc in the right-hand list (-1 if absent), (_1) the parsed queue before this replacement
    GAT(z1,BOX,4,1,0); x=AAV(z1);   // Allocate the sentence, point to its data
    GATV(y,BOX,m+3,1,0); yv=AAV(y);   // Allocate the argument
    c=-1; k=AN(v[0]); s=NAV(v[0])->s;   // get length and address of abc
    j=4; DO(m, yv[i]=p=v[j]; j+=2; if(AN(p)==k&&!memcmp(s,NAV(p)->s,k))c=i;);  // move name into argument, remember if matched abc
    yv[m]=v[2]; RZ(yv[m+1]=sc(c)); yv[m+2]=z;    // add the 3 ending elements
    x[0]=v[0]; x[1]=v[1]; x[2]=ds(CCASEV); x[3]=y;  // build the sentence
    R z1;  // that's what we'll execute
   }
  }
 }
 R z;
}    /* produce boxed list of words suitable for parsing */
                                                            
/* locals in enqueue:                                           */
/* b:  1 iff current word is a primitive spelled with . or :    */
/* c:  first character in current word                          */
/* d:  last  character in current word                          */
/* e:  first character in current word, after spellin           */
/* i:  index of current word                                    */
/* n:  number of words                                          */
/* p:  character type of current character                      */
/* s:  ptr to value part of input string w                      */
/* u:  ptr to value part of word index & length info a          */
/* v:  ptr to value part of z                                   */
/* wi: index of current word in input string                    */
/* wl: length of current word                                   */
/* x:  ptr to result word being built                           */
/* y:  array temp                                               */
/* z:  result array of boxed list of words                      */

// env is the environment: 0=tacit translator, 1=keyboard/immex with no local symbol, 2=explicit definition running
A jttokens(J jt,A w,I env){R enqueue(wordil(w),w,env);}


#define CHKJ(j)             ASSERT(0<=(j),EVINDEX);
#define EXTZ(T,p)           while(uu<p+u){k=u-(T*)AV(z); RZ(z=ext(0,z)); u=k+(T*)AV(z); uu=(T*)AV(z)+AN(z);}

#define EMIT0c(T,j,i,r,c)   {CHKJ(j); p=(i)-(j); EXTZ(T,1); RZ(*u++=str(p,(j)+wv));}
#define EMIT0b(T,j,i,r,c)   {CHKJ(j); p=(i)-(j); EXTZ(T,1); RZ(*u++=vec(B01,p,(j)+wv));}
#define EMIT0x(T,j,i,r,c)   {CHKJ(j); p=(i)-(j); EXTZ(T,1); GA(x,t0,p*wm,wr,AS(w0));  \
                                *AS(x)=p; MC(AV(x),wv0+wk*(j),wk*p); *u++=x;}
#define EMIT1(T,j,i,r,c)    {CHKJ(j); p=(i)-(j);            cc=(j)+wv; DO(p, *u++=*cc++;);}
#define EMIT1x(T,j,i,r,c)   {CHKJ(j); p=wk*((i)-(j));       MC(u,wv0+j*wk,p); u+=p;}
#define EMIT2(T,j,i,r,c)    {CHKJ(j); p=(i)-(j); EXTZ(T,2); *u++=(j); *u++=p;}
#define EMIT3(T,j,i,r,c)    {CHKJ(j);            EXTZ(T,1);                   *u++=(c)+q*(r);}
#define EMIT4(T,j,i,r,c)    {CHKJ(j); p=(i)-(j); EXTZ(T,3); *u++=(j); *u++=p; *u++=(c)+q*(r);}
#define EMIT5(T,j,x,r,c)    {if(0>(j))i=n;}

#define DO_ONE(T,EMIT) \
 switch(e=v[1]){                                                          \
  case 6:         i=n; break;                                             \
  case 2: case 3: if(0<=vi){EMIT(T,vj,vi,vr,vc); vi=vr=-1;} EMIT(T,j,i,r,c);       j=2==e?i:-1; break;  \
  case 4: case 5: if(r!=vr){if(0<=vi)EMIT(T,vj,vi,vr,vc); vj=j; vr=r; vc=c;} vi=i; j=4==e?i:-1; break;  \
  case 1:         j=i;                                                    \
 }

#define ZVAx                {}
#define ZVA5                {*zv++=i; *zv++=j; *zv++=r; *zv++=c; *zv++=v[0]; *zv++=v[1];}

#define FSMF(T,zt,zr,zm,cexp,EMIT,ZVA)    \
 {T*u,*uu;                                                                  \
  RZ(z=exta((zt),(zr),(zm),1==f||5==f?n:n/3));                              \
  if(1<(zr)){I*s=AS(z); s[1]=(zm); if(1==f&&2<wr)ICPY(1+s,1+AS(w0),wr-1);}  \
  zv=AV(z); u=(T*)zv; uu=u+AN(z);                                           \
  for(;i<n;++i,r=*v){c=(cexp); v=sv+2*(c+r*q); ZVA; DO_ONE(T,EMIT);}        \
  if(6!=e){                                                                 \
   if(0<=d)         {c=d;      v=sv+2*(c+r*q); ZVA; DO_ONE(T,EMIT);}        \
   else{                                                                    \
    if(0<=vi      )EMIT(T,vj,r==vr?n:vi,vr,vc);                             \
    if(r!=vr&&0<=j)EMIT(T,j,n,r,c);                                         \
  }}                                                                        \
  if(5==f)u=(T*)zv;                                                         \
  i=AN(z); AN(z)=j=u-(T*)AV(z); *AS(z)=j/(zm); if(i>3*j)RZ(z=ca(z));        \
 }

static A jtfsmdo(J jt,I f,A s,A m,I*ijrd,A w,A w0){A x,z;C*cc,*wv0;
     I c,d,e,i,j,k,*mv,n,p,q,r,*sv,t,t0,*v,vc,vi,vj,vr,wk,wm,wr,*zv;
 n=IC(w); t=AT(w);
 q=*(1+AS(s));
 sv=AV(s); mv=AV(m);
 i=ijrd[0]; j=ijrd[1]; r=ijrd[2]; d=ijrd[3]; vi=vj=vr=vc=-1;
 if(t&INT){t0=AT(w0); wr=AR(w0); wm=aii(w0); wk=wm*bp(AT(w0)); wv0=CAV(w0);}
 switch(f+(t&B01?0:t&LIT?10:20)){
  case  0: {B *wv=BAV(w); FSMF(A,BOX,1, 1,   wv[i] ,EMIT0b,ZVAx);} break;
  case  1: {B *wv=UAV(w); FSMF(B,B01,1, 1,   wv[i] ,EMIT1, ZVAx);} break;
  case  2: {B *wv=BAV(w); FSMF(I,INT,2, 2,   wv[i] ,EMIT2, ZVAx);} break;
  case  3: {B *wv=BAV(w); FSMF(I,INT,1, 1,   wv[i] ,EMIT3, ZVAx);} break;
  case  4: {B *wv=BAV(w); FSMF(I,INT,2, 3,   wv[i] ,EMIT4, ZVAx);} break;
  case  5: {B *wv=BAV(w); FSMF(I,INT,2, 6,   wv[i] ,EMIT5, ZVA5);} break;

  case 10: {UC*wv=UAV(w); FSMF(A,BOX,1, 1,mv[wv[i]],EMIT0c,ZVAx);} break;
  case 11: {UC*wv=UAV(w); FSMF(C,LIT,1, 1,mv[wv[i]],EMIT1, ZVAx);} break;
  case 12: {UC*wv=UAV(w); FSMF(I,INT,2, 2,mv[wv[i]],EMIT2, ZVAx);} break;
  case 13: {UC*wv=UAV(w); FSMF(I,INT,1, 1,mv[wv[i]],EMIT3, ZVAx);} break;
  case 14: {UC*wv=UAV(w); FSMF(I,INT,2, 3,mv[wv[i]],EMIT4, ZVAx);} break;
  case 15: {UC*wv=UAV(w); FSMF(I,INT,2, 6,mv[wv[i]],EMIT5, ZVA5);} break;

  case 20: {I *wv= AV(w); FSMF(A,BOX,1, 1,   wv[i] ,EMIT0x,ZVAx);} break;
  case 21: {I *wv= AV(w); FSMF(C,t0, wr,wm,  wv[i] ,EMIT1x,ZVAx);} break;
  case 22: {I *wv= AV(w); FSMF(I,INT,2, 2,   wv[i] ,EMIT2, ZVAx);} break;
  case 23: {I *wv= AV(w); FSMF(I,INT,1, 1,   wv[i] ,EMIT3, ZVAx);} break;
  case 24: {I *wv= AV(w); FSMF(I,INT,2, 3,   wv[i] ,EMIT4, ZVAx);} break;
  case 25: {I *wv= AV(w); FSMF(I,INT,2, 6,   wv[i] ,EMIT5, ZVA5);}
 }
 R z;
}

F1(jtfsmvfya){PROLOG(0099);A a,*av,m,s,x,z,*zv;I ad,an,c,e,f,ijrd[4],k,p,q,*sv,*v;
 RZ(a=w);
 ASSERT(1==AR(a),EVRANK);
 ASSERT(BOX&AT(a),EVDOMAIN);
 an=AN(a); av=AAV(a); ad=(I)a*ARELATIVE(a);
 ASSERT(2<=an&&an<=4,EVLENGTH);
 RE(f=i0(AVR(0)));
 ASSERT(0<=f&&f<=5,EVINDEX);
 RZ(s=vi(AVR(1))); sv=AV(s);
 ASSERT(3==AR(s),EVRANK);
 v=AS(s); p=v[0]; q=v[1]; ASSERT(2==v[2],EVLENGTH);
 v=sv; DO(p*q, k=*v++; e=*v++; ASSERT(0<=k&&k<p&&0<=e&&e<=6,EVINDEX););
 ijrd[0]=0; ijrd[1]=-1; ijrd[2]=0; ijrd[3]=-1;
 if(4==an){I d,i,j,n,r;
  RZ(x=vi(AVR(3))); n=AN(x); v=AV(x);
  ASSERT(1==AR(x),EVRANK);
  ASSERT(4>=n,EVLENGTH);
  if(1<=n) ijrd[0]=i=*v++;
  if(2<=n){ijrd[1]=j=*v++; ASSERT(j==-1||0<=j&&j<i,EVINDEX);}
  if(3<=n){ijrd[2]=r=*v++; ASSERT(       0<=r&&r<p,EVINDEX);}
  if(4==n){ijrd[3]=d=*v++; ASSERT(d==-1||0<=d&&d<q,EVINDEX);}
 }
 m=2==an?mtv:AVR(2); c=AN(m);
 ASSERT(1>=AR(m),EVRANK);
 if(!c&&1==AR(m)){   /* m is empty; w must be integer vector */  }
 else if(NUMERIC&AT(m)){
  ASSERT(c==AN(alp),EVLENGTH);
  RZ(m=vi(m)); v=AV(m); DO(c, k=v[i]; ASSERT(0<=k&&k<q,EVINDEX););
 }else ASSERT(BOX&AT(m),EVDOMAIN);
 GAT(z,BOX,4,1,0); zv=AAV(z);
 RZ(zv[0]=sc(f)); zv[1]=s; zv[2]=m; RZ(zv[3]=vec(INT,4L,ijrd));
 R z;
}    /* check left argument of x;:y */

static A jtfsm0(J jt,A a,A w,C chka){PROLOG(0100);A*av,m,s,x,w0=w;B b;I ad,c,f,*ijrd,k,md,n,p,q,*v;
 RZ(a&&w);
 if(chka)RZ(a=fsmvfya(a)); 
 av=AAV(a); ad=(I)a*ARELATIVE(a);
 f=i0(AVR(0)); s=AVR(1); m=AVR(2); ijrd=AV(AVR(3));
 n=AN(w); v=AS(s); p=v[0]; q=v[1];
 ASSERT(0<=ijrd[0]&&ijrd[0]<n,EVINDEX);
 b=1>=AR(w)&&(!n||LIT&AT(w)); c=AN(m);
 if(!c&&1==AR(m)){
  ASSERT(1>=AR(w),EVRANK);
  if(!(B01&AT(w))){RZ(w=w0=vi(w)); v=AV(w); DO(n, k=v[i]; ASSERT(0<=k&&k<q,EVINDEX););}
 }else if(NUMERIC&AT(m)){
  ASSERT(b,EVDOMAIN); 
  ASSERT(1>=AR(w),EVRANK);
 }else{A*mv,t,y;I j,r;
  ASSERT(BOX&AT(m),EVDOMAIN);
  RZ(y=raze(m)); r=AR(y); k=AN(y);
  ASSERT(r==AR(w)||r==1+AR(w),EVRANK);
  GATV(x,INT,1+k,1,0); v=AV(x); v[k]=c; mv=AAV(m); md=(I)m*ARELATIVE(m); 
  DO(c, j=i; t=AADR(md,mv[i]); if(r&&r==AR(t))DO(*AS(t), *v++=j;) else *v++=j;);
  if(b){RZ(m=from(indexof(y,alp),x)); v=AV(m); DO(AN(alp), k=v[i]; ASSERT(0<=k&&k<q,EVINDEX););}
  else {ASSERT(q>c,EVINDEX); RZ(w=from(indexof(y,w),x));}
 }
 A z=fsmdo(f,s,m,ijrd,w,w0);
 EPILOG(z);
}

F2(jtfsm){R fsm0(a,w,1);}
     /* x;:y */

DF1(jtfsmfx){RZ(w&&self); R fsm0(VAV(self)->f,w,0);}
     /* x&;: y */
