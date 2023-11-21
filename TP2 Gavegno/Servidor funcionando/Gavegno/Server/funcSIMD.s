	.arch armv7-a
	.eabi_attribute 28, 1
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 6
	.eabi_attribute 34, 1
	.eabi_attribute 18, 4
	.file	"funcSIMD.c"
	.text
	.section	.rodata
	.align	2
.LC0:
	.ascii	"\012*****************************************\000"
	.align	2
.LC1:
	.ascii	"Valor de Acelerometro X en servidor: %.02fg\012\000"
	.align	2
.LC2:
	.ascii	"Valor de Acelerometro Y en servidor: %.02fg\012\000"
	.align	2
.LC3:
	.ascii	"Valor de Acelerometro Z en servidor: %.02fg\012\000"
	.align	2
.LC4:
	.ascii	"Valor de Temperatura en servidor:    %.02f\302\260\012"
	.ascii	"\000"
	.align	2
.LC5:
	.ascii	"Valor de Gyroscopio X en servidor:   %.02f\302\260/"
	.ascii	"seg\012\000"
	.align	2
.LC6:
	.ascii	"Valor de Gyroscopio Y en servidor:   %.02f\302\260/"
	.ascii	"seg\012\000"
	.align	2
.LC7:
	.ascii	"Valor de Gyroscopio Z en servidor:   %.02f\302\260/"
	.ascii	"seg\012\000"
	.align	2
.LC8:
	.ascii	"******************************************\012\000"
	.text
	.align	1
	.global	PromedioSIMD
	.arch armv7-a
	.syntax unified
	.thumb
	.thumb_func
	.fpu vfpv3-d16
	.type	PromedioSIMD, %function
PromedioSIMD:
	@ args = 0, pretend = 0, frame = 88
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r4, r5, r7, lr}
	sub	sp, sp, #88
	add	r7, sp, #0
	str	r0, [r7, #12]
	str	r1, [r7, #8]
	str	r2, [r7, #4]
	movs	r3, #0
	str	r3, [r7, #84]
	mov	r3, #0
	str	r3, [r7, #80]	@ float
	mov	r3, #0
	str	r3, [r7, #76]	@ float
	mov	r3, #0
	str	r3, [r7, #72]	@ float
	mov	r3, #0
	str	r3, [r7, #68]	@ float
	mov	r3, #0
	str	r3, [r7, #64]	@ float
	mov	r3, #0
	str	r3, [r7, #60]	@ float
	mov	r3, #0
	str	r3, [r7, #56]	@ float
	movs	r3, #0
	strh	r3, [r7, #54]	@ movhi
	movs	r3, #0
	strh	r3, [r7, #52]	@ movhi
	movs	r3, #0
	strh	r3, [r7, #50]	@ movhi
	b	.L2
.L3:
	ldr	r3, [r7, #84]
	ldr	r2, [r7, #8]
	add	r3, r3, r2
	ldrb	r3, [r3]	@ zero_extendqisi2
	lsls	r3, r3, #8
	sxth	r2, r3
	ldr	r3, [r7, #84]
	adds	r3, r3, #1
	ldr	r1, [r7, #8]
	add	r3, r3, r1
	ldrb	r3, [r3]	@ zero_extendqisi2
	sxth	r3, r3
	orrs	r3, r3, r2
	strh	r3, [r7, #52]	@ movhi
	ldrsh	r3, [r7, #52]
	vmov	s15, r3	@ int
	vcvt.f32.s32	s15, s15
	vadd.f32	s14, s15, s15
	vldr.32	s13, .L5+8
	vdiv.f32	s15, s14, s13
	vldr.32	s14, [r7, #80]
	vadd.f32	s15, s14, s15
	vstr.32	s15, [r7, #80]
	ldr	r3, [r7, #84]
	adds	r3, r3, #2
	ldr	r2, [r7, #8]
	add	r3, r3, r2
	ldrb	r3, [r3]	@ zero_extendqisi2
	lsls	r3, r3, #8
	sxth	r2, r3
	ldr	r3, [r7, #84]
	adds	r3, r3, #3
	ldr	r1, [r7, #8]
	add	r3, r3, r1
	ldrb	r3, [r3]	@ zero_extendqisi2
	sxth	r3, r3
	orrs	r3, r3, r2
	strh	r3, [r7, #52]	@ movhi
	ldrsh	r3, [r7, #52]
	vmov	s15, r3	@ int
	vcvt.f32.s32	s15, s15
	vadd.f32	s14, s15, s15
	vldr.32	s13, .L5+8
	vdiv.f32	s15, s14, s13
	vldr.32	s14, [r7, #76]
	vadd.f32	s15, s14, s15
	vstr.32	s15, [r7, #76]
	ldr	r3, [r7, #84]
	adds	r3, r3, #4
	ldr	r2, [r7, #8]
	add	r3, r3, r2
	ldrb	r3, [r3]	@ zero_extendqisi2
	lsls	r3, r3, #8
	sxth	r2, r3
	ldr	r3, [r7, #84]
	adds	r3, r3, #5
	ldr	r1, [r7, #8]
	add	r3, r3, r1
	ldrb	r3, [r3]	@ zero_extendqisi2
	sxth	r3, r3
	orrs	r3, r3, r2
	strh	r3, [r7, #52]	@ movhi
	ldrsh	r3, [r7, #52]
	vmov	s15, r3	@ int
	vcvt.f32.s32	s15, s15
	vadd.f32	s14, s15, s15
	vldr.32	s13, .L5+8
	vdiv.f32	s15, s14, s13
	vldr.32	s14, [r7, #72]
	vadd.f32	s15, s14, s15
	vstr.32	s15, [r7, #72]
	ldr	r3, [r7, #84]
	adds	r3, r3, #6
	ldr	r2, [r7, #8]
	add	r3, r3, r2
	ldrb	r3, [r3]	@ zero_extendqisi2
	lsls	r3, r3, #8
	sxth	r2, r3
	ldr	r3, [r7, #84]
	adds	r3, r3, #7
	ldr	r1, [r7, #8]
	add	r3, r3, r1
	ldrb	r3, [r3]	@ zero_extendqisi2
	sxth	r3, r3
	orrs	r3, r3, r2
	strh	r3, [r7, #54]	@ movhi
	vldr.32	s15, [r7, #56]
	vcvt.f64.f32	d6, s15
	ldrsh	r3, [r7, #54]
	vmov	s15, r3	@ int
	vcvt.f32.s32	s14, s15
	vldr.32	s11, .L5+12
	vdiv.f32	s15, s14, s11
	vcvt.f64.f32	d7, s15
	vldr.64	d5, .L5
	vadd.f64	d7, d7, d5
	vadd.f64	d7, d6, d7
	vcvt.f32.f64	s15, d7
	vstr.32	s15, [r7, #56]
	ldr	r3, [r7, #84]
	adds	r3, r3, #8
	ldr	r2, [r7, #8]
	add	r3, r3, r2
	ldrb	r3, [r3]	@ zero_extendqisi2
	lsls	r3, r3, #8
	sxth	r2, r3
	ldr	r3, [r7, #84]
	adds	r3, r3, #9
	ldr	r1, [r7, #8]
	add	r3, r3, r1
	ldrb	r3, [r3]	@ zero_extendqisi2
	sxth	r3, r3
	orrs	r3, r3, r2
	strh	r3, [r7, #50]	@ movhi
	ldrsh	r3, [r7, #50]
	vmov	s15, r3	@ int
	vcvt.f32.s32	s15, s15
	vldr.32	s14, .L5+16
	vmul.f32	s14, s15, s14
	vldr.32	s13, .L5+8
	vdiv.f32	s15, s14, s13
	vldr.32	s14, [r7, #68]
	vadd.f32	s15, s14, s15
	vstr.32	s15, [r7, #68]
	ldr	r3, [r7, #84]
	adds	r3, r3, #10
	ldr	r2, [r7, #8]
	add	r3, r3, r2
	ldrb	r3, [r3]	@ zero_extendqisi2
	lsls	r3, r3, #8
	sxth	r2, r3
	ldr	r3, [r7, #84]
	adds	r3, r3, #11
	ldr	r1, [r7, #8]
	add	r3, r3, r1
	ldrb	r3, [r3]	@ zero_extendqisi2
	sxth	r3, r3
	orrs	r3, r3, r2
	strh	r3, [r7, #50]	@ movhi
	ldrsh	r3, [r7, #50]
	vmov	s15, r3	@ int
	vcvt.f32.s32	s15, s15
	vldr.32	s14, .L5+16
	vmul.f32	s14, s15, s14
	vldr.32	s13, .L5+8
	vdiv.f32	s15, s14, s13
	vldr.32	s14, [r7, #64]
	vadd.f32	s15, s14, s15
	vstr.32	s15, [r7, #64]
	ldr	r3, [r7, #84]
	adds	r3, r3, #12
	ldr	r2, [r7, #8]
	add	r3, r3, r2
	ldrb	r3, [r3]	@ zero_extendqisi2
	lsls	r3, r3, #8
	sxth	r2, r3
	ldr	r3, [r7, #84]
	adds	r3, r3, #13
	ldr	r1, [r7, #8]
	add	r3, r3, r1
	ldrb	r3, [r3]	@ zero_extendqisi2
	sxth	r3, r3
	orrs	r3, r3, r2
	strh	r3, [r7, #50]	@ movhi
	ldrsh	r3, [r7, #50]
	vmov	s15, r3	@ int
	vcvt.f32.s32	s15, s15
	vldr.32	s14, .L5+16
	vmul.f32	s14, s15, s14
	vldr.32	s13, .L5+8
	vdiv.f32	s15, s14, s13
	vldr.32	s14, [r7, #60]
	vadd.f32	s15, s14, s15
	vstr.32	s15, [r7, #60]
	ldr	r3, [r7, #84]
	adds	r3, r3, #14
	str	r3, [r7, #84]
.L2:
	ldr	r3, [r7, #4]
	movs	r2, #14
	mul	r2, r2, r3
	ldr	r3, [r7, #84]
	cmp	r2, r3
	bhi	.L3
	movs	r3, #0
	str	r3, [r7, #84]
	ldr	r3, [r7, #4]
	vmov	s15, r3	@ int
	vcvt.f32.s32	s14, s15
	vldr.32	s13, [r7, #80]
	vdiv.f32	s15, s13, s14
	vstr.32	s15, [r7, #20]
	ldr	r3, [r7, #4]
	vmov	s15, r3	@ int
	vcvt.f32.s32	s14, s15
	vldr.32	s13, [r7, #76]
	vdiv.f32	s15, s13, s14
	vstr.32	s15, [r7, #24]
	ldr	r3, [r7, #4]
	vmov	s15, r3	@ int
	vcvt.f32.s32	s14, s15
	vldr.32	s13, [r7, #72]
	vdiv.f32	s15, s13, s14
	vstr.32	s15, [r7, #28]
	ldr	r3, [r7, #4]
	vmov	s15, r3	@ int
	vcvt.f32.s32	s14, s15
	vldr.32	s13, [r7, #56]
	vdiv.f32	s15, s13, s14
	vstr.32	s15, [r7, #32]
	ldr	r3, [r7, #4]
	vmov	s15, r3	@ int
	vcvt.f32.s32	s14, s15
	vldr.32	s13, [r7, #68]
	vdiv.f32	s15, s13, s14
	vstr.32	s15, [r7, #36]
	ldr	r3, [r7, #4]
	vmov	s15, r3	@ int
	vcvt.f32.s32	s14, s15
	vldr.32	s13, [r7, #64]
	vdiv.f32	s15, s13, s14
	vstr.32	s15, [r7, #40]
	ldr	r3, [r7, #4]
	vmov	s15, r3	@ int
	vcvt.f32.s32	s14, s15
	vldr.32	s13, [r7, #60]
	vdiv.f32	s15, s13, s14
	vstr.32	s15, [r7, #44]
	ldr	r3, .L5+20
.LPIC0:
	add	r3, pc
	mov	r0, r3
	bl	puts(PLT)
	vldr.32	s15, [r7, #20]
	vcvt.f64.f32	d7, s15
	vmov	r2, r3, d7
	ldr	r1, .L5+24
.LPIC1:
	add	r1, pc
	mov	r0, r1
	bl	printf(PLT)
	vldr.32	s15, [r7, #24]
	vcvt.f64.f32	d7, s15
	vmov	r2, r3, d7
	ldr	r1, .L5+28
.LPIC2:
	add	r1, pc
	mov	r0, r1
	bl	printf(PLT)
	vldr.32	s15, [r7, #28]
	vcvt.f64.f32	d7, s15
	vmov	r2, r3, d7
	ldr	r1, .L5+32
.LPIC3:
	add	r1, pc
	mov	r0, r1
	bl	printf(PLT)
	vldr.32	s15, [r7, #32]
	vcvt.f64.f32	d7, s15
	vmov	r2, r3, d7
	ldr	r1, .L5+36
.LPIC4:
	add	r1, pc
	mov	r0, r1
	bl	printf(PLT)
	vldr.32	s15, [r7, #36]
	vcvt.f64.f32	d7, s15
	vmov	r2, r3, d7
	ldr	r1, .L5+40
.LPIC5:
	add	r1, pc
	mov	r0, r1
	bl	printf(PLT)
	vldr.32	s15, [r7, #40]
	vcvt.f64.f32	d7, s15
	vmov	r2, r3, d7
	ldr	r1, .L5+44
.LPIC6:
	add	r1, pc
	mov	r0, r1
	bl	printf(PLT)
	vldr.32	s15, [r7, #44]
	vcvt.f64.f32	d7, s15
	vmov	r2, r3, d7
	ldr	r1, .L5+48
.LPIC7:
	add	r1, pc
	mov	r0, r1
	bl	printf(PLT)
	ldr	r3, .L5+52
.LPIC8:
	add	r3, pc
	mov	r0, r3
	bl	puts(PLT)
	mov	r3, #0
	str	r3, [r7, #80]	@ float
	mov	r3, #0
	str	r3, [r7, #76]	@ float
	mov	r3, #0
	str	r3, [r7, #72]	@ float
	mov	r3, #0
	str	r3, [r7, #56]	@ float
	mov	r3, #0
	str	r3, [r7, #68]	@ float
	mov	r3, #0
	str	r3, [r7, #64]	@ float
	mov	r3, #0
	str	r3, [r7, #60]	@ float
	b	.L6
.L7:
	.align	3
.L5:
	.word	171798692
	.word	1078084567
	.word	1191182336
	.word	1135214592
	.word	1132068864
	.word	.LC0-(.LPIC0+4)
	.word	.LC1-(.LPIC1+4)
	.word	.LC2-(.LPIC2+4)
	.word	.LC3-(.LPIC3+4)
	.word	.LC4-(.LPIC4+4)
	.word	.LC5-(.LPIC5+4)
	.word	.LC6-(.LPIC6+4)
	.word	.LC7-(.LPIC7+4)
	.word	.LC8-(.LPIC8+4)
.L6:
	ldr	r3, [r7, #12]
	mov	r5, r3
	add	r4, r7, #20
	ldmia	r4!, {r0, r1, r2, r3}
	stmia	r5!, {r0, r1, r2, r3}
	ldm	r4, {r0, r1, r2}
	stm	r5, {r0, r1, r2}
	ldr	r0, [r7, #12]
	adds	r7, r7, #88
	mov	sp, r7
	@ sp needed
	pop	{r4, r5, r7, pc}
	.size	PromedioSIMD, .-PromedioSIMD
	.ident	"GCC: (Debian 8.3.0-6) 8.3.0"
	.section	.note.GNU-stack,"",%progbits
