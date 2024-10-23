#pragma once
#include "MyMath.h"

/// <summary>
/// 座標変換行列
/// </summary>
struct TransformationMatrix final{
	Matrix4x4 WVP;
	Matrix4x4 World;
};
