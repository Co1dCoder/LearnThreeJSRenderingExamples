//
//  ViewController.m
//  ArcballProj
//
//  Created by SeanRen on 2019/1/11.
//  Copyright © 2019 SeanRen. All rights reserved.
//

#import "ViewController.h"
#include "NativeTemplate.h"

#include "perfMonitor.h"

extern float zTranslationDistance;

@interface ViewController () {
    UITextField *textField;
    UILabel *sliderLabel;
    UISlider *localClipPlaneSlider;
    OpenGL_Helper::PerfMonitor perfMonitor;
}
@property (strong, nonatomic) EAGLContext *context;

- (void)setupGL;
- (void)tearDownGL;
- (void)doSliding:(id)sender;

@end

@implementation ViewController

- (void)dealloc
{
    [self tearDownGL];
    
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
    
    //    [_context release];
    //    [super dealloc];
}

- (BOOL)shouldAutorotate{
    [EAGLContext setCurrentContext:self.context];
    
//    //  not accurate
//    float scaleFactor = self.view.layer.contentsScale;
    float scaleFactor = [UIScreen mainScreen].nativeScale;
    
    GraphicsResize(self.view.bounds.size.width * scaleFactor, self.view.bounds.size.height * scaleFactor);
    
    return true;
}

////重载函数的方式无法获得60FPS的frame rate，此设置无效
//-(NSInteger)preferredFramesPerSecond{
//    return 60;
//}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
    
    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    view.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;  //默认格式
    
    self.preferredFramesPerSecond = 60;
    
    textField = [[UITextField alloc] initWithFrame:CGRectMake(10, 20, 80, 24)];
    [textField setFont:[UIFont systemFontOfSize:16]];
    [textField setTextColor:[UIColor cyanColor]];
    [view addSubview:textField];
    
    localClipPlaneSlider = [[UISlider alloc] initWithFrame:CGRectMake(100, 20, 120, 24)];
    [localClipPlaneSlider setValue:zTranslationDistance];
    localClipPlaneSlider.minimumValue = -15.0;// 设置最小值
    localClipPlaneSlider.maximumValue = 4.0;// 设置最大值
    localClipPlaneSlider.continuous = YES;
    [localClipPlaneSlider addTarget:self action:@selector(doSliding:) forControlEvents:UIControlEventValueChanged];// 针对值变化添加响应方法
    [view addSubview:localClipPlaneSlider];
    
    sliderLabel = [[UILabel alloc] initWithFrame:CGRectMake(230, 20, 120, 24)];
    [sliderLabel setFont:[UIFont systemFontOfSize:16]];
    [sliderLabel setText:[NSString stringWithFormat:@"z轴平移 %.1f",zTranslationDistance]];
    [sliderLabel setTextColor:[UIColor cyanColor]];
    [view addSubview:sliderLabel];
    
    [self setupGL];
}

- (void)setupGL
{
    [EAGLContext setCurrentContext:self.context];
    
    //Optional code to demonstrate how can you bind frame buffer and render buffer.
    GLint defaultFBO;
    GLint defaultRBO;
    
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFBO);
    glGetIntegerv(GL_RENDERBUFFER_BINDING, &defaultRBO);
    
    glBindFramebuffer( GL_FRAMEBUFFER, defaultFBO );
    glBindRenderbuffer( GL_RENDERBUFFER, defaultRBO );
    
    GraphicsInit();
    
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch;
    CGPoint pos;
    
    for( touch in touches )
        {
        pos = [ touch locationInView:self.view ];
        
        TouchEventDown( pos.x, pos.y,touch.tapCount,true );
        }
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch;
    CGPoint pos;
    
    for( touch in touches )
        {
        pos = [ touch locationInView:self.view ];
        TouchEventMove( pos.x, pos.y,touches.count );
        }
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch;
    CGPoint pos;
    
    for( touch in touches )
        {
        pos = [ touch locationInView:self.view ];
        
        TouchEventRelease( pos.x, pos.y,touch.tapCount,false );
        }
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    GraphicsRender();
    float fps;
    perfMonitor.Update(fps);
    [textField setText:[NSString stringWithFormat:@"FPS %.2f",fps]];
    [sliderLabel setText:[NSString stringWithFormat:@"z轴平移 %.1f",zTranslationDistance]];
    
}

- (void)doSliding:(id)sender {
    zTranslationDistance = [(UISlider *)sender value];
}


@end
