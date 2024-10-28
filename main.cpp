#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include <time.h>
#include <functional>

#include "Math.h"

int main(int argc,char* argv[]) {
	// int型の比較
	Math<int,int> b1(10, 20);

	printf("小さい方の値を表示します\n");
	printf("%dと%dの比較結果 : %d\n",b1.a,b1.b, b1.Min(b1.a, b1.b));

	// float型の比較
	Math<float,float> b2(30.0f, 20.0f);
	printf("%fと%fの比較結果 : %f\n", b2.a, b2.b, b2.Min(b2.a, b2.b));

	// double型の比較
	Math<double,double> b3(20.0, 5.0);
	printf("%lfと%lfの比較結果 : %lf\n", b3.a, b3.b, b3.Min(b3.a, b3.b));

	// intとfloat型の比較
	Math<int, float> b4(40, 39.0f);
	printf("%dと%fの比較結果 : %d\n", b4.a, b4.b, b4.Min(b4.a, b4.b));


	return 0;
}