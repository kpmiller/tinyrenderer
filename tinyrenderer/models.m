//
//  models.c
//  tinyrenderer
//
//  Created by Kent Miller on 5/31/24.
//

#include "models.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <Cocoa/Cocoa.h>


typedef struct TRMIvec3 {
    int v, t, n;
} TRMIvec3;


int ParseVertex(char* s, float *f1, float *f2, float *f3)
{
    if (3 != sscanf(s, "v %f %f %f", f1, f2, f3))
        return -1;
    return 0;
}

int ParseNormal(char* s, float *f1, float *f2, float *f3)
{
    if (3 != sscanf(s, "vn %f %f %f", f1, f2, f3))
        return -1;
    return 0;
}

int ParseTexture(char* s, float *f1, float *f2, float *f3)
{
    if (3 != sscanf(s, "vt %f %f %f", f1, f2, f3))
        return -1;
    return 0;
}

int ParseFace(char* s, TRMIvec3 *i1, TRMIvec3 *i2, TRMIvec3 *i3)
{
    if (9 == sscanf(s, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                    &i1->v, &i1->t, &i1->n,
                    &i2->v, &i2->t, &i2->n,
                    &i3->v, &i3->t, &i3->n)
        )
        return 0;
    return 1;
}


int LoadModel(TRModel *model, char *filename)
{
    NSString *fn = [NSString stringWithFormat:@"%s", filename];
    NSString *absPath= [[NSBundle mainBundle] pathForResource:fn ofType:@""];

    FILE *f = fopen(absPath.UTF8String, "r");

    bzero((void*)model, sizeof(model));

    if (f == NULL)
        return -1;

    char linebuf[512];

    float t1, t2, t3;
    TRMIvec3  i1, i2, i3;
    while (fgets(linebuf, 512, f))
    {
        if (0 == ParseVertex(linebuf, &t1, &t2, &t3) )
            model->vtxcount++;
        else if (0 == ParseNormal(linebuf, &t1, &t2, &t3) )
            model->nrmcount++;
        else if (0 == ParseTexture(linebuf, &t1, &t2, &t3) )
            model->texcount++;
        else if (0 == ParseFace(linebuf, &i1, &i2, &i3 ))
            model->numFaces++;
    }

    rewind(f);
    
    model->vtxidx = malloc(sizeof(int) * model->numFaces * 3);
    model->texidx = malloc(sizeof(int) * model->numFaces * 3);
    model->nrmidx = malloc(sizeof(int) * model->numFaces * 3);

    if (model->vtxcount > 0)
    {
        model->vtx = malloc(sizeof(TRVec3f) * model->vtxcount);
    }
    if (model->texcount > 0)
    {
        model->tex = malloc(sizeof(TRVec3f) * model->texcount);
    }
    if (model->nrmcount > 0)
    {
        model->nrm = malloc(sizeof(TRVec3f) * model->nrmcount);
    }

    int v = 0;
    int t = 0;
    int n = 0;
    int face = 0;

    while (fgets(linebuf, 512, f))
    {
        if (0 == ParseVertex(linebuf, &t1, &t2, &t3) )
        {
            model->vtx[v].x = t1;
            model->vtx[v].y = t2;
            model->vtx[v].z = t3;
            v++;
        }
        else if (0 == ParseNormal(linebuf, &model->nrm[n].x, &model->nrm[n].y, &model->nrm[n].z) )
        {
            n++;
        }
        else if (0 == ParseTexture(linebuf, &model->tex[t].x, &model->tex[t].y, &model->tex[t].z) )
        {
            t++;
        }
        else if (0 == ParseFace(linebuf, &i1, &i2, &i3 ))
        {
            {
                int *vp = &model->vtxidx[face * 3];
                vp[0] = i1.v - 1;
                vp[1] = i2.v - 1;
                vp[2] = i3.v - 1;
            }
            {
                int *tp = &model->texidx[face * 3];
                tp[0] = i1.t - 1;
                tp[1] = i2.t - 1;
                tp[2] = i3.t - 1;
            }
            {
                int *np = &model->nrmidx[face * 3];
                np[0] = i1.n - 1;
                np[1] = i2.n - 1;
                np[2] = i3.n - 1;
            }
            face++;
        }
    }



    fclose(f);

    return 0;
}


void DestroyModel(TRModel *model)
{
    if (model->vtx)
        free (model->vtx);
    if (model->tex)
        free (model->tex);
    if (model->nrm)
        free (model->nrm);
}
