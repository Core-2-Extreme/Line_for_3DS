    .global test
    .type test, "function"
    .global testg
    .type testg, "function"
    .global testr
    .type testr, "function"

test:
    mov r2, #0 
    mov r3, #0
    sub r0, #16
    sub r1, #128
    mov r2, #256
    add r2, #42
    mov r3, #256
    add r3, #260
    mul r4, r0, r2
    mul r5, r1, r3
    mov r0, #128
    add r0, r4, r5
    asr r0, #8
    cmp r0, #255
    movgt r0, #255;
    cmp r0, #0
    movlt r0, #0;
    asr r0, #3
    bx lr

testg:
    mov r3, #0 
    mov r4, #0
    mov r5, #0
    sub r0, #16
    sub r1, #128
    sub r2, #128    
    mov r3, #256
    add r3, #42
    mov r4, #100
    mov r5, #208
    mul r6, r0, r3
    mul r0, r1, r4
    mul r1, r2, r5
    mov r0, r6
    sub r6, r0;
    sub r6, r1;
    add r6, #128;
    mov r0, r6;
    asr r0, #8
    cmp r0, #255
    movgt r0, #255;
    cmp r0, #0
    movlt r0, #0;
    asr r0, #2
    bx lr

testr:
    mov r2, #0 
    mov r3, #0
    sub r0, #16
    sub r1, #128
    mov r2, #256
    add r2, #42
    mov r3, #256
    add r3, #153
    mul r4, r0, r2
    mul r5, r1, r3
    mov r0, #128
    add r0, r4, r5
    asr r0, #8
    cmp r0, #255
    movgt r0, #255;
    cmp r0, #0
    movlt r0, #0;
    asr r0, #3
    bx lr
