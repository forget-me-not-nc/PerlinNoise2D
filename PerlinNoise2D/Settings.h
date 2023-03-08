#pragma once

#include "RandomNoise.h"
#include "PerlinNoiseOctaves.h"

///////////////////////////////////
//
//
//////////////////////////////////

extern int multiplier;
extern unsigned int seed;
extern int octaves;

extern WORLD_SIZE worldSize;
extern INTERPOLATION_TYPES interpolationType;
extern NOISE_TYPE noiseType;

extern GLenum displayType;
extern GLenum polygonType;

extern float minValue;
extern float harshness;
extern float amplitude;
extern float persistence;
extern float lacunarity;

extern bool animate;
extern bool paintTerrainLayers;

extern glm::vec3 cameraPos;
extern glm::vec3 worldUp;

///////////////////////////////////
//
//
//////////////////////////////////