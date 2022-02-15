#!/usr/bin/env python
import sys
import glob
import os
import time
#import pygame

### Define sensor callback
def callback(event):
    print(event.timestamp)
    for actor in event:
        print(actor)

try:
    sys.path.append(glob.glob('../carla/dist/carla-*%d.%d-%s.egg' % (sys.version_info.major,sys.version_info.minor,'win-amd64' if os.name == 'nt' else 'linux-x86_64'))[0])
except IndexError:
    pass
try:
    sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))) + '/carla')
except IndexError:
    pass

import carla

### It starts the communication between server and client
client = carla.Client('localhost', 2000)

### After 10 seconds without communication with CARLA server the script stops
client.set_timeout(10.0)  # seconds

### Get current world from the server
world = client.get_world()

### Find the first vehicle spawned in the world
vehicles = world.get_actors().filter('vehicle.*')
ego = vehicles[0]


### Find Probabilistic sensor blueprint and set the attibutes
blueprint_library = world.get_blueprint_library()
blueprint = blueprint_library.find('sensor.other.probabilistic')
blueprint.set_attribute('noise_type', '2') # 0 for Ideal, 1 for cartesian and 2 for spherical
blueprint.set_attribute('range', '30.0')
blueprint.set_attribute('horizontal_fov', '180.0')
blueprint.set_attribute('noise_seed', '12345')
blueprint.set_attribute('noise_stddev_x', '2.0')          # standard deviation in meters - necessary for cartesian error
blueprint.set_attribute('noise_stddev_y', '2.0')          # standard deviation in meters - necessary for cartesian error
blueprint.set_attribute('noise_stddev_radius', '2.0')     # standard deviation in meters - necessary for spherical error
blueprint.set_attribute('noise_stddev_horizontal', '2.0') # standard deviation in degrees - necessary for spherical error
blueprint.set_attribute('noise_stddev_vel_radius', '2.0') # standard deviation in m/s - necessary for spherical error
blueprint.set_attribute('stddev_vel_horizontal', '2.0')   # standard deviation in degrees/s - necessary for spherical error

### Define position of the sensor on the vehicle and in which actor (vehicle) the sensor will be attached
trans = carla.Transform(carla.Location(x=2.0, y=0.0, z=1.1), carla.Rotation(pitch=0.0, yaw=0.0, roll=0.0))
sensor = world.spawn_actor(blueprint, trans, attach_to=ego)

### Initialize the sensor callback
sensor.listen(callback)

### Avoid that the callback is not initialized on time -- SHALL BE IMPROVED
while True:
    try:
        time.sleep(5)
    except KeyboardInterrupt:
        print('\nCancelled by user. Destroying Probabilistic Sensor!')
        sensor.destroy()
