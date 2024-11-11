#include <stdio.h>
#include <Windows.h>
#include <time.h>
#include <functional>
#include <vector>

#include "Circle.h"
#include "Rect.h"

int main(int argc,char* argv[]) {	

	std::vector<std::unique_ptr<IShape>> shapes;

	shapes.push_back(std::make_unique<Circle>());
	shapes.push_back(std::make_unique<Rect>());

	for (auto& shape : shapes) {
		shape->Info();
		shape->Size();
		shape->Draw();
	}

	return 0;
}