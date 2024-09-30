#include <stdio.h>
#include <iostream>
#include <Windows.h>

template<typename Type>

Type Min(Type a, Type b) {
	if (a < b) {
		return a;
	} else {
		return b;
	}
}

template<>

char Min<char>(char a, char b) {
	printf("数字以外は代入できません！\n");
	return 0;
}

int main() {
	// int型
	int x = 3;
	int y = 5;

	// float型
	float a = 90.0f;
	float b = 25.0f;

	// double型
	double p = 3.141592;
	double q = 4.252603;

	char str1 = 'K';
	char str2 = 'G';

	printf("intの比較 : %d,%d\n",x,y);
	printf("%d\n", Min(x,y));

	printf("floatの比較 : %f,%f\n",a,b);
	printf("%f\n", Min(a,b));

	printf("doubleの比較 : %f,%f\n",p,q);
	printf("%f\n", Min(p,q));

	printf("charの比較 : %c,%c\n",str1,str2);
	Min(str1, str2);

	return 0;
}