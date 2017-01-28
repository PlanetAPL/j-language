NB. f/. f\  f\. models --------------------------------------------------

randuni''

en     =: #@]
em     =: (en >.@% -@[)`(en 0&>.@>:@- [) @. (0&<:@[)
kay    =: en`em @. (0&<@[)
omask  =: (em,en) $ ($&0@|@[ , $&1@kay)

base   =: 1&>.@-@[ * i.@em
iind   =: base ,. |@[ <. en - base
seg    =: ((+i.)/@[ { ])"1 _

infix  =: 1 : '(iind  x@seg ])"0 _'
outfix =: 1 : '(omask x@#   ])"0 _'
prefix =: 1 : '>:@,.@i.@# x@{. ]'
suffix =: 1 : ',.@i.@#    x@}. ]'

key    =: 1 : '=@[ x@# ]'

osub   =: >@]`(>@[ >@:{ ]) @. (*@#@])
oind   =: (+/&i./ </.&, i.)@(2&{.)@(,&1 1)@$
ob     =: 1 : 'oind x@osub"0 1 ,@(<"_2)'

bs     =: 1 : '(x prefix) : (x infix )'
bsd    =: 1 : '(x suffix) : (x outfix)'
sd     =: 1 : '(x ob    ) : (x key   )'


NB.  f/.y ---------------------------------------------------------------

NB. Boolean
a=:1=?10 5$2
(</.   -: < ob) a
(</.   -: < ob) ,a
(]/.   -: ] ob) a
(+.//. -: +./ ob) a
(</.   -: < ob) 1
(</.   -: < ob) 0$1

NB. literal
a=:a.{~32+?10 5 3$95
(</. -: < ob) a
(</. -: < ob) ,a
(]/. -: ] ob) a
(</. -: < ob) 'a'
(</. -: < ob) ''

NB. literal2
a=:adot1{~32+?10 5 3$95
(</. -: < ob) a
(</. -: < ob) ,a
(]/. -: ] ob) a
(</. -: < ob) u:'a'
(</. -: < ob) u:''

NB. literal4
a=:adot2{~32+?10 5 3$95
(</. -: < ob) a
(</. -: < ob) ,a
(]/. -: ] ob) a
(</. -: < ob) 10&u:'a'
(</. -: < ob) 10&u:''

NB. symbol
a=:sdot0{~32+?10 5 3$95
(</. -: < ob) a
(</. -: < ob) ,a
(]/. -: ] ob) a
(</. -: < ob) s:@<"0 'a'
(</. -: < ob) s:''

NB. integer
a=:?3 4 5 6$100
(</.  -: < ob) a
(</.  -: < ob) ,a
(]/.  -: ] ob) a
(+//. -: +/ ob) a
(</.  -: < ob) i.0 7
(</.  -: < ob) i.7 0 4
(</.  -: < ob) i.0 0 4 5
(</.  -: < ob) 34
(</.  -: < ob) 0$34

NB. floating point
a=:o._40+?10 5$100
(</.  -: < ob) a
(</.  -: < ob) ,a
(]/.  -: ] ob) a
(+//. -: +/ ob) a
(</.  -: < ob) _3.454
(</.  -: < ob) 0$_3.4

NB. complex
a=:^0j0.01*_400+?10 5$1000
(</.  -: < ob) a
(</.  -: < ob) ,a
(]/.  -: ] ob) a
(+//. -: +/ ob) a
(</.  -: < ob) _3j454
(</.  -: < ob) 0$_3j4

NB. boxed
t=:(1=?70$3)<;.1 ?70$100
a=:t{~?10 3$#t
(</. -: < ob) a
(</. -: < ob) ,a
(]/. -: ] ob) a
(</. -: < ob) <i.3 4
(</. -: < ob) 0$<'_3j4'

NB. 4 7 -: $ i.@7:/. i. 2 3    withdrawn
NB. 4 7 -: $ i.@7:/. i. 2 3 4
NB. 3 7 -: $ i.@7:/. i. 1 3
NB. 3 7 -: $ i.@7:/. i. 1 3 4
NB. 2 7 -: $ i.@7:/. i. 0 3
NB. 2 7 -: $ i.@7:/. i. 0 3 4
NB. 1 7 -: $ i.@7:/. i. 2 0
NB. 1 7 -: $ i.@7:/. i. 2 0 4
NB. 0 7 -: $ i.@7:/. i. 1 0
NB. 0 7 -: $ i.@7:/. i. 1 0 4
NB. 0 7 -: $ i.@7:/. i. 0 0
NB. 0 7 -: $ i.@7:/. i. 0 0 4

NB.  f/.y convolution ---------------------------------------------------

pru=: [: ^ 0j2p1&%               NB. principal n-th root of unity

conv=: +//.@(*/)                 NB. convolution

conv1=: 4 : 0
 n=. #x
 A=. (pru 2*n)^*/~i.2*n          NB. (%.A) = (2*n)%~r^-*/~i.2*n  
 }: x *&.(A&(+/ .*))&((2*n)&{.) y
)

pconv=: (| +/~@i.)@#@[ +//.&, */ NB. positive wrapped convolution

pconv1=: 4 : 0
 n=. #x
 A=. (*:pru 2*n)^*/~i.n          NB. (%.A) = n%~r^-*/~i.n
 x *&.(A&(+/ .*)) y
)

s=: ?10$100
t=: ?10$100
eq=: 1e_8&> @: (>./) @: | @: -
s ( conv eq  conv1) t
s (pconv eq pconv1) t


NB.  x f/. y ------------------------------------------------------------

NB. Boolean
a=:1=?11 5$2
k (</.   -: < key) a    [ k=:?11$4
k (]/.   -: ] key) a    [ k=:?11$4
k (+.//. -: +./ key) a  [ k=:?11$4

NB. literal
a=:a.{~32+?11$95
k (</. -: < key) a      [ k=:?11$4
k (]/. -: ] key) a      [ k=:?11$4

NB. literal2
a=:adot1{~32+?11$95
k (</. -: < key) a      [ k=:?11$4
k (]/. -: ] key) a      [ k=:?11$4

NB. literal2
a=:adot2{~32+?11$95
k (</. -: < key) a      [ k=:?11$4
k (]/. -: ] key) a      [ k=:?11$4

NB. symbol
a=:sdot0{~32+?11$95
k (</. -: < key) a      [ k=:?11$4
k (]/. -: ] key) a      [ k=:?11$4

NB. integer
a=:?11 5$110
k (</.  -: < key) a     [ k=:?11$4
k (]/.  -: ] key) a     [ k=:?11$4
k (+//. -: +/ key) a    [ k=:?11$4

NB. floating point
a=: (2^_12) * _4e5+?11$1e6
k (</.  -: < key) a     [ k=:?11$4
k (]/.  -: ] key) a     [ k=:?11$4
k (+//. -: +/ key) a    [ k=:?11$4

NB. complex
a=: j./_4e5+?2 11 5$1e6
k (</.  -: < key) a     [ k=:?11$4
k (]/.  -: ] key) a     [ k=:?11$4
k (+//. -: +/ key) a    [ k=:?11$4

NB. boxed
a=:x{~?11 3$#x=:(1=?70$3)<;.1 ?70$110
k (</. -: < key) a      [ k=:?11$4
k (]/. -: ] key) a      [ k=:?11$4

''        -: '' </. ''
''        -: '' </. i.0 4 5
''        -: (i.0 4 5) </. ''
''        -: (u:'') </. u:''
''        -: (u:'') </. i.0 4 5
''        -: (i.0 4 5) </. u:''
''        -: (10&u:'') </. 10&u:''
''        -: (10&u:'') </. i.0 4 5
''        -: (i.0 4 5) </. 10&u:''
''        -: (s:'') </. s:''
''        -: (s:'') </. i.0 4 5
''        -: (i.0 4 5) </. s:''
(,<,5)    -: 4 </. 5
(,<i.1 9) -: 4 </. i.1 9
(,<,4)    -: (i.1 9) </. 4
(,<,4)    -: (i.1 0) </. 4
(,<x )    -: (i.(#x),0) </. x=:'abcdefghij'
(,<x )    -: (i.(#x),0) </. x=:u:'abcdefghij'
(,<x )    -: (i.(#x),0) </. x=:10&u:'abcdefghij'
(,<x )    -: (i.(#x),0) </. x=:s:@<"0 'abcdefghij'


'length error' -: 'abc'  </. etx i.4
'length error' -: (u:'abc')  </. etx i.4
'length error' -: (10&u:'abc')  </. etx i.4
'length error' -: (s:@<"0 'abc')  </. etx i.4
'length error' -: 'abcd' </. etx i.3
'length error' -: (u:'abcd') </. etx i.3
'length error' -: (10&u:'abcd') </. etx i.3
'length error' -: (s:@<"0 'abcd') </. etx i.3
'length error' -: ''     </. etx i.4
'length error' -: 4      </. etx i.4
'length error' -: 'abcd' </. etx 4
'length error' -: (u:'abcd') </. etx 4
'length error' -: (10&u:'abcd') </. etx 4
'length error' -: (s:@<"0 'abcd') </. etx 4

(,<,1) -: 1 1 1 1 (<@~.)/.!.0 (1) + 1e_15 * i. 4
(4#<,1) -: (<@~.)/.!.0~ (1) + 1e_15 * i. 4
(,<,1) -: 1 1 1 1 (<@~.)/. (1) + 1e_15 * i. 4


NB. x f/.y on empty x ---------------------------------------------------

test=: 2 : 0
 n=: p: ?200
 xx=: n ?@$ >.-:n
 assert. xx             (u/. -: v/.) i.n,0
 assert. ((n,0)$0     ) (u/. -: v/.) i.n,0
 assert. ((n,0)$0     ) (u/. -: v/.) i.n
 assert. ((n,0)$''    ) (u/. -: v/.) i.n
 assert. ((n,0)$u: 0  ) (u/. -: v/.) i.n
 assert. ((n,0)$10&u: 0) (u/. -: v/.) i.n
 assert. ((n,0)$2     ) (u/. -: v/.) i.n
 assert. ((n,0)$2.5   ) (u/. -: v/.) i.n
 assert. ((n,0)$2j5   ) (u/. -: v/.) i.n
 assert. ((n,0)$2x    ) (u/. -: v/.) i.n
 assert. ((n,0)$2r5   ) (u/. -: v/.) i.n
 assert. ((n,0)$a:    ) (u/. -: v/.) i.n
 assert. ((n,0)$s:<'x') (u/. -: v/.) i.n
 assert. ((n,0)$s:<u: 128+a.i. 'x') (u/. -: v/.) i.n
 assert. ((n,0)$s:<10&u: 65536+a.i. 'x') (u/. -: v/.) i.n
 1
)

<       test (3 : '<y')
#       test (3 : '#y')
+ /     test (3 : '+ /y')
>./     test (3 : '>./y')
({.,# ) test (3 : '({.,# )y')
(# ,{.) test (3 : '(# ,{.)y')


4!:55 ;:'a adot1 adot2 sdot0 base bs bsd conv conv1 em en eq iind infix k '
4!:55 ;:'kay key n ob oind omask osub outfix pconv pconv1 prefix pru '
4!:55 ;:'s sd seg suffix t test x xx'


