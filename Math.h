#pragma once

template <typename Type1>

class Math
{
public:
	// メンバ変数
	Type1 a;
	Type1 b;

	// コンストラクタ
	Math(Type1 a, Type1 b) : a(a), b(b) {};

	// メンバ関数
	
	/// <summary>
	/// 2つの引数の値を比べて小さい値を返す
	/// </summary>
	auto Min(Type1 _a, Type1 _b) {
		return (_a < _b) ? _a : _b;
	}

	/// <summary>
	/// 2つの引数の値を比べて小さい値を返す<複数ver.>
	/// </summary>
	template <typename Type2>

	// decltype = 引数の型に応じて適切な型を推論する
	auto Min(Type1 _a, Type2 _b) -> decltype((_a < _b) ? _a : _b) {
		return (_a < _b) ? _a : _b;
	}

};
