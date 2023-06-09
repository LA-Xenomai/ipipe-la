/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2020 Loongson Technology Corporation Limited
 *
 * Switch a MMU context.
 */
#ifndef _ASM_MMU_CONTEXT_H
#define _ASM_MMU_CONTEXT_H

#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/mm_types.h>
#include <linux/smp.h>
#include <linux/slab.h>

#include <asm/cacheflush.h>
#include <asm/tlbflush.h>
#include <asm-generic/mm_hooks.h>

#include <asm-generic/ipipe.h>

#define TLBMISS_HANDLER_SETUP_PGD(pgd)	\
	csr_writeq((unsigned long)pgd, LOONGARCH_CSR_PGDL);

#define TLBMISS_HANDLER_RESTORE()					\
	csr_writeq((unsigned long) smp_processor_id(),		\
			    LOONGARCH_CSR_TMID)

#define TLBMISS_HANDLER_SETUP()						\
	do {								\
		TLBMISS_HANDLER_SETUP_PGD(swapper_pg_dir);		\
		TLBMISS_HANDLER_RESTORE();				\
	} while (0)


/*
 *  All unused by hardware upper bits will be considered
 *  as a software asid extension.
 */
static inline u64 asid_version_mask(unsigned int cpu)
{
	unsigned long asid_mask = cpu_asid_mask(&cpu_data[cpu]);

	return ~(u64)(asid_mask);
}

static inline u64 asid_first_version(unsigned int cpu)
{
	return ~asid_version_mask(cpu) + 1;
}

#define cpu_context(cpu, mm)	((mm)->context.asid[cpu])
#define asid_cache(cpu)		(cpu_data[cpu].asid_cache)
#define cpu_asid(cpu, mm)	(cpu_context((cpu), (mm)) & cpu_asid_mask(&cpu_data[cpu]))

static inline int asid_valid(struct mm_struct *mm, unsigned int cpu)
{
	if ((cpu_context(cpu, mm) ^ asid_cache(cpu)) & asid_version_mask(cpu))
		return 0;

	return 1;
}

static inline void enter_lazy_tlb(struct mm_struct *mm, struct task_struct *tsk)
{
}

/* Normal, classic get_new_mmu_context */
static inline void
get_new_mmu_context(struct mm_struct *mm, unsigned long cpu)
{
	unsigned long asid = asid_cache(cpu);

	if (!((++asid) & cpu_asid_mask(&cpu_data[cpu]))) {
		local_flush_tlb_user();

		/* start new asid cycle
		 * 0 - asid_mask is used for software, need get new asid
		 * asid_mask above is used for hardware
		 */
		if (unlikely(asid == 0))
			asid = asid_first_version(cpu);
	}

	cpu_context(cpu, mm) = asid_cache(cpu) = asid;
}

/*
 * Initialize the context related info for a new mm_struct
 * instance.
 */
static inline int
init_new_context(struct task_struct *tsk, struct mm_struct *mm)
{
	int i;

	for_each_possible_cpu(i)
		cpu_context(i, mm) = 0;

	return 0;
}

static inline void do_switch_mm(struct mm_struct *prev, struct mm_struct *next,
			     struct task_struct *tsk)
{
	unsigned int cpu = smp_processor_id();

	/* Check if our ASID is of an older version and thus invalid */
	if (asid_valid(next, cpu) == 0)
		get_new_mmu_context(next, cpu);
	write_csr_asid(cpu_asid(cpu, next));
	TLBMISS_HANDLER_SETUP_PGD(next->pgd);

	/*
	 * Mark current->active_mm as not "active" anymore.
	 * We don't want to mislead possible IPI tlb flush routines.
	 */
	cpumask_set_cpu(cpu, mm_cpumask(next));
}

static inline void switch_mm(struct mm_struct *prev, struct mm_struct *next,
			struct task_struct *tsk)
{
	unsigned long flags;

	flags = hard_cond_local_irq_save();
	do_switch_mm(prev, next, tsk);
	hard_cond_local_irq_restore(flags);
}

/*
 * Destroy context related info for an mm_struct that is about
 * to be put to rest.
 */
static inline void destroy_context(struct mm_struct *mm)
{

}

#define deactivate_mm(tsk, mm)		do { } while (0)

/*
 * After we have set current->mm to a new value, this activates
 * the context for the new mm so we see the new mappings.
 */
static inline void
activate_mm(struct mm_struct *prev, struct mm_struct *next)
{
	unsigned long flags;
	unsigned int cpu = smp_processor_id();

	flags = hard_cond_local_irq_save();

	/* Unconditionally get a new ASID.  */
	get_new_mmu_context(next, cpu);

	write_csr_asid(cpu_asid(cpu, next));
	TLBMISS_HANDLER_SETUP_PGD(next->pgd);

	/* mark mmu ownership change */
	cpumask_set_cpu(cpu, mm_cpumask(next));

	hard_cond_local_irq_restore(flags);
}

/*
 * If mm is currently active_mm, we can't really drop it.  Instead,
 * we will get a new one for it.
 */
static inline void
drop_mmu_context(struct mm_struct *mm, unsigned cpu)
{
	unsigned long flags;
	int pid;

	flags = hard_local_irq_save();

	pid = read_csr_asid();
	if ((pid & cpu_asid_mask(&current_cpu_data)) == (cpu_asid(cpu, mm))) {
		/* there are four conditions:
		 * 1. for lazy tlb, current->mm is null
		 * 2. current thread is running and it is user thread
		 * 3. happened in context_switch, current pointer is
		 *    not updated to next thread
		 * 4. mm is overtimed, its asid is equal to current asid,
		 *    hardware asid should be overwritten here
		 */
		if (!(current->mm && (current->mm != mm))) {
			get_new_mmu_context(mm, cpu);
			write_csr_asid(cpu_asid(cpu, mm));
			hard_local_irq_restore(flags);
			return;
		}
	}

	/* will get a new context next time */
	cpu_context(cpu, mm) = 0;
	cpumask_clear_cpu(cpu, mm_cpumask(mm));
	hard_local_irq_restore(flags);
}

#ifdef CONFIG_IPIPE
static inline void
ipipe_switch_mm_head(struct mm_struct *prev, struct mm_struct *next,
		     struct task_struct *tsk)
{
	do_switch_mm(prev, next, tsk);
}
#endif

#endif /* _ASM_MMU_CONTEXT_H */
