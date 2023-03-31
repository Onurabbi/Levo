#ifndef ASSET_H_
#define ASSET_H_

uint8_t * createAsset(AssetType assetType, char * fileName, int *new);
uint32_t getAssetIndex(AssetType assetType, char * fileName);
uint8_t * getAsset(AssetType assetType, char * fileName);
uint8_t * getAssetByIndex(AssetType assetType, uint32_t index);
bool initAssets(void);

#endif