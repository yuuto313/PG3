#pragma once

struct Matrix4x4 final {
	float m[4][4];
};

Matrix4x4 MakeIdentity4x4() {
	Matrix4x4 result{};
	result.m[0][0] = 1.f;
	result.m[1][1] = 1.f;
	result.m[2][2] = 1.f;
	result.m[3][3] = 1.f;
	return result;
}
