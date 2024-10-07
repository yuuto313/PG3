#include <stdio.h>
#include <iostream>
#include <Windows.h>

// 一般的な賃金体系
int RegularWage(int hours) {
	// 時給
	const int hourlyWage = 1072;
	return hourlyWage * hours;
}

// 再帰的な賃金体系
int RecursiveWage(int hour) {
	if (hour == 1) {
		// 最初の1時間目の時給
		return 100;
	} else {
		// それ以降の時給
		return RecursiveWage(hour - 1) * 2 - 50;
	}
}

int main() {

	// 働く時間
	int workingHours = 10;
	// 結果
	int result = {};

	for (int hour = 1; hour < workingHours; hour++) {
		result = RegularWage(hour);
		printf("一般的な賃金体系\n");
		printf("%d\n", result);

		result = RecursiveWage(hour);
		printf("再帰的な賃金体系\n");
		printf("%d\n", result);
	}

	return 0;
}