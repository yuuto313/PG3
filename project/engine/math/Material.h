#pragma once
#include "MyMath.h"

/// <summary>
/// マテリアル(構造体)
/// </summary>
struct Material final{
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;

};



/// <summary>
/// マテリアルデータ(構造体)
/// </summary>
struct MaterialData final{
	std::string textureFilePath;
	// テクスチャ番号を保存する
	uint32_t textureIndex = 0;
};
