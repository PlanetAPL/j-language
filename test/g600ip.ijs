
NB. atomic"n in-place and not -------------------------------
randuni''

NB. Use-count test
by =: (<51);(20 20+2.5-2.5);''
(22 0)  4 : 0  by
tlc =. x
'dos dotl wires' =. 3 {. y
dotl =. dotl +"1 tlc
1
)
by -: (<51);(20 20+2.5-2.5);''
NB. Use-count test
(22 0)  4 : 0  by
tlc =. x
'dos dotl wires' =. 3 {. y
dotl =. tlc +"1 dotl
1
)
by -: (<51);(20 20+2.5-2.5);''

NB. Test plusBI and plusIB with overflow and inplace

bx =: 0 1 0 0 0 1 0 1 0 0 0 0 1
IMAX =. _1 - IMIN =. <. - 2 ^ IF64 { 31 63
iy =: (($bx) ,3 5) $ IMAX,IMIN,3 5 6 7,IMIN,2 3,IMAX
(bx + iy) -: bx +"0"0 iy
(bx + (0 + iy)) -: bx +"0"0 iy
((0 +. bx) + iy) -: bx +"0"0 iy
((0 +. bx) + (0 + iy)) -: bx +"0"0 iy

bx =: 4 3$0 1 0 0 0 1 0 1 0 0 0 0 1 0
iy =: (($bx) ,3 5) $ IMAX,IMIN,3 5 6 7,IMIN,2 3,IMAX
(bx + iy) -: bx +"0"0 iy
(bx + (0 + iy)) -: bx +"0"0 iy
((0 +. bx) + iy) -: bx +"0"0 iy
((0 +. bx) + (0 + iy)) -: bx +"0"0 iy
iy =: (($bx) ,3 5) $ IMAX,IMIN,3 5 6 7,IMIN,2 3,IMAX
(bx +"1 2 iy) -: bx +"0"1 2 iy
(bx +"1 2 (0 + iy)) -: bx +"0"1 2 iy
((0 +. bx) +"1 2 iy) -: bx +"0"1 2 iy
((0 +. bx) +"1 2 (0 + iy)) -: bx +"0"1 2 iy

NB. Test minusBI and minusIB with overflow and inplace
bx =: 0 1 0 0 0 1 0 1 0 0 0 0 1
iy =: (($bx) ,3 5) $ IMAX,IMIN,3 5 6 7,IMIN,2 3,IMAX
(bx - iy) -: bx -"0"0 iy
(bx - (iy - 0)) -: bx -"0"0 iy
((0 +. bx) - iy) -: bx -"0"0 iy
((0 +. bx) - (iy - 0)) -: bx -"0"0 iy

bx =: 4 3$0 1 0 0 0 1 0 1 0 0 0 0 1 0
iy =: (($bx) ,3 5) $ IMAX,IMIN,3 5 6 7,IMIN,2 3,IMAX
(bx - iy) -: bx -"0"0 iy
(bx - (iy - 0)) -: bx -"0"0 iy
((0 +. bx) - iy) -: bx -"0"0 iy
((0 +. bx) - (iy - 0)) -: bx -"0"0 iy
iy =: (($bx) ,3 5) $ IMAX,IMIN,3 5 6 7,IMIN,2 3,IMAX
(bx -"1 2 iy) -: bx -"0"1 2 iy
(bx -"1 2 (iy - 0)) -: bx -"0"1 2 iy
((0 +. bx) -"1 2 iy) -: bx -"0"1 2 iy
((0 +. bx) -"1 2 (iy - 0)) -: bx -"0"1 2 iy


NB. x is selection;shapes;prediction
NB.  selection selects a prediction
NB.  prediction is string, 27 predictors separated by '/'
NB.   predictor contains the following chars:
NB.    k,l,L  allocate B01/INT/FL block for conversion of left arg
NB.    q,r,R  allocate B01/INT/FL block for conversion of right arg
NB.    B,b,I,D,d   allocate B01/INT/FL for result; d means (FL only if FL is bigger than INT); b is removed by caller if inplacing allowed
NB.  shapes is (xshape,yshape,resultshape)
NB. y is # of bytes used in execution of the sentence
NB. Result is 1 if in range
checkallosize =: 4 : 0
'sel shapes pred' =: x
bytesused =: y
if. 0 = #pred do. 1 return. end.  NB. If no prediction, don't fail
assert. 27 = # predflds =: <;._2 pred
spred =: ' ' -.~ sel {:: predflds  NB. Select fielda
atomct =: 3 3 5 # */@> shapes   NB. Get #atoms in each arg/result
atomsz =: IF64 { 1 4 8 1 4 8 1 1 4 8 8 ,: 1 8 8 1 8 8 1 1 8 8 0   NB. Length of atoms
allobytes =: +/ >.&.(2&^.) (atomct*atomsz) {~ 'klLqrRBbIDd' i. spred  NB. Total # bytes allocated
(bytesused>allobytes-3000) *. (bytesused<allobytes+3000)
)


NB. u is verb; y is (left shape;right shape). n is 27x3x3 table of # of (b,i,d) buffers expected to be allocated in shape (x,y,result)
NB. Create random x and y; run u not-in-place and save result
NB. Run on combinations of inplaceable executions; verify result
testinplace =: 2 : 0
vb =: u
'xs ys' =: y
'resvalidity allopred' =: n

if. 'B' e. resvalidity do.
NB. Test binary data, in all precisions

NB. Create random data
bx =: xs ?@$ 2
by =: ys ?@$ 2
NB. Make the first value 0.  This creates 0|0 which goes through a different path in the code; we need to do so predictably
if. #,bx do. bx =: 0 0:} bx end.
if. #,by do. by =: 0 0:} by end.
NB. Create copies in other precisions
ix =: bx + (2-2)
iy =: by + (2-2)
dx =: ix + (2.5-2.5)
dy =: iy + (2.5-2.5)
NB. Save all precisions in a separate area
'svbx svby svix sviy svdx svdy' =: svxy =: 3!:1&.> bx;by;ix;iy;dx;dy
NB. Get result with all looping performed in rank conjunction
r =: bx u"u"u by
xyzs =: xs;ys;$r  NB. Used to predict allocation
NB. For each precision combination, verify identical result
assert. (0;xyzs;allopred) checkallosize 7!:2 'tr =: bx u by'
assert. r -: tr [ 00 [  'tr =: bx u by'
assert. (1;xyzs;allopred) checkallosize 7!:2 'tr =: bx u iy'
assert. r -: tr [ 01 [  'tr =: bx u iy'
assert. (2;xyzs;allopred) checkallosize 7!:2 'tr =: bx u dy'
assert. r -: tr [ 02 [  'tr =: bx u dy'
assert. (3;xyzs;allopred) checkallosize 7!:2 'tr =: ix u by'
assert. r -: tr [ 03 [ 'tr =: ix u by'
assert. (4;xyzs;allopred) checkallosize 7!:2 'tr =: ix u iy'
assert. r -: tr [ 04 [ 'tr =: ix u iy'
assert. (5;xyzs;allopred) checkallosize 7!:2 'tr =: ix u dy'
assert. r -: tr [ 05 [ 'tr =: ix u dy'
assert. (6;xyzs;allopred) checkallosize 7!:2 'tr =: dx u by'
assert. r -: tr [ 06 [ 'tr =: dx u by'
assert. (7;xyzs;allopred) checkallosize 7!:2 'tr =: dx u iy'
assert. r -: tr [ 07 [ 'tr =: dx u iy'
assert. (8;xyzs;allopred) checkallosize 7!:2 'tr =: dx u dy'
assert. r -: tr [ 08 [ 'tr =: dx u dy'
NB. Verify nothing modified in place
assert. svxy -: 3!:1&.> bx;by;ix;iy;dx;dy
NB. Make x inplaceable.  Verify correct result
tx =: 3!:2 (3!:1) bx
assert. (9;xyzs;allopred) checkallosize 7!:2 'tx =: tx u by'
assert. r -: tx [ 10 [ 'tx =: tx u by'
tx =: 3!:2 (3!:1) bx
assert. (10;xyzs;allopred) checkallosize 7!:2 'tx =: tx u iy'
assert. r -: tx [ 11 [ 'tx =: tx u iy'
tx =: 3!:2 (3!:1) bx
assert. (11;xyzs;allopred) checkallosize 7!:2 'tx =: tx u dy'
assert. r -: tx [ 12 [ 'tx =: tx u dy'
tx =: 3!:2 (3!:1) ix
assert. (12;xyzs;allopred) checkallosize 7!:2 'tx =: tx u by'
assert. r -: tx [ 13 [ 'tx =: tx u by'
tx =: 3!:2 (3!:1) ix
assert. (13;xyzs;allopred) checkallosize 7!:2 'tx =: tx u iy'
assert. r -: tx [ 14 [ 'tx =: tx u iy'
tx =: 3!:2 (3!:1) ix
assert. (14;xyzs;allopred) checkallosize 7!:2 'tx =: tx u dy'
assert. r -: tx [ 15 [ 'tx =: tx u dy'
tx =: 3!:2 (3!:1) dx
assert. (15;xyzs;allopred) checkallosize 7!:2 'tx =: tx u by'
assert. r -: tx [ 16 [ 'tx =: tx u by'
tx =: 3!:2 (3!:1) dx
assert. (16;xyzs;allopred) checkallosize 7!:2 'tx =: tx u iy'
assert. r -: tx [ 17 [ 'tx =: tx u iy'
tx =: 3!:2 (3!:1) dx
assert. (17;xyzs;allopred) checkallosize 7!:2 'tx =: tx u dy'
assert. r -: tx [ 18 [ 'tx =: tx u dy'
NB. Verify originals not modified in place
assert. svxy -: 3!:1&.> bx;by;ix;iy;dx;dy
NB. Make y inplaceable.  Verify correct result
ty =: 3!:2 (3!:1) by
assert. (18;xyzs;allopred) checkallosize 7!:2 'ty =: bx u ty'
assert. r -: ty [ 20 [ 'ty =: bx u ty'
ty =: 3!:2 (3!:1) iy
assert. (19;xyzs;allopred) checkallosize 7!:2 'ty =: bx u ty'
assert. r -: ty [ 21 [ 'ty =: bx u ty'
ty =: 3!:2 (3!:1) dy
assert. (20;xyzs;allopred) checkallosize 7!:2 'ty =: bx u ty'
assert. r -: ty [ 22 [ 'ty =: bx u ty'
ty =: 3!:2 (3!:1) by
assert. (21;xyzs;allopred) checkallosize 7!:2 'ty =: ix u ty'
assert. r -: ty [ 23 [ 'ty =: ix u ty'
ty =: 3!:2 (3!:1) iy
assert. (22;xyzs;allopred) checkallosize 7!:2 'ty =: ix u ty'
assert. r -: ty [ 24 [ 'ty =: ix u ty'
ty =: 3!:2 (3!:1) dy
assert. (23;xyzs;allopred) checkallosize 7!:2 'ty =: ix u ty'
assert. r -: ty [ 25 [ 'ty =: ix u ty'
ty =: 3!:2 (3!:1) by
assert. (24;xyzs;allopred) checkallosize 7!:2 'ty =: dx u ty'
assert. r -: ty [ 26 [ 'ty =: dx u ty'
ty =: 3!:2 (3!:1) iy
assert. (25;xyzs;allopred) checkallosize 7!:2 'ty =: dx u ty'
assert. r -: ty [ 27 [ 'ty =: dx u ty'
ty =: 3!:2 (3!:1) dy
assert. (26;xyzs;allopred) checkallosize 7!:2 'ty =: dx u ty'
assert. r -: ty [ 28 [ 'ty =: dx u ty'
NB. Verify originals not modified in place
assert. svxy -: 3!:1&.> bx;by;ix;iy;dx;dy
end.

if. 'I' e. resvalidity do.
NB. Test integer data, in integer and float precisions

NB. Create random data.  This will not generate overflow so we don't have to worry about unreproducible space requirements
ix =: xs ?@$ IF64 { 30000 1000000
iy =: ys ?@$ IF64 { 30000 1000000
NB. Make the first value 0.  This creates 0|0 which goes through a different path in the code; we need to do so predictably
if. #,ix do. ix =: 0 0:} ix end.
if. #,iy do. iy =: 0 0:} iy end.
NB. Create copies in other precisions
dx =: ix + (2.5-2.5)
dy =: iy + (2.5-2.5)
NB. Save all precisions in a separate area
'svix sviy svdx svdy' =: svxy =: 3!:1&.> ix;iy;dx;dy
NB. Get result with all looping performed in rank conjunction
r =: ix u"u"u iy
NB. For each precision combination, verify identical result
assert. (4;xyzs;allopred) checkallosize 7!:2 'tr =: ix u iy'
assert. r -: tr [ 30 [ 'tr =: ix u iy'
assert. (5;xyzs;allopred) checkallosize 7!:2 'tr =: ix u dy'
assert. r -: tr [ 31 [ 'tr =: ix u dy'
assert. (7;xyzs;allopred) checkallosize 7!:2 'tr =: dx u iy'
assert. r -: tr [ 32 [ 'tr =: dx u iy'
assert. (8;xyzs;allopred) checkallosize 7!:2 'tr =: dx u dy'
assert. r -: tr [ 33 [ 'tr =: dx u dy'
NB. Verify nothing modified in place
assert. svxy -: 3!:1&.> ix;iy;dx;dy
NB. Make x inplaceable.  Verify correct result
tx =: 3!:2 (3!:1) ix
assert. (13;xyzs;allopred) checkallosize 7!:2 'tx =: tx u iy'
assert. r -: tx [ 40 [ 'tx =: tx u iy'
tx =: 3!:2 (3!:1) ix
assert. (14;xyzs;allopred) checkallosize 7!:2 'tx =: tx u dy'
assert. r -: tx [ 41 [ 'tx =: ix u dy'
tx =: 3!:2 (3!:1) dx
assert. (16;xyzs;allopred) checkallosize 7!:2 'tx =: tx u iy'
assert. r -: tx [ 42 [ 'tx =: tx u iy'
tx =: 3!:2 (3!:1) dx
assert. (17;xyzs;allopred) checkallosize 7!:2 'tx =: tx u dy'
assert. r -: tx [ 43 [ 'tx =: tx u dy'
NB. Verify originals not modified in place
assert. svxy -: 3!:1&.> ix;iy;dx;dy
NB. Make y inplaceable.  Verify correct result
ty =: 3!:2 (3!:1) iy
assert. (22;xyzs;allopred) checkallosize 7!:2 'ty =: ix u ty'
assert. r -: ty [ 50 [ 'ty =: ix u ty'
ty =: 3!:2 (3!:1) dy
assert. (23;xyzs;allopred) checkallosize 7!:2 'ty =: ix u ty'
assert. r -: ty [ 51 [ 'ty =: ix u ty'
ty =: 3!:2 (3!:1) iy
assert. (25;xyzs;allopred) checkallosize 7!:2 'ty =: dx u ty'
assert. r -: ty [ 52 [ 'ty =: dx u ty'
ty =: 3!:2 (3!:1) dy
assert. (26;xyzs;allopred) checkallosize 7!:2 'ty =: dx u ty'
assert. r -: ty [ 53 [ 'ty =: dx u ty'
NB. Verify originals not modified in place
assert. svxy -: 3!:1&.> ix;iy;dx;dy
end.

if. 'O' e. resvalidity do.
NB. Test integer data, in integer and float precisions, with overflow

NB. Create random data.
ix =: xs (?@$ - <.@:-:@:]) IF64 { 30000 1000000
iy =: ys (?@$ - <.@:-:@:]) IF64 { 30000 1000000
NB. Install random overflow values, with enough coverage to exercise the paths; say 10% of each argument, both IMAX and IMIN
IMAX =. _1 - IMIN =. <. - 2 ^ IF64 { 31 63
if. #ixx =. ($ix) <@#: I. 0.10 > ?@$&0 */ $ ix do.
 ix =: (IMAX - (#ixx) ?@$ 10) ixx} ix
elseif. '' -: $ix do. ix =: IMIN
end.
if. #iyx =. ($iy) <@#: I. 0.10 > ?@$&0 */ $ iy do.
 iy =: (IMIN + (#iyx) ?@$ 10) iyx} iy
elseif. '' -: $iy do. iy =: IMAX
end.
NB. Make the first value 0.  This creates 0|0 which goes through a different path in the code; we need to do so predictably
if. #,ix do. ix =: 0 0:} ix end.
if. #,iy do. iy =: 0 0:} iy end.
NB. Create copies in other precisions
dx =: ix + (2.5-2.5)
dy =: iy + (2.5-2.5)
NB. Save all precisions in a separate area
'svix sviy svdx svdy' =: svxy =: 3!:1&.> ix;iy;dx;dy
NB. Get result with all looping performed in rank conjunction
r =: ix u"u"u iy
NB. For each precision combination, verify reasonable result.  Because we convert to float,
NB. IMAX+IMIN may lose precision if INTs are 64-bit.  We are mainly making sure we don't
NB. turn IMIN to IMAX or vice versa
fuzz =. IF64 { 8 16384 
assert. (4;xyzs;allopred) checkallosize 7!:2 'tr =: ix u iy'
assert. *./ , fuzz > | r - tr [ 60 [ 'tr =: ix u iy'
assert. (5;xyzs;allopred) checkallosize 7!:2 'tr =: ix u dy'
assert. *./ , fuzz > | r - tr [ 61 [ 'tr =: ix u dy'
assert. (7;xyzs;allopred) checkallosize 7!:2 'tr =: dx u iy'
assert. *./ , fuzz > | r - tr [ 62 [ 'tr =: dx u iy'
assert. (8;xyzs;allopred) checkallosize 7!:2 'tr =: dx u dy'
assert. *./ , fuzz > | r - tr [ 63 [ 'tr =: dx u dy'
NB. Verify nothing modified in place
assert. svxy -: 3!:1&.> ix;iy;dx;dy
NB. Make x inplaceable.  Verify correct result
tx =: 3!:2 (3!:1) ix
assert. (13;xyzs;allopred) checkallosize 7!:2 'tx =: tx u iy'
assert. *./ , fuzz > | r - tx [ 70 [ 'tx =: tx u iy'
tx =: 3!:2 (3!:1) ix
assert. (14;xyzs;allopred) checkallosize 7!:2 'tx =: tx u dy'
assert. *./ , fuzz > | r - tx [ 71 [ 'tx =: tx u dy'
tx =: 3!:2 (3!:1) dx
assert. (16;xyzs;allopred) checkallosize 7!:2 'tx =: tx u iy'
assert. *./ , fuzz > | r - tx [ 72 [ 'tx =: tx u iy'
tx =: 3!:2 (3!:1) dx
assert. (17;xyzs;allopred) checkallosize 7!:2 'tx =: tx u dy'
assert. *./ , fuzz > | r - tx [ 73 [ 'tx =: tx u dy'
NB. Verify originals not modified in place
assert. svxy -: 3!:1&.> ix;iy;dx;dy
NB. Make y inplaceable.  Verify correct result
ty =: 3!:2 (3!:1) iy
assert. (22;xyzs;allopred) checkallosize 7!:2 'ty =: ix u ty'
assert. *./ , fuzz > | r - ty [ 80 [ 'ty =: ix u ty'
ty =: 3!:2 (3!:1) dy
assert. (23;xyzs;allopred) checkallosize 7!:2 'ty =: ix u ty'
assert. *./ , fuzz > | r - ty [ 81 [ 'ty =: ix u ty'
ty =: 3!:2 (3!:1) iy
assert. (25;xyzs;allopred) checkallosize 7!:2 'ty =: dx u ty'
assert. *./ , fuzz > | r - ty [ 82 [ 'ty =: dx u ty'
ty =: 3!:2 (3!:1) dy
assert. (26;xyzs;allopred) checkallosize 7!:2 'ty =: dx u ty'
assert. *./ , fuzz > | r - ty [ 83 [ 'ty =: dx u ty'
NB. Verify originals not modified in place
assert. svxy -: 3!:1&.> ix;iy;dx;dy
end.

if. 'D' e. resvalidity do.
NB. Test float data, in float precision

NB. Create random data.  This will not generate overflow so we don't have to worry about unreproducible space requirements
dx =: xs ?@$ 0
dy =: ys ?@$ 0
NB. Save all precisions in a separate area
'svdx svdy' =: svxy =: 3!:1&.> dx;dy
NB. Get result with all looping performed in rank conjunction
r =: dx u"u"u dy
NB. For each precision combination, verify identical result
assert. (8;xyzs;allopred) checkallosize 7!:2 'tr =: dx u dy'
assert. r -: tr [ 90 [ 'tr =: dx u dy'
NB. Verify nothing modified in place
assert. svxy -: 3!:1&.> dx;dy
NB. Make x inplaceable.  Verify correct result
tx =: 3!:2 (3!:1) dx
assert. (17;xyzs;allopred) checkallosize 7!:2 'tx =: tx u dy'
assert. r -: tx [ 91 [ 'tx =: tx u dy'
NB. Verify originals not modified in place
assert. svxy -: 3!:1&.> dx;dy
NB. Make y inplaceable.  Verify correct result
ty =: 3!:2 (3!:1) dy
assert. (26;xyzs;allopred) checkallosize 7!:2 'ty =: dx u ty'
assert. r -: ty [ 92 [ 'ty =: dx u ty'
NB. Verify originals not modified in place
assert. svxy -: 3!:1&.> dx;dy
end.

1
)

NB. u is verb, y is (result precisions to test, string incl BDI),(buffer-allocation predictions)
NB. We try the verb on different ranks and shapes
testinplacer =: 1 : 0
'resultprec predr' =: y
NB. Test for validity.  Checks for inplacing too much
NB. Choose a random shape, then try all combinations of prefixes.
if. 'V' e. resultprec do.
  (u testinplace (resultprec;''))"1 ,"0/~ a: , <\ >: ? 4 # 10
  NB. Repeat for other ranks of u.  Also create a random cell-shape and use a prefix of it for each argument
  (u"0 0 testinplace (resultprec;''))"1 (0 0) (] ,&.> [ {.&.> [: < 10 ?@$~ >./@[)"1   ,"0/~ a: , <\ >: ? 2 # 10
  (u"0 1 testinplace (resultprec;''))"1 (0 1) (] ,&.> [ {.&.> [: < 10 ?@$~ >./@[)"1   ,"0/~ a: , <\ >: ? 2 # 10
  (u"1 0 testinplace (resultprec;''))"1 (1 0) (] ,&.> [ {.&.> [: < 10 ?@$~ >./@[)"1   ,"0/~ a: , <\ >: ? 2 # 10
  (u"1 1 testinplace (resultprec;''))"1 (1 1) (] ,&.> [ {.&.> [: < 10 ?@$~ >./@[)"1   ,"0/~ a: , <\ >: ? 3 # 10
  (u"0 2 testinplace (resultprec;''))"1 (0 2) (] ,&.> [ {.&.> [: < 10 ?@$~ >./@[)"1   ,"0/~ a: , <\ >: ? 3 # 10
  (u"1 2 testinplace (resultprec;''))"1 (1 2) (] ,&.> [ {.&.> [: < 10 ?@$~ >./@[)"1   ,"0/~ a: , <\ >: ? 3 # 10
  (u"2 2 testinplace (resultprec;''))"1 (2 2) (] ,&.> [ {.&.> [: < 10 ?@$~ >./@[)"1   ,"0/~ a: , <\ >: ? 2 # 10
  (u"2 1 testinplace (resultprec;''))"1 (2 1) (] ,&.> [ {.&.> [: < 10 ?@$~ >./@[)"1   ,"0/~ a: , <\ >: ? 2 # 10
  (u"2 0 testinplace (resultprec;''))"1 (2 0) (] ,&.> [ {.&.> [: < 10 ?@$~ >./@[)"1   ,"0/~ a: , <\ >: ? 2 # 10
  (u"0 3 testinplace (resultprec;''))"1 (0 3) (] ,&.> [ {.&.> [: < 10 ?@$~ >./@[)"1   ,"0/~ a: , <\ >: ? 2 # 10
  (u"1 3 testinplace (resultprec;''))"1 (1 3) (] ,&.> [ {.&.> [: < 10 ?@$~ >./@[)"1   ,"0/~ a: , <\ >: ? 2 # 10
  (u"2 3 testinplace (resultprec;''))"1 (2 3) (] ,&.> [ {.&.> [: < 10 ?@$~ >./@[)"1   ,"0/~ a: , <\ >: ? 2 # 10
  (u"3 3 testinplace (resultprec;''))"1 (3 3) (] ,&.> [ {.&.> [: < 10 ?@$~ >./@[)"1   ,"0/~ a: , <\ >: ? 2 # 10
  (u"3 2 testinplace (resultprec;''))"1 (3 2) (] ,&.> [ {.&.> [: < 10 ?@$~ >./@[)"1   ,"0/~ a: , <\ >: ? 2 # 10
  (u"3 1 testinplace (resultprec;''))"1 (3 1) (] ,&.> [ {.&.> [: < 10 ?@$~ >./@[)"1   ,"0/~ a: , <\ >: ? 2 # 10
  (u"3 0 testinplace (resultprec;''))"1 (3 0) (] ,&.> [ {.&.> [: < 10 ?@$~ >./@[)"1   ,"0/~ a: , <\ >: ? 2 # 10
end.

NB. Verify size of large operands.  Checks for inplacing too little
u testinplace (resultprec;(predr -. 'b'))"1 (16000;16000)  NB. 'b' means inplace only if rank not specified
u"1 testinplace (resultprec;predr)"1 (10 1600;10 1600)
)
NB. Full test with everything allowed
9!:53 (2)

+. testinplacer 'VBID';'B/lI/L/rI/I/L/R/R/D/    b/lI/L/rI/I/L/R/R//   b/lI/L/rI/I/L/R/R//'
*. testinplacer 'VBID';'B/lI/L/rI/I/L/R/R/D/    b/lI/L/rI/I/L/R/R//   b/lI/L/rI/I/L/R/R//'

27 b. testinplacer 'VBI';'lr/l/lr/r/I/r/lr/l/lr/    lr/l/lr/r//r/lr/l/lr/   lr/l/lr/r//r/lr/l/lr/'

! testinplacer 'VBID';'B/LRI/L/LRI/LRI/L/R/R/D/    b/LRI/L/LRI/LRI/L/R/R//   b/LRI/L/LRI/LRI/L/R/R//'

% testinplacer 'VBID';'D/D/D/D/D/D/D/D/D/    D/D/D/d/d/d////   D/d//D/d//D/d//'

* testinplacer 'VBID';'B/I/D/I/I/D/D/D/D/    b/I/D///d////   b///I///D/d//'  NB. non-overflow
* testinplacer 'VO';'B/I/D/I/Id/D/D/D/D/    b/I/D//d/d////   b///I/d//D/d//'  NB. overflow
NB. mulinasm * testinplacer 'VBID';'B/I/D/I/I/D/D/D/D/    b/I/D//I/d////   b///I/I//D/d//'  NB. non-overflow
NB. mulinasm * testinplacer 'VO';'B/I/D/I/ID/D/D/D/D/    b/I/D//ID/d////   b///I/ID//D/d//'  NB. overflow

+ testinplacer 'VBID';'I/I/D/I/I/D/D/D/D/    I/I/D///d////   I///I///D/d//'  NB. non-overflow
+ testinplacer 'VO';'I/Id/D/Id/Id/D/D/D/D/    I/Id/D/d/d/d////   I/d//Id/d//D/d//'   NB. overflow
- testinplacer 'VBID';'I/I/D/I/I/D/D/D/D/    I/I/D///d////   I///I///D/d//'
- testinplacer 'VO';'I/Id/D/Id/Id/D/D/D/D/    I/Id/D/d/d/d////   I/d//Id/d//D/d//'   NB. overflow

< testinplacer 'VBID';'B/B/B/B/B/B/B/B/B/    b/b/b/B/B/B/B/B/B/   b/B/B/b/B/B/b/B/B/'
= testinplacer 'VBID';'B/B/B/B/B/B/B/B/B/    b/b/b/B/B/B/B/B/B/   b/B/B/b/B/B/b/B/B/'
> testinplacer 'VBID';'B/B/B/B/B/B/B/B/B/    b/b/b/B/B/B/B/B/B/   b/B/B/b/B/B/b/B/B/'
<: testinplacer 'VBID';'B/B/B/B/B/B/B/B/B/    b/b/b/B/B/B/B/B/B/   b/B/B/b/B/B/b/B/B/'
>: testinplacer 'VBID';'B/B/B/B/B/B/B/B/B/    b/b/b/B/B/B/B/B/B/   b/B/B/b/B/B/b/B/B/'
~: testinplacer 'VBID';'B/B/B/B/B/B/B/B/B/    b/b/b/B/B/B/B/B/B/   b/B/B/b/B/B/b/B/B/'

+: testinplacer 'VB';'B/qb/qb/kb/kqb/kqb/kb/kqb/kqb/    b/qb/qb/kb/kqb/kqb/kb/kqb/kqb/   b/qb/qb/kb/kqb/kqb/kb/kqb/kqb/'
*: testinplacer 'VB';'B/qb/qb/kb/kqb/kqb/kb/kqb/kqb/    b/qb/qb/kb/kqb/kqb/kb/kqb/kqb/   b/qb/qb/kb/kqb/kqb/kb/kqb/kqb/'

NB. INT ^ INT produces slightly different values than INT ^ FL on 902753 ^ 39  which is close to the IEEE limit.  So don't check those values
^ testinplacer 'VBD';'B/D/D/I/D/D/D/D/D/    b/D/D/I/D/D/D/D/D/   b/D/D/I/D/D/D/D/D/'

NB. 0|0 allocates an extra FL output buffer.  We ensure that we go through this code
| testinplacer 'VBID';'B/l/L/r/I/ID/R/R/D/    b/l/L/r//ID/R/R//   b/l/L/r//ID/R/R//'  NB. I | D fails, then runs in-place

<. testinplacer 'VBID';'B/I/D/I/I/D/D/D/D/    b/I/D///d////   b///I///D/d//'
>. testinplacer 'VBID';'B/I/D/I/I/D/D/D/D/    b/I/D///d////   b///I///D/d//'

NB. Go back and recheck with partial execution not allowed.  No need for checking overlap again, or binary, or overflow
9!:53 (1)
+. testinplacer 'ID';'B/lI/L/rI/I/L/R/R/D/    b/lI/L/rI/I/L/R/R/D/   b/lI/L/rI/I/L/R/R/D/'
*. testinplacer 'ID';'B/lI/L/rI/I/L/R/R/D/    b/lI/L/rI/I/L/R/R/D/   b/lI/L/rI/I/L/R/R/D/'

27 b. testinplacer 'I';'lr/l/lr/r/I/r/lr/l/lr/    lr/l/lr/r//r/lr/l/lr/   lr/l/lr/r//r/lr/l/lr/'

! testinplacer 'ID';'B/LRI/L/LRI/LRI/L/R/R/D/    b/LRI/L/LRI/LRI/L/R/R//   b/LRI/L/LRI/LRI/L/R/R//'

% testinplacer 'ID';'D/D/D/D/D/D/D/D/D/    D/D/D/d/d/d///D/   D/d//D/d//D/d/D/'

* testinplacer 'ID';'B/I/D/I/I/D/D/D/D/    b/I/D///d////   b///I///D/d//'
NB. mulinasm * testinplacer 'ID';'B/I/D/I/I/D/D/D/D/    b/I/D//I/d////   b///I/I//D/d//'

+ testinplacer 'ID';'I/I/D/I/I/D/D/D/D/    I/I/D///d///D/   I///I///D/d/D/'
- testinplacer 'ID';'I/I/D/I/I/D/D/D/D/    I/I/D///d///D/   I///I///D/d/D/'

< testinplacer 'ID';'B/B/B/B/B/B/B/B/B/    b/b/b/B/B/B/B/B/B/   b/B/B/b/B/B/b/B/B/'
= testinplacer 'ID';'B/B/B/B/B/B/B/B/B/    b/b/b/B/B/B/B/B/B/   b/B/B/b/B/B/b/B/B/'
> testinplacer 'ID';'B/B/B/B/B/B/B/B/B/    b/b/b/B/B/B/B/B/B/   b/B/B/b/B/B/b/B/B/'
<: testinplacer 'ID';'B/B/B/B/B/B/B/B/B/    b/b/b/B/B/B/B/B/B/   b/B/B/b/B/B/b/B/B/'
>: testinplacer 'ID';'B/B/B/B/B/B/B/B/B/    b/b/b/B/B/B/B/B/B/   b/B/B/b/B/B/b/B/B/'
~: testinplacer 'ID';'B/B/B/B/B/B/B/B/B/    b/b/b/B/B/B/B/B/B/   b/B/B/b/B/B/b/B/B/'

NB. INT ^ INT produces slightly different values than INT ^ FL on 902753 ^ 39  which is close to the IEEE limit.  So don't check those values
^ testinplacer 'D';'B/D/D/I/D/D/D/D/D/    b/D/D/I/D/D/D/D/D/   b/D/D/I/D/D/D/D/D/'

NB. 0|0 allocates an extra FL output buffer.  We ensure that we go through this code
| testinplacer 'ID';'B/l/L/r/I/ID/R/R/D/    b/l/L/r//ID/R/R/D/   b/l/L/r//ID/R/R/D/'  NB. I | D fails, then runs in-place

<. testinplacer 'ID';'B/I/D/I/I/D/D/D/D/    b/I/D///d////   b///I///D/d//'
>. testinplacer 'ID';'B/I/D/I/I/D/D/D/D/    b/I/D///d////   b///I///D/d//'

9!:53 (1)   NB. Reset the default

4!:55 ;:'adot1 adot2 sdot0 allobytes allopred atomct atomsz bx by bytesused checkallosize dx dy ix iy pred'
4!:55 ;:'predflds predr r resultprec resvalidity sel shapes sn spred svbx svby svdx svdy'
4!:55 ;:'svix sviy svxy testinplace testinplacer tr tx ty vb xs xyzs ys IMIN IMAX'


