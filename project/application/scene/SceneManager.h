#pragma once
#include "BaseScene.h"
#include "AbstractSceneFactory.h"

/// <summary>
/// シーン管理
/// </summary>
class SceneManager
{
private:// シングルトン設計

	static SceneManager* instance;

	/// <summary>
	/// コンストラクタ、デストラクタの隠蔽
	/// </summary>
	SceneManager() = default;
	~SceneManager() = default;

	/// <summary>
	/// コピーコンストラクタの封印
	/// </summary>
	/// <param name=""></param>
	SceneManager(SceneManager&) = delete;

	/// <summary>
	/// コピー代入演算の封印
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	SceneManager& operator=(SceneManager&) = delete;

public:

	/// <summary>
	///	シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static SceneManager* GetInstance();

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

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
	/// <param name="sceneName"></param>
	void ChangeScene(const std::string& sceneName);

	/// <summary>
	/// シーンファクトリーのセッター
	/// </summary>
	/// <param name="sceneFactory"></param>
	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; }

private:
	// 今のシーン（実行中シーン)
	std::unique_ptr<BaseScene> scene_;

	// 次のシーン
	std::unique_ptr<BaseScene> nextScene_;

	// シーンファクトリー（借りてくる）
	AbstractSceneFactory* sceneFactory_ = nullptr;


};

