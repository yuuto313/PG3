#include <stdio.h>
#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable>

using namespace std;

int num = 1;
mutex mtx;              // 手動でロック (lock()) とアンロック (unlock()) を行う必要がある
condition_variable cv;  // スレッドが特定の条件を満たすまで待機するためのツール
// notify_all()         //条件変数を使用して待機しているすべてのスレッドを再開させるための関数 


void PrintThread1() {
	unique_lock<mutex> lock(mtx);
	while (num != 1) {
		cv.wait(lock);  // numが1になるまで待つ
	}

	cout << "thread1" << endl;

	num = 2;  // 次に実行するスレッドを2に設定
	cv.notify_all();  // 他のスレッドに通知
}

void PrintThread3() {
	unique_lock<mutex> lock(mtx);
	while (num != 3) {
		cv.wait(lock);  // numが3になるまで待つ
	}

	cout << "thread3" << endl;

	num = 3;  // 次に実行するスレッドを設定
	cv.notify_all();
}

void PrintThread2() {
	unique_lock<mutex> lock(mtx);
	while (num != 2) {
		cv.wait(lock);  // numが2になるまで待つ
	}

	cout << "thread2" << endl;

	num = 3;  // 次に実行するスレッドを3に設定
	cv.notify_all();
}

int main(int argc,char* argv[]) {	

	thread th1(PrintThread1);
	thread th2(PrintThread2);
	thread th3(PrintThread3);

	th1.join();
	th2.join();
	th3.join();


	return 0;
}