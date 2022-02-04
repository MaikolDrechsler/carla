#!/usr/bin/env python
import sys
import glob
import os
import time
#import pygame

try:
    sys.path.append(glob.glob('../carla/dist/carla-*%d.%d-%s.egg' % (sys.version_info.major,sys.version_info.minor,'win-amd64' if os.name == 'nt' else 'linux-x86_64'))[0])
except IndexError:
    pass
try:
    sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))) + '/carla')
except IndexError:
    pass

import carla
#clock = pygame.time.Clock()

client = carla.Client('localhost', 2000)  ### It starts the communication between server and client
client.set_timeout(10.0)  # seconds       ### After 10 seconds without communication with CARLA server the script stops
world = client.get_world()
vehicles = world.get_actors().filter('vehicle.*')
ego = vehicles[0]
blueprint_library = world.get_blueprint_library()

blueprint = blueprint_library.find('sensor.other.probabilistic')
#print (dir(blueprint))
blueprint.set_attribute('noise_seed', '0.0')
blueprint.set_attribute('range', '10.0')
trans = carla.Transform(carla.Location(x=2.0, y=0.0, z=1.1), carla.Rotation(pitch=0.0, yaw=0.0, roll=0.0))
sensor = world.spawn_actor(blueprint, trans, attach_to=ego)
print(carla.Transform)


def callback(event):
    print(event)
    for actor_id in event:
        vehicle = world.get_actor(actor_id)
        print('Vehicle too close: %s' % vehicle.type_id)

sensor.listen(callback)

while True:
    #clock.tick()
    print("running")
    time.sleep(5)