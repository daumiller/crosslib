#ifndef CROSSLIB_THREAD_HEADER
#define CROSSLIB_THREAD_HEADER

#include <stdint.h>
#include <stdbool.h>

//==============================================================================
// TYPES
//==============================================================================
/// <summary>Platform-specific thread handle.</summary>
typedef void* clThread_ThreadHandle;
/// <summary>Platform-specific mutex handle.</summary>
typedef void* clThread_MutexHandle;
/// <summary>Thread main function.</summary>
typedef void (*clThread_ThreadFunction)(void* data);

//==============================================================================
// THREAD FUNCTIONS
//==============================================================================
/// <summary>Create and start a new thread.</summary>
/// <param name="function">function for thread to run</param>
/// <param name="data">data passed to the thread function</param>
/// <param name="thread">location to store thread handle in</param>
/// <returns>boolean indicating success</returns>
/// <remarks>Given thread function should end with a call to <see cref="clThread_threadReturn">clThread_threadReturn</see></remarks>
bool clThread_threadCreate(clThread_ThreadFunction function, void* data, clThread_ThreadHandle* thread);

/// <summary>Call before returning from a thread function.</summary>
/// <remarks>This function will not stop a thread, but should be called immediately before ending the thread function.</remarks>
void clThread_threadReturn(void);

/// <summary>Wait for given thread to complete (join thread).</summary>
/// <param name="thread">thread to wait for</param>
/// <returns>boolean indicating success</returns>
/// <remarks>Blocks until the given thread stops.</remarks>
bool clThread_threadWait(clThread_ThreadHandle thread);

/// <summary>Free data from a created thread.</summary>
/// <param name="thread">thread to clean up</param>
/// <remarks>Do not reuse thead handle after calling this function.</remarks>
void clThread_threadFree(clThread_ThreadHandle thread);

//==============================================================================
// MUTEX FUNCTIONS
//==============================================================================
/// <summary>Create a new mutex.</summary>
/// <param name="mutex">location to store mutex handle in</param>
/// <returns>boolean indicating success</returns>
bool clThread_mutexCreate(clThread_MutexHandle* mutex);

/// <summary>Lock a mutex.</summary>
/// <param name="mutex">mutex to lock</param>
/// <returns>boolean indicating success</returns>
/// <remarks>Blocks until the mutex can be locked.</remarks>
bool clThread_mutexLock(clThread_MutexHandle mutex);

/// <summary>Attempt to lock a mutex.</summary>
/// <param name="mutex">mutex to lock</param>
/// <param name="milliseconds">maximum amount of time, in milliseconds, to attempt to lock</param>
/// <returns>boolean indicating success</returns>
/// <remarks>Blocks until either, the mutex is locked, or the maximum time is reached.</remarks>
bool clThread_mutexTryLock(clThread_MutexHandle mutex, uint32_t milliseconds);

/// <summary>Unlock a mutex.</summary>
/// <param name="mutex">mutex to unlock</param>
/// <remarks>Mutex must have been previous locked by the calling thread.</remarks>
void clThread_mutexUnlock(clThread_MutexHandle mutex);

/// <summary>Free data from a created mutex.</summary>
/// <param name="mutex">mutex to clean up</param>
/// <remarks>Do not reuse mutex handle after calling this function.</remarks>
void clThread_mutexFree(clThread_MutexHandle mutex);

//==============================================================================
// OTHER FUNCTIONS
//==============================================================================
/// <summary>Sleep current thread.</summary>
/// <param name="milliseconds">time, in milliseconds, to sleep for</param>
/// <remarks>Blocks current thread for specified duration.</remarks>
void clThread_sleep(uint32_t milliseconds);

#endif //CROSSLIB_THREAD_HEADER
