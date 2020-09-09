#ifndef IAP_PORT_H_
#define IAP_PORT_H_

// STM32L479VEX
#define FAL_FLASH_PORT_DRIVER_STM32L4
#include "sys.h"

/** Flash API

*/

u32 flash_read_word(u32 faddr);
void flash_port_read(uint32_t faddr, uint8_t *buf, int size);
int flash_port_write(uint32_t faddr, uint8_t *buf, int size);


#endif
