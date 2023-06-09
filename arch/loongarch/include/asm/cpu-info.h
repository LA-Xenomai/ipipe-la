/* SPDX-License-Identifier: GPL-2.0 */
/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2020 Loongson Technology Corporation Limited
 */
#ifndef __ASM_CPU_INFO_H
#define __ASM_CPU_INFO_H

#include <linux/cache.h>
#include <linux/types.h>

#include <asm/loongarchregs.h>

/*
 * Descriptor for a cache
 */
struct cache_desc {
	unsigned int waysize;	/* Bytes per way */
	unsigned short sets;	/* Number of lines per set */
	unsigned char ways;	/* Number of ways */
	unsigned char linesz;	/* Size of line in bytes */
	unsigned char waybit;	/* Bits to select in a cache set */
	unsigned char flags;	/* Flags describing cache properties */
};

struct guest_info {
	unsigned long		ases;
	unsigned long		ases_dyn;
	unsigned long long	options;
	unsigned long long	options_dyn;
	int			tlbsize;
	u8			conf;
	unsigned int		kscratch_mask;
};

struct cpuinfo_loongarch {
	u64			asid_cache;
	unsigned long		asid_mask;

	/*
	 * Capability and feature descriptor structure for LoongArch CPU
	 */
	unsigned long		ases;
	unsigned long long	options;
	unsigned int		udelay_val;
	unsigned int		processor_id;
	unsigned int		fpu_vers;
	unsigned int		fpu_csr0;
	unsigned int		fpu_mask;
	unsigned int		cputype;
	int			isa_level;
	int			tlbsize;
	int			tlbsizemtlb;
	int			tlbsizestlbsets;
	int			tlbsizestlbways;
	struct cache_desc	icache; /* Primary I-cache */
	struct cache_desc	dcache; /* Primary D or combined I/D cache */
	struct cache_desc	vcache; /* Victim cache, between pcache and scache */
	struct cache_desc	scache; /* Secondary cache */
	struct cache_desc	tcache; /* Tertiary/split secondary cache */
	int			package;/* physical package number */
	unsigned int		globalnumber;
	int			vabits; /* Virtual Address size in bits */
	int			pabits; /* Physical Address size in bits */
	void			*data;	/* Additional data */
	unsigned int		watch_dreg_count;   /* Number data breakpoints */
	unsigned int		watch_ireg_count;   /* Number instruction breakpoints */
	unsigned int		watch_reg_use_cnt; /* min(NUM_WATCH_REGS, watch_dreg_count + watch_ireg_count), Usable by ptrace */
	unsigned int		kscratch_mask; /* Usable KScratch mask. */

	/* VZ & Guest features */
	struct guest_info	guest;
	unsigned int		guestid_mask;
	unsigned int		guestid_cache;
	unsigned long		guest_cfg;
} __attribute__((aligned(SMP_CACHE_BYTES)));

extern struct cpuinfo_loongarch cpu_data[];
#define boot_cpu_data cpu_data[0]
#define current_cpu_data cpu_data[smp_processor_id()]
#define raw_current_cpu_data cpu_data[raw_smp_processor_id()]

extern void cpu_probe(void);

extern const char *__cpu_family[];
extern const char *__cpu_full_name[];
#define cpu_family_string()	__cpu_family[raw_smp_processor_id()]
#define cpu_full_name_string()	__cpu_full_name[raw_smp_processor_id()]

struct seq_file;
struct notifier_block;

extern int register_proc_cpuinfo_notifier(struct notifier_block *nb);
extern int proc_cpuinfo_notifier_call_chain(unsigned long val, void *v);

#define proc_cpuinfo_notifier(fn, pri)					\
({									\
	static struct notifier_block fn##_nb = {			\
		.notifier_call = fn,					\
		.priority = pri						\
	};								\
									\
	register_proc_cpuinfo_notifier(&fn##_nb);			\
})

struct proc_cpuinfo_notifier_args {
	struct seq_file *m;
	unsigned long n;
};

static inline unsigned int cpu_cluster(struct cpuinfo_loongarch *cpuinfo)
{
	return (cpuinfo->globalnumber & LOONGARCH_GLOBALNUMBER_CLUSTER) >>
		LOONGARCH_GLOBALNUMBER_CLUSTER_SHF;
}

static inline unsigned int cpu_core(struct cpuinfo_loongarch *cpuinfo)
{
	return (cpuinfo->globalnumber & LOONGARCH_GLOBALNUMBER_CORE) >>
		LOONGARCH_GLOBALNUMBER_CORE_SHF;
}

extern void cpu_set_cluster(struct cpuinfo_loongarch *cpuinfo, unsigned int cluster);
extern void cpu_set_core(struct cpuinfo_loongarch *cpuinfo, unsigned int core);

static inline bool cpus_are_siblings(int cpua, int cpub)
{
	struct cpuinfo_loongarch *infoa = &cpu_data[cpua];
	struct cpuinfo_loongarch *infob = &cpu_data[cpub];
	unsigned int gnuma, gnumb;

	if (infoa->package != infob->package)
		return false;

	gnuma = infoa->globalnumber & ~LOONGARCH_GLOBALNUMBER_VP;
	gnumb = infob->globalnumber & ~LOONGARCH_GLOBALNUMBER_VP;
	if (gnuma != gnumb)
		return false;

	return true;
}

static inline unsigned long cpu_asid_mask(struct cpuinfo_loongarch *cpuinfo)
{
	return cpuinfo->asid_mask;
}

static inline void set_cpu_asid_mask(struct cpuinfo_loongarch *cpuinfo,
				     unsigned long asid_mask)
{
	cpuinfo->asid_mask = asid_mask;
}

#endif /* __ASM_CPU_INFO_H */
