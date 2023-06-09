// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2020 Loongson Technology Co., Ltd.
 *
 * Handle unaligned accesses by emulation.
 *
 * This file contains exception handler for address error exception with the
 * special capability to execute faulting instructions in software.  The
 * handler does not try to handle the case when the program counter points
 * to an address not aligned to a word boundary.
 *
 */
#include <linux/context_tracking.h>
#include <linux/mm.h>
#include <linux/signal.h>
#include <linux/smp.h>
#include <linux/sched.h>
#include <linux/debugfs.h>
#include <linux/perf_event.h>
#include <linux/uaccess.h>

#include <asm/asm.h>
#include <asm/branch.h>
#include <asm/byteorder.h>
#include <asm/debug.h>
#include <asm/fpu.h>
#include <asm/inst.h>

extern unsigned long unaligned_read(void *addr, void *value, unsigned long n, bool symbol);
extern unsigned long unaligned_write(void *addr, unsigned long value, unsigned long n);

enum {
	UNALIGNED_ACTION_QUIET,
	UNALIGNED_ACTION_SIGNAL,
	UNALIGNED_ACTION_SHOW,
};
#ifdef CONFIG_DEBUG_FS
static u32 unaligned_instructions;
static u32 unaligned_action;
#else
#define unaligned_action UNALIGNED_ACTION_QUIET
#endif
extern void show_registers(struct pt_regs *regs);

static inline void write_fpr(unsigned int fd, unsigned long value)
{
#define WRITE_FPR(fd, value)		\
{					\
	__asm__ __volatile__(		\
	"movgr2fr.d $f%1, %0\n\t"	\
	:: "r"(value), "i"(fd));	\
}

	switch (fd) {
	case 0:
		WRITE_FPR(0, value);
		break;
	case 1:
		WRITE_FPR(1, value);
		break;
	case 2:
		WRITE_FPR(2, value);
		break;
	case 3:
		WRITE_FPR(3, value);
		break;
	case 4:
		WRITE_FPR(4, value);
		break;
	case 5:
		WRITE_FPR(5, value);
		break;
	case 6:
		WRITE_FPR(6, value);
		break;
	case 7:
		WRITE_FPR(7, value);
		break;
	case 8:
		WRITE_FPR(8, value);
		break;
	case 9:
		WRITE_FPR(9, value);
		break;
	case 10:
		WRITE_FPR(10, value);
		break;
	case 11:
		WRITE_FPR(11, value);
		break;
	case 12:
		WRITE_FPR(12, value);
		break;
	case 13:
		WRITE_FPR(13, value);
		break;
	case 14:
		WRITE_FPR(14, value);
		break;
	case 15:
		WRITE_FPR(15, value);
		break;
	case 16:
		WRITE_FPR(16, value);
		break;
	case 17:
		WRITE_FPR(17, value);
		break;
	case 18:
		WRITE_FPR(18, value);
		break;
	case 19:
		WRITE_FPR(19, value);
		break;
	case 20:
		WRITE_FPR(20, value);
		break;
	case 21:
		WRITE_FPR(21, value);
		break;
	case 22:
		WRITE_FPR(22, value);
		break;
	case 23:
		WRITE_FPR(23, value);
		break;
	case 24:
		WRITE_FPR(24, value);
		break;
	case 25:
		WRITE_FPR(25, value);
		break;
	case 26:
		WRITE_FPR(26, value);
		break;
	case 27:
		WRITE_FPR(27, value);
		break;
	case 28:
		WRITE_FPR(28, value);
		break;
	case 29:
		WRITE_FPR(29, value);
		break;
	case 30:
		WRITE_FPR(30, value);
		break;
	case 31:
		WRITE_FPR(31, value);
		break;
	default:
		panic("unexpected fd '%d'", fd);
	}
#undef WRITE_FPR
}

static inline unsigned long read_fpr(unsigned int fd)
{
#define READ_FPR(fd, __value)		\
{					\
	__asm__ __volatile__(		\
	"movfr2gr.d\t%0, $f%1\n\t"	\
	: "=r"(__value) : "i"(fd));	\
}

	unsigned long __value;

	switch (fd) {
	case 0:
		READ_FPR(0, __value);
		break;
	case 1:
		READ_FPR(1, __value);
		break;
	case 2:
		READ_FPR(2, __value);
		break;
	case 3:
		READ_FPR(3, __value);
		break;
	case 4:
		READ_FPR(4, __value);
		break;
	case 5:
		READ_FPR(5, __value);
		break;
	case 6:
		READ_FPR(6, __value);
		break;
	case 7:
		READ_FPR(7, __value);
		break;
	case 8:
		READ_FPR(8, __value);
		break;
	case 9:
		READ_FPR(9, __value);
		break;
	case 10:
		READ_FPR(10, __value);
		break;
	case 11:
		READ_FPR(11, __value);
		break;
	case 12:
		READ_FPR(12, __value);
		break;
	case 13:
		READ_FPR(13, __value);
		break;
	case 14:
		READ_FPR(14, __value);
		break;
	case 15:
		READ_FPR(15, __value);
		break;
	case 16:
		READ_FPR(16, __value);
		break;
	case 17:
		READ_FPR(17, __value);
		break;
	case 18:
		READ_FPR(18, __value);
		break;
	case 19:
		READ_FPR(19, __value);
		break;
	case 20:
		READ_FPR(20, __value);
		break;
	case 21:
		READ_FPR(21, __value);
		break;
	case 22:
		READ_FPR(22, __value);
		break;
	case 23:
		READ_FPR(23, __value);
		break;
	case 24:
		READ_FPR(24, __value);
		break;
	case 25:
		READ_FPR(25, __value);
		break;
	case 26:
		READ_FPR(26, __value);
		break;
	case 27:
		READ_FPR(27, __value);
		break;
	case 28:
		READ_FPR(28, __value);
		break;
	case 29:
		READ_FPR(29, __value);
		break;
	case 30:
		READ_FPR(30, __value);
		break;
	case 31:
		READ_FPR(31, __value);
		break;
	default:
		panic("unexpected fd '%d'", fd);
	}
#undef READ_FPR
	return __value;
}

static void emulate_load_store_insn(struct pt_regs *regs,
	void __user *addr, unsigned int __user *pc)
{
	union loongarch_instruction insn;
	unsigned long value = 0;
	unsigned int res;
	unsigned long origpc;
	unsigned long origra;
	origpc = (unsigned long)pc;
	origra = regs->regs[1];

	perf_sw_event(PERF_COUNT_SW_EMULATION_FAULTS, 1, regs, 0);

	/*
	 * This load never faults.
	 */
	__get_user(insn.word, pc);
	if (!access_ok(VERIFY_READ, addr, 8))
		goto sigbus;

	if (insn.reg2i12_format.opcode == ldd_op ||
		insn.reg2i14_format.opcode == ldptrd_op ||
		insn.reg3_format.opcode == ldxd_op) {
		res = unaligned_read(addr, &value, 8, 1);
		if (res)
			goto fault;
		regs->regs[insn.reg2i12_format.rd] = value;
	} else if (insn.reg2i12_format.opcode == ldw_op ||
		insn.reg2i14_format.opcode == ldptrw_op ||
		insn.reg3_format.opcode == ldxw_op) {
		res = unaligned_read(addr, &value, 4, 1);
		if (res)
			goto fault;
		regs->regs[insn.reg2i12_format.rd] = value;
	} else if (insn.reg2i12_format.opcode == ldwu_op ||
		insn.reg3_format.opcode == ldxwu_op) {
		res = unaligned_read(addr, &value, 4, 0);
		if (res)
			goto fault;
		regs->regs[insn.reg2i12_format.rd] = value;
	} else if (insn.reg2i12_format.opcode == ldh_op ||
		insn.reg3_format.opcode == ldxh_op) {
		res = unaligned_read(addr, &value, 2, 1);
		if (res)
			goto fault;
		regs->regs[insn.reg2i12_format.rd] = value;
	} else if (insn.reg2i12_format.opcode == ldhu_op ||
		insn.reg3_format.opcode == ldxhu_op) {
		res = unaligned_read(addr, &value, 2, 0);
		if (res)
			goto fault;
		regs->regs[insn.reg2i12_format.rd] = value;
	} else if (insn.reg2i12_format.opcode == std_op ||
		insn.reg2i14_format.opcode == stptrd_op ||
		insn.reg3_format.opcode == stxd_op) {
		value = regs->regs[insn.reg2i12_format.rd];
		res = unaligned_write(addr, value, 8);
		if (res)
			goto fault;
	} else if (insn.reg2i12_format.opcode == stw_op ||
		insn.reg2i14_format.opcode == stptrw_op ||
		insn.reg3_format.opcode == stxw_op) {
		value = regs->regs[insn.reg2i12_format.rd];
		res = unaligned_write(addr, value, 4);
		if (res)
			goto fault;
	} else if (insn.reg2i12_format.opcode == sth_op ||
		insn.reg3_format.opcode == stxh_op) {
		value = regs->regs[insn.reg2i12_format.rd];
		res = unaligned_write(addr, value, 2);
		if (res)
			goto fault;
	} else if (insn.reg2i12_format.opcode == fldd_op ||
		insn.reg3_format.opcode == fldxd_op) {
		res = unaligned_read(addr, &value, 8, 1);
		if (res)
			goto fault;
		write_fpr(insn.reg2i12_format.rd, value);
	} else if (insn.reg2i12_format.opcode == flds_op ||
		insn.reg3_format.opcode == fldxs_op) {
		res = unaligned_read(addr, &value, 4, 1);
		if (res)
			goto fault;
		write_fpr(insn.reg2i12_format.rd, value);
	} else if (insn.reg2i12_format.opcode == fstd_op ||
		insn.reg3_format.opcode == fstxd_op) {
		value = read_fpr(insn.reg2i12_format.rd);
		res = unaligned_write(addr, value, 8);
		if (res)
			goto fault;
	} else if (insn.reg2i12_format.opcode == fsts_op ||
		insn.reg3_format.opcode == fstxs_op) {
		value = read_fpr(insn.reg2i12_format.rd);
		res = unaligned_write(addr, value, 4);
		if (res)
			goto fault;
	} else
		goto sigbus;


#ifdef CONFIG_DEBUG_FS
	unaligned_instructions++;
#endif

	compute_return_era(regs);
	return;

fault:
	/* roll back jump/branch */
	regs->csr_era = origpc;
	regs->regs[1] = origra;
	/* Did we have an exception handler installed? */
	if (fixup_exception(regs))
		return;

	die_if_kernel("Unhandled kernel unaligned access", regs);
	force_sig(SIGSEGV, current);

	return;

sigbus:
	die_if_kernel("Unhandled kernel unaligned access", regs);
	force_sig(SIGBUS, current);

	return;
}

asmlinkage void do_ale(struct pt_regs *regs)
{
	enum ctx_state prev_state;
	unsigned int __user *pc;
	mm_segment_t seg;

	prev_state = exception_enter();
	perf_sw_event(PERF_COUNT_SW_ALIGNMENT_FAULTS,
			1, regs, regs->csr_badv);
	/*
	 * Did we catch a fault trying to load an instruction?
	 */
	if (regs->csr_badv == regs->csr_era)
		goto sigbus;

	if (user_mode(regs) && !test_thread_flag(TIF_FIXADE))
		goto sigbus;
	if (unaligned_action == UNALIGNED_ACTION_SIGNAL)
		goto sigbus;

	/*
	 * Do branch emulation only if we didn't forward the exception.
	 * This is all so but ugly ...
	 */

	if (unaligned_action == UNALIGNED_ACTION_SHOW)
		show_registers(regs);
	pc = (unsigned int __user *)exception_era(regs);

	seg = get_fs();
	if (!user_mode(regs))
		set_fs(KERNEL_DS);
	emulate_load_store_insn(regs, (void __user *)regs->csr_badv, pc);
	set_fs(seg);

	return;

sigbus:
	die_if_kernel("Kernel unaligned instruction access", regs);
	force_sig(SIGBUS, current);

	/*
	 * XXX On return from the signal handler we should advance the era
	 */
	exception_exit(prev_state);
}

asmlinkage void do_ade(struct pt_regs *regs)
{
	enum ctx_state prev_state;

	prev_state = exception_enter();
	perf_sw_event(PERF_COUNT_SW_ALIGNMENT_FAULTS,
			1, regs, regs->csr_badv);

	if (unaligned_action == UNALIGNED_ACTION_SHOW)
		show_registers(regs);

	die_if_kernel("Kernel ade access", regs);
	force_sig(SIGBUS, current);

	/*
	 * XXX On return from the signal handler we should advance the era
	 */
	exception_exit(prev_state);
}

#ifdef CONFIG_DEBUG_FS
static int __init debugfs_unaligned(void)
{
	struct dentry *d;

	if (!loongarch_debugfs_dir)
		return -ENODEV;
	d = debugfs_create_u32("unaligned_instructions", S_IRUGO,
			       loongarch_debugfs_dir, &unaligned_instructions);
	if (!d)
		return -ENOMEM;
	d = debugfs_create_u32("unaligned_action", S_IRUGO | S_IWUSR,
			       loongarch_debugfs_dir, &unaligned_action);
	if (!d)
		return -ENOMEM;
	return 0;
}
arch_initcall(debugfs_unaligned);
#endif
