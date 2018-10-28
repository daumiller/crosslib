#import <Foundation/Foundation.h>
#include <string.h>
#include "crosslib/string.h"

static int u16len(char16_t* string) {
    int length = 0;
    while(string[length]) { ++length; }
    return length;
}

static char16_t* u16dup(char16_t* string) {
    int byteLength = (u16len(string) + 1) * sizeof(char16_t);
    char16_t* copy = malloc(byteLength);
    memcpy(copy, string, byteLength);
    return copy;
}

static char16_t* u16bom(char16_t* string) {
    int byteLength = (u16len(string) + 1) * sizeof(char16_t);
    char16_t* copy = malloc(byteLength + sizeof(char16_t));
    memcpy(((uint8_t*)copy)+sizeof(char16_t), string, byteLength);
    copy[0] = 0xFEFF;
    return copy;
}

char* clString_utf8from16(char16_t* utf16) {
    char16_t* bom = u16bom(utf16); // [NSString initWithBytes:encoding:NSUTF16StringEncoding] requires a leading BOM
    NSString* string = [[NSString alloc] initWithBytes:(uint8_t*)bom length:(u16len(bom) * sizeof(char16_t)) encoding:NSUTF16StringEncoding];
    free(bom);
    if(!string) { return NULL; }
    NSData* data = [string dataUsingEncoding:NSUTF8StringEncoding];
    return strdup((char*)[data bytes]);
}

char16_t* clString_utf16from8(char* utf8) {
    NSString* string = [[NSString alloc] initWithBytes:utf8 length:strlen(utf8) encoding:NSUTF8StringEncoding];
    if(!string) { return NULL; }
    NSData* data = [string dataUsingEncoding:NSUTF16StringEncoding];
    return u16dup((char16_t*)([data bytes]+sizeof(char16_t))); // offset here -- ignore BOM prepended by NSString
}
