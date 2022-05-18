#pragma once

//#include "carla/rpc/Location.h"
//#include "carla/rpc/Rotation.h"
//#include "carla/sensor/data/SemanticLidarData.h"
//#include "Math/Vector.h"


#include <cstdint>
#include <vector>

namespace carla {
namespace sensor {

namespace s11n {
  class ProbabilisticSerializer;
} // namespace s11n

namespace data {

  struct ProbabilisticData 
  {
    int objectid;
    float position_x;
    float position_y;
    float velocity_x; 
    float velocity_y; 
    float orientation; 
    float length; 
    float width;
    float height;    
    int classification;
  };

} // namespace data
} // namespace sensor
} // namespace carla
