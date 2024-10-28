#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include <time.h>
#include <functional>

#include "Math.h"

int main(int argc,char* argv[]) {	

	Math<int,int> b1(10, 20);
	Math<float,float> b2(30.0f, 20.5f);
	Math<double,double> b3(20.295, 20.352);
	Math<int, float> b4(30, 29.0f);
	Math<int, double> b5(20, 15.219);
	Math<float, double> b6(3.0f, 5.658);

	// int型の比較
	printf("小さい方の値を表示します\n");
	printf("int型の比較\n");
	printf("%dと%dの比較結果 : %d\n",b1.a,b1.b, b1.Min(b1.a, b1.b));
	printf("\n");

	// float型の比較
	printf("float型の比較\n");
	printf("%fと%fの比較結果 : %f\n", b2.a, b2.b, b2.Min(b2.a, b2.b));
	printf("\n");

	// double型の比較
	printf("double型の比較\n");
	printf("%lfと%lfの比較結果 : %lf\n", b3.a, b3.b, b3.Min(b3.a, b3.b));
	printf("\n");

	// intとfloat型の比較
	printf("intとfloat型の比較\n");
	printf("%dと%fの比較結果 : %f\n", b4.a, b4.b, b4.Min(b4.a, b4.b));
	printf("\n");

	// intとdouble型の比較
	printf("intとdouble型の比較\n");
	printf("%dと%lfの比較結果 : %f\n", b5.a, b5.b, b5.Min(b5.a, b5.b));
	printf("\n");

	// floatとdouble型の比較
	printf("floatとdouble型の比較\n");
	printf("%fと%lfの比較結果 : %lf\n", b6.a, b6.b, b6.Min(b6.a, b6.a));
	printf("\n");

	return 0;
}