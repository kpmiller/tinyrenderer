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
#include <math.h>
#include "models.h"

TRContext * CreateTinyRenderer(int width, int height)
{
    TRContext *tr = (TRContext*) malloc((size_t) sizeof(TRContext));
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


void SetClearColor(TRContext *tr, float r, float g, float b, float a)
{
    tr->clearcolor.r = r;
    tr->clearcolor.g = g;
    tr->clearcolor.b = b;
    tr->clearcolor.a = a;
}

void SetColor(TRContext *tr, float r, float g, float b, float a)
{
    tr->color.r = r;
    tr->color.g = g;
    tr->color.b = b;
    tr->color.a = a;
}

void Clear(TRContext *tr)
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

void SetPixel(TRContext *tr, int x, int y)
{
    if ((x >= tr->width) || (y >= tr->height))
        return;

    float *f = (float*) &tr->data[(y * tr->width + x) * 4 * sizeof(float)];
    f[0] = tr->color.r;
    f[1] = tr->color.g;
    f[2] = tr->color.b;
    f[3] = tr->color.a;
}


void Rect(TRContext *tr, int x, int y, int w, int h)
{
    for (int JJ=y; JJ<y+h; JJ++)
    {
        for (int II=x; II<x+w; II++)
        {
            SetPixel(tr, II, JJ);
        }
    }

}

void Line( TRContext *tr, int x0, int y0, int x1, int y1 )
{
    if (abs(x1-x0) > abs(y1-y0))
    {
        //xmajor
        if (x0 > x1)
        {
            int t;
            t = x1; x1 = x0; x0 = t;
            t = y1; y1 = y0; y0 = t;
        }
        float xd = (float) x1 - x0;
        for (float x = x0; x <= x1; x++)
        {
            float t = (x - x0) / xd;
            int y = y0 * (1.0 - t) + y1 * t;
            SetPixel(tr, x, y);
        }
    }
    else
    {
        //ymajor
        if (y0 > y1)
        {
            int t;
            t = x1; x1 = x0; x0 = t;
            t = y1; y1 = y0; y0 = t;
        }
        float yd = (float) y1 - y0;
        for (float y = y0; y <= y1; y++)
        {
            float t = (y - y0) / yd;
            int x = x0 * (1.0f - t) + x1 * t;
            SetPixel(tr, x, y);
        }
    }
}


# pragma mark Draw

void Grid(TRContext *tr)
{
    SetClearColor(tr, 0.0, 0.2, 0.3, 1.0);
    SetColor(tr, 0.8, 0.2, 0.3, 1.0);

    Clear(tr);
    for (int JJ = 5; JJ < tr->height; JJ += 50)
    {
        for (int II = 5; II < tr->width; II+= 50)
        {
            Rect(tr, II, JJ, 25, 25);
        }
    }
}

void LineStarburst(TRContext *tr)
{
    SetClearColor(tr, 0.0, 0.2, 0.3, 1.0);
    SetColor(tr, 0.8, 0.2, 0.3, 1.0);
    Clear(tr);
    int cx = tr->width / 2;
    int cy = tr->height / 2;
    int r  = cy - 10;

    for (float theta=0.0; theta < M_PI*2.0; theta = theta + (M_PI*2.0 / 50.0))
    {
        float x = cx + r * cosf(theta);
        float y = cy + r * sinf(theta);
        Line(tr, cx, cy, x, y);
    }

}

void WireframeHead(TRContext *tr)
{
    TRModel m = {};

    Clear(tr);

    LoadModel(&m, "african_head.obj");

    float xoffset = tr->width / 4.0f;
    float xscale  = tr->width / 4.0f;
    float yoffset = 0.0f;
    float yscale  = tr->height / 2.0f;

    for (int II=0; II < m.numFaces; II++)
    {
        int i1 = m.vtxidx[II*3];
        int i2 = m.vtxidx[II*3+1];
        int i3 = m.vtxidx[II*3+2];

        TRMFvec3 v0 = m.vtx[i1];
        TRMFvec3 v1 = m.vtx[i2];
        TRMFvec3 v2 = m.vtx[i3];

        Line(tr, xoffset + ((v0.x + 1.0) * xscale), yoffset + ((v0.y + 1.0) * yscale),
             xoffset + ((v1.x + 1.0) * xscale), yoffset + ((v1.y + 1.0) * yscale));
        Line(tr, xoffset + ((v0.x + 1.0) * xscale), yoffset + ((v0.y + 1.0) * yscale),
             xoffset + ((v2.x + 1.0) * xscale), yoffset + ((v2.y + 1.0) * yscale));
        Line(tr, xoffset + ((v2.x + 1.0) * xscale), yoffset + ((v2.y + 1.0) * yscale),
             xoffset + ((v1.x + 1.0) * xscale), yoffset + ((v1.y + 1.0) * yscale));

    }


    DestroyModel(&m);
}

void Draw(TRContext *tr)
{
    WireframeHead(tr);
}
