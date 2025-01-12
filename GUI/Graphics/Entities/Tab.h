#pragma once
#include "Entity.h"

class Tab : public Container
{
protected:
	int Index;
	int* Selected;
	int TextSize = 12;
	std::string Font = "Verdana";
public:
	Tab(const std::wstring& name, float x, float y, int* selectedtab, float width = 0, float height = 0);
	void Update();
	void Draw();
};
