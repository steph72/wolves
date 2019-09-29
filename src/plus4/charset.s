.export _copychars

.code
.importzp ptr1, ptr2

tedctl1 = $ff12
tedctl2 = $ff13

romsel = $ff3e
ramsel = $ff3f

csize     = $800
ramchars  = $f000
romchars  = $d000

_copychars:

; save context to stack

    pha
    txa
    pha
    tya
    pha

; setup pointers for rom charset copy

    ldx #<romchars
    ldy #>romchars
    stx ptr1
    sty ptr1+1
    ldx #<ramchars
    ldy #>ramchars
    stx ptr2
    sty ptr2+1
    ldx #<csize
    ldy #>csize
    stx sizel
    sty sizeh

    sta romsel
    jsr moveup
    sta ramsel

    lda tedctl1
    and %11111011
    sta tedctl1
    lda #>ramchars  ; activate custom charset
    sta tedctl2

; restore context from stack

    pla
    txa
    pla
    tya
    pla
    rts

moveup:  LDX sizeh    ; the last byte must be moved first
         CLC          ; start at the final pages of FROM and TO
         TXA
         ADC ptr1+1
         STA ptr1+1
         CLC
         TXA
         ADC ptr2+1
         STA ptr2+1
         INX          ; allows the use of BNE after the DEX below
         LDY sizel
         BEQ MU3
         DEY          ; move bytes on the last page first
         BEQ MU2
MU1:     LDA (ptr1),Y
         STA (ptr2),Y
         DEY
         BNE MU1
MU2:     LDA (ptr1),Y ; handle Y = 0 separately
         STA (ptr2),Y
MU3:     DEY
         DEC ptr1+1   ; move the next page (if any)
         DEC ptr2+1
         DEX
         BNE MU1
         RTS

sizel:      .byte 0
sizeh:      .byte 0  
