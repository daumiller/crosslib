#include <stdio.h>
#include <stdlib.h>
#include "crosslib/string.h"

int main(int argc, char** argv) {
    char* input     = "Hello MacOS World!";
    char16_t* utf16 = clString_utf16from8(input);
    char* utf8      = clString_utf8from16(utf16);

    //printf(  "Input: \"%s\"", input); printf("\n");
    //wprintf(L"UTF16: \"%S\"", utf16); printf("\n");
    //printf(  "UTF8 : \"%s\"", utf8);  printf("\n");
    
    printf("08 (%d): %04X,%04X,%04X,%04X,...\n", (int)sizeof(char),     utf8[0],  utf8[1],  utf8[2],  utf8[3]);
    printf("16 (%d): %04X,%04X,%04X,%04X,...\n", (int)sizeof(char16_t), utf16[0], utf16[1], utf16[2], utf16[3]);

    //printf("\n");
    //wprintf(L"%C,%C,%C...", utf16[0], utf16[1], utf16[2]);
    //printf("\n");

    free(utf16);
    free(utf8);
    return 0;
}
