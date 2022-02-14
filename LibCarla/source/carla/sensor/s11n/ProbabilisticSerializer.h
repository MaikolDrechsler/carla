
// Copyright (c) 2019 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

//#include "Carla.h"
#include "carla/Memory.h"
#include "carla/sensor/RawData.h"
#include "carla/sensor/data/ProbabilisticData.h"

#include <cstdint>
#include <cstring>

namespace carla {
namespace sensor {

  class SensorData;

namespace s11n {

  class ProbabilisticSerializer 
  {
    using FProbabilisticSensor = carla::sensor::data::ProbabilisticData;
    
    public:

      template <typename SensorT, typename ActorListT>
      static Buffer Serialize( const SensorT &, const ActorListT &data) 
      {
      const uint32_t size_in_bytes = sizeof(FProbabilisticSensor) * data.size();
      Buffer buffer{size_in_bytes};
      buffer.copy_from(data); 
      return buffer;
      }

    static SharedPtr<SensorData> Deserialize(RawData &&data);
  };

} // namespace s11n
} // namespace sensor
} // namespace carla