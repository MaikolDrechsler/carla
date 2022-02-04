#pragma once

#include "carla/rpc/ActorId.h"
#include "carla/sensor/data/Array.h"

namespace carla {
namespace sensor {
namespace data {

  class ProbabilisticEvent : public Array<rpc::ActorId> {
  public:

    explicit ProbabilisticEvent(RawData &&data)
      : Array<rpc::ActorId>(0u,std::move(data)) {}
  };

} // namespace data
} // namespace sensor
} // namespace carla