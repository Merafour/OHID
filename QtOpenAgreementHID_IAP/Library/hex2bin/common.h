#ifndef _HEX2BIN_COMMON_H_
#define _HEX2BIN_COMMON_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "binary.h"
#include "libcrc.h"

#ifdef __cplusplus
extern "C" {
#endif

/* To compile with Microsoft Visual Studio */
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

//#if defined(_WIN32) || defined(_WIN64)
//#include <windows.h>
//#endif /* _WIN32 */

/* option character */
#if defined(MSDOS) || defined(__DOS__) || defined(__MSDOS__) || defined(_MSDOS)
#define _IS_OPTION_(x)	 (((x) == '-') || ((x) == '/'))
#else
/* Assume unix and similar */
/* We don't accept an option beginning with a '/' because it could be a file name. */
#define _IS_OPTION_(x)	 ((x) == '-')
#endif

/* We use buffer to speed disk access. */
#ifdef USE_FILE_BUFFERS
#define BUFFSZ 4096
#endif

/* FIXME how to get it from the system/OS? */
#define MAX_FILE_NAME_SIZE 260

#ifdef DOS
#define MAX_EXTENSION_SIZE 4
#else
#define MAX_EXTENSION_SIZE 16
#endif

/* The data records can contain 255 bytes: this means 512 characters. */
#define MAX_LINE_SIZE 1024

typedef char filetype[MAX_FILE_NAME_SIZE];
typedef unsigned char byte;
typedef unsigned short word;

#define LAST_CHECK_METHOD 5

typedef enum Crc
{
    CHK8_SUM =0,
    CHK16,
    CRC8,
    CRC16,
    CRC32,
    CHK16_8
} t_CRC;

extern const char *Pgm_Name;
extern void usage(void);
extern void DisplayCheckMethods(void);

extern void *NoFailMalloc (size_t size);
extern void NoFailOpenInputFile (char *Flnm);
extern void NoFailOpenOutputFile (char *Flnm);
extern void GetLine(char* str,FILE *in);
extern int GetBin(const char *str);
extern int GetDec(const char *str);
extern int GetHex(const char *str);
extern bool GetBoolean(const char *str);
extern void GetFilename(char *dest, const char *const src);
extern void GetExtension(const char *str,char *ext);
extern void PutExtension(char *Flnm, char *Extension);

extern filetype    Filename;           /* string for opening files */
extern char        Extension[MAX_EXTENSION_SIZE];       /* filename extension for output files */

extern FILE        *Filin,             /* input files */
            *Filout;            /* output files */

#ifdef USE_FILE_BUFFERS
extern char		*FilinBuf,          /* text buffer for file input */
            *FiloutBuf;         /* text buffer for file output */
#endif

extern int Pad_Byte;
extern bool Enable_Checksum_Error;
extern bool Status_Checksum_Error;
extern byte 	Checksum;
extern unsigned int Record_Nb;
extern unsigned int Nb_Bytes;

/* This will hold binary codes translated from hex file. */
extern byte *Memory_Block;
extern unsigned int Lowest_Address, Highest_Address;
extern unsigned int Starting_Address, Phys_Addr;
extern unsigned int Records_Start; // Lowest address of the records
extern unsigned int Max_Length;
extern unsigned int Minimum_Block_Size;
extern unsigned int Ceiling_Address;
extern unsigned int Floor_Address;
extern int Module;
extern bool Minimum_Block_Size_Setted;
extern bool Starting_Address_Setted;
extern bool Floor_Address_Setted;
extern bool Ceiling_Address_Setted;
extern bool Max_Length_Setted;
extern bool Swap_Wordwise;
extern bool Address_Alignment_Word;
extern bool Batch_Mode;
extern bool Verbose_Flag;

extern int Endian;

extern t_CRC Cks_Type;
extern unsigned int Cks_Start, Cks_End, Cks_Addr, Cks_Value;
extern bool Cks_range_set;
extern bool Cks_Addr_set;
extern bool Force_Value;

extern unsigned int Crc_Poly, Crc_Init, Crc_XorOut;
extern bool Crc_RefIn;
extern bool Crc_RefOut;

extern void VerifyChecksumValue(void);
extern void VerifyRangeFloorCeil(void);
extern void CrcParamsCheck(void);
extern void WriteMemBlock16(uint16_t Value);
extern void WriteMemBlock32(uint32_t Value);
extern void WriteMemory(void);
extern void Allocate_Memory_And_Rewind(void);
extern char *ReadDataBytes(char *p);
extern void ParseOptions(const int argc, const char *const argv[]);

#ifdef __cplusplus
}
#endif

#endif // _HEX2BIN_COMMON_H_
