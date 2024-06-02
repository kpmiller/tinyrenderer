//
//  models.h
//  tinyrenderer
//
//  Created by Kent Miller on 5/31/24.
//

#ifndef models_h
#define models_h

#include <stdio.h>

typedef struct TRMIvec3 {
    int v, t, n;
} TRMIvec3;

typedef struct TRMFvec3 {
    float x, y, z;
} TRMFvec3;

typedef struct TRModel {
    int   numFaces;

    int    vtxcount;
    TRMFvec3 *vtx;
    int   *vtxidx;

    int    texcount;
    TRMFvec3 *tex;
    int   *texidx;

    int    nrmcount;
    TRMFvec3 *nrm;
    int   *nrmidx;
} TRModel;


int LoadModel(TRModel *model, char *filename);
void DestroyModel(TRModel *model);

#endif /* models_h */
