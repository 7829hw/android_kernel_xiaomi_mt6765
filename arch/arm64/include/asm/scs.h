/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_SCS_H
#define __ASM_SCS_H

#ifdef __ASSEMBLY__
#include <asm/asm-offsets.h>

#ifdef CONFIG_SHADOW_CALL_STACK
	scs_sp	.req	x18
#endif

	.macro scs_load task
#ifdef CONFIG_SHADOW_CALL_STACK
	ldr	scs_sp, [\task, #TSK_TI_SCS_SP]
#endif
	.endm

	.macro scs_save task
#ifdef CONFIG_SHADOW_CALL_STACK
	str	scs_sp, [\task, #TSK_TI_SCS_SP]
#endif
	.endm

#endif /* __ASSEMBLY__ */
#endif /* __ASM_SCS_H */
