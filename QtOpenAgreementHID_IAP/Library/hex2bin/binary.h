#ifndef _BINARY_H_
#define _BINARY_H_

#include <stdint.h>

#ifndef __cplusplus
typedef enum {false, true} bool;
#endif

#ifdef __cplusplus
extern "C" {
#endif

//typedef enum {false, true} bool;
//typedef enum {FALSE=0, TRUE} BOOL;

extern const unsigned char Reflect8[256];

extern uint16_t Reflect16(uint16_t Value16);
extern uint32_t Reflect24(uint32_t Value24);
extern uint32_t Reflect32(uint32_t Value32);
extern uint64_t Reflect40(uint64_t Value40);
extern uint64_t Reflect64(uint64_t Value64);

extern uint8_t u16_hi(uint16_t value);
extern uint8_t u16_lo(uint16_t value);

extern uint8_t u32_b3(uint32_t value);
extern uint8_t u32_b2(uint32_t value);
extern uint8_t u32_b1(uint32_t value);
extern uint8_t u32_b0(uint32_t value);

extern uint8_t u64_b7(uint64_t value);
extern uint8_t u64_b6(uint64_t value);
extern uint8_t u64_b5(uint64_t value);
extern uint8_t u64_b4(uint64_t value);
extern uint8_t u64_b3(uint64_t value);
extern uint8_t u64_b2(uint64_t value);
extern uint8_t u64_b1(uint64_t value);
extern uint8_t u64_b0(uint64_t value);

extern uint8_t nibble2ascii(uint8_t value);
extern bool cs_isdecdigit(char c);
extern unsigned char tohex(unsigned char c);
extern unsigned char todecimal(unsigned char c);

#ifdef __cplusplus
}
#endif

#endif /* _BINARY_H_ */
