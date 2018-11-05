#include "crosslib/process.h"
#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#include <strings.h>

static bool execute(char* path, uint32_t argumentCount, char** argumentValues, char* workingPath) {
	NSTask* task = [[NSTask alloc] init];
	task.executableURL = [NSURL fileURLWithPath:[NSString stringWithUTF8String:path]];
	if(workingPath) {
		task.currentDirectoryURL = [NSURL fileURLWithPath:[NSString stringWithUTF8String:workingPath]];
	}
	NSMutableArray* arrayArguments = [[NSMutableArray alloc] initWithCapacity:argumentCount];
	for(uint32_t index=0; index<argumentCount; ++index) {
		[arrayArguments addObject:[NSString stringWithUTF8String:argumentValues[index]]];
	}
	task.arguments = arrayArguments;

	return !![task launchAndReturnError:NULL];
}

static bool launch(char* path, uint32_t argumentCount, char** argumentValues, char* workingPath) {
	NSMutableArray* arrayArguments = [[NSMutableArray alloc] initWithCapacity:argumentCount];
	for(uint32_t index=0; index<argumentCount; ++index) {
		[arrayArguments addObject:[NSString stringWithUTF8String:argumentValues[index]]];
	}

	NSMutableDictionary* configuration = [[NSMutableDictionary alloc] init];
	[configuration setValue:arrayArguments forKey:NSWorkspaceLaunchConfigurationArguments];

	NSRunningApplication* result = [[NSWorkspace sharedWorkspace]
		launchApplicationAtURL: [NSURL fileURLWithPath:[NSString stringWithUTF8String:path]]
                       options: NSWorkspaceLaunchDefault
 				 configuration: configuration
						 error: NULL
	];

	return (result != nil);
}

bool clProcess_runCommand(char* path, uint32_t argumentCount, char** argumentValues, char* workingPath) {
	uint32_t pathLen = strlen(path);
	if((pathLen > 4) && (strcasecmp(".app", path + pathLen - 4) == 0)) {
		return launch(path, argumentCount, argumentValues, workingPath);
	}
	return execute(path, argumentCount, argumentValues, workingPath);
}
