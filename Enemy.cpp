#include "Enemy.h"
#include <stdio.h>

void (Enemy::*Enemy::spPhaseFuncTable[])() = {
	&Enemy::UpdateApproach, // 要素番号0
	&Enemy::UpdateAttack, // 1
	&Enemy::UpdateLeave, // 2
};

void Enemy::UpdateApproach()
{
	printf("敵が迫ってきます!\n");
	phase_ = Phase::Attack;
}

void Enemy::UpdateAttack()
{
	printf("敵が攻撃してきます！\n");
	phase_ = Phase::Leave;
}

void Enemy::UpdateLeave()
{
	printf("敵が離脱します！\n");
	phase_ = Phase::Approach;
}

void Enemy::Execute()
{
	// メンバ関数ポインタに入ってる関数を実行する
	(this->*spPhaseFuncTable[static_cast<size_t>(phase_)])();
}
