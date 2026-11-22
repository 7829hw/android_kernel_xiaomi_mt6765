// SPDX-License-Identifier: GPL-2.0
/*
 * Shadow Call Stack support.
 *
 * Copyright (C) 2019 Google LLC
 */

#include <linux/init.h>
#include <linux/scs.h>
#include <linux/slab.h>

static struct kmem_cache *scs_cache;

void __init scs_init(void)
{
	scs_cache = kmem_cache_create("shadow_call_stack", SCS_SIZE,
				      sizeof(unsigned long), SLAB_PANIC, NULL);
}

int scs_prepare(struct task_struct *tsk, int node)
{
	void *base;

	base = kmem_cache_alloc_node(scs_cache, GFP_KERNEL | __GFP_ZERO, node);
	if (!base)
		return -ENOMEM;

	*scs_magic(base) = SCS_END_MAGIC;
	task_scs(tsk) = base;
	scs_task_reset(tsk);
	return 0;
}

void scs_release(struct task_struct *tsk)
{
	void *base = task_scs(tsk);

	WARN_ON_ONCE(task_scs_end_corrupted(tsk));
	task_scs(tsk) = NULL;
	task_scs_sp(tsk) = NULL;
	kmem_cache_free(scs_cache, base);
}
