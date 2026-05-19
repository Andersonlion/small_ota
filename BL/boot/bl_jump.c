#include "bl_jump.h"

/*support armcc iar gcc*/
#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
    // ARM Compiler
    #include <cmsis_compiler.h>
    
#elif defined(__GNUC__) || defined(__clang__)
    // GCC/Clang (GNU 语法)
    #define __DSB()  __asm__ volatile ("dsb 0xF" ::: "memory")
    #define __ISB()  __asm__ volatile ("isb 0xF" ::: "memory")
    #define __set_MSP(val) __asm__ volatile ("msr msp, %0" :: "r"(val))
    #define __disable_irq() __asm__ volatile ("cpsid i" ::: "memory")
    
#elif defined(__ICCARM__)
    // IAR 编译器 (IAR 语法)
    #define __DSB()  asm("dsb 0xF")
    #define __ISB()  asm("isb 0xF")
    #define __set_MSP(val) asm("msr msp, %0" :: "r"(val))
    #define __disable_irq() asm("cpsid i")
    
#else
    #error "Unsupported compiler!"
#endif


/*support m3 m4 or more which support vtor*/
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
