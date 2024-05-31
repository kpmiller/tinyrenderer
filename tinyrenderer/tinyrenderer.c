//
//  tinyrenderer.c
//  tinyrenderer
//
//  Created by Kent Miller on 5/30/24.
//

#include "tinyrenderer.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

TRcontext * CreateTinyRenderer(int width, int height)
{
    TRcontext *tr = (TRcontext*) malloc((size_t) sizeof(TRcontext));
    tr->data    = (uint8_t*) calloc(height*width*sizeof(float)*4, 1);
    tr->width   = width;
    tr->height  = height;

    TRColor black = {0.0, 0.0, 0.0, 1.0};
    TRColor white = {1.0, 1.0, 1.0, 1.0};
    tr->clearcolor = black;
    tr->color      = white;

//    float *f = (float*) rt->data;
//    for (int y = 0; y < height; y++)
//    {
//        float yv = (float) y / (float)height;
//        for (int x=0; x< width; x++)
//        {
//            f[0] = (float) x / (float)width;
//            f[1] = yv;
//            f[2] = 0.2f;
//            f+=3;
//        }
//    }
    tr->changed = 1;
    return tr;
}


void SetClearColor(TRcontext *tr, float r, float g, float b, float a)
{
    tr->clearcolor.r = r;
    tr->clearcolor.g = g;
    tr->clearcolor.b = b;
    tr->clearcolor.a = a;
}

void SetColor(TRcontext *tr, float r, float g, float b, float a)
{
    tr->color.r = r;
    tr->color.g = g;
    tr->color.b = b;
    tr->color.a = a;
}

void Clear(TRcontext *tr)
{
    float *pix = (float*) tr->data;
    for (int j = 0; j < tr->height; j++)
    {
        for (int i = 0; i < tr->width; i++)
        {
            pix[0] = tr->clearcolor.r;
            pix[1] = tr->clearcolor.g;
            pix[2] = tr->clearcolor.b;
            pix[3] = tr->clearcolor.a;
            pix += 4;
        }
    }
}

void SetPixel(TRcontext *tr, int x, int y)
{
    float *f = (float*) &tr->data[(y * tr->width + x) * 4 * sizeof(float)];
    f[0] = tr->color.r;
    f[1] = tr->color.g;
    f[2] = tr->color.b;
    f[3] = tr->color.a;
}


void Draw(TRcontext *tr)
{
    for (int JJ=89; JJ<161; JJ++)
    {
        for (int II=48; II<567; II++)
        {
            SetPixel(tr, II, JJ);
        }
    }
}
