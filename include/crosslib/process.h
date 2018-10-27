#ifndef CROSSLIB_PROCESS_HEADER
#define CROSSLIB_PROCESS_HEADER

#include <stdint.h>
#include <stdbool.h>

//==============================================================================
// FUNCTIONS
//==============================================================================\
/// <summary>Run a command.</summary>
/// <param name="path">path of command to run</param>
/// <param name="argumentCount">number of arguments to pass</param>
/// <param name="argumentValues">values of argument to pass</param>
/// <param name="workingPath">path of working directory, or NULL to inherit</param>
/// <returns>boolean indicating success</returns>
/// <remarks>Function does not wait for command to complete. On MacOS, will launch bundle if given a path ending in '.app'.</remarks>
bool clProcess_runCommand(char* path, uint32_t argumentCount, char** argumentValues, char* workingPath);

#endif //CROSSLIB_PROCESS_HEADER
