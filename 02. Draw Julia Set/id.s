.data

Enter :
  .asciz "\n"
title2 :
  .asciz "*****Input ID*****\n"
sum :
  .word 0
id1 :
  .word 0
id2 :
  .word 0
id3 :
  .word 0
inputID :
  .asciz "%d"
str1 :
  .asciz "** Please Enter Member 1 ID:**\n"
str2 :
  .asciz "** Please Enter Member 2 ID:**\n"
str3 :
  .asciz "** Please Enter Member 3 ID:**\n"
str4 :
  .asciz "** Please Enter Command **\n"
cmd :
  .asciz " "
cmds :
  .asciz "%s"
commandP :
  .byte 'p'
info1 :
  .asciz "*****Print Team Member ID and ID Summation*****\n"
info2 :
  .asciz "ID Summation = "
end2 :
  .asciz "\n*****End Print*****\n"

.text
.global ID

ID :
  stmfd sp!, {r4-r12, lr}
  mov   r5, r0
  mov   r6, r1
  mov   r7, r2
  mov   r8, r3
  ldr   r0, =title2
  bl    printf

  ldr   r0, =str1
  bl    printf
  ldr   r0, =inputID
  ldr   r1, =id1
  bl    scanf

  ldr   r0, =str2
  bl    printf
  ldr   r0, =inputID
  ldr   r1, =id2
  bl    scanf

  ldr   r0, =str3
  bl    printf
  ldr   r0, =inputID
  ldr   r1, =id3
  bl    scanf

  ldr   r0, =str4
  bl    printf

  ldr   r0, =cmds
  ldr   r1, =cmd
  bl    scanf
  ldr   r0, =commandP
  ldr   r1, =cmd
  ldrb  r0, [r0]
  ldrb  r2, [r1]
  cmp   r2, r0

  ldreq r0, =info1
  bleq  printf

  ldr   r0, =inputID
  ldr   r1, =id1
  ldreq r1,[r1]
  str   r1, [r5]
  ldrne r1,[r1]
  moveq r9, r1
  moveq r9, r1, lsl #0
  moveq r9, r1, lsr #0
  bleq  printf
  ldreq r0, =Enter
  bleq  printf

  ldr   r0, =inputID
  ldr   r1, =id2
  ldreq r1,[r1]
  str   r1, [r6]
  moveq r10, r1
  bleq  printf
  ldreq r0, =Enter
  bleq  printf

  ldr   r0, =inputID
  ldr   r1, =id3
  ldreq r1,[r1]
  str   r1, [r7]
  moveq r11, r1
  bleq  printf
  ldreq r0, =Enter
  bleq  printf
  ldreq r0, =Enter
  bleq  printf

  ldreq r0, =id1
  ldreq r0, [r0]
  ldreq r1, =id2
  ldreq r1, [r1], r1
  ldreq r2, =id3
  ldreq r2, [r2], #2
  mov   r3, #0

  addvc r3, r0, r1
  addvc r3, r3, r2
  ldr   r4, =sum
  str   r3, [r4]

  ldreq r0, =info2
  bleq  printf
  ldreq r0, =inputID
  ldr   r1, =sum
  moveq r4, r1
  ldreq r4,[r4]
  str   r4, [r8]
  ldreq r1,[r1]
  bleq  printf
  ldreq r0, =end2
  bleq  printf

  mov   r0, r5
  mov   r1, r6
  mov   r2, r7
  mov   r3, r8
  ldmfd sp!, {r4-r12, pc}

.end

