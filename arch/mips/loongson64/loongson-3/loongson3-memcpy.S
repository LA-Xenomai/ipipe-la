/*
 ============================================================================
 Name		: memcpy.S
 Author	: Heiher <root@heiher.info>
		  Chen Jie <chenj@lemote.com>
 Version	: 20140307
 Copyright	: GPLv2
 Description	: The memcpy for Loongson 3.
 ============================================================================
 */

#define LINUX_KERNEL
#ifndef LINUX_KERNEL
#include <sys/asm.h>
#include <sys/regdef.h>

#define EXC(inst_reg,addr,handler)		\
	inst_reg, addr;
#define EXCQ(inst,reg1,reg2,addr,handler)	\
	inst	reg1, reg2, addr;

#if _MIPS_SIM == _ABI64
#define CONFIG_64BIT
#else
#define CONFIG_32BIT
#endif

#define FEXPORT(symbol)

#else /* LINUX_KERNEL */

#include <asm/alternative-asm.h>
#include <asm/asm.h>
#include <asm/asm-offsets.h>
#include <asm/cpu.h>
#include <asm/export.h>
#include <asm/regdef.h>

#define EXC(inst_reg,addr,handler)		\
9:	inst_reg, addr;				\
	.section __ex_table,"a";		\
	PTR	9b, handler;			\
	.previous

#define EXCQ(inst,reg1,reg2,addr,handler)	\
9:	inst	reg1, reg2, addr;		\
	.section __ex_table,"a";		\
	PTR	9b, handler;			\
	.previous

#endif

#define dst a0
#define src a1
#define len a2
#define rem t8

/*
 * 64bit ABI vs 32bit ABI
 */
#ifdef CONFIG_64BIT
#define ADDU	daddu
#define ADDI	daddi
#define SUBU	dsubu
#define SLL	dsll
#define SRL	dsrl
#define PTR_LA	dla

#define LOAD	ld

/*
 * As we are sharing code base with the mips32 tree (which use the o32 ABI
 * register definitions). We need to redefine the register definitions from
 * the n64 ABI register naming to the o32 ABI register naming.
 */
#undef t0
#undef t1
#undef t2
#undef t3
#define t0	$8
#define t1	$9
#define t2	$10
#define t3	$11
#define t4	$12
#define t5	$13
#define t6	$14
#define t7	$15

#else

#define ADDU	addu
#define ADDI	addi
#define SUBU	subu
#define SLL	sll
#define SRL	srl
#define PTR_LA	la

#define LOAD	lw
#endif /* CONFIG_64BIT */

#define LDFIRST	ldr
#define LDREST	ldl

#define SDFIRST sdr
#define SDREST	sdl

#define LWFIRST	lwr
#define LWREST	lwl

#define SWFIRST swr
#define SWREST	swl

#define _MIPS_LSE_CPUCFG 1

/* void * memcpy (void *s1, const void *s2, size_t n); */
	.text
	.align	5
	.set	noreorder
	.set	noat
	.set	loongson-ext2

LEAF(memcpy)				/* a0=dst a1=src a2=len */
EXPORT_SYMBOL(memcpy)
	move	v0, dst
.L__memcpy:
FEXPORT(__copy_user)
EXPORT_SYMBOL(__copy_user)
	/* if less then 0x28 bytes */
	sltu	t2, a2, 0x28
	andi	t0, dst, 0xf
	bnez	t2, .L_memcpy_less
	 andi	t1, src, 0xf

	beqz	t0, 1f
	 ADDI	rem, t0, -0x10

	/* upgrade */
EXC(	LDFIRST	t3, 0(src),		.Ll_exc)
	sltu	t4, t0, 0x8
EXC(	LDREST	t3, 7(src),		.Ll_exc_copy)
	SUBU	src, rem
EXC(	SDFIRST	t3, 0(dst),		.Ls_exc)
	SUBU	dst, rem
	beqz	t4, 1f
	 ADDU	len, rem
EXC(	LDFIRST	t3, -8(src),		.Ll_exc_a8)
EXC(	LDREST	t3, -1(src),		.Ll_exc_copy_a8)
EXC(	sd	t3, -8(dst),		.Ls_exc_p8)

1:	andi	t7, src, 0x7
	beq	t0, t1, .L_memcpy_16_16
	 nop

	/* t7 is $15 */
	ALTERNATIVE "bnez $15, .L_memcpy_16_4_2_1",\
		"bnez $15, .L_memcpy_16_4_2_1_unalign",\
		_MIPS_LSE_CPUCFG

.L_memcpy_16_8:
	 SRL	t0, len, 6	# 64B per iteration
	beqz	t0, 2f
	 and	rem, len, 0x3f
	.align	4
1:
EXC(	ld	t4,	(16 * 0)(src),	.Ll_exc)
EXC(	ld	t7, (8 + 16 * 0)(src),	.Ll_exc_copy)
EXC(	ld	t2,	(16 * 1)(src),	.Ll_exc_copy)
EXC(	ld	t3, (8 + 16 * 1)(src),	.Ll_exc_copy)
EXC(	ld	t0,	(16 * 2)(src),	.Ll_exc_copy)
EXC(	ld	t1, (8 + 16 * 2)(src),	.Ll_exc_copy)
	ADDI	len, -16 * 4
EXCQ(	gssq, t7, t4, (16 * 0)(dst),	.Ls_exc_p64)
EXC(	ld	t4,	(16 * 3)(src),	.Ll_exc_copy)
EXC(	ld	t7, (8 + 16 * 3)(src),	.Ll_exc_copy)
	ADDU	src, 16 * 4
	ADDU	dst, 16 * 4
EXCQ(	gssq, t3, t2, (-16 * 3)(dst),	.Ls_exc_p48)
EXCQ(	gssq, t1, t0, (-16 * 2)(dst),	.Ls_exc_p32)
EXCQ(	gssq, t7, t4, (-16 * 1)(dst),	.Ls_exc_p16)
	bne	len, rem, 1b
	 nop
	beqz	len, .Ldone
2:	 sltu	t0, len, 32
	bnez	t0, 3f
	 and	rem, len, 0xf
EXC(	ld	t2,	(16 * 0)(src),	.Ll_exc)
EXC(	ld	t3, (8 + 16 * 0)(src),	.Ll_exc_copy)
EXC(	ld	t0,	(16 * 1)(src),	.Ll_exc_copy)
EXC(	ld	t1, (8 + 16 * 1)(src),	.Ll_exc_copy)
	ADDI	len, -16 * 2
	ADDU	src, 16 * 2
EXCQ(	gssq, t3, t2, (16 * 0)(dst),	.Ls_exc_p32)
EXCQ(	gssq, t1, t0, (16 * 1)(dst),	.Ls_exc_p16)
	beqz	len, .Ldone
	 ADDU	dst, 32
3:	/* copy less than 32B */
	beq	rem, len, .L_memcpy_1_15B_8B_aligned
	 nop
EXC(	ld	t2,	0(src),		.Ll_exc)
EXC(	ld	t3,	8(src),		.Ll_exc_copy)
	ADDI	len, -16
	ADDU	src, 16
EXCQ(	gssq, t3, t2, 0(dst),		.Ls_exc_p16)
	bnez	len, .L_memcpy_1_15B_8B_aligned
	 ADDU	dst, 16

	jr	ra
	 nop

.L_memcpy_16_16:
	.align	4
	SRL	t0, len, 7      # 128B per iteration
	beqz	t0, 5f
	and	rem, len, 0x7f
6:
EXCQ(   gslq, t7, t4, (16 * 0)(src),    .Ll_exc)
EXCQ(   gslq, t3, t2, (16 * 1)(src),    .Ll_exc_copy)
EXCQ(	gslq, t1, t0, (16 * 2)(src),	.Ll_exc_copy)
        ADDI    len, -16 * 8
EXCQ(   gssq, t7, t4, (16 * 0)(dst),    .Ls_exc_p128)
EXCQ(   gslq, t7, t4, (16 * 3)(src),    .Ll_exc_copy)
EXCQ(   gssq, t3, t2, (16 * 1)(dst),    .Ls_exc_p112)
EXCQ(   gslq, t3, t2, (16 * 4)(src),    .Ll_exc_copy)
EXCQ(   gssq, t1, t0, (16 * 2)(dst),    .Ls_exc_p96)
EXCQ(   gslq, t1, t0, (16 * 5)(src),    .Ll_exc_copy)
EXCQ(   gssq, t7, t4, (16 * 3)(dst),    .Ls_exc_p80)
EXCQ(   gslq, t7, t4, (16 * 6)(src),    .Ll_exc_copy)
EXCQ(   gssq, t3, t2, (16 * 4)(dst),    .Ls_exc_p64)
EXCQ(   gslq, t3, t2, (16 * 7)(src),    .Ll_exc_copy)

EXCQ(   gssq, t1, t0, (16 * 5)(dst),    .Ls_exc_p48)
EXCQ(   gssq, t7, t4, (16 * 6)(dst),    .Ls_exc_p32)
EXCQ(   gssq, t3, t2, (16 * 7)(dst),    .Ls_exc_p16)
	ADDU    src, 16 * 8
	bne     len, rem, 6b
	ADDU    dst, 16 * 8
	beqz    len, .Ldone
5:
	SRL	t0, len, 6	# 64B per iteration
	beqz	t0, 2f
	and	rem, len, 0x3f
1:
EXCQ(	gslq, t7, t4, (16 * 0)(src),	.Ll_exc)
EXCQ(	gslq, t3, t2, (16 * 1)(src),	.Ll_exc_copy)
EXCQ(	gslq, t1, t0, (16 * 2)(src),	.Ll_exc_copy)
	ADDI	len, -16 * 4
EXCQ(	gssq, t7, t4, (16 * 0)(dst),	.Ls_exc_p64)
EXCQ(	gslq, t7, t4, (16 * 3)(src),	.Ll_exc_copy)
	ADDU	src, 16 * 4
	ADDU	dst, 16 * 4
EXCQ(	gssq, t3, t2, (-16 * 3)(dst),	.Ls_exc_p48)
EXCQ(	gssq, t1, t0, (-16 * 2)(dst),	.Ls_exc_p32)
EXCQ(	gssq, t7, t4, (-16 * 1)(dst),	.Ls_exc_p16)
	bne	len, rem, 1b
	 nop
	beqz	len, .Ldone
2:	 sltu	t0, len, 32
	bnez	t0, 3f
	 and	rem, len, 0xf
EXCQ(	gslq, t3, t2, (16 * 0)(src),	.Ll_exc)
EXCQ(	gslq, t1, t0, (16 * 1)(src),	.Ll_exc_copy)
	ADDI	len, -16 * 2
	ADDU	src, 32
EXCQ(	gssq, t3, t2, (16 * 0)(dst),	.Ls_exc_p32)
EXCQ(	gssq, t1, t0, (16 * 1)(dst),	.Ls_exc_p16)
	beqz	len, .Ldone
	 ADDU	dst, 32
3:	/* copy less than 32B */
	beq	rem, len, .L_memcpy_1_15B_8B_aligned
	 nop
EXCQ(	gslq, t3, t2, 0(src),		.Ll_exc)
	ADDI	len, -16
	ADDU	src, 16
EXCQ(	gssq, t3, t2, 0(dst),		.Ls_exc_p16)
	beqz	len, .Ldone
	 ADDU	dst, 16
/*
 * copy 1 - 15B, src & dst are 8B aligned
 */
.L_memcpy_1_15B_8B_aligned:
	sltu	t0, len, 0x9
	bnez	t0, 1f
	 nop
EXC(	ld	t1, (src),		.Ll_exc)
EXC(	sd	t1, (dst),		.Ls_exc)
1:	ADDU	src, len
	ADDU	dst, len
EXC(	LDREST	t1, -1(src),		.Ll_exc_copy_len)
EXC(	SDREST	t1, -1(dst),		.Ls_exc)
.Ldone:
	jr	ra
	 move	len, zero

.L_memcpy_16_4_2_1_unalign:
        SRL     t0, len, 6      # 64B per iteration
        beqz    t0, .L_memcpy_32
        and    rem, len, 0x3f
        .align  4
1:
EXCQ(   gslq, t7, t4, (16 * 0)(src),    .Ll_exc)
EXCQ(   gslq, t3, t2, (16 * 1)(src),    .Ll_exc_copy)
EXCQ(   gslq, t1, t0, (16 * 2)(src),    .Ll_exc_copy)
        ADDI    len, -16 * 4
EXCQ(   gssq, t7, t4, (16 * 0)(dst),    .Ls_exc_p64)
EXCQ(   gslq, t7, t4, (16 * 3)(src),    .Ll_exc_copy)
        ADDU    src, 16 * 4
        ADDU    dst, 16 * 4
EXCQ(   gssq, t3, t2, (-16 * 3)(dst),   .Ls_exc_p48)
EXCQ(   gssq, t1, t0, (-16 * 2)(dst),   .Ls_exc_p32)
EXCQ(   gssq, t7, t4, (-16 * 1)(dst),   .Ls_exc_p16)
        bne     len, rem, 1b
        nop
        beqz    len, .Ldone
       nop

.L_memcpy_32:
        SRL     t0, len, 5      # 32B per iteration
        beqz    t0, 2f
        and    rem, len, 0x1f
1:
EXCQ(   gslq, t7, t4, 0(src),          .Ll_exc)
EXCQ(   gslq, t3, t2, 16(src),         .Ll_exc_copy)
        ADDI    len, -16 * 2
        ADDU    src, 16 * 2
EXCQ(   gssq, t7, t4, (16 * 0)(dst),    .Ls_exc_p32)
EXCQ(   gssq, t3, t2, (16 * 1)(dst),    .Ls_exc_p16)
        bne     len, rem, 1b
        ADDU   dst, 16 * 2
        beqz    len, .Ldone
2:      and    rem, len, 0xf
        beq     rem, len, .L_memcpy_less
         nop

EXCQ(   gslq,  t1, t0, 0(src),                 .Ll_exc)
        ADDI    len, -16
        ADDU    src, 16
EXCQ(   gssq, t1, t0, 0(dst),           .Ls_exc_p16)
        beqz    len, .Ldone
        ADDU    dst, 16
        b .L_memcpy_less
        nop


.L_memcpy_16_4_2_1:
	SRL	t0, len, 5	# 32B per iteration
	beqz	t0, 2f
	 and	rem, len, 0x1f
1:
EXC(	LDFIRST	t4, 0(src),		.Ll_exc)
EXC(	LDFIRST	t7, 8(src),		.Ll_exc_copy)
	ADDI	len, -16 * 2
EXC(	LDREST	t4, 7(src),		.Ll_exc_copy)
EXC(	LDREST	t7, 15(src),		.Ll_exc_copy)
EXC(	LDFIRST	t2, 16(src),		.Ll_exc_copy)
EXC(	LDFIRST	t3, 24(src),		.Ll_exc_copy)
EXC(	LDREST	t2, 23(src),		.Ll_exc_copy)
EXC(	LDREST	t3, 31(src),		.Ll_exc_copy)
	ADDU	src, 16 * 2
EXCQ(	gssq, t7, t4, (16 * 0)(dst),	.Ls_exc_p32)
EXCQ(	gssq, t3, t2, (16 * 1)(dst),	.Ls_exc_p16)
	bne	len, rem, 1b
	 ADDU	dst, 16 * 2
	beqz	len, .Ldone
2:	 and	rem, len, 0xf
	beq	rem, len, .L_memcpy_less
	 nop
EXC(	LDFIRST	t0, 0(src),		.Ll_exc)
EXC(	LDFIRST	t1, 8(src),		.Ll_exc_copy)
	ADDI	len, -16
EXC(	LDREST	t0, 7(src),		.Ll_exc_copy)
EXC(	LDREST	t1, 15(src),		.Ll_exc_copy)
	ADDU	src, 16
EXCQ(	gssq, t1, t0, 0(dst),		.Ls_exc_p16)
	beqz	len, .Ldone
	 ADDU	dst, 16

.L_memcpy_less:
	andi	t0, len, 0x7
	beq	t0, len, 2f
	 andi	t4, len, 0x3

	.set	reorder
	SUBU	t1, len, t0
	ADDU	dst, t1
	ADDU	src, t1
	.set	at=t2
	PTR_LA	t3, 1f
	.set	noat
	SLL	t2, t1, 0x1		/* 4 * 4B instructions move 8B data*/
	SUBU	t3, t2
	jr	t3
	.set	noreorder

EXC(	LDFIRST	t1, (-8 * 4)(src),	.Ll_exc_a32)
EXC(	LDREST	t1, (-8 * 4 + 7)(src),	.Ll_exc_copy_a32)
EXC(	SDFIRST	t1, (-8 * 4)(dst),	.Ls_exc)
EXC(	SDREST	t1, (-8 * 4 + 7)(dst),	.Ls_exc)

EXC(	LDFIRST	t2, (-8 * 3)(src),	.Ll_exc_a24)
EXC(	LDREST	t2, (-8 * 3 + 7)(src),	.Ll_exc_copy_a24)
EXC(	SDFIRST	t2, (-8 * 3)(dst),	.Ls_exc_r24)
EXC(	SDREST	t2, (-8 * 3 + 7)(dst),	.Ls_exc_r24)

EXC(	LDFIRST	t3, (-8 * 2)(src),	.Ll_exc_a16)
EXC(	LDREST	t3, (-8 * 2 + 7)(src),	.Ll_exc_copy_a16)
EXC(	SDFIRST	t3, (-8 * 2)(dst),	.Ls_exc_r16)
EXC(	SDREST	t3, (-8 * 2 + 7)(dst),	.Ls_exc_r16)

EXC(	LDFIRST	t1, (-8 * 1)(src),	.Ll_exc_a8)
EXC(	LDREST	t1, (-8 * 1 + 7)(src),	.Ll_exc_copy_a8)
EXC(	SDFIRST	t1, (-8 * 1)(dst),	.Ls_exc_r8)
EXC(	SDREST	t1, (-8 * 1 + 7)(dst),	.Ls_exc_r8)
1:	beqz	t0, .Ldone
	 ADDU	src, t0
	ADDU	dst, t0
EXC(	LDFIRST	t2, -8(src),		.Ll_exc_a8)
EXC(	LDREST	t2, -1(src),		.Ll_exc_copy_a8)
EXC(	SDFIRST	t2, -8(dst),		.Ls_exc_r0)
EXC(	SDREST	t2, -1(dst),		.Ls_exc_r0)
	jr	ra
	 move	len, zero

2:
	beq	t4, len, 3f
	 nop
EXC(	LWFIRST	t2, (src),		.Ll_exc)
EXC(	LWREST	t2, 3(src),		.Ll_exc_copy)
	ADDU	src, len
EXC(	SWFIRST	t2, (dst),		.Ls_exc)
EXC(	SWREST	t2, 3(dst),		.Ls_exc)
	beqz	t4, .Ldone
	 ADDU	dst, len

EXC(	LWFIRST	t1, -4(src),		.Ll_exc_a4)
EXC(	LWREST	t1, -1(src),		.Ll_exc_copy_a4)
EXC(	SWFIRST	t1, -4(dst),		.Ls_exc)
EXC(	SWREST	t1, -1(dst),		.Ls_exc)
	jr	ra
	 move	len, zero

3:
	beqz	len, .Ldone
	 ADDU	t0, src, len
1:
EXC(	lb	t2, (src),		.Ll_exc)
	ADDU	src, 1
EXC(	sb	t2, (dst),		.Ls_exc)
	bne	t0, src, 1b
	 ADDU	dst, 1

	jr	ra
	 move	len, zero

	END(memcpy)

#ifdef LINUX_KERNEL

#define LEXC_a(n)						\
.Ll_exc_copy_a ## n:						\
	ADDI	src, -n;					\
	b	.Ll_exc_copy;					\
	 ADDI	dst, -n;					\
.Ll_exc_a ## n:							\
	ADDI	src, -n;					\
	b	.Ll_exc;					\
	 ADDI	dst, -n;

.Ll_exc_copy_len:
	SUBU	src, len
	b	.Ll_exc_copy
	 SUBU	dst, len

LEXC_a(4)
LEXC_a(8)
LEXC_a(16)
LEXC_a(24)
LEXC_a(32)
LEXC_a(40)
LEXC_a(48)

.Ll_exc_copy:
	/*
	 * Copy bytes from src until faulting load address (or until a
	 * lb faults)
	 *
	 * When reached by a faulting LDFIRST/LDREST, THREAD_BUADDR($28)
	 * may be more than a byte beyond the last address.
	 * Hence, the lb below may get an exception.
	 *
	 * Assumes src < THREAD_BUADDR($28)
	 */
	LOAD	t0, TI_TASK($28)
	 nop
	LOAD	t0, THREAD_BUADDR(t0)
1:
EXC(	lb	t1, 0(src),	.Ll_exc)
	ADDU	src, 1
	sb	t1, 0(dst)	# can't fault -- we're copy_from_user
	bne	src, t0, 1b
	 ADDU	dst, 1
.Ll_exc:
	LOAD	t0, TI_TASK($28)
	 nop
	LOAD	t0, THREAD_BUADDR(t0)	# t0 is just past last good address
	 nop
	SUBU	len, AT, t0		# len number of uncopied bytes
	jr	ra
	 nop

#define SEXC_p(n)						\
.Ls_exc_p ## n:							\
	jr	ra;						\
	 ADDU	len, n;

#define SEXC_r(n)						\
.Ls_exc_r ## n:							\
	jr	ra;						\
	 ADDI	len, t0, n;

SEXC_p(1)
SEXC_p(8)
SEXC_p(16)
SEXC_p(32)
SEXC_p(48)
SEXC_p(64)
SEXC_p(80)
SEXC_p(96)
SEXC_p(112)
SEXC_p(128)
SEXC_r(0)
SEXC_r(8)
SEXC_r(16)
SEXC_r(24)

.Ls_exc:
	jr	ra
	 nop

	.align	5
LEAF(memmove)
EXPORT_SYMBOL(memmove)
	ADDU	t0, a0, a2
	ADDU	t1, a1, a2
	sltu	t0, a1, t0			# dst + len <= src -> memcpy
	sltu	t1, a0, t1			# dst >= src + len -> memcpy
	and	t0, t1
	beqz	t0, .L__memcpy
	 move	v0, a0				/* return value */
	beqz	a2, .Lr_out
	END(memmove)

	/* fall through to __rmemcpy */
LEAF(__rmemcpy)					/* a0=dst a1=src a2=len */
	 sltu	t0, a1, a0
	beqz	t0, .Lr_end_bytes_up		# src >= dst
	 nop
	ADDU	a0, a2				# dst = dst + len
	ADDU	a1, a2				# src = src + len

.Lr_end_bytes:
	lb	t0, -1(a1)
	ADDI	a2, -0x1
	sb	t0, -1(a0)
	ADDI	a1, -0x1
	bnez	a2, .Lr_end_bytes
	 ADDI	a0, -0x1

.Lr_out:
	jr	ra
	 move	a2, zero

.Lr_end_bytes_up:
	lb	t0, (a1)
	ADDI	a2, -0x1
	sb	t0, (a0)
	ADDU	a1, 0x1
	bnez	a2, .Lr_end_bytes_up
	 ADDU	a0, 0x1

	jr	ra
	 move	a2, zero
	END(__rmemcpy)
#endif
