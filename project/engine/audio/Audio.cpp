#include "Audio.h"
#include <assert.h>

Audio* Audio::instance = nullptr;

Audio* Audio::GetInstance()
{
	if (instance == nullptr) {
		instance = new Audio();
	}

	return instance;
}

void Audio::InitXAudio2()
{
	// XAudioエンジンのインスタンスを生成
	HRESULT result = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);

	// マスターボイスを生成
	result = xAudio2_->CreateMasteringVoice(&masterVoice_);

}

void Audio::Finalize()
{
	delete instance;
	instance = nullptr;
}

SoundData Audio::SoundLoadWave(const char* filename)
{
	//--------------------------------
	// 1.ファイルオープン
	//--------------------------------
	
	//ファイル入力ストリームのインスタンス
	std::ifstream file;
	//.wavファイルをバイナリモードで開く
	file.open(filename, std::ios_base::binary);
	//ファイルオープン失敗を検出する
	assert(file.is_open());

	//--------------------------------
	// 2.wavデータ読み込み
	//--------------------------------
	
	//Riffヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	//ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	//タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}

	//Formatチャンクの読み込み
	FormatChunk format = {};
	//チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt", 4) == 0) {
		assert(0);
	}
	//チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	//Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	//JUNKチャンクを検索した場合
	if (strncmp(data.id, "JUNK", 4) == 0) {
		//読み取り位置をJUNKチャンク位置まで進める
		file.seekg(data.size, std::ios_base::cur);
		//再読み込み
		file.read((char*)&data, sizeof(data));
	}

	if (strncmp(data.id, "data", 4) != 0) {
		assert(0);
	}

	//Dataチャンクのデータ部（波形データ）の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	//--------------------------------
	// 3.ファイルクローズ
	//--------------------------------

	file.close();

	//--------------------------------
	// 4.読み込んだ音声データをreturn
	//--------------------------------

	//returnするための音声データ
	SoundData soundData = {};

	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;

	return soundData;
}

void Audio::SoundUnload(SoundData* soundData)
{
	//バッファのメモリを開放
	delete[] soundData->pBuffer;

	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};

	masterVoice_ = nullptr;

}

void Audio::SoundPlayWave(const SoundData& soundData)
{
	HRESULT result;

	//--------------------------------
	// 波形フォーマットをもとにSourceVoiceの生成
	//--------------------------------

	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2_.Get()->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
	assert(SUCCEEDED(result));

	//--------------------------------
	// 再生する波形データの設定
	//--------------------------------
	
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer;
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	//--------------------------------
	// 波形データの再生
	//--------------------------------

	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->Start();
}

void Audio::ResetXAudio2()
{
	// XAudio2の解放
	xAudio2_.Reset();
}
