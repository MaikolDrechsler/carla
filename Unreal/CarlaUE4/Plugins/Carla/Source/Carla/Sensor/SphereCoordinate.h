// Copyright (c) 2021 Maikol Funk Drechsler/CARISSMA Institute of Automated Driving (C-IAD) at the Technische Hochschule Ingolstadt (THI). 

#pragma once
# include "Engine.h"
# include "SphereCoordinate.generated.h" 

USTRUCT()
struct FSphereCoordinate
{
    GENERATED_BODY()

    float Radius;
    float Horizontal;
    float Vertical;
};

