#pragma once

/// <summary>
/// シーン基底クラス
/// </summary>
class BaseScene
{
public:

	~BaseScene() = default;

	virtual void Initialize() = 0;

	virtual void Finalize() = 0;

	virtual void Update() = 0;

	virtual void Draw() = 0;
};

