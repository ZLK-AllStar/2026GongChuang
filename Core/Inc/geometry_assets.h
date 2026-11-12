#ifndef __GEOMETRY_ASSETS_H__
#define __GEOMETRY_ASSETS_H__
#include <stdint.h>
#define GEOMETRY_ASSET_SIZE 20U
#define GEOMETRY_ASSET_COUNT 72U
extern const uint16_t GeometryAssets[GEOMETRY_ASSET_COUNT][GEOMETRY_ASSET_SIZE * GEOMETRY_ASSET_SIZE];
uint16_t GeometryAssetIndex(uint8_t shape, uint8_t color);
#endif
