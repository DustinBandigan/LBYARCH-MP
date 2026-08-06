; imgCvtGrayIntToDouble.asm
;
; System V AMD64 calling convention:
;
; RDI = address of input integer array
; RSI = address of output double array
; RDX = number of pixels
;
; C function declaration:
;
; void imgCvtGrayIntToDouble(
;     const int *input,
;     double *output,
;     size_t count
; );

default rel

section .rodata
    divisor: dq 255.0

section .text

global imgCvtGrayIntToDouble

imgCvtGrayIntToDouble:
    ; Check whether the pixel count is zero
    test rdx, rdx
    jz .done

    ; Load 255.0 into scalar SIMD register XMM1
    movsd xmm1, [divisor]

.loop:
    ; Load one 32-bit integer pixel
    mov eax, [rdi]

    ; Convert integer pixel to scalar double-precision float
    cvtsi2sd xmm0, eax

    ; Divide the converted value by 255.0
    divsd xmm0, xmm1

    ; Store the resulting double value
    movsd [rsi], xmm0

    ; Move to the next integer
    add rdi, 4

    ; Move to the next double
    add rsi, 8

    ; Decrease remaining pixel count
    dec rdx

    ; Continue while pixels remain
    jnz .loop

.done:
    ret
