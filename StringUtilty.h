#pragma once
#include <string>

//文字コードユーティリティ
namespace StringUtility {
	//string->wstringに変換する
	std::wstring ConvertString(const std::string& str);

	//wstring->stringに変換する
	std::string ConvertString(const std::wstring& str);

};
