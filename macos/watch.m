#include "crosslib/watch.h"

void changeHookProxy(void* data) {
}

bool clWatch_changeHookCreate(char* path, clWatch_ChangeFunction callback, void* userData, clWatch_ChangeHook** hook) {
    return false;
}

void clWatch_changeHookFree(clWatch_ChangeHook* hook) {
}
