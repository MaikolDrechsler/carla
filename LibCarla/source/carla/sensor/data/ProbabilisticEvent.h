#pragma once

//#include "carla/rpc/ActorId.h"
#include "carla/sensor/data/Array.h"
#include "carla/sensor/s11n/ProbabilisticSerializer.h"

namespace carla {
namespace sensor {
namespace data {

  class ProbabilisticEvent : public Array<data::ProbabilisticData> {
  public:

    explicit ProbabilisticEvent(RawData &&data)
      : Array<data::ProbabilisticData>(0u,std::move(data)) {}
    using Serializer = s11n::ProbabilisticSerializer;
    friend Serializer;
  };

} // namespace data
} // namespace sensor
} // namespace carla
