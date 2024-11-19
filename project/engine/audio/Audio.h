#pragma once
#include <wrl.h>
#include <xaudio2.h>
#pragma comment(lib,"xaudio2.lib")
#include <fstream>

#include "WavFile.h"

/// <summary>
/// サウンド再生機能
/// </summary>
class Audio
{
private:// シングルトン設計

	static Audio* instance;

	/// <summary>
	/// コンストラクタ、デストラクタの隠蔽
	/// </summary>
	Audio() = default;
	~Audio() = default;

	/// <summary>
	/// コピーコンストラクタの封印
	/// </summary>
	/// <param name=""></param>
	Audio(Audio&) = delete;

	/// <summary>
	/// コピー代入演算の封印
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	Audio operator=(Audio&) = delete;

public:

	/// <summary>
	///	シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static Audio* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void InitXAudio2();
	
	/// <summary>
	/// 音声データの読み込み
	/// </summary>
	/// <param name="filename"></param>
	/// <returns></returns>
	SoundData SoundLoadWave(const char* filename);

	/// <summary>
	/// 音声データ開放
	/// </summary>
	/// <param name="soundData"></param>
	void SoundUnload(SoundData* soundData);

	/// <summary>
	/// 音声再生
	/// </summary>
	/// <param name="xAduio2"></param>
	/// <param name="soundData"></param>
	void SoundPlayWave(const SoundData& soundData);

	/// <summary>
	/// XAudio2解放処理
	/// </summary>
	void ResetXAudio2();

private:
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	IXAudio2MasteringVoice* masterVoice_ = nullptr;
};

