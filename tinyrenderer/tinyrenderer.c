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

#define RED() {SetColor(tr, 1.0, 0.0, 0.0, 1.0);}
#define GREEN() {SetColor(tr, 0.0, 1.0, 0.0, 1.0);}
#define BLUE() {SetColor(tr, 0.0, 0.0, 1.0, 1.0);}
#define WHITE() {SetColor(tr, 1.0, 1.0, 1.0, 1.0);}
#define LTGRAY() {SetColor(tr, 0.85, 0.85, 0.85, 1.0);}
#define DKGRAY() {SetColor(tr, 0.4, 0.4, 0.4, 1.0);}
#define BLACK() {SetColor(tr, 0.0, 0.0, 0.0, 1.0);}

TRContext * CreateTinyRenderer(int width, int height)
{
    TRContext *tr   = (TRContext*) malloc((size_t) sizeof(TRContext));
    tr->colorbuffer = (float*) calloc(height*width*sizeof(float)*4, 1);
    tr->depthbuffer = (float*) calloc(height*width*sizeof(float), 1);
    tr->width       = width;
    tr->height      = height;

    TRColor black = {0.0, 0.0, 0.0, 1.0};
    TRColor white = {1.0, 1.0, 1.0, 1.0};
    tr->clearcolor = black;
    tr->color      = white;
    tr->cleardepth = -MAXFLOAT;
    tr->changed    = 1;
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
    float *pix = tr->colorbuffer;
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

void ClearDepth(TRContext *tr)
{
    for (int II=0; II<tr->width*tr->height; II++)
        tr->depthbuffer[II] = tr->cleardepth;
}

void SetPixel(TRContext *tr, int x, int y)
{
    if ((x >= tr->width) || (y >= tr->height))
        return;

    float *f = &tr->colorbuffer[(y * tr->width + x) * 4];
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

TRVec3f cross(TRVec3f v1, TRVec3f v2)
{
    TRVec3f r;
    
    r.x = v1.y*v2.z - v1.z*v2.y;
    r.y = v1.z*v2.x - v1.x*v2.z;
    r.z = v1.x*v2.y - v1.y*v2.x;

    return r;
}

float magnitude(TRVec3f *v)
{
    return sqrtf(v->x*v->x + v->y*v->y + v->z*v->z);
}

void normalize(TRVec3f *v)
{
    float mag = magnitude(v);
    v->x = v->x / mag;
    v->y = v->y / mag;
    v->z = v->z / mag;
}

float dot3(TRVec3f a, TRVec3f b)
{
    return (a.x * b.x + a.y * b.y + a.z * b.z);
}

void barycentric(TRVec3f v0, TRVec3f v1, TRVec3f v2, TRVec3f point, TRVec3f *b)
{
    TRVec3f i = { v2.x-v0.x, v1.x-v0.x, v0.x - point.x};
    TRVec3f j = { v2.y-v0.y, v1.y-v0.y, v0.y - point.y};
    TRVec3f u = cross(i,j);
    if (fabsf(u.z) < 1.0)
    {
        b->x = -1.0;
    }
    else
    {
        b->x = 1.0f - (u.x+u.y) / u.z;
        b->y = u.y/u.z;
        b->z = u.x/u.z;
    }
}

void Triangle(TRContext *tr, TRVec3f vv0, TRVec3f vv1, TRVec3f vv2, int wireframe )
{
    if (vv0.y==vv1.y && vv0.y==vv2.y) return; // i dont care about degenerate triangles

    TRVec3f v[3] = {vv0, vv1, vv2};
    //sort verts
    
    if (v[0].y > v[1].y) {TRVec3f t = v[1]; v[1] = v[0]; v[0] = t;}
    if (v[1].y > v[2].y) {TRVec3f t = v[2]; v[2] = v[1]; v[1] = t;}
    if (v[0].y > v[1].y) {TRVec3f t = v[1]; v[1] = v[0]; v[0] = t;}

#define MAX(XX, YY) (XX > YY ? XX : YY)
#define MIN(XX, YY) (XX < YY ? XX : YY)

    if (wireframe)
    {
        RED();
        Line(tr, v[0].x, v[0].y, v[1].x, v[1].y);
        GREEN();
        Line(tr, v[1].x, v[1].y, v[2].x, v[2].y);
        Line(tr, v[2].x, v[2].y, v[0].x, v[0].y);
        //find bounding box
        TRVec3f bb0 = v[0], bb1=v[0];
        bb0.x = MIN(bb0.x, v[1].x); bb0.x = MIN(bb0.x, v[2].x);
        bb1.x = MAX(bb0.x, v[1].x); bb1.x = MAX(bb1.x, v[2].x);
        bb0.y = MIN(bb0.y, v[1].y); bb0.y = MIN(bb0.y, v[2].y);
        bb1.y = MAX(bb1.y, v[1].y); bb1.y = MAX(bb1.y, v[2].y);
#ifdef BOUNDING_BOX
        WHITE();
        Line (tr, bb0.x, bb0.y, bb1.x, bb0.y);
        Line (tr, bb1.x, bb0.y, bb1.x, bb1.y);
        Line (tr, bb0.x, bb1.y, bb1.x, bb1.y);
        Line (tr, bb0.x, bb1.y, bb0.x, bb0.y);
#endif

    }
    else
    {
        //find bounding box
        TRVec3f bb0 = v[0], bb1=v[0];
        bb0.x = MIN(bb0.x, v[1].x); bb0.x = MIN(bb0.x, v[2].x);
        bb1.x = MAX(bb1.x, v[1].x); bb1.x = MAX(bb1.x, v[2].x);
        bb0.y = MIN(bb0.y, v[1].y); bb0.y = MIN(bb0.y, v[2].y);
        bb1.y = MAX(bb1.y, v[1].y); bb1.y = MAX(bb1.y, v[2].y);

//        printf("%d %d   %d %d   %d %d\n", v[0].x, v[0].y, v[1].x, v[1].y, v[2].x, v[2].y);
//        printf("bbox: %d %d   %d %d\n", bb0.x, bb0.y, bb1.x, bb1.y);
        FILE *f = fopen("/tmp/stdout.csv", "w");
        for (int JJ=bb0.y; JJ<bb1.y; JJ++)
        {
            for (int II=bb0.x; II<bb1.x; II++)
            {
                TRVec3f bc;
                TRVec3f p = {II, JJ, 0.0};
                barycentric(v[0], v[1], v[2], p, &bc);
                if ((bc.x < 0) || (bc.y < 0) || (bc.z < 0))
                {
//                    printf("out: %d %d  %f %f %f\n", II, JJ, bc.x, bc.y, bc.z);
                    continue;
                }
//                printf("in: %d %d  %f %f %f\n", II, JJ, bc.x, bc.y, bc.z);
                fprintf(f, "%f,%f,%f,%f,%f\n", (float)II, (float)JJ, bc.x, bc.y, bc.z);
                float z = v[0].z * bc.x + v[1].z * bc.y + v[2].z * bc.z;
                float *depth = &tr->depthbuffer[JJ*tr->width | II];
                if (z > *depth)
                {
                    *depth = z;
//                    printf("%f,%f,%f\n",(float)II,(float)JJ,z);
                    SetColor(tr, z, z, z, 1.0);
                    SetPixel(tr, II, JJ);
                }
            }
        }
        fclose(f);
#ifdef BOUNDING_BOX
        SetColor(tr, 1.0, 0.0, 1.0, 1.0);
        SetPixel(tr, bb0.x, bb0.y);
        SetPixel(tr, bb1.x, bb0.y);
        SetPixel(tr, bb1.x, bb1.y);
        SetPixel(tr, bb0.x, bb1.y);
#endif
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

        TRVec3f v0 = m.vtx[i1];
        TRVec3f v1 = m.vtx[i2];
        TRVec3f v2 = m.vtx[i3];

        Line(tr, xoffset + ((v0.x + 1.0) * xscale), yoffset + ((v0.y + 1.0) * yscale),
             xoffset + ((v1.x + 1.0) * xscale), yoffset + ((v1.y + 1.0) * yscale));
        Line(tr, xoffset + ((v0.x + 1.0) * xscale), yoffset + ((v0.y + 1.0) * yscale),
             xoffset + ((v2.x + 1.0) * xscale), yoffset + ((v2.y + 1.0) * yscale));
        Line(tr, xoffset + ((v2.x + 1.0) * xscale), yoffset + ((v2.y + 1.0) * yscale),
             xoffset + ((v1.x + 1.0) * xscale), yoffset + ((v1.y + 1.0) * yscale));

    }


    DestroyModel(&m);
}

void BareTriangle(TRContext *tr, int x0, int y0, int x1, int y1, int x2, int y2)
{
    TRVec3f a = {x0, y0, 0.0};
    TRVec3f b = {x1, y1, 0.0};
    TRVec3f c = {x2, y2, 0.0};
    Triangle(tr, a, b, c, 0);
}

void SimpleTriangles(TRContext *tr)
{
    Clear(tr);
    RED();
    BareTriangle(tr, 10, 70, 50, 160, 70, 80);
    WHITE();
    BareTriangle(tr, 180, 50, 150, 1, 70, 180);
    GREEN();
    BareTriangle(tr, 180, 150, 120, 160, 130, 180);

}

void PrintVec2i(TRVec2i v)
{
    printf("%d,%d,", v.x, v.y);
}

void PrintVec3f(TRVec3f v)
{
    printf("%f,%f,%f,", v.x, v.y, v.z);
}

void FakeLitHead(TRContext *tr)
{
    TRModel m = {};

    Clear(tr);
    ClearDepth(tr);

    LoadModel(&m, "african_head.obj");

    float xoffset = 0.0f;
    float xscale  = tr->width / 2.0f;
    float yoffset = 0.0f;
    float yscale  = tr->height / 2.0f;

    TRVec3f light_dir = {0,0,-1};

//    printf("vtx,sc0.x,sc0.y,w0.x,w0.y,w0.z,sc1.x,sc1.y,w1.x,w1.y,w1.z,sc2.x,sc2.y,w2.x,w2.y,w2.z,");
//    printf("c0.x,c0.y,c0.z,n0.x,n0.y,n0.z,intensity\n");
    for (int II=0; II < m.numFaces; II++)
    {
//        printf("%d,", II);
        int idx[3] = {m.vtxidx[II*3], m.vtxidx[II*3+1], m.vtxidx[II*3+2]};

        TRVec3f world_coords[3];
        TRVec3f screen_coords[3];
        
        for (int JJ=0; JJ<3; JJ++)
        {
            TRVec3f v = m.vtx[idx[JJ]];
            screen_coords[JJ].x = (xoffset + ((v.x + 1.0f) * xscale));
            screen_coords[JJ].y = (yoffset + ((v.y + 1.0f) * yscale));
            screen_coords[JJ].z = v.z;
            world_coords[JJ] = v;
//            PrintVec2i(screen_coords[JJ]);
//            PrintVec3f(world_coords[JJ]);
        }
        
        TRVec3f n;
        {
            TRVec3f a = {world_coords[2].x - world_coords[0].x,
                         world_coords[2].y - world_coords[0].y,
                         world_coords[2].z - world_coords[0].z
            };
            TRVec3f b = {world_coords[1].x - world_coords[0].x,
                         world_coords[1].y - world_coords[0].y,
                         world_coords[1].z - world_coords[0].z
            };
            n = cross(a, b);
        }
//        PrintVec3f(n);
        normalize(&n);
//        PrintVec3f(n);
        float c = dot3(n, light_dir);
//        printf("%f\n", c);
        //this is effectively back-face culling
        if (c > 0.0f)
        {
            SetColor(tr, c, c, c, 1.0);
            Triangle(tr, screen_coords[0], screen_coords[1], screen_coords[2], 0);
        }
    }


    DestroyModel(&m);
}

void TriangleTest(TRContext *tr)
{
    int x = 5, y = 5;
    Clear(tr);
    RED();
    int ycnt = 0, xcnt;
    while (y < tr->height)
    {
        x = 0;
        xcnt = 0;
        while (x < tr->width)
        {
            TRVec3f a = {x + (random()&0x3f), y + (random()&0x3f), 0.0f};
            TRVec3f b = {x + (random()&0x3f), y + (random()&0x3f), 0.0f};
            TRVec3f c = {x + (random()&0x3f), y + (random()&0x3f), 0.0f};
            
            Triangle(tr, a, b, c, 0);
            
            printf("(%d, %d)  ", xcnt, ycnt);
            PrintVec3f(a);
            PrintVec3f(b);
            PrintVec3f(c);
            printf("\n");

            x += 70;
            xcnt++;
        }
        y += 70;
        ycnt++;
    }
}

void Draw(TRContext *ctx)
{
//    TriangleTest(ctx);
    FakeLitHead(ctx);
}
