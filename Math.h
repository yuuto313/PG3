#pragma once

template <typename Type1, typename Type2>

class Math
{
public:
	// メンバ変数
	Type1 a;
	Type2 b;

	// コンストラクタ
	Math(Type1 a, Type2 b) : a(a), b(b) {};

	// メンバ関数
	
	/// <summary>
	/// 2つの引数の値を比べて小さい値を返す
	/// </summary>
	auto Min(Type1 _a, Type2 _b) {
		return (_a < _b) ? _a : _b;
	}

};
