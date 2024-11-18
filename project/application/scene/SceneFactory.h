#pragma once
#include "AbstractSceneFactory.h"

/// <summary>
/// このゲーム用のシーン工場
/// </summary>
class SceneFactory : public AbstractSceneFactory
{
public:

	BaseScene* CreateScene(const std::string& sceneName) override;
};

