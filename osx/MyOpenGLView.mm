#include <OpenGL/gl.h>

#import "MyOpenGLView.h"

#include "texture.h"
#include "entity.h"
#include "objects.h"
#include "hud.h"
#include "text.h"
#include "game.h"

#include <OpenGL/glu.h>

@implementation MyOpenGLView

/*
	Override NSView's initWithFrame: to specify our pixel format:
	
	Note: initWithFrame is called only if a "Custom View" is used in Interface BBuilder 
	and the custom class is a subclass of NSView. For more information on resource loading
	see: developer.apple.com (ADC Home > Documentation > Cocoa > Resource Management > Loading Resources)
*/	

- (id) initWithFrame: (NSRect) frame
{
	GLuint attribs[] = 
	{
		NSOpenGLPFANoRecovery,
		NSOpenGLPFAWindow,
		NSOpenGLPFAAccelerated,
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFAColorSize, 24,
		NSOpenGLPFAAlphaSize, 8,
		NSOpenGLPFADepthSize, 24,
		NSOpenGLPFAStencilSize, 8,
		NSOpenGLPFAAccumSize, 0,
		0
	};

	NSOpenGLPixelFormat* fmt = [[NSOpenGLPixelFormat alloc] initWithAttributes: (NSOpenGLPixelFormatAttribute*) attribs]; 
	
	if (!fmt)
		NSLog(@"No OpenGL pixel format");

	return self = [super initWithFrame:frame pixelFormat: [fmt autorelease]];
}

- (void) prepareOpenGL
{
	texture_init();
	objects_init();
	text_init("helvetica_bold.txf",20);
	game_init("BlapOut/classic.wld");
	update_world(0);
	
	glEnable(GL_TEXTURE_2D);
	// Enable Texture Mapping
	glShadeModel(GL_FLAT/*GL_SMOOTH*/);
	// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);                           // Black Background
	glClearDepth(1.0f);
	// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);
	// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);
	// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, /*GL_NICEST*/GL_FASTEST);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, 640.0f / 480.0f, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_TEXTURE_2D);
	//        glDisable(GL_KOS_AUTO_UV);
	glDisable(GL_CULL_FACE);
	
	[super prepareOpenGL];
}

- (void) awakeFromNib
{
	timer = [[NSTimer
                scheduledTimerWithTimeInterval: (1.0f / 120.0f)
                                        target: self
																			selector: @selector( updateGame )
																			userInfo: nil
																			 repeats: YES] retain];
}

/*
	Override the view's drawRect: to draw our GL content.
*/	 

- (void) drawRect: (NSRect) rect
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	render_world_solid();
	glEnable(GL_BLEND);
	render_world_trans();
	glDisable(GL_BLEND);
	render_HUD();
	
	[[self openGLContext] flushBuffer];
}

- (void) updateGame
{
	update_world([timer timeInterval]);
	
	[self setNeedsDisplay: YES];
}

@end
