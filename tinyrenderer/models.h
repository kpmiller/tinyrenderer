//
//  models.h
//  tinyrenderer
//
//  Created by Kent Miller on 5/31/24.
//

#ifndef models_h
#define models_h

#include <stdio.h>
#include "tinyrenderer.h"

typedef struct TRModel {
    int   numFaces;

    int    vtxcount;
    TRVec3f *vtx;
    int   *vtxidx;

    int    texcount;
    TRVec3f *tex;
    int   *texidx;

    int    nrmcount;
    TRVec3f *nrm;
    int   *nrmidx;
} TRModel;


int LoadModel(TRModel *model, char *filename);
void DestroyModel(TRModel *model);

#endif /* models_h */
