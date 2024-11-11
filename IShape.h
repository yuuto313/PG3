#pragma once
#include <iostream>

class IShape
{
public:

	IShape();
	~IShape();

	/// <summary>
	/// 図形の情報
	/// </summary>
	virtual void Info() = 0;
	
	/// <summary>
	/// 面積を求める
	/// </summary>
	virtual void Size() = 0;

	/// <summary>
	/// 求めた面積を求める表示
	/// </summary>
	virtual void Draw() = 0;

protected:

	// 面積
	float area_ = 0;

};

