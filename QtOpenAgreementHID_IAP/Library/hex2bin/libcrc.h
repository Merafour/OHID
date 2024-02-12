    /********************************************************************
    *                                                                   *
    *   Library         : lib_crc                                       *
    *   File            : lib_crc.h                                     *
    *   Author          : Lammert Bies  1999-2008                       *
    *   E-mail          : info@lammertbies.nl                           *
    *   Language        : ANSI C                                        *
    *                                                                   *
    *                                                                   *
    *   Description                                                     *
    *   ===========                                                     *
    *                                                                   *
    *   The file lib_crc.h contains public definitions  and  proto-     *
    *   types for the CRC functions present in lib_crc.c.               *
    *                                                                   *
    *                                                                   *
    *   Dependencies                                                    *
    *   ============                                                    *
    *                                                                   *
    *   none                                                            *
    *                                                                   *
    *                                                                   *
    ********************************************************************/
#ifndef _LIBCRC_H_
#define _LIBCRC_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void *crc_table;
//extern char *crc_table;

extern void init_crc8_normal_tab(uint8_t polynom);
extern void init_crc8_reflected_tab(uint8_t polynom);

extern void init_crc16_normal_tab(uint16_t polynom);
extern void init_crc16_reflected_tab(uint16_t polynom);
extern void init_crc32_normal_tab(uint32_t polynom);
extern void init_crc32_reflected_tab(uint32_t polynom);

extern uint8_t update_crc8(uint8_t crc, uint8_t c);

extern uint16_t update_crc16_normal(uint16_t crc, char c );
extern uint16_t update_crc16_reflected(uint16_t crc, char c );
extern uint32_t update_crc32_normal(uint32_t crc, char c );
extern uint32_t update_crc32_reflected(uint32_t crc, char c );

#ifdef __cplusplus
}
#endif

#endif /* _LIBCRC_H_ */
