// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once
#include "SphereCoordinate.h"
#include "Math/Vector.h"
#include "ProbabilisticDescription.generated.h"

USTRUCT()
struct CARLA_API FProbabilisticDescription
{
  GENERATED_BODY()

  /// Type of noise 0:Ideal, 1:Cartesian, 2:Spherical.
  UPROPERTY(EditAnywhere)
  int NoiseType = 0;

  /// Field of view of the probabilistic sensor.
  UPROPERTY(EditAnywhere)
  FSphereCoordinate Fov; // = {10000.f, 360.f, 360.f};

  /// Probabilistic errors.
  /// Random seed for the noise/dropoff used by this sensor.
  UPROPERTY(EditAnywhere)
  int NoiseSeed = 0.0f;

  /// Position noise cartesian
  UPROPERTY(EditAnywhere)
  FVector PosNoise; // = {0.f, 0.f, 0.f};
  
  /// Position noise Spherical
  UPROPERTY(EditAnywhere)
  FSphereCoordinate PosNoiseSph; // = {0.f, 0.f, 0.f};

  /// Velocity noise Spherical (Radar)
  UPROPERTY(EditAnywhere)
  FSphereCoordinate VelNoiseSph; // = {0.f, 0.f, 0.f};
};