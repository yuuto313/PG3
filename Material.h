#pragma once
#include "MyMath.h"

/// <summary>
/// マテリアル(構造体)
/// </summary>
struct Material {
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;

};

