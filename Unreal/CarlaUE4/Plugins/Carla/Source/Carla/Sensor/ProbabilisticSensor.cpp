#include "Carla.h"
#include "Carla/Sensor/ProbabilisticSensor.h"
#include "Carla/Actor/ActorBlueprintFunctionLibrary.h"
#include "Carla/Game/CarlaEpisode.h"
#include "Carla/Util/BoundingBoxCalculator.h"
#include "Carla/Vehicle/CarlaWheeledVehicle.h"

#define PI 3.14159265

AProbabilisticSensor::AProbabilisticSensor(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
  Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleOverlap"));
  Capsule->SetupAttachment(RootComponent);
  Capsule->SetHiddenInGame(false); // Disable for debugging.
  Capsule->SetCollisionProfileName(FName("OverlapAll"));

  PrimaryActorTick.bCanEverTick = true;
}

FActorDefinition AProbabilisticSensor::GetSensorDefinition()
{
  auto Definition = UActorBlueprintFunctionLibrary::MakeGenericSensorDefinition(
      TEXT("other"),
      TEXT("probabilistic"));

  // - Sensor Type  ----------------------------------
  // Ideal
  // Radial noise
  // Cartesian noise
  FActorVariation NoiseType;
  NoiseType.Id = TEXT("noise_type");
  NoiseType.Type = EActorAttributeType::Int;
  NoiseType.RecommendedValues = {TEXT("0")}; // Ideal Sensor
  NoiseType.bRestrictToRecommended = false;

  // - Sensor Range  ----------------------------------
  FActorVariation Range;
  Range.Id = TEXT("range");
  Range.Type = EActorAttributeType::Float;
  Range.RecommendedValues = {TEXT("100.0")}; // 100 meters
  Range.bRestrictToRecommended = false;

  // - Sensor Field of View  --------------------------
  FActorVariation VerticalFOV;
  VerticalFOV.Id = TEXT("vertical_fov");
  VerticalFOV.Type = EActorAttributeType::Float;
  VerticalFOV.RecommendedValues = {TEXT("180.0")};
  VerticalFOV.bRestrictToRecommended = false;

  FActorVariation HorizontalFOV;
  HorizontalFOV.Id = TEXT("horizontal_fov");
  HorizontalFOV.Type = EActorAttributeType::Float;
  HorizontalFOV.RecommendedValues = {TEXT("180.0")};
  HorizontalFOV.bRestrictToRecommended = false;

  // - Standard Deviation -----------------------------
  FActorVariation NoiseSeed;
  NoiseSeed.Id = TEXT("noise_seed");
  NoiseSeed.Type = EActorAttributeType::Float;
  NoiseSeed.RecommendedValues = {TEXT("0")};
  NoiseSeed.bRestrictToRecommended = false;

  FActorVariation StdDevX;
  StdDevX.Id = TEXT("noise_stddev_x");
  StdDevX.Type = EActorAttributeType::Float;
  StdDevX.RecommendedValues = {TEXT("1.0")};
  StdDevX.bRestrictToRecommended = false;

  FActorVariation StdDevY;
  StdDevY.Id = TEXT("noise_stddev_y");
  StdDevY.Type = EActorAttributeType::Float;
  StdDevY.RecommendedValues = {TEXT("1.0")};
  StdDevY.bRestrictToRecommended = false;

  FActorVariation StdDevZ;
  StdDevZ.Id = TEXT("noise_stddev_z");
  StdDevZ.Type = EActorAttributeType::Float;
  StdDevZ.RecommendedValues = {TEXT("0.1")};
  StdDevZ.bRestrictToRecommended = false;

  FActorVariation StdDevR;
  StdDevR.Id = TEXT("noise_stddev_radius");
  StdDevR.Type = EActorAttributeType::Float;
  StdDevR.RecommendedValues = {TEXT("1.0")};
  StdDevR.bRestrictToRecommended = false;

  FActorVariation StdDevHorizontal;
  StdDevHorizontal.Id = TEXT("noise_stddev_horizontal");
  StdDevHorizontal.Type = EActorAttributeType::Float;
  StdDevHorizontal.RecommendedValues = {TEXT("1.0")};
  StdDevHorizontal.bRestrictToRecommended = false;

  FActorVariation StdDevVertical;
  StdDevVertical.Id = TEXT("noise_stddev_vertical");
  StdDevVertical.Type = EActorAttributeType::Float;
  StdDevVertical.RecommendedValues = {TEXT("1.0")};
  StdDevVertical.bRestrictToRecommended = false;

  FActorVariation VelStdDevR;
  VelStdDevR.Id = TEXT("noise_stddev_vel_radius");
  VelStdDevR.Type = EActorAttributeType::Float;
  VelStdDevR.RecommendedValues = {TEXT("1.0")};
  VelStdDevR.bRestrictToRecommended = false;

  FActorVariation VelStdDevHorizontal;
  VelStdDevHorizontal.Id = TEXT("stddev_vel_horizontal");
  VelStdDevHorizontal.Type = EActorAttributeType::Float;
  VelStdDevHorizontal.RecommendedValues = {TEXT("1.0")};
  VelStdDevHorizontal.bRestrictToRecommended = false;

  FActorVariation VelStdDevVertical;
  VelStdDevVertical.Id = TEXT("stddev_vel_vertical");
  VelStdDevVertical.Type = EActorAttributeType::Float;
  VelStdDevVertical.RecommendedValues = {TEXT("1.0")};
  VelStdDevVertical.bRestrictToRecommended = false;

  Definition.Variations.Append({NoiseType, Range, VerticalFOV, HorizontalFOV, NoiseSeed, StdDevX, StdDevY, StdDevZ, StdDevR, StdDevHorizontal, StdDevVertical, VelStdDevR, VelStdDevHorizontal, VelStdDevVertical});

  return Definition;
}

void AProbabilisticSensor::Set(const FActorDescription &Description)
{
  Super::Set(Description);

  Probabilistic.NoiseType = UActorBlueprintFunctionLibrary::RetrieveActorAttributeToInt(
      "noise_type",
      Description.Variations,
      0);
  Probabilistic.Fov.Radius = UActorBlueprintFunctionLibrary::RetrieveActorAttributeToFloat(
      "range",
      Description.Variations,
      100.f);
  Probabilistic.Fov.Vertical = UActorBlueprintFunctionLibrary::RetrieveActorAttributeToFloat(
      "vertical_fov",
      Description.Variations,
      180.f);
  Probabilistic.Fov.Horizontal = UActorBlueprintFunctionLibrary::RetrieveActorAttributeToFloat(
      "horizontal_fov",
      Description.Variations,
      180.f);
  Probabilistic.NoiseSeed = UActorBlueprintFunctionLibrary::RetrieveActorAttributeToFloat(
      "noise_seed",
      Description.Variations,
      0.0f);
  Probabilistic.PosNoise.X = UActorBlueprintFunctionLibrary::RetrieveActorAttributeToFloat(
      "noise_stddev_x",
      Description.Variations,
      1.f);
  Probabilistic.PosNoise.Y = UActorBlueprintFunctionLibrary::RetrieveActorAttributeToFloat(
      "noise_stddev_y",
      Description.Variations,
      1.f);
  Probabilistic.PosNoise.Z = UActorBlueprintFunctionLibrary::RetrieveActorAttributeToFloat(
      "noise_stddev_z",
      Description.Variations,
      1.f);
  Probabilistic.PosNoiseSph.Radius = UActorBlueprintFunctionLibrary::RetrieveActorAttributeToFloat(
      "noise_stddev_radius",
      Description.Variations,
      1.f);
  Probabilistic.PosNoiseSph.Horizontal = UActorBlueprintFunctionLibrary::RetrieveActorAttributeToFloat(
      "noise_stddev_horizontal",
      Description.Variations,
      1.f);
  Probabilistic.PosNoiseSph.Vertical = UActorBlueprintFunctionLibrary::RetrieveActorAttributeToFloat(
      "noise_stddev_vertical",
      Description.Variations,
      1.f);
  Probabilistic.VelNoiseSph.Radius = UActorBlueprintFunctionLibrary::RetrieveActorAttributeToFloat(
      "noise_stddev_vel_radius",
      Description.Variations,
      1.f);
  Probabilistic.VelNoiseSph.Horizontal = UActorBlueprintFunctionLibrary::RetrieveActorAttributeToFloat(
      "noise_stddev_vel_horizontal",
      Description.Variations,
      1.f);
  Probabilistic.VelNoiseSph.Vertical = UActorBlueprintFunctionLibrary::RetrieveActorAttributeToFloat(
      "noise_stddev_vel_vertical",
      Description.Variations,
      1.f);

  if (Probabilistic.NoiseType == 0)
  {
    UE_LOG(LogTemp, Warning, TEXT("An Ideal Sensor was spawned"));
  }
  else if (Probabilistic.NoiseType == 1)
  {
    UE_LOG(LogTemp, Warning, TEXT("A probabilistic sensor with spherical noise was spawned"));
  }
  else if (Probabilistic.NoiseType == 2)
  {
    UE_LOG(LogTemp, Warning, TEXT("A probabilistic sensor with cartesian noise was spawned"));
  }
  else
  {
    Probabilistic.NoiseType = 0;
    UE_LOG(LogTemp, Warning, TEXT("An Ideal Sensor was spawned"))
  }

  constexpr float M_TO_CM = 100.0f; // Unit conversion.
  Capsule->SetCapsuleSize(M_TO_CM * Probabilistic.Fov.Radius, 50.f);
}

void AProbabilisticSensor::SetOwner(AActor *Owner)
{
  Super::SetOwner(Owner);

  // auto BoundingBox = UBoundingBoxCalculator::GetActorBoundingBox(Owner);
  // Box->SetBoxExtent(BoundingBox.Extent + Box->GetUnscaledBoxExtent());
}

void AProbabilisticSensor::Tick(float DeltaSeconds)
{
  Super::Tick(DeltaSeconds);

  // Define the Parameters for Linetrace
  TraceParams = FCollisionQueryParams(FName(TEXT("Laser_Trace")), true, GetOwner());
  TraceParams.bTraceComplex = true;
  TraceParams.bReturnPhysicalMaterial = false;

  // Get actors inside the Capsule
  TSet<AActor *> DetectedVehicles;
  TSet<AActor *> DetectedActors;
  Capsule->GetOverlappingActors(DetectedVehicles, ACarlaWheeledVehicle::StaticClass());
  Capsule->GetOverlappingActors(DetectedActors, ACharacter::StaticClass()); // verificar para pegar pedestres
  DetectedActors.Append(DetectedVehicles);

  // Remove Own vehicle
  DetectedActors.Remove(GetOwner());

  // Get the Capsule Transformation from component to world.
  FTransform SensorTransform = Capsule->GetComponentTransform();
  // Get the Registry of actors
  const FActorRegistry &Registry = GetEpisode().GetActorRegistry();

  // If any object was detected inside the capsule
  if (DetectedActors.Num() > 0)
  {
    // For each one of the objects inside the capsule
    for (auto &Object : DetectedActors)
    {
      // Take the relative position between sensor and object
      FTransform ObjectTransform = Object->GetTransform();
      FVector RelativePosition = ObjectTransform.GetRelativeTransform(SensorTransform).GetTranslation();
      // UE_LOG(LogTemp, Warning, TEXT("%s"), *RelativePosition.ToString()); // Uncomment for debugging

      // if the Object is inside the FOV
      float Angle = 90.f; 
      if (RelativePosition.X != 0)
      {
        Angle = ((atan(abs(RelativePosition.Y) / RelativePosition.X)) * (180 / PI)); 
      }
      if (Angle <= 0.f)
      {
        Angle += 180; 
      }
      if (Angle <= Probabilistic.Fov.Horizontal/2.0f)
      {
        UE_LOG(LogTemp, Warning, TEXT("It is inside the FOV")); // uncomment for Debugging

        // Draw Linetrace line for debug
        DrawDebugLine(
            GetWorld(),
            SensorTransform.GetTranslation(),
            ObjectTransform.GetTranslation() + FVector(0.f, 0.f, 50.f),
            FColor(0, 255, 0),
            false,
            0.0f,
            0,
            5.f);

        // Get Carla Id of the object being evaluated
        const FCarlaActor *collide = Registry.FindCarlaActor(Object);
        int ObjIdx = collide->GetActorId();
        int LineTraceObjIdx = 0;

        // Execute the Linetrace between the sensor and the object,
        FHitResult Hit;
        GetWorld()->ParallelLineTraceSingleByChannel(
            Hit,
            SensorTransform.GetTranslation(),
            ObjectTransform.GetTranslation() + FVector(0.f, 0.f, 50.f), //(50 cm are add to the object hight so that the Linetrace does not pass under the vehicle)
            ECC_GameTraceChannel2,
            TraceParams,
            FCollisionResponseParams::DefaultResponseParam);

        // If the Linetrace hits something
        if (Hit.bBlockingHit)
        {
          // Get the hitted actor
          AActor *actor = Hit.Actor.Get();
          // Get the Carla ID of this actor
          if (actor != nullptr)
          {
            UE_LOG(LogTemp, Warning, TEXT("The Laser hit the %s"), *actor->GetName()) // Uncomment for Debugging
            const FCarlaActor *view = Registry.FindCarlaActor(actor);
            if (view)
            {
              LineTraceObjIdx = view->GetActorId();
            }
          }
          else
          {
            UE_LOG(LogCarla, Warning, TEXT("Actor not valid %p!!!!"), actor);
          }
        }

        if (LineTraceObjIdx != ObjIdx)
        {
          DetectedActors.Remove(Object);
        }
        else
        {
          UE_LOG(LogTemp, Warning, TEXT("%p is in the FOV and is visible!!!!"), Object);
        }
      }

      // In Case it is out of the FOV
      else
      {
        DetectedActors.Remove(Object);
      }
    }

    // include errors
    /*
    const auto Noise = ForwardVector * RandomEngine->GetNormalDistribution(0.0f, Description.NoiseStdDev);
    Detection.point += Noise*/

    /*  float ARadar::CalculateRelativeVelocity(const FHitResult& OutHit, const FVector& RadarLocation){
      constexpr float TO_METERS = 1e-2;

      const TWeakObjectPtr<AActor> HittedActor = OutHit.Actor;
      const FVector TargetVelocity = HittedActor->GetVelocity();
      const FVector TargetLocation = OutHit.ImpactPoint;
      const FVector Direction = (TargetLocation - RadarLocation).GetSafeNormal();
      const FVector DeltaVelocity = (TargetVelocity - CurrentVelocity);
      const float V = TO_METERS * FVector::DotProduct(DeltaVelocity, Direction);

      return V;
    }*/
    auto Stream = GetDataStream(*this);
    Stream.Send(*this, GetEpisode(), DetectedActors);

    // Change in the ProbabilisticSerializer.h what is being sent.
  }
}

/* TODO:
Create a separated method to process each object;
Include errors; 
Permit sending the complete data; 
*/