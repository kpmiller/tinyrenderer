//
//  tinyrenderer.h
//  tinyrenderer
//
//  Created by Kent Miller on 5/30/24.
//

#ifndef tinyrenderer_h
#define tinyrenderer_h

#include <stdio.h>

typedef struct TRColor {
    float r, g, b, a;
} TRColor;

typedef struct TRcontext {
    int     changed;
    int     width;
    int     height;
    TRColor clearcolor;
    TRColor color;
    uint8_t *data;
} TRcontext;

TRcontext * CreateTinyRenderer(int width, int height);
void SetClearColor(TRcontext *tr, float r, float g, float b, float a);
void SetColor(TRcontext *tr, float r, float g, float b, float a);
void Clear(TRcontext *tr);
void Clear(TRcontext *tr);
void Draw(TRcontext *ctx);

#endif /* tinyrenderer_h */
