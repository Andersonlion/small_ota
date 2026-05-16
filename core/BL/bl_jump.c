#include "bl_jump.h"

#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
#include <cmsis_compiler.h>
#else
#include <stdint.h>
/* CMSIS-like inline functions for ARM */
#ifndef __DSB
#define __DSB()     __asm__ volatile ("dsb 0xF" ::: "memory")
#endif
#ifndef __ISB
#define __ISB()     __asm__ volatile ("isb 0xF" ::: "memory")
#endif
#ifndef __set_MSP
#define __set_MSP(val)  __asm__ volatile ("msr msp, %0" :: "r"(val))
#endif
#ifndef __disable_irq
#define __disable_irq() __asm__ volatile ("cpsid i" ::: "memory")
#endif
#endif

void bl_jump_to_app(uint32_t app_addr)
{
    uint32_t app_stack;
    uint32_t app_reset_handler;

    /* app vector table: [0]=MSP initial, [1]=reset handler */
    app_stack        = *((volatile uint32_t *)(app_addr));
    app_reset_handler = *((volatile uint32_t *)(app_addr + 4));

    /* sanity check */
    if (app_stack == 0 || app_reset_handler == 0) {
        return;
    }

    /* disable global interrupts */
    __disable_irq();

    /* set vector table offset */
#if defined(SCB_BASE) && defined(SCB)
    SCB->VTOR = app_addr;
#else
    /* some platforms need VTOR set via system control block at 0xE000ED00 */
    *(volatile uint32_t *)0xE000ED08 = app_addr;
#endif
    __DSB();
    __ISB();

    /* set main stack pointer from app vector */
    __set_MSP(app_stack);

    /* jump to app reset handler */
    ((void (*)(void))app_reset_handler)();
}
