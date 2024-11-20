#pragma once

/// <summary>
/// 　2次元ベクトル
/// </summary>
struct Vector2 final {
	float x;
	float y;

	// オペレーター

	Vector2 operator+=(Vector2 position) {
		return Vector2(this->x += position.x, this->y += position.y);
	}

};

