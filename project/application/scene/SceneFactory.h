#pragma once
#include "AbstractSceneFactory.h"

/// <summary>
/// このゲーム用のシーン工場
/// </summary>
class SceneFactory : public AbstractSceneFactory
{
public:

	std::unique_ptr<BaseScene> CreateScene(const std::string& sceneName) override;
};

