#ifndef _BL_MAIN_H_
#define _BL_MAIN_H_

/**
 * Bootloader entry point.
 * Called from startup assembly after basic HW init (clock, RAM).
 * Returns only if no app is bootable (unlikely).
 */
void bl_main(void);

#endif /* _BL_MAIN_H_ */
