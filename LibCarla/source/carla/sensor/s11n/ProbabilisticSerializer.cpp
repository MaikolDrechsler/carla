// Copyright (c) 2019 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "carla/sensor/s11n/ProbabilisticSerializer.h"

#include "carla/sensor/data/ProbabilisticEvent.h"

namespace carla {
namespace sensor {
namespace s11n {

  SharedPtr<SensorData> ProbabilisticSerializer::Deserialize(RawData &&data) 
  {
    return SharedPtr<data::ProbabilisticEvent>(new data::ProbabilisticEvent{std::move(data)});

  }

} // namespace s11n
} // namespace sensor
} // namespace carla