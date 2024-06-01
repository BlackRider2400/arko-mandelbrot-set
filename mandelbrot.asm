; Arguments


	section .text
	global mandelbrotgen

mandelbrotgen:
    
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    push r13
    push r14
    push r15

preparation:

    mov r9, rsi
    ;; zapamietujemy w r9 szerokosc
    mov r10, rdx
    ;; zapamietujemy w r10 wysokosc
    ;;rcx max_it

    dec r9;
    dec r10;

    mov r11, 4

    cvtsi2sd xmm0, r11
    
    movsd xmm3, xmm0
    cvtsi2sd xmm7, r9
    
    divsd xmm3, xmm7 ;;real_factor

    movsd xmm4, xmm0
    cvtsi2sd xmm7, r10 
    
    divsd xmm4, xmm7 ;;imag_factor
    
    mov r11, -2
    cvtsi2sd xmm7, r11

    inc r10 ;;height
    inc r9 ;;width
    
    mov r15, 0 ;;y

height_loop:
    cmp r15, r10
    jge end

    cvtsi2sd xmm15, r15
    movsd xmm2, xmm15
    mulsd xmm2, xmm4
    addsd xmm2, xmm7 ;;imag

    mov r14, 0 ;;x

width_loop:
    cmp r14, r9
    jge increment_height

    cvtsi2sd xmm14, r14
    movsd xmm1, xmm14
    mulsd xmm1, xmm3
    addsd xmm1, xmm7 ;;real

    mov r13, 0 ;;it
    pxor xmm13, xmm13 ;;zr
    pxor xmm12, xmm12 ;;zi
pixel_loop:
    cmp r13, rcx
    jge draw

    movsd xmm8, xmm13 ;;zr2
    movsd xmm9, xmm12 ;;zi2
    mulsd xmm8, xmm8
    mulsd xmm9, xmm9

    movsd xmm10, xmm9
    addsd xmm10, xmm8 ;;zr2 + zi2

    comisd xmm0, xmm10
    jbe draw

    movsd xmm10, xmm8
    subsd xmm10, xmm9
    addsd xmm10, xmm1 ;;temp

    mulsd xmm12, xmm13  
    addsd xmm12, xmm12
    addsd xmm12, xmm2 ;;2*zr*zi + imag

    movsd xmm13, xmm10
    
    inc r13
    jmp pixel_loop

draw:

    mov r11, r15
    imul r11, r9
    add r11, r14
    imul r11, 3

    cmp rcx, r13
    je black_pixel

    imul r13, 255

    xor rdx, rdx
    mov rax, r13
    div rcx
    mov r13, rax

    mov byte [rdi + r11], r13b
    mov byte [rdi + r11 + 1], r13b
    mov byte [rdi + r11 + 2], r13b
    jmp increment
black_pixel:
    ;; same zera jak czarny
    mov byte [rdi + r11], 255
    mov byte [rdi + r11 + 1], 20
    mov byte [rdi + r11 + 2], 147

increment:
    inc r14
    jmp width_loop
increment_height:
    inc r15
    jmp height_loop

end:
    ; Epilogue
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret