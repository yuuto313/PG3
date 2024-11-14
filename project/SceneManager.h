#pragma once
#include "BaseScene.h"

/// <summary>
/// シーン管理
/// </summary>
class SceneManager
{
public:

	~SceneManager();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 次のシーン予約
	/// </summary>
	/// <param name="nextScene"></param>
	void SetNextScene(BaseScene* nextScene) { nextScene_ = nextScene; }

private:
	// 今のシーン（実行中シーン)
	BaseScene* scene_ = nullptr;

	// 次のシーン
	BaseScene* nextScene_ = nullptr;

};

