#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include<time.h>

typedef void(*PFunc)(std::string);

// サイコロの出目を計算
int CalculateTheRoll() {
	unsigned int currentTime = time(nullptr);
	srand(currentTime);
	// 1~6の出目のサイコロ
	int dice = rand() % 6 + 1;
	
	return dice;
}

// サイコロの結果を表示
void PrintResult(int dice) {
	std::cout << "サイコロの出目 = " << dice << std::endl;
}

// ユーザーから回答を受付
std::string Reception() {
	std::cout << "これからサイコロを振ります" << std::endl;

	// ユーザーから奇数か偶数かの入力を受付、表示
	std::string userInput;
	std::cout << "奇数か偶数かを入力してください(奇数/偶数) : ";
	std::cin >> userInput;

	if (userInput != "偶数" && userInput != "奇数") {
		std::cout << "受け付けられない文字です" << std::endl;
		exit(1);
	}

	return userInput;
}

// ユーザーからの回答をジャッジ
void Judgement(std::string userInput) {

	int dice = CalculateTheRoll();

	if (dice % 2 == 0 && userInput == "偶数" || dice % 2 != 0 && userInput == "奇数") {
		std::cout << "正解" << std::endl;
	} else {
		std::cout << "不正解" << std::endl;
	}

	PrintResult(dice);
}

void SetTimeout(PFunc p, int second,std::string userInput) {
	// コールバック関数を呼び出す
	Sleep(second * 1000);

	p(userInput);
}

int main(void) {

	int coolTime = 3;

	std::string userInput = Reception();

	// 関数ポインタを宣言
	void (*pfunc)(std::string);

	pfunc = Judgement;
	// 3秒待つようにセット
	SetTimeout(pfunc, coolTime,userInput);

	return 0;
}