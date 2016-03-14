# UE4VolumeSlicing

## Overview

This is a plugin for Unreal Engine 4 that uses a technique called Half Angle Slicing from the Nvidia book GPUGems.

http://http.developer.nvidia.com/GPUGems/gpugems_ch39.html

It works by slicing up square bounds along a vector that is half way between the camera vector and a user controlled light vector. 
This allows for slices that always point towards both the camera and the light.

This UE4 plugin is a procedural geometry component that is modifies from https://wiki.unrealengine.com/Procedural_Mesh_Generation

## Usage

You attach the component to an actor and then you should add a directional light to a Property on the component. 

The volumetric source content is created in maya as a 2D Texture atlas which is rendered from an orthographic camera. 
The indidual slices were rendered by setting the camera clipping planes start and end to be one slice thick and then animating them over time to move through the volume.
They were rendered as frames in the timeline and then the program TexturePacker was used to generate an atlas.

## Reading the volume in the Material

The first part of the material uses the BoundsUVW node in the Material Editor to generate a colour which is 0,0,0 to 1,1,1 to represent the min/max bounds of the geometry in the scene.
Then this colour is used as 3d coordinates to index into the 3D texture.

The second part of the Material takes the 3d colour uvw and does some modulo math to work out which 2D slic is needs to sample.

## Drawbacks

Because the sampling of the texture is done in 2D space and the input is 3D the sampling can jump accross the 2D tiles if mipmapping is turned on. To combat this at this stage, Mipmapping is turned off.
The plan for the future is implement the sampling using gather4/textureGather to get the samples and do interpolation manually.

## License
I've put this under MIT. Lemme know if that is an issue for anyone.
