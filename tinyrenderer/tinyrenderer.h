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

typedef struct TRVec2i {
    int x,y;
} TRVec2i;


typedef struct TRVec3f {
    float x,y,z;
} TRVec3f;


typedef struct TRContext {
    int     changed;
    int     width;
    int     height;
    TRColor clearcolor;
    TRColor color;
    uint8_t *data;
} TRContext;

TRContext * CreateTinyRenderer(int width, int height);
void SetClearColor(TRContext *tr, float r, float g, float b, float a);
void SetColor(TRContext *tr, float r, float g, float b, float a);
void Clear(TRContext *tr);
void Draw(TRContext *ctx);

#endif /* tinyrenderer_h */
