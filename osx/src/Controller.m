/* Controller.m - Cocoa window controller
 * Copyright (c) 2001-2005 Sam Steele
 *
 * This file is part of DCBlap.
 *
 * DCBlap is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * DCBlap is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */
#import "Controller.h"
#import <Tiki/TikiMain.h>
#import <assert.h>

void tiki_main();

@implementation Controller

- (void) applicationDidFinishLaunching: (NSNotification *) note
{
	//NSString * resPath = [[NSBundle mainBundle] resourcePath];
	NSString * resPath = [[NSBundle mainBundle] pathForResource:@"data" ofType:nil];
	chdir([resPath cString]);
	//printf("%s\n",[resPath cString]);
	TikiMain * otm = [[TikiMain alloc] retain];
	tm = otm;
	[tm doMainWithWindow: mainWindow andView: mainView andMainFunc: tiki_main];
	tm = nil;
	[otm release];
}

- (NSApplicationTerminateReply) applicationShouldTerminate: (NSApplication *)sender
{
	assert( tm );
	[tm quitSoon];
	return NSTerminateNow;
}

@end
