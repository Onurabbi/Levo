#ifndef ASSET_H_
#define ASSET_H_

uint8_t * createAsset(AssetType assetType, char * fileName, size_t size, uint32_t maxCount, int *new);
uint32_t getAssetIndex(AssetType assetType, char * fileName, size_t size, uint32_t maxCount);
uint8_t * getAsset(AssetType assetType, char * fileName, size_t size, uint32_t maxCount);
uint8_t * getAssetByIndex(AssetType assetType, uint32_t index, size_t size);
bool initAssets(void);

#endif