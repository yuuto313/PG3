#include <stdio.h>
#include <Windows.h>
#include <time.h>
#include <functional>
#include <list>
#include <iostream>
#include <cstring>

using namespace std;

int main(int argc,char* argv[]) {	

	list<char*> lst;

	const char* strings[] = { "Tokyo","Kanda","Akihabara","Okachimachi","Ueno","Uguisudani","Nippori","Tabata","Komagome","Sugamo","Otsuka","Ikebukuro","Mejiro","Takadanobaba",
		"Shin-Okubo","Shinjuku","Yoyogi","Harajuku","Shibuya","Ebisu","Meguro","Gotanda","Osaki","Shinagawa","Tamachi","Hamamatucho","Shimbashi","Yurakucho" };

	// 文字列を追加
	for (const char* string : strings) {
		// 必要なメモリを確保
		char* copy = new char[strlen(string) + 1];
		// 文字列をコピー
		strcpy_s(copy, strlen(string) + 1, string);
		// リストに追加               
		lst.push_back(copy);          
	}

	// 出力
	cout << "<<1970年の山手線です>>" << endl;
	for (const char* string : lst) {
		cout << string << endl;
	}

	// 田端の前に西日暮里を追加する
	for (list<char*>::iterator itr = lst.begin(); itr != lst.end(); ++itr) {
		if (strcmp(*itr,"Tabata") == 0) {
			char* newStation = new char[strlen("Nishi-Nippori") + 1];
			// 文字列をコピー
			strcpy_s(newStation, strlen("Nishi-Nippori") + 1, "Nishi-Nippori");
			// リストに挿入
			itr = lst.insert(itr, newStation);
			++itr;
		}
	}

	// 出力
	cout << "<<2020年の山手線です>>" << endl;
	for (const char* string : lst) {
		cout << string << endl;
	}

	// 田町の前に高輪ゲートウェイを追加
	for (list<char*>::iterator itr = lst.begin(); itr != lst.end(); ++itr) {
		if (strcmp(*itr, "Tamachi") == 0) {
			char* newStation = new char[strlen("Takanawa Gateway") + 1];
			// 文字列をコピー
			strcpy_s(newStation, strlen("Takanawa Gateway") + 1, "Takanawa Gateway");
			// リストに挿入
			itr = lst.insert(itr, newStation);
			++itr;
		}
	}

	// 出力
	cout << "<<2022年の山手線です>>" << endl;
	for (const char* string : lst) {
		cout << string << endl;
	}

	// メモリ解放と要素の削除
	while (!lst.empty()) {
		// 先頭の要素を取得
		char* str = lst.front(); 
		// メモリ解放
		delete[] str;   
		// リストから削除                
		lst.pop_front();         
	}

	return 0;
}