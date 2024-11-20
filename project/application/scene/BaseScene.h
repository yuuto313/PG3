#pragma once
#include <memory>

class SceneManager;

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

	virtual void ImGui() = 0;

	virtual void Draw() = 0;

	virtual void SetSceneManager(SceneManager* sceneManager) { sceneManager_ = sceneManager; }

private:

	// シーンマネージャー（借りてくる）
	SceneManager* sceneManager_ = nullptr;

};

