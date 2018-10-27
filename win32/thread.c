#include "crosslib/thread.h"
#include <Windows.h>
#include <process.h>

//==============================================================================
// THREAD FUNCTIONS
//==============================================================================
bool clThread_threadCreate(clThread_ThreadFunction function, void* data, clThread_ThreadHandle* thread) {
	*thread = NULL;
	uintptr_t result = _beginthread(function, 0, data);
	if(result == -1L) { return false; }

	*thread = (clThread_ThreadHandle*)result;
	return true;
}

void clThread_threadReturn() {
	_endthreadex(0);
}

bool clThread_threadWait(clThread_ThreadHandle thread) {
	if(WaitForSingleObject(thread, INFINITE) == WAIT_FAILED) { return false; }
	CloseHandle(thread); // exception during debugging? See CloseHandle() documentation. Thread has stopped, but still needs closed.
	return true;
}

void clThread_threadFree(clThread_ThreadHandle thread) {
	// nothing to do for windows (shouldn't close handle until joined, and no extra data stored in handle pointer)
}

//==============================================================================
// MUTEX FUNCTIONS
//==============================================================================
bool clThread_mutexCreate(clThread_MutexHandle* mutex) {
	*mutex = NULL;
	HANDLE result = CreateMutexW(NULL, FALSE, NULL);
	if(!result) { return false; }

	*mutex = (clThread_MutexHandle*)result;
	return true;
}

bool clThread_mutexLock(clThread_MutexHandle mutex) {
	return (WaitForSingleObject((HANDLE)mutex, INFINITE) != WAIT_FAILED);
}

bool clThread_mutexTryLock(clThread_MutexHandle mutex, uint32_t milliseconds) {
	DWORD result = WaitForSingleObject((HANDLE)mutex, (DWORD)milliseconds);
	return ((result != WAIT_FAILED) && (result != WAIT_TIMEOUT));
}

void clThread_mutexUnlock(clThread_MutexHandle mutex) {
	ReleaseMutex((HANDLE)mutex);
}

void clThread_mutexFree(clThread_MutexHandle mutex) {
	CloseHandle((HANDLE)mutex);
}

//==============================================================================
// OTHER FUNCTIONS
//==============================================================================
void clThread_sleep(uint32_t milliseconds) {
	Sleep((DWORD)milliseconds);
}
