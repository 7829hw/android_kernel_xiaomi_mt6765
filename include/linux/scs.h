/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Shadow Call Stack support.
 *
 * Copyright (C) 2019 Google LLC
 */
#ifndef _LINUX_SCS_H
#define _LINUX_SCS_H

#include <linux/sched.h>

#ifdef CONFIG_SHADOW_CALL_STACK

#define SCS_SIZE	1024UL
#define SCS_END_MAGIC	0xaf0194819b1635f6UL

#define task_scs(tsk)		(task_thread_info(tsk)->scs_base)
#define task_scs_sp(tsk)	(task_thread_info(tsk)->scs_sp)

void scs_init(void);
int scs_prepare(struct task_struct *tsk, int node);
void scs_release(struct task_struct *tsk);

static inline unsigned long *scs_magic(void *base)
{
	return (unsigned long *)((char *)base + SCS_SIZE) - 1;
}

static inline bool task_scs_end_corrupted(struct task_struct *tsk)
{
	void *base = task_scs(tsk);
	unsigned long offset = (unsigned long)task_scs_sp(tsk) -
			       (unsigned long)base;

	return !base || offset >= SCS_SIZE ||
	       READ_ONCE(*scs_magic(base)) != SCS_END_MAGIC;
}

static inline void scs_task_reset(struct task_struct *tsk)
{
	task_scs_sp(tsk) = task_scs(tsk);
}

#else /* CONFIG_SHADOW_CALL_STACK */

static inline void scs_init(void) {}
static inline int scs_prepare(struct task_struct *tsk, int node) { return 0; }
static inline void scs_release(struct task_struct *tsk) {}
static inline bool task_scs_end_corrupted(struct task_struct *tsk)
{
	return false;
}
static inline void scs_task_reset(struct task_struct *tsk) {}

#endif /* CONFIG_SHADOW_CALL_STACK */
#endif /* _LINUX_SCS_H */
