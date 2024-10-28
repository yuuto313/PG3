#pragma once

// 行動フェーズ
enum class Phase {
	Approach, // 接近
	Attack, // 射撃
	Leave, // 離脱
};

/// <summary>
///	敵 
/// </summary>
class Enemy
{
public:
	/// <summary>
	/// 接近フェーズ更新処理
	/// </summary>
	void UpdateApproach();

	/// <summary>
	/// 射撃フェーズ更新処理
	/// </summary>
	void UpdateAttack();

	/// <summary>
	/// 離脱フェーズ更新処理
	/// </summary>
	void UpdateLeave();

	/// <summary>
	/// 実行
	/// </summary>
	void Execute();

private:
	// メンバ関数ポインタのテーブル
	static void (Enemy::*spPhaseFuncTable[])();

	// 接近フェーズからスタート
	Phase phase_ = Phase::Approach;

};

