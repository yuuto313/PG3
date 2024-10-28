#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include <time.h>
#include <functional>

#include "Enemy.h"

#define FREQUENCY 3

int main(int argc,char* argv[]) {

	Enemy* pEnemy = nullptr;
	pEnemy = new Enemy();

	for (int i = 0; i < FREQUENCY; i++) {
		pEnemy->Execute();
	}

	return 0;
}