#include "Carla.h"
#include "Carla/Sensor/ProbabilisticSensor.h"
#include "Carla/Actor/ActorBlueprintFunctionLibrary.h"
#include "Carla/Game/CarlaEpisode.h"
#include "Carla/Util/BoundingBoxCalculator.h"
#include "Carla/Vehicle/CarlaWheeledVehicle.h"
#include "Math/UnrealMathUtility.h"

#define PI 3.14159265

AProbabilisticSensor::AProbabilisticSensor(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
  Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleOverlap"));
  Capsule->SetupAttachment(RootComponent);
  //Capsule->SetHiddenInGame(false); // Uncomment for debugging.
  Capsule->SetCollisionProfileName(FName("OverlapAll"));
  RandomProbabilistic = CreateDefaultSubobject<URandomEngine>(TEXT("RandomProbabilistic"));

  PrimaryActorTick.bCanEverTick = true;
}

FActorDefinition AProbabilisticSensor::GetSensorDefinition()
{
  auto Definition = UActorBlueprintFunctionLibrary::MakeGenericSensorDefinition(
      TEXT("other"),
      TEXT("probabilistic"));

  // - Sensor Type  ----------------------------------
  // Ideal == 0;   // Cartesian noise == 1; // Radial noise == 2;
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
  FActorVariation HorizontalFOV;
  HorizontalFOV.Id = TEXT("horizontal_fov");
  HorizontalFOV.Type = EActorAttributeType::Float;
  HorizontalFOV.RecommendedValues = {TEXT("180.0")};
  HorizontalFOV.bRestrictToRecommended = false;

  // - Standard Deviation -----------------------------
  FActorVariation NoiseSeed;
  NoiseSeed.Id = TEXT("noise_seed");
  NoiseSeed.Type = EActorAttributeType::Int;
  NoiseSeed.RecommendedValues = {TEXT("123456789")};
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

  Definition.Variations.Append({NoiseType, Range, HorizontalFOV, NoiseSeed, StdDevX, StdDevY, StdDevR, StdDevHorizontal, VelStdDevR, VelStdDevHorizontal});

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
  Probabilistic.Fov.Horizontal = UActorBlueprintFunctionLibrary::RetrieveActorAttributeToFloat(
      "horizontal_fov",
      Description.Variations,
      180.f);
  Probabilistic.NoiseSeed = UActorBlueprintFunctionLibrary::RetrieveActorAttributeToInt(
      "noise_seed",
      Description.Variations,
      123456798);
  Probabilistic.PosNoise.X = UActorBlueprintFunctionLibrary::RetrieveActorAttributeToFloat(
      "noise_stddev_x",
      Description.Variations,
      1.f);
  Probabilistic.PosNoise.Y = UActorBlueprintFunctionLibrary::RetrieveActorAttributeToFloat(
      "noise_stddev_y",
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
  Probabilistic.VelNoiseSph.Radius = UActorBlueprintFunctionLibrary::RetrieveActorAttributeToFloat(
      "noise_stddev_vel_radius",
      Description.Variations,
      1.f);
  Probabilistic.VelNoiseSph.Horizontal = UActorBlueprintFunctionLibrary::RetrieveActorAttributeToFloat(
      "noise_stddev_vel_horizontal",
      Description.Variations,
      1.f); 

  if (Probabilistic.NoiseType == 0)
  {
    UE_LOG(LogTemp, Warning, TEXT("An Ideal Sensor was spawned"));
  }
  else if (Probabilistic.NoiseType == 1)
  {
    UE_LOG(LogTemp, Warning, TEXT("A probabilistic sensor with cartesian noise was spawned"));
  }
  else if (Probabilistic.NoiseType == 2)
  {
    UE_LOG(LogTemp, Warning, TEXT("A probabilistic sensor with spherical noise was spawned"));
  }
  else
  {
    Probabilistic.NoiseType = 0;
    UE_LOG(LogTemp, Warning, TEXT("An Ideal Sensor was spawned"))
  }

  constexpr float M_TO_CM = 100.0f; // Unit conversion.
  Capsule->SetCapsuleSize(M_TO_CM * Probabilistic.Fov.Radius, 50.f);
  //RandomSeed = Probabilistic.NoiseSeed;
}

void AProbabilisticSensor::SetOwner(AActor *Owner)
{
  Super::SetOwner(Owner);
}

void AProbabilisticSensor::Tick(float DeltaSeconds)
{
  Super::Tick(DeltaSeconds);

  //Initiate the Objectlist to the streaming
  SendActors.clear();

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
  FRotator SensorRotation = Capsule -> GetComponentRotation();
  FVector SensorVel = SensorRotation.UnrotateVector(GetOwner()-> GetVelocity()); 
  // Get the Registry of actors
  const FActorRegistry &Registry = GetEpisode().GetActorRegistry();

  // If any object was detected inside the capsule
  if (DetectedActors.Num() >= 0)
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
        //UE_LOG(LogTemp, Warning, TEXT("It is inside the FOV")); // uncomment for Debugging

        // Draw Linetrace line for debug
        /*DrawDebugLine(
            GetWorld(),
            SensorTransform.GetTranslation(),
            ObjectTransform.GetTranslation() + FVector(0.f, 0.f, 50.f),
            FColor(0, 255, 0),
            false,
            0.0f,
            0,
            5.f);*/

        // Get Carla Id of the object being evaluated
        const FCarlaActor *collide = Registry.FindCarlaActor(Object);
        int ObjIdx = collide->GetActorId();
        int LineTraceObjIdx = 0;
        FBoundingBox Box; 


        // Execute the Linetrace between the sensor and the object,
        FHitResult Hit;
        GetWorld()->ParallelLineTraceSingleByChannel(
            Hit,
            SensorTransform.GetTranslation(),
            ObjectTransform.GetTranslation() + FVector(0.f, 0.f, 30.f), //(30 cm are add to the object hight so that the Linetrace does not pass under the vehicle)
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
            //UE_LOG(LogTemp, Warning, TEXT("The Laser hit the %s"), *actor->GetName()) // Uncomment for Debugging
            const FCarlaActor *ActualActor = Registry.FindCarlaActor(actor);
            if (ActualActor)
            {
              LineTraceObjIdx = ActualActor->GetActorId();
              Box = ActualActor->GetActorInfo()->BoundingBox;
            }
          }
          else
          {
            //UE_LOG(LogCarla, Warning, TEXT("Actor not valid %p!!!!"), actor);
          }
        }

        if (LineTraceObjIdx != ObjIdx)
        {
          DetectedActors.Remove(Object);
        }
        else
        {
        
          FVector TempDimension = Box.Extent;
          
          FVector TempVel = Object-> GetVelocity();
          TempVel = SensorRotation.UnrotateVector(TempVel);

          float Yaw = Object -> GetActorRotation().Yaw - SensorRotation.Yaw;
          if (Yaw > 180.f)
          {
            Yaw = Yaw - 360.f; 
          }
          else if (Yaw < -180.f)
          {
            Yaw = Yaw + 360.f; 
          }

          //UE_LOG(LogTemp, Warning, TEXT("%p is in the FOV and is visible!!!!"), Object);  //  Uncomment to debug

          if (Probabilistic.NoiseType == 0)/// Type of noise 0:Ideal
          {         
            //UE_LOG(LogTemp, Warning, TEXT("Sensor velocity X = %f, Sensor velocity Y = %f, Object velocity X = %f, Object velocity Y = %f, Sensor Yaw = %f "), SensorVel.X, SensorVel.Y,TempVel.X,TempVel.Y, SensorRotation.Yaw); // uncomment for Debugging

            SendActors.push_back({LineTraceObjIdx,
              RelativePosition.X/100.f, // cm to m
              RelativePosition.Y/100.f, // cm to m
              TempVel.X/100.f - SensorVel.X/100.f, // Relative Velocity cm/s to m/s
              TempVel.Y/100.f - SensorVel.Y/100.f,// Relative Velocity cm/s to m/s
              Yaw,
              TempDimension.X*0.02f,
              TempDimension.Y*0.02f,
              TempDimension.Z*0.02f,
              GetClassification(Object -> GetName(), TempDimension.X*0.02f)});
 
          }
          
          else if (Probabilistic.NoiseType == 1)/// Type of noise 1:Cartesian (Camera and Lidar).
          {
            // include errors
            SendActors.push_back({LineTraceObjIdx,
              RelativePosition.X/100.f + RandomProbabilistic -> GetNormalDistribution(0.0f, Probabilistic.PosNoise.X), // cm to m
              RelativePosition.Y/100.f + RandomProbabilistic -> GetNormalDistribution(0.0f, Probabilistic.PosNoise.Y), // cm to m
              nanf("NaN"), // No velocity data
              nanf("Nan"),// No velocity data
              Yaw,
              TempDimension.X*0.02f,
              TempDimension.Y*0.02f,
              TempDimension.Z*0.02f,
              GetClassification(Object -> GetName(), TempDimension.X*0.02f)
              });          
          }
          
          else if (Probabilistic.NoiseType == 2)/// Type of noise 2:Spherical (Radar).
          {
            // Calculate Relative Velocity
            FVector RelativeVelocity = {TempVel.X/100.f - SensorVel.X/100.f,TempVel.Y/100.f - SensorVel.Y/100.f,0.0f};
            
            // Calculate spherical position to apply errors 
            float Theta = CalculateArcTan(RelativePosition.X,RelativePosition.Y) + RandomProbabilistic -> GetNormalDistribution(0.0f, Probabilistic.PosNoiseSph.Horizontal);
            float Radius = sqrt(pow(RelativePosition.X,2) + pow(RelativePosition.Y,2))+RandomProbabilistic -> GetNormalDistribution(0.0f, Probabilistic.PosNoiseSph.Radius);
            
            // Calculate spherical velocity to apply errors
            float ThetaVel = CalculateArcTan(RelativeVelocity.X,RelativeVelocity.Y)+ RandomProbabilistic -> GetNormalDistribution(0.0f, Probabilistic.VelNoiseSph.Horizontal);
            float RadiusVel = sqrt(pow(RelativeVelocity.X,2) + pow(RelativeVelocity.Y,2)) + RandomProbabilistic -> GetNormalDistribution(0.0f, Probabilistic.VelNoiseSph.Radius);
            
            // Append the object data to the Array
            SendActors.push_back({LineTraceObjIdx,
              Radius*sin(Theta), 
              Radius*cos(Theta),
              RadiusVel*sin(ThetaVel), 
              RadiusVel*cos(ThetaVel),
              Yaw,
              TempDimension.X*0.02f,
              TempDimension.Y*0.02f,
              TempDimension.Z*0.02f,
              GetClassification(Object -> GetName(), TempDimension.X*0.02f)
              });
          }
        }
      }
      else // In Case it is out of the FOV
      {
        DetectedActors.Remove(Object);
      }
    }
    auto Stream = GetDataStream(*this);
    Stream.Send(*this, SendActors);
  }
}


int AProbabilisticSensor::GetClassification(FString Name, float Length) {

    //  TYPE_UNKNOWN = 0, TYPE_OTHER = 1, TYPE_SMALL_CAR = 2, TYPE_COMPACT_CAR = 3,
    //  TYPE_MEDIUM_CAR = 4, TYPE_LUXURY_CAR = 5, TYPE_DELIVERY_VAN = 6, TYPE_HEAVY_TRUCK = 7,
    //  TYPE_SEMITRAILER = 8, TYPE_TRAILER = 9, TYPE_MOTORBIKE = 10, TYPE_BICYCLE = 11,
    //  TYPE_BUS = 12, TYPE_TRAM = 13, TYPE_TRAIN = 14, TYPE_WHEELCHAIR = 15, TYPE_PEDESTRIAN = 99

    int Classification = 0; // Unknown

    //UE_LOG(LogTemp, Warning, TEXT("The Name is %s"), *Name); // Uncomment for debugging
    
    if (Name.Contains("walker"))
    {
      Classification = 99; //pedestrian
    }
    else if ( Name.Contains("audi") || Name.Contains("bmw") || Name.Contains("chevrolet")|| Name.Contains("citroen")|| Name.Contains("mercedes")|| Name.Contains("charger")|| Name.Contains("jeep")|| Name.Contains("lincoln")|| Name.Contains("mini")|| Name.Contains("mustang")|| Name.Contains("crown")|| Name.Contains("nissan")|| Name.Contains("seat")|| Name.Contains("tesla")|| Name.Contains("toyota")|| Name.Contains("cybertruck"))
    {
      if (Length <= 4.1)
      {
        Classification = 2; // Small Car
      }
      else if (Length <= 4.4)
      {
        Classification = 3; // Compact Car
      }
      else if (Length < 4.7)
      {
        Classification = 4; // Medium Car
      }
      else
      {
        Classification = 5; // Luxury Car
      }
    }
    else if (Name.Contains("sprinter") || Name.Contains("volkswagen")|| Name.Contains("ambulance"))
    {
      Classification = 6; //Delivery Van - Including VW T2 and Ambulance
    }
    else if (Name.Contains("harley") || Name.Contains("kawasaki") ||Name.Contains("yamaha")|| Name.Contains("vespa"))
    {
      Classification = 10; // Motorbike
    }
    else if (Name.Contains("bike"))
    {
      Classification = 11; // Bicycle
    }
    else if (Name.Contains("carlacola") || Name.Contains("firetruck"))
    {
      Classification = 7; // Truck
    }

  return Classification;
}

float AProbabilisticSensor::CalculateArcTan(float X, float Y){
  if (X >= -0.0000001f && X <= 0.0000001f)
  {
    X = 0.0000001f;
  }
  float Theta = atan(Y/X);
  return Theta;
}