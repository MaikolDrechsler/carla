
// Copyright (c) 2019 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "carla/Memory.h"
#include "carla/rpc/ActorId.h"
#include "carla/sensor/RawData.h"
#include "carla/sensor/data/ProbabilisticData.h"

#include <cstdint>
#include <cstring>

namespace carla {
namespace sensor {

  class SensorData;

namespace s11n {

  class ProbabilisticSerializer {
  using FProbabilisticSensor = carla::sensor::data::ProbabilisticData;
  
  public:

    template <typename SensorT, typename ActorListT>
    static Buffer Serialize( const SensorT &, const ActorListT &detected_actors) 
    {
      const uint32_t size_in_bytes = sizeof(FProbabilisticSensor) * detected_actors.Num();
      Buffer buffer{size_in_bytes};
      unsigned char *it = buffer.data();
      //std::memcpy(it, ActorListT, sizeof(FProbabilisticSensor));
      
      for (carla::sensor::data::ProbabilisticData actor : detected_actors) 
      {
        std::memcpy(it, &actor, sizeof(FProbabilisticSensor));
        it += sizeof(FProbabilisticSensor);
      }
      return buffer;
    }
    static SharedPtr<SensorData> Deserialize(RawData &&data);
  };



  //class ProbabilisticSerializer {
  //public:

  //  template <typename SensorT, typename EpisodeT, typename ActorListT>
  //  static Buffer Serialize(
  //      const SensorT &,
  //      const EpisodeT &episode,
  //      const ActorListT &detected_actors) {
  //    const uint32_t size_in_bytes = sizeof(ActorId) * detected_actors.Num();
  //    Buffer buffer{size_in_bytes};
  //    unsigned char *it = buffer.data();
  //    for (auto *actor : detected_actors) {
  //      ActorId id = episode.FindCarlaActor(actor) -> GetActorId();
  //      std::memcpy(it, &id, sizeof(ActorId));
  //      it += sizeof(ActorId);
  //    }
  //    return buffer;
  //  }

  //  static SharedPtr<SensorData> Deserialize(RawData &&data);
  //};


} // namespace s11n
} // namespace sensor
} // namespace carla