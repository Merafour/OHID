/*
---------------------------------------------------------------------------
  hex2bin converts an Intel hex file to binary.
---------------------------------------------------------------------------
*/

#define PROGRAM "hex2bin"
#define VERSION "2.5.1"

#include <string.h>
#include "hex2bin/common.h"

#define NO_ADDRESS_TYPE_SELECTED 0
#define LINEAR_ADDRESS 1
#define SEGMENTED_ADDRESS 2

//const char *Pgm_Name = PROGRAM;

static uint8_t bin[1024*1024];  // 1MB
struct file_serach {
  const char *const _text;  // 文件内容
  const int _stext;         // 文件大小
  int pos;                  // 当前文件指针位置
};

static char* serach_gets(char* const str, const int n, struct file_serach* const stream)
{
    int _end;
    int count=0;
    const char* const _text = stream->_text;
    char _byte=0;
    _end = stream->pos + n;
    memset(str, 0, (size_t)n);
    for(count=0; count<n; count++)
    {
        if(stream->pos>=stream->_stext) return  NULL;
        if(stream->pos>=_end) return  NULL;
        _byte = _text[stream->pos++];
        str[count] = _byte;
        if('\n'==_byte) break;
        if('\0'==_byte) break;
    }
    //str[count] = '\0';
    /*if(0==count)
    {
        printf("[%s-%d] stream->pos:%d stream->_stext:%d _end:%d _byte:0x%X\n", __func__, __LINE__, stream->pos, stream->_stext, _end, _byte);
        return  NULL;
    }*/
    return  str;
}

void CpyMemory(void)
{
    if ((Cks_Addr >= Lowest_Address) && (Cks_Addr < Highest_Address))
    {
        if(Force_Value)
        {
            switch (Cks_Type)
            {
                case 0:
                    Memory_Block[Cks_Addr - Lowest_Address] = Cks_Value;
                    fprintf(stdout,"Addr %08X set to %02X\n",Cks_Addr, Cks_Value);
                    break;
                case 1:
                    WriteMemBlock16(Cks_Value);
                    fprintf(stdout,"Addr %08X set to %04X\n",Cks_Addr, Cks_Value);
                    break;
                case 2:
                    WriteMemBlock32(Cks_Value);
                    fprintf(stdout,"Addr %08X set to %08X\n",Cks_Addr, Cks_Value);
                    break;
                default:;
            }
        }
        else if (Cks_Addr_set)
        {
            /* Add a checksum to the binary file */
            if (!Cks_range_set)
            {
                Cks_Start = Lowest_Address;
                Cks_End = Highest_Address;
            }
            /* checksum range MUST BE in the array bounds */

            if (Cks_Start < Lowest_Address)
            {
                fprintf(stdout,"Modifying range start from %X to %X\n",Cks_Start,Lowest_Address);
                Cks_Start = Lowest_Address;
            }
            if (Cks_End > Highest_Address)
            {
                fprintf(stdout,"Modifying range end from %X to %X\n",Cks_End,Highest_Address);
                Cks_End = Highest_Address;
            }

            crc_table = NULL;

            switch (Cks_Type)
            {
            case CHK8_SUM:
            {
                uint8_t wCKS = 0;

                for (unsigned int i=Cks_Start; i<=Cks_End; i++)
                {
                    wCKS += Memory_Block[i - Lowest_Address];
                }

                fprintf(stdout,"8-bit Checksum = %02X\n",wCKS & 0xff);
                Memory_Block[Cks_Addr - Lowest_Address] = wCKS;
                fprintf(stdout,"Addr %08X set to %02X\n",Cks_Addr, wCKS);
            }
            break;

            case CHK16:
            {
                uint16_t wCKS, w;

                wCKS = 0;

                if (Endian == 1)
                {
                    for (unsigned int i=Cks_Start; i<=Cks_End; i+=2)
                    {
                        w =  Memory_Block[i - Lowest_Address +1] | ((word)Memory_Block[i - Lowest_Address] << 8);
                        wCKS += w;
                    }
                }
                else
                {
                    for (unsigned int i=Cks_Start; i<=Cks_End; i+=2)
                    {
                        w =  Memory_Block[i - Lowest_Address] | ((word)Memory_Block[i - Lowest_Address +1] << 8);
                        wCKS += w;
                    }
                }
                fprintf(stdout,"16-bit Checksum = %04X\n",wCKS);
                WriteMemBlock16(wCKS);
                fprintf(stdout,"Addr %08X set to %04X\n",Cks_Addr, wCKS);
            }
            break;

            case CHK16_8:
            {
                uint16_t wCKS;

                wCKS = 0;

                for (unsigned int i=Cks_Start; i<=Cks_End; i++)
                {
                    wCKS += Memory_Block[i - Lowest_Address];
                }

                fprintf(stdout,"16-bit Checksum = %04X\n",wCKS);
                WriteMemBlock16(wCKS);
                fprintf(stdout,"Addr %08X set to %04X\n",Cks_Addr, wCKS);
            }
            break;

            case CRC8:
            {
                uint8_t CRC8;
                crc_table = NoFailMalloc(256);

                if (Crc_RefIn)
                {
                    init_crc8_reflected_tab(Reflect8[Crc_Poly]);
                    CRC8 = Reflect8[Crc_Init];
                }
                else
                {
                    init_crc8_normal_tab(Crc_Poly);
                    CRC8 = Crc_Init;
                }

                for (unsigned int i=Cks_Start; i<=Cks_End; i++)
                {
                    CRC8 = update_crc8(CRC8,Memory_Block[i - Lowest_Address]);
                }

                CRC8 = (CRC8 ^ Crc_XorOut) & 0xff;
                Memory_Block[Cks_Addr - Lowest_Address] = CRC8;
                fprintf(stdout,"Addr %08X set to %02X\n",Cks_Addr, CRC8);
            }
            break;

            case CRC16:
            {
                uint16_t CRC16;
                crc_table = NoFailMalloc(256 * 2);

                if (Crc_RefIn)
                {
                    init_crc16_reflected_tab(Reflect16(Crc_Poly));
                    CRC16 = Reflect16(Crc_Init);

                    for (unsigned int i=Cks_Start; i<=Cks_End; i++)
                    {
                        CRC16 = update_crc16_reflected(CRC16,Memory_Block[i - Lowest_Address]);
                    }
                }
                else
                {
                    init_crc16_normal_tab(Crc_Poly);
                    CRC16 = Crc_Init;


                    for (unsigned int i=Cks_Start; i<=Cks_End; i++)
                    {
                        CRC16 = update_crc16_normal(CRC16,Memory_Block[i - Lowest_Address]);
                    }
                }

                CRC16 = (CRC16 ^ Crc_XorOut) & 0xffff;
                WriteMemBlock16(CRC16);
                fprintf(stdout,"Addr %08X set to %04X\n",Cks_Addr, CRC16);
            }
            break;

            case CRC32:
            {
                uint32_t CRC32;

                crc_table = NoFailMalloc(256 * 4);
                if (Crc_RefIn)
                {
                    init_crc32_reflected_tab(Reflect32(Crc_Poly));
                    CRC32 = Reflect32(Crc_Init);

                    for (unsigned int i=Cks_Start; i<=Cks_End; i++)
                    {
                        CRC32 = update_crc32_reflected(CRC32,Memory_Block[i - Lowest_Address]);
                    }
                }
                else
                {
                    init_crc32_normal_tab(Crc_Poly);
                    CRC32 = Crc_Init;

                    for (unsigned int i=Cks_Start; i<=Cks_End; i++)
                    {
                        CRC32 = update_crc32_normal(CRC32,Memory_Block[i - Lowest_Address]);
                    }
                }

                CRC32 ^= Crc_XorOut;
                WriteMemBlock32(CRC32);
                fprintf(stdout,"Addr %08X set to %08X\n",Cks_Addr, CRC32);
            }
            break;

            default:
                ;
            }

            if (crc_table != NULL) free(crc_table);
        }
    }
    else
    {
        if(Force_Value || Cks_Addr_set)
        {
            fprintf (stderr,"Force/Check address outside of memory range\n");
        }
    }

    /* write binary file */
    //fwrite (Memory_Block, Max_Length, 1, Filout);

    free (Memory_Block);

    // Minimum_Block_Size is set; the memory buffer is multiple of this?
    if (Minimum_Block_Size_Setted==true)
    {
        Module = Max_Length % Minimum_Block_Size;
        if (Module)
        {
            Module = Minimum_Block_Size - Module;
            Memory_Block = (byte *) NoFailMalloc(Module);
            memset (Memory_Block,Pad_Byte,Module);
            //fwrite (Memory_Block, Module, 1, Filout);
            free (Memory_Block);
            if (Max_Length_Setted==true)
                fprintf(stdout,"Attention Max Length changed by Minimum Block Size\n");
            // extended
            Max_Length += Module;
            Highest_Address += Module;
            fprintf(stdout,"Extended\nHighest address:  %08X\n",Highest_Address);
            fprintf(stdout,"Max Length:       %u\n\n",Max_Length);

        }
    }
}

extern int hex2bin_main (const int argc, const char *const argv[]);

int hex2bin_test (const char path[])
{
    //const char *const argv[] = { "./hex2bin", "-a", "512", path};
    const char *const argv[] = { "./hex2bin", path};
    int argc = sizeof(argv)/sizeof(argv[0]);
    fprintf(stdout,"path: %s\n\n", path);
    return hex2bin_main (argc, argv);
}
