#pragma once

#include "Carla/Sensor/Sensor.h"
#include "ProbabilisticDescription.h"
#include "Carla/Actor/ActorDefinition.h"
#include "Carla/Actor/ActorDescription.h"
#include "Components/CapsuleComponent.h"
#include "ProbabilisticSensor.generated.h"

UCLASS()
class CARLA_API AProbabilisticSensor : public ASensor
{
  GENERATED_BODY()
  using FProbabilisticSensor = carla::sensor::data::ProbabilisticData;

public:
  AProbabilisticSensor(const FObjectInitializer &ObjectInitializer);

  static FActorDefinition GetSensorDefinition();

  virtual void Set(const FActorDescription &Description) override;

  void SetOwner(AActor *Owner) override;

  virtual void Tick(float DeltaSeconds) override;


private:

  std::vector<FProbabilisticSensor> SendActors;

  static int GetClassification(FString Name, float Length);

  UPROPERTY()
  FProbabilisticDescription Probabilistic;

  UPROPERTY()
  UCapsuleComponent *Capsule = nullptr;

  FCollisionQueryParams TraceParams;

};