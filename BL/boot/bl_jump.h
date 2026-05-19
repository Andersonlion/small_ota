#ifndef _BL_JUMP_H_
#define _BL_JUMP_H_

#include <stdint.h>

/**
 * Jump to application at specified address.
 * Disables interrupts, sets VTOR, MSP from app vector, then jumps.
 * No and never returns
 *
 * @param app_addr  start address of app partition
 */
void bl_jump_to_app(uint32_t app_addr);

#endif /* _BL_JUMP_H_ */
