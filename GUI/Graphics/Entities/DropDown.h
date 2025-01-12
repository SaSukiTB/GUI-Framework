#pragma once
#include "Entity.h"
class DropDown : public Entity
{
protected:
   
    std::vector<std::wstring>Names;
    int* Index;
    bool Active = false;
    std::wstring SelectedName;
    float DropWidth;
    float TextWidth = 0;
    float CutOffBuffer = 0;
    float SizeDifference;

    int MaxVisibleItems = 5;

    int PointerStart = 0;
    int PointerEnd = MaxVisibleItems;

    void ConvertSelectedName();
    void CalculateBuffer();
    void SetDropDownWidth();
    void ArrowNavigation();
    void UpdateScrollBar();

    bool ScrollBar = false;

    int TextSize = 11;
    std::string Font = "Verdana";
public:
    DropDown(float x, float y,  const std::wstring& text, int* items, std::vector<std::wstring>names);
    void Update();
    void Draw();
};
