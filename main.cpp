#include <stdio.h>
#include <Windows.h>
#include <time.h>
#include <functional>
#include <list>
#include <iostream>

using namespace std;

int main(int argc,char* argv[]) {	

	list<char> lst{ "Tokyo","Kanda" };


	for (list<char>::iterator itr = lst.begin(); itr != lst.end(); ++itr) {
		cout << *itr << "\n";
	}

	return 0;
}