	.global	__aeabi_idiv
	.text
	.global	drawJuliaSet
drawJuliaSet:
	stmfd	sp!, {r4, lr}
	add	r4, sp, #4
	sub	sp, sp, #48
	str	r0, [r4, #-40] @cX
	str	r1, [r4, #-44] @cY
	str	r2, [r4, #-48] @width
	str	r3, [r4, #-52] @height
	mov	r3, #255
	str	r3, [r4, #-28] @maxIter
	mov	r3, #0  @ x = 0
	str	r3, [r4, #-20] @x
	b	Big_For_Judge
Big_For_Assign:
	mov	r3, #0
	str	r3, [r4, #-24] @y
	b	Small_For_Judge
Small_For_Working:
	ldr	r3, [r4, #-48]
	mov	r3, r3, asr #1
	ldr	r2, [r4, #-20]
	rsb	r3, r3, r2
	ldr	r2, computeNum 
	mul	r2, r2, r3
	ldr	r3, [r4, #-48]
	mov	r3, r3, asr #1
	mov	r0, r2
	mov	r1, r3
	bl	__aeabi_idiv
	mov	r3, r0
	str	r3, [r4, #-8] @zx

	ldr	r3, [r4, #-52]
	mov	r3, r3, asr #1 @height>>1
	ldr	r2, [r4, #-24]
	rsb	r3, r3, r2
	mov	r2, #1000
	mul	r2, r2, r3
	ldr	r3, [r4, #-52]
	mov	r3, r3, asr #1
	mov	r0, r2
	mov	r1, r3
	bl	__aeabi_idiv
	mov	r3, r0
	str	r3, [r4, #-12] @zy

	ldr	r3, [r4, #-28]
	str	r3, [r4, #-16] @i
	b	While_Judge
While:
	ldr	r3, [r4, #-8]
	mov	r2, r3
	mul	r2, r2, r3 @ zx * zx
	ldr	r3, [r4, #-12]
	mov	r1, r3
	mul	r3, r1, r3 @ zy * zy
	rsb	r3, r3, r2 @ zx * zx - zy * zy

	ldr	r2, computeNum+4
	smull	r1, r2, r2, r3
	mov	r2, r2, asr #6
	mov	r3, r3, asr #31
	rsb	r2, r3, r2

	ldr	r3, [r4, #-40] @cX
	add	r3, r2, r3
	str	r3, [r4, #-32] @tmp

	ldr	r3, [r4, #-8]
	mov	r3, r3, lsl #1 @ 2 * zx
	mov	r3, r3, lsr #0
	ldr	r2, [r4, #-12]
	mul	r3, r2, r3 @ 2 * zx * zy

	ldr	r2, computeNum+4
	smull	r1, r2, r2, r3
	mov	r2, r2, asr #6
	mov	r3, r3, asr #31
	rsb	r2, r3, r2

	ldr	r3, [r4, #-44] @ cY
	add	r3, r2, r3
	str	r3, [r4, #-12]

	ldr	r3, [r4, #-32]
	str	r3, [r4, #-8] @ zx = tmp

	ldr	r3, [r4, #-16]
	sub	r3, r3, #1 @i--
	str	r3, [r4, #-16]
While_Judge:
	ldr	r3, [r4, #-8]
	mov r2,r3
	mul	r2, r2, r3 @ zx * zx
	ldr	r3, [r4, #-12]
	mov	r1, r3
	mul	r3, r1, r3 @ zy * zy
	add	r2, r2, r3
	ldr	r3, computeNum+8 @3999999
	cmp	r2, r3
	bge	ColorCountrol @ zx * zx + zy * zy < 4000000 (>=4000000)
	ldr	r3, [r4, #-16]
	cmp	r3, #0
	bgt	While @ i > 0
ColorCountrol:
	ldr	r3, [r4, #-16] @i
	mov	r3, r3, asl #8
	uxth	r2, r3
	ldr	r3, [r4, #-16]
	orr	r3, r2, r3 @ ((i&0xff)<<8) | (i&0xff)
	strh	r3, [r4, #-34]
	ldrh	r3, [r4, #-34]
	mvn	r3, r3 @ r3 => -r3
	strh	r3, [r4, #-34]

	ldr	r2, [r4, #-24]  @y
	mov	r3, r2
	mov	r3, r3, asl #2
	add	r3, r3, r2
	mov	r3, r3, asl #8  @640*2
	ldr	r2, [r4, #4]
	add	r2, r2, r3
	ldr	r3, [r4, #-20]  @x
	mov	r3, r3, asl #1
	add	r3, r2, r3
	
	ldrh	r2, [r4, #-34] 
	strh	r2, [r3, #0]

	ldr	r3, [r4, #-24]
	add	r3, r3, #1 @ y++
	str	r3, [r4, #-24]
Small_For_Judge:
	ldr	r2, [r4, #-24]
	ldr	r3, [r4, #-52]
	cmp	r2, r3
	blt	Small_For_Working
	ldr	r3, [r4, #-20]
	add	r3, r3, #1
	str	r3, [r4, #-20]
Big_For_Judge:
	ldr	r2, [r4, #-20]
	ldr	r3, [r4, #-48]
	cmp	r2, r3
	blt	Big_For_Assign
	sub	sp, r4, #4
	adds r14, r0, r1
	ldmfd	sp!, {r4, pc}
computeNum:
	.word	1500
	.word	274877907
	.word	40000000



