#pragma once
#include "VertexData.h"
#include "Material.h"
#include "TransformationMatrix.h"
#include "DirectXCommon.h"

class SpriteCommon;

/// <summary>
/// スプライト
/// </summary>
class Sprite
{
public:

	//-------------基本処理-------------//

	Sprite();
	~Sprite();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="spriteCommon"></param>
	/// <param name="dxCommon"></param>
	void Initialize(SpriteCommon* spriteCommon, DirectXCommon* dxCommon, std::string textureFilePath);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// ImGuiで変更する処理
	/// </summary>
	void ImGui();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource);

	//-------------ゲッター・セッター-------------//

	const Vector2& GetPosition()const { return position_; }
	
	float GetRotation()const { return rotation_; }

	const Vector2& GetSize()const { return size_; }

	const Vector4& GetColor()const { return materialData_->color; }

	const Vector2& GetAnchorPoint()const { return anchorPoint_; }

	bool GetIsFlipX()const { return isFlipX_; }

	bool GetIsFlipY()const { return isFlipY_; }

	const Vector2& GetTextureLeftTop()const { return textureLeftTop_; }

	const Vector2& GetTextureSize()const { return textureSize_; }

	void SetPosition(const Vector2& position) { this->position_ = position; }
	
	void SetRotation(float rotation) { this->rotation_ = rotation; }
	
	void SetSize(const Vector2& size) { this->size_ = size; }

	void SetColor(const Vector4& color) { this->materialData_->color = color; }

	void SetAnchorPoint(const Vector2& anchorPoint) { this->anchorPoint_ = anchorPoint; }

	void SetFlipX(bool isFlipX) { this->isFlipX_ = isFlipX; }

	void SetFlipY(bool isFlipY) { this->isFlipY_ = isFlipY; }

	void GetTextureLeftTop(const Vector2& textureLeftTop) { this->textureLeftTop_ = textureLeftTop; }

	void GetTextureSize(const Vector2& textureSize) { this->textureSize_ = textureSize; }


private:
	//-------------メンバ変数-------------//

	// 座標
	Vector2 position_ = { 0.0f,0.0f };
	// 回転
	float rotation_ = 0.0f;
	// サイズ
	Vector2 size_ = { 640.0f,360.0f };

	// アンカーポイント
	// {0,0} = 左上、{0,1} = 左下
	// {0.5,0.5} = 中央、{-0.5,-0.5} = 画面外
	Vector2 anchorPoint_ = { 0.0f,0.0f };

	// 左右フリップ
	bool isFlipX_ = false;
	// 上下フリップ
	bool isFlipY_ = false;

	// テクスチャ左上座標
	Vector2 textureLeftTop_ = { 0.0f,0.0f };

	// テクスチャ切り出しサイズ
	Vector2 textureSize_ = { 100.f,100.0f };

	// テクスチャ番号
	uint32_t textureIndex_ = 0;

	SpriteCommon* spriteCommon_ = nullptr;
	DirectXCommon* dxCommon_ = nullptr;

	Transform transform_ = {};
	Transform uvTransform_ = {};

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrix_;

	// バッファリソース内のデータを指すポインタ
	VertexData* vertexData_ = nullptr;
	uint32_t* indexData_ = nullptr;
	Material* materialData_ = nullptr;
	TransformationMatrix* transformationMatrixData_ = nullptr;

	// バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};
	D3D12_INDEX_BUFFER_VIEW indexBufferView_ = {};

	//-------------メンバ関数-------------//

	/// <summary>
	/// 頂点データ作成
	/// </summary>
	void CreateVertexData();

	/// <summary>
	/// 配列データ作成
	/// </summary>
	void CreateIndexData();

	/// <summary>
	/// マテリアルデータ作成
	/// </summary>
	void CreateMaterialData();

	/// <summary>
	/// 座標変換行列データ作成
	/// </summary>
	void CreateTrasnformationMatrixData();

	/// <summary>
	/// WVP行列を作成する
	/// </summary>
	void CreateWVPMatrix();

	/// <summary>
	/// テクスチャサイズをイメージに合わせる
	/// </summary>
	void AdjustTextureSize();

};

