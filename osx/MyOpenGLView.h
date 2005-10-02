#import <Cocoa/Cocoa.h>

typedef enum
{
	redIndex,
	greenIndex,
	blueIndex,
	alphaIndex
} ClearColors;

@interface MyOpenGLView : NSOpenGLView
{
	ClearColors color_index;
}

- (IBAction) setClearColor: (id) sender;

@end
