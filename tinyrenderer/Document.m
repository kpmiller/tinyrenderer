//
//  Document.m
//  tinyrenderer
//
//  Created by Kent Miller on 5/30/24.
//

#import "Document.h"
#import "tinyrenderer.h"

@interface Document ()

@property TRcontext *ctx;
@property NSTimer *timer;
@property (atomic) bool updating;

@end

@implementation Document

- (instancetype)init {
    self = [super init];
    if (self) {
    }
    self.ctx = CreateTinyRenderer(1024,512);
    self.updating = NO;
    return self;
}

+ (BOOL)autosavesInPlace {
    return YES;
}

-(void) windowControllerDidLoadNib:(NSWindowController *)windowController
{
    /*
    self.timer = [NSTimer timerWithTimeInterval:0.2f target:self selector:@selector(updateImage) userInfo:nil repeats:YES];

    [[NSRunLoop currentRunLoop] addTimer:self.timer forMode:NSDefaultRunLoopMode];

//    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        Draw(self.ctx);
//    });
     */
    
    SetClearColor(self.ctx, 0.0, 0.2, 0.3, 1.0);
    Clear(self.ctx);
    Draw(self.ctx);
    self.ctx->changed = 1;
    [self updateImage];
}

- (NSString *)windowNibName {
    // Override returning the nib file name of the document
    // If you need to use a subclass of NSWindowController or if your document supports multiple NSWindowControllers, you should remove this method and override -makeWindowControllers instead.
    return @"Document";
}


- (BOOL)writeToURL:(NSURL *)url ofType:(NSString *)typeName error:(NSError * _Nullable __autoreleasing *)outError
{

    CGImageRef cgRef = [self.image.image CGImageForProposedRect:NULL
                                             context:nil
                                               hints:nil];
    NSBitmapImageRep *newRep = [[NSBitmapImageRep alloc] initWithCGImage:cgRef];
    [newRep setSize:self.image.image.size];   // if you want the same resolution
    NSData *pngData = [newRep representationUsingType:NSBitmapImageFileTypePNG properties: @{}];
    [pngData writeToURL:url atomically:YES];

    return YES;
}

-(void) updateImage
{
    if (self.updating)
        return;

    if ((self.ctx == NULL) || (self.ctx->data == NULL))
    {
        self.image.image = nil;
        return;
    }

    if (self.ctx->changed == 0)
        return;
    self.updating = YES;
    self.ctx->changed = 0;

    int w = self.ctx->width;
    int h = self.ctx->height;

    uint8_t *pixels = (uint8_t*) malloc(w*h*4);
    memset(pixels, 0x334455FF, w*h*4);
    uint8_t *p = pixels;
    float *f = (float*) self.ctx->data;
    for (int II=0 ; II < (w*h); II++)
    {
        p[0] = (uint8_t) (f[0] * 255.0);
        p[1] = (uint8_t) (f[1] * 255.0);
        p[2] = (uint8_t) (f[2] * 255.0);
        p[3] = (uint8_t) (f[3] * 255.0);
        p+=4;
        f+=4;
    }

    //convert from RTImage (float rgb packed) to UIImage
    NSBitmapImageRep *bir = [[NSBitmapImageRep alloc] initWithBitmapDataPlanes:&pixels
            pixelsWide:self.ctx->width
            pixelsHigh:self.ctx->height
            bitsPerSample:8
            samplesPerPixel:4
            hasAlpha:YES
            isPlanar:NO
            colorSpaceName:NSDeviceRGBColorSpace
            bytesPerRow:w*4
            bitsPerPixel:32];

    if (!bir)
    {
        NSLog(@"couldn't make NSBitmapImageRep");
        return;
    }

    if ([NSThread isMainThread])
    {
        CGImageSourceRef isr = CGImageSourceCreateWithData((CFDataRef)[bir TIFFRepresentation], NULL);
        CGImageRef image     = CGImageSourceCreateImageAtIndex(isr, 0, NULL);
        [self.image setImage:[[NSImage alloc] initWithCGImage:image size:NSMakeSize(w, h)]];
        [self writeToURL:[NSURL fileURLWithPath:@"/tmp/file.png"] ofType:@"png" error:nil];
    }
    else
    {
        dispatch_sync(dispatch_get_main_queue(), ^{
            CGImageSourceRef isr = CGImageSourceCreateWithData((CFDataRef)[bir TIFFRepresentation], NULL);
            CGImageRef image     = CGImageSourceCreateImageAtIndex(isr, 0, NULL);
            [self.image setImage:[[NSImage alloc] initWithCGImage:image size:NSMakeSize(w, h)]];
        });
    }
    free(pixels);
    self.updating = NO;
}

@end
