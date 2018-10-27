#ifndef CROSSLIB_STRING_HEADER
#define CROSSLIB_STRING_HEADER

#include <stdint.h>
#include <stdbool.h>

#ifdef _WIN32
#include <stddef.h>
#include <wchar.h>
#endif

//==============================================================================
// FUNCTIONS
//==============================================================================
/// <summary>Convert a string from UTF16 to UTF8.</summary>
/// <param name="utf16">utf16 input source</param>
/// <returns>pointer to newly allocated, null terminated, utf8 string; or NULL on failure</returns>
/// <remarks>pointer returned must be freed by the caller</remarks>
char* clString_utf8from16(wchar_t* utf16);

/// <summary>Convert a string from UTF8 to UTF16.</summary>
/// <param name="utf8">utf8 input source</param>
/// <returns>pointer to newly allocated, null terminated, utf16 string; or NULL on failure</returns>
/// <remarks>pointer returned must be freed by the caller</remarks>
wchar_t* clString_utf16from8(char* utf8);

#endif //CROSSLIB_STRING_HEADER
