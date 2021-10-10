
.data

title1 :
  .asciz "*****Print Name*****\n"
team :
  .asciz "Team 52\n"
member1 :
  .asciz "Uxiang Hong\n"
member2 :
  .asciz "Chen Shen\n"
member3 :
  .asciz "Angela Cheng\n"
end1 :
  .asciz "*****End Print*****\n"

.text
.global NAME


NAME :
  stmfd sp!, {r4-r12, lr}
  mov   r4, r0
  mov   r5, r1
  mov   r6, r2
  mov   r7, r3
  ldr   r0, =title1
  bl    printf

  ldr   r0, =team
  bl    printf
  mov   r0, r4
  ldr   r1, =team
  bl    strcpy


  ldr   r0, =member1
  bl    printf
  mov   r0, r5
  ldr   r1, =member1
  bl    strcpy

  ldr   r0, =member2
  bl    printf
  mov   r0, r6
  ldr   r1, =member2
  bl    strcpy

  ldr   r0, =member3
  bl    printf
  mov   r0, r7
  ldr   r1, =member3
  bl    strcpy

  ldr   r0, =end1
  bl    printf

  sbcs  r0, r3, r4
  mov r0, #0
  ldmfd sp!, {r4-r12,pc}

.end

