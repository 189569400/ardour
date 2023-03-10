#include <CoreFoundation/CFLocale.h>
#import  <CoreFoundation/CFString.h>
#import  <Foundation/NSString.h>
#import  <Foundation/NSURL.h>
#import  <Foundation/NSAutoreleasePool.h>
#import  <AppKit/NSWorkspace.h>

bool
cocoa_open_url (const char* uri)
{
	NSString* struri = [[NSString alloc] initWithUTF8String:uri];
	NSURL* nsurl = [[NSURL alloc] initWithString:struri];

	bool ret = [[NSWorkspace sharedWorkspace] openURL:nsurl];

	[struri release];
	[nsurl release];

	return ret;
}

bool
cocoa_open_folder (const char* path)
{
	NSString* struri = [[NSString alloc] initWithUTF8String:path];
	bool rv = [[NSWorkspace sharedWorkspace] selectFile:nil inFileViewerRootedAtPath:struri];
	[struri release];
	return rv;
}
