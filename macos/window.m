#include "crosslib/window.h"
#include <string.h>
#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

//==============================================================================
// ACTIVE WINDOW READ
//==============================================================================
bool clWindow_activeWindowRead(clWindow_ActiveWindow* window) {
	NSRunningApplication* activeApp = [[NSWorkspace sharedWorkspace] frontmostApplication];
    window->pid     = [activeApp processIdentifier];
    window->title   = NULL;
    window->program = NULL;

    NSArray* windows = (__bridge_transfer NSArray*)(CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenOnly | kCGWindowListExcludeDesktopElements, kCGNullWindowID));
    for(NSDictionary* current in windows) {
        if(window->pid == [current[(__bridge NSString*)(kCGWindowOwnerPID)] intValue]) {
            NSString* title  = current[(__bridge NSString*)(kCGWindowName)];
            NSString* bundle = [[NSRunningApplication runningApplicationWithProcessIdentifier:window->pid] bundleIdentifier];
            if(title)  { window->title   = strdup([title  UTF8String]); }
            if(bundle) { window->program = strdup([bundle UTF8String]); }
            return true;
        }
    }

    return true;
}

void clWindow_activeWindowFree(clWindow_ActiveWindow* window) {
	if(window->title)   { free(window->title);   window->title   = NULL; }
	if(window->program) { free(window->program); window->program = NULL; }
}

//==============================================================================
// ACTIVE WINDOW HOOK
//==============================================================================
@interface ActiveWindowHook : NSObject
-(id)init;
-(bool)listenStart:(clWindow_ActiveWindowCallback)callback;
-(bool)listenStop;
-(bool)listenIsActive;
-(void)activeChanged:(NSNotification*)notification;
@end
@implementation ActiveWindowHook {
	bool running;
	clWindow_ActiveWindowCallback callback;
}
-(id)init {
	self = [super init];
	running  = false;
	callback = NULL;
	return self;
}

-(bool)listenStart:(clWindow_ActiveWindowCallback)userCallback {
	if(running) { return false; }
	callback = userCallback;
	[[[NSWorkspace sharedWorkspace] notificationCenter] addObserver:self selector:@selector(activeChanged:) name:NSWorkspaceDidActivateApplicationNotification object:nil];
	running = true;
	return true;
}

-(bool)listenStop {
	if(!running) { return false; }
	[[[NSWorkspace sharedWorkspace] notificationCenter] removeObserver:self name:NSWorkspaceDidActivateApplicationNotification object:nil];
	running  = false;
	callback = NULL;
	return true;
}

-(bool)listenIsActive {
	return running;
}

-(void)activeChanged:(NSNotification *)notification {
	static clWindow_ActiveWindow active;
	if(!clWindow_activeWindowRead(&active)) { return; }
	if(callback) { callback(&active); }
	clWindow_activeWindowFree(&active);
}
@end

static ActiveWindowHook* globalHook = NULL;

bool clWindow_activeHookStart(clWindow_ActiveWindowCallback callback) {
	if(!callback) { return false; }
	if(!globalHook) { globalHook = [[ActiveWindowHook alloc] init]; }
	return [globalHook listenStart:callback];
}

bool clWindow_activeHookRunning(void) {
	if(!globalHook) { return false; }
	return [globalHook listenIsActive];
}

bool clWindow_activeHookStop(void) {
	if(!globalHook) { return false; }
	return [globalHook listenStop];
}

//==============================================================================
// GUI MAIN LOOP FUNCTIONS
//==============================================================================
bool clWindow_guiLoopBegin(void* guiData) {
	[NSApplication sharedApplication];
	[NSApp setActivationPolicy:NSApplicationActivationPolicyProhibited];
	[NSApp run];
	return true;
}

void clWindow_guiLoopStop(void) {
	[[NSApplication sharedApplication] terminate:nil];
}
