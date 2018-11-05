#include "crosslib/thread.h"
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

typedef struct struct_NativeThread {
	pthread_t               handle;
	clThread_ThreadFunction function;
	void*                   userData;
} NativeThread;

static void* threadProxy(void* data) {
	NativeThread* nativeThread = (NativeThread*)data;
	nativeThread->function(nativeThread->userData);
	return NULL;
}

//==============================================================================
// THREAD FUNCTIONS
//==============================================================================
bool clThread_threadCreate(clThread_ThreadFunction function, void* data, clThread_ThreadHandle* thread) {
	if(!function) { return false; }

	NativeThread* nativeThread = malloc(sizeof(NativeThread));
	nativeThread->function = function;
	nativeThread->userData = data;
	*thread = nativeThread;

	int threadCreateResult = pthread_create(&(nativeThread->handle), NULL, threadProxy, nativeThread);

	if(threadCreateResult != 0) {
		*thread = NULL;
		free(nativeThread);
		return false;
	}
	return true;
}

void clThread_threadReturn(void) {
	// don't need pthread_exit(), we always return NULL from threadProxy
}

bool clThread_threadWait(clThread_ThreadHandle thread) {
	NativeThread* nativeThread = (NativeThread*)thread;
	pthread_join(nativeThread->handle, NULL);
	return true;
}

void clThread_threadFree(clThread_ThreadHandle thread) {
	free(thread); // 'thread' is our malloced NativeThread structure
}

//==============================================================================
// MUTEX FUNCTIONS
//==============================================================================
bool clThread_mutexCreate(clThread_MutexHandle* mutex) {
	pthread_mutex_t* nativeMutex = malloc(sizeof(pthread_mutex_t));
	if(pthread_mutex_init(nativeMutex, NULL) != 0) { free(nativeMutex); return false; }
	*mutex = nativeMutex;
	return true;
}

bool clThread_mutexLock(clThread_MutexHandle mutex) {
	return (pthread_mutex_lock(mutex) == 0);
}

static int pthread_mutex_timedlock_macos(pthread_mutex_t* mutex, struct timespec* timeout, uint32_t milliseconds) {
	struct timespec now;
	int attemptResult = -1;
	while(attemptResult != 0) {
		attemptResult = pthread_mutex_trylock(mutex);
		if(attemptResult == 0) { return 0; }

		clock_gettime(CLOCK_REALTIME, &now);
		if((now.tv_sec > timeout->tv_sec) || ((now.tv_sec == timeout->tv_sec) && (now.tv_nsec > timeout->tv_nsec))) { return -1; }

		if(milliseconds > 100) {
			clThread_sleep(100);
			milliseconds -= 100;
		} else if(milliseconds > 10) {
			clThread_sleep(10);
			milliseconds -= 10;
		} else {
			clThread_sleep(1);
			milliseconds -= 1;
		}
	}
	return attemptResult;
}

bool clThread_mutexTryLock(clThread_MutexHandle mutex, uint32_t milliseconds) {
	struct timespec timeout;
	clock_gettime(CLOCK_REALTIME, &timeout);
	uint32_t mod = milliseconds % 1000;
	milliseconds -= mod;
	timeout.tv_sec  += (milliseconds / 1000);
	timeout.tv_nsec += mod * 1000000;

	return (pthread_mutex_timedlock_macos(mutex, &timeout, milliseconds) == 0);
}

void clThread_mutexUnlock(clThread_MutexHandle mutex) {
	pthread_mutex_unlock(mutex);
}

void clThread_mutexFree(clThread_MutexHandle mutex) {
	free(mutex);
}

//==============================================================================
// OTHER FUNCTIONS
//==============================================================================
void clThread_sleep(uint32_t milliseconds) {
	usleep(milliseconds * 1000);
}
