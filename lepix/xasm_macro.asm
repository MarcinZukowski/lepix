* ADD - addition without carry
add .macro ' '

 .if :0<>1
  .error 'Unexpected eol'
 .endif

 clc
 adc :1

 .endm


* SUB - subtraction
sub .macro ' '

 .if :0<>1
  .error 'Unexpected eol'
 .endif

 sec
 sbc :1

 .endm


* INW - increment word 
inw .macro ' '

 .if :0<>1
  .error 'Unexpected eol'
 .endif

 inc :1
 bne __inw
 inc :1+1
 
__inw

 .endm


* MVA, MVX, MVY - move byte using accumulator, X or Y
mv? .macro ' '

 .if :4<>2
  .error 'Unexpected eol'
 .endif

 ld:3 :1
 st:3 :2

 .endm

* MVA - move byte using accumulator
mva .macro ' '
 mv? :1 :2 a :0
 .endm

* MVX - move byte using register X
mvx .macro ' '
 mv? :1 :2 x :0
 .endm

* MVY - move byte using register Y
mvy .macro ' '
 mv? :1 :2 y :0
 .endm


* MWA, MWX, MWY - move word using accumulator, X or Y 
mw? .macro ' '

  .if :3<>' '||:0<>5
    .error 'Illegal addresing mode'
  .endif

  .if :1='#'

    .if :2=0
      ld:5 #0
      st:5 :4
      st:5 :4+1
      .exit
    .endif

    .if :2=1
      .if 'a'<>':5'&&'A'<>':5'
        ld:5 #:2
        st:5 :4
        de:5
        st:5 :4+1
      .else 
        lda #:2
        sta :4
        lda #0
        sta :4+1
      .endif

     .exit
    .endif

    .if :2<256&&:2>0
      ld:5 #:2
      st:5 :4
      ld:5 #0
      st:5 :4+1
    .else
      ld:5 <:2
      st:5 :4
      ld:5 >:2
      st:5 :4+1
    .endif

  .else

    ld:5 :2
    st:5 :4
    ld:5 :2+1
    st:5 :4+1

  .endif
 .endm

* MWA - move word using accumulator
mwa .macro " "
 mw? :1 :2 :3 :4 a
 .endm

* MWX - move word using register X
mwx .macro " "
 mw? :1 :2 :3 :4 x
 .endm

* MWY - move word using register X
mwy .macro " "
 mw? :1 :2 :3 :4 y
 .endm


* ADD:STA
add?sta .macro " "

 .if :1<>' '
   .error 'Illegal adressing mode'
 .endif

 add :2
 sta :2  

 .endm


* SCC:INC
scc?inc .macro " "

 .if :1<>' '
   .error 'Illegal adressing mode'
 .endif

 bcc __scc?inc
 inc :2
__scc?inc

  .endm


* SCC:INY
scc?iny .macro " "

 bcc __scc?iny
 iny
__scc?iny

 .endm


* ___?RNE
___?rne .macro ' '

 .if :1<>' '
   .error 'Illegal adressing mode'
 .endif

___?rne
 :3 :2
 bne ___?rne

 .endm

* INC:RNE
inc?rne .macro " "
 ___?rne :1 :2 inc
 .endm

* DEC:RNE
dec?rne .macro " "
 ___?rne :1 :2 dec
 .endm

* STA:RNE
sta?rne .macro " "
 ___?rne :1 :2 sta
 .endm


* LDA:TAX
lda?tax .macro ' '

 lda :1
 tax

 .endm


* LDA:TAY
lda?tay .macro ' '

 lda :1
 tay

 .endm


* SEQ:ADC
seq?adc .macro ' '

 beq __seq?adc
 adc :1
__seq?adc

 .endm


* TYA:STA
tya?sta .macro ' '

 tya
 sta :1

 .endm


* LDY:INC
ldy?inc .macro ' '

 ldy :1
 inc :1

 .endm


* MVA:RPL
mva?rpl .macro ' '

__mva?rpl
 lda :1
 sta :2
 bpl __mva?rpl

 .endm


* LDA:PHA
lda?pha .macro ' '

 lda :1
 pha

 .endm


* TYA:PHA
tya?pha .macro ' '

 tya
 pha

 .endm


* PLA:TAY
pla?tay .macro ' '

 pla
 tay

 .endm
