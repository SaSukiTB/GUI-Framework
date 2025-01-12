#include "pch.h"
#include "Drawing.h"

ID2D1LinearGradientBrush* LinearBrush;
ID2D1GradientStopCollection* GradientStops = NULL;

std::unordered_map<std::wstring, IDWriteTextLayout*> TextCache;

void CreateLayer(const float& alpha)
{
	RenderTarget->PushLayer(
		D2D1::LayerParameters(
			D2D1::InfiniteRect(),
			nullptr,
			D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
			D2D1::IdentityMatrix(),
			alpha,
			nullptr,
			D2D1_LAYER_OPTIONS_NONE
		),
		nullptr);
}
void CreateLayer(const int& startx, const int& starty, const int& limitwidth, const int& limitheight)
{
	RenderTarget->PushLayer(
		D2D1::LayerParameters(
			D2D1::RectF(startx, starty, limitwidth, limitheight),
			nullptr,
			D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
			D2D1::IdentityMatrix(),
			1.0f,
			nullptr,
			D2D1_LAYER_OPTIONS_NONE
		),
		nullptr);
}
void CreateLayer(const int& startx, const int& starty, const int& limitwidth, const int& limitheight, const float& alpha)
{
	RenderTarget->PushLayer(
		D2D1::LayerParameters(
			D2D1::RectF(startx, starty, limitwidth, limitheight),
			nullptr,
			D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
			D2D1::IdentityMatrix(),
			alpha,
			nullptr,
			D2D1_LAYER_OPTIONS_NONE
		),
		nullptr);
}
void EndLayer()
{
	RenderTarget->PopLayer();
}
void CreateLinearBrush(ID2D1LinearGradientBrush** brush, const float& point1, MyColour colour1, const float& point2, MyColour colour2, const Vector2& start, const Vector2& end)
{
	if (*brush)
		(*brush)->Release();

	D2D1_GRADIENT_STOP stops[] =
	{
		{point1, colour1.Get()},
		{point2, colour2.Get()},
	};

	ID2D1GradientStopCollection* stopcollection;
	RenderTarget->CreateGradientStopCollection(
		stops,
		_countof(stops),
		D2D1_GAMMA_2_2,
		D2D1_EXTEND_MODE_CLAMP,
		&stopcollection
	);

	RenderTarget->CreateLinearGradientBrush(
		D2D1::LinearGradientBrushProperties(
			D2D1::Point2F(start.x, start.y),
			D2D1::Point2F(start.x + end.x, start.y + end.y)),
		stopcollection,
		brush
	);

}
void CreateLinearBrush(ID2D1LinearGradientBrush** brush, const float& point1, MyColour colour1, const float& point2, MyColour colour2, const float& point3, MyColour colour3, const Vector2& start, const Vector2& end)
{
	if (*brush)
		(*brush)->Release();

	D2D1_GRADIENT_STOP stops[] =
	{
		{point1, colour1.Get()},
		{point2, colour2.Get()},
		{point3, colour3.Get()}
	};

	ID2D1GradientStopCollection* stopcollection;
	RenderTarget->CreateGradientStopCollection(
		stops,
		_countof(stops),
		D2D1_GAMMA_2_2,
		D2D1_EXTEND_MODE_CLAMP,
		&stopcollection
	);

	RenderTarget->CreateLinearGradientBrush(
		D2D1::LinearGradientBrushProperties(
			D2D1::Point2F(start.x, start.y),
			D2D1::Point2F(start.x + end.x, start.y + end.y)),
		stopcollection,
		brush
	);
}
void PrintHRESULTError(HRESULT hr)
{
	LPVOID errorMsg;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&errorMsg,
		0, NULL);

	wprintf(L"Error: %s\n", (LPWSTR)errorMsg);
	LocalFree(errorMsg); // Free the buffer allocated by FormatMessage
}

void DrawText(int x, int y, const std::wstring& text, const std::string& font, const int& fontsize, MyColour colour, const FontAlignment& alignment)
{
	Brush->SetColor(colour.Get());
	Brush->SetOpacity(colour.a);
	IDWriteTextLayout* layout = nullptr;
	if (TextCache.find(text) == TextCache.end())
	{
		const HRESULT status = FontFactory->CreateTextLayout(text.data(), static_cast<std::uint32_t>(text.length()), Fonts[font].font, 4096.f, 4096.f, &TextCache[text]);
		if (!SUCCEEDED(status))
		{
			printf("Failed to create text layout. HRESULT: 0x%lx\n", status);
			PrintHRESULTError(status); // Print the error message
			return;
		}
		layout = TextCache[text];
	}
	else
		layout = TextCache[text];

	DWRITE_TEXT_RANGE range = DWRITE_TEXT_RANGE();
	range.length = text.length();
	range.startPosition = 0;
	layout->SetFontSize(fontsize, range);
	Vector2 txtSize = GetTextSize(text, font, fontsize);

	D2D1_POINT_2F point; // position point
	// set the position
	switch (alignment)
	{
	case FontAlignment::Centre:
		x -= (txtSize.x / 2);
		break;
	case FontAlignment::Right:
		x += txtSize.x;
		break;
	case FontAlignment::Left:
		x -= txtSize.x;
		break;
	case FontAlignment::None:
		break;
	case FontAlignment::CentreCentre:
		x -= (txtSize.x / 2);
		y -= (txtSize.y / 2);
		break;
	case FontAlignment::CentreLeft:
		x -= txtSize.x;
		y += (txtSize.y / 2);
		break;
	case FontAlignment::CentreRight:
		x += txtSize.x;
		y -= (txtSize.y / 2);
		break;
	}
	point.x = x;
	point.y = y;

	// draw
	RenderTarget->DrawTextLayout(point, layout, Brush, D2D1_DRAW_TEXT_OPTIONS::D2D1_DRAW_TEXT_OPTIONS_NONE);
	//layout->Release(); // free memory
}

void OutlineCircle(const int& x, const int& y, const float& radius, const float& linewidth, MyColour colour)
{
	Brush->SetColor(colour.Get());
	Brush->SetOpacity(colour.a);
	RenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	RenderTarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius), Brush, linewidth);
	RenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
}

void FilledCircle(const int& x, const int& y, const float& radius, MyColour colour)
{
	Brush->SetColor(colour.Get());
	Brush->SetOpacity(colour.a);
	RenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	RenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius), Brush);
	RenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
}

void OutlineRectangle(const int& x, const int& y, const int& width, const int& height, const int& linewidth, MyColour colour)
{
	RenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
	Brush->SetColor(colour.Get());
	Brush->SetOpacity(colour.a);
	D2D1_RECT_F rect = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(width + x), static_cast<float>(height + y) };
	RenderTarget->DrawRectangle(rect, Brush, linewidth);
	RenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
}

void FilledRectangle(const int& x, const int& y, const int& width, const int& height, MyColour colour)
{
	Brush->SetColor(colour.Get());
	Brush->SetOpacity(colour.a);
	D2D1_RECT_F rect = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(width + x), static_cast<float>(height + y) };
	RenderTarget->FillRectangle(rect, Brush);
}
void FilledRectangle(const int& x, const int& y, const int& width, const int& height, ID2D1LinearGradientBrush* brush, const LinearBrushStyle& style)
{
	switch (style)
	{
	case LinearBrushStyle::Rightwards:
		brush->SetStartPoint(D2D1::Point2F(x, y));
		brush->SetEndPoint(D2D1::Point2F(x + width, y));
		break;

	case LinearBrushStyle::Leftwards:
		brush->SetStartPoint(D2D1::Point2F(x + width, y));
		brush->SetEndPoint(D2D1::Point2F(x, y));
		break;

	case LinearBrushStyle::Downwards:
		brush->SetStartPoint(D2D1::Point2F(x, y));
		brush->SetEndPoint(D2D1::Point2F(x, y + height));
		break;

	case LinearBrushStyle::Upwards:
		brush->SetStartPoint(D2D1::Point2F(x, y + height));
		brush->SetEndPoint(D2D1::Point2F(x, y));
		break;
	}
	D2D1_RECT_F rect = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(width + x), static_cast<float>(height + y) };
	RenderTarget->FillRectangle(rect, brush);
}


void OutlineRoundedRectangle(const int& x, const int& y, const int& width, const int& height, const int& linewidth, const int& rounding, MyColour colour)
{
	// RenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
	RenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	Brush->SetColor(colour.Get());
	Brush->SetOpacity(colour.a);
	D2D1_RECT_F rect = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(width + x), static_cast<float>(height + y) };
	D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(
		rect,
		static_cast<float>(rounding),
		static_cast<float>(rounding)
	);
	RenderTarget->DrawRoundedRectangle(roundedRect, Brush, linewidth);
	//RenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
}

void FilledRoundedRectangle(const int& x, const int& y, const int& width, const int& height, const int& rounding, MyColour colour)
{
	Brush->SetColor(colour.Get());
	Brush->SetOpacity(colour.a);
	D2D1_RECT_F rect = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(width + x), static_cast<float>(height + y) };
	D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(
		rect,
		static_cast<float>(rounding),
		static_cast<float>(rounding)
	);
	RenderTarget->FillRoundedRectangle(roundedRect, Brush);
}

void FilledLine(const int& xstart, const int& ystart, const int& xend, const int& yend, const int& width, MyColour colour)
{
	RenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	D2D1_POINT_2F start = { static_cast<float>(xstart), static_cast<float>(ystart) };
	D2D1_POINT_2F finish = { static_cast<float>(xend), static_cast<float>(yend) };
	Brush->SetColor(colour.Get());
	Brush->SetOpacity(colour.a);
	RenderTarget->DrawLine(start, finish, Brush);
}

// allows you to draw single lines, rather than being forced to use double
void FilledLineAliased(const int& xstart, const int& ystart, const int& xend, const int& yend, const int& width, MyColour colour)
{
	RenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
	D2D1_POINT_2F start = { static_cast<float>(xstart), static_cast<float>(ystart) };
	D2D1_POINT_2F finish = { static_cast<float>(xend), static_cast<float>(yend) };
	Brush->SetColor(colour.Get());
	Brush->SetOpacity(colour.a);
	RenderTarget->DrawLine(start, finish, Brush);
	RenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
}

void SaturationSlider(const int& x, const int& y, const int& width, const int& height, MyColour colour)
{
	{
		D2D1_GRADIENT_STOP stops[] =
		{
			{0.0f, colour.Get()},
			{1.0f, D2D1::ColorF::Black},
		};
		RenderTarget->CreateGradientStopCollection(
			stops,
			_countof(stops),
			D2D1_GAMMA_2_2,
			D2D1_EXTEND_MODE_CLAMP,
			&GradientStops
		);
		RenderTarget->CreateLinearGradientBrush(
			D2D1::LinearGradientBrushProperties(
				D2D1::Point2F(x + width, y),
				D2D1::Point2F(x, y + height)),
			GradientStops,
			&LinearBrush
		);

		D2D1_RECT_F rect = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(width + x), static_cast<float>(height + y) };

		RenderTarget->FillRectangle(rect, LinearBrush);
		GradientStops->Release();
		LinearBrush->Release();
	}
	{
		D2D1_GRADIENT_STOP stops[] =
		{
			{0.00f, Colour(255, 255, 255, 200)},
			{0.15f, Colour(colour.r, colour.g, colour.b, 100)},
			{1.0f, Colour(0, 0, 0, 100)},
			{0.6f, Colour(40, 40, 40, 100)},
		};

		RenderTarget->CreateGradientStopCollection(
			stops,
			_countof(stops),
			D2D1_GAMMA_2_2,
			D2D1_EXTEND_MODE_CLAMP,
			&GradientStops
		);

		RenderTarget->CreateLinearGradientBrush(
			D2D1::LinearGradientBrushProperties(
				D2D1::Point2F(x, y),
				D2D1::Point2F(x, y + height)),
			GradientStops,
			&LinearBrush
		);

		D2D1_RECT_F rect = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(width + x), static_cast<float>(height + y) };

		RenderTarget->FillRectangle(rect, LinearBrush);
		GradientStops->Release();
		LinearBrush->Release();
	}
}

void AlphaSlider(const int& x, const int& y, const int& width, const int& height, MyColour col)
{
	D2D1_GRADIENT_STOP stops[] =
	{
		{0.0f, D2D1::ColorF(col.r, col.g, col.b, 255)},
		{1.0f, D2D1::ColorF(col.r, col.g, col.b, 0)},
	};
	RenderTarget->CreateGradientStopCollection(
		stops,
		_countof(stops),
		D2D1_GAMMA_2_2,
		D2D1_EXTEND_MODE_CLAMP,
		&GradientStops
	);
	RenderTarget->CreateLinearGradientBrush(
		D2D1::LinearGradientBrushProperties(
			D2D1::Point2F(x, y),
			D2D1::Point2F(x, y + height)),
		GradientStops,
		&LinearBrush
	);

	D2D1_RECT_F rect = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(width + x), static_cast<float>(height + y) };

	RenderTarget->FillRectangle(rect, LinearBrush);
	GradientStops->Release();
	LinearBrush->Release();
}

void HueSlider(const int& x, const int& y, const int& width, const int& height)
{
	D2D1_GRADIENT_STOP stops[] =
	{
		{0.00f, Colour(255, 0, 0, 255)},
		{0.00f, Colour(255, 0, 0, 255)},
		{0.16f, Colour(255, 255, 0, 255)},
		{0.32f, Colour(0, 255, 0, 255)},
		{0.48f, Colour(0, 255, 255, 255)},
		{0.64f, Colour(0, 0, 255, 255)},
		{0.80f, Colour(255, 0, 255, 255)},
		{0.96f, Colour(255, 0, 0, 255)},
	};

	RenderTarget->CreateGradientStopCollection(
		stops,
		_countof(stops),
		D2D1_GAMMA_2_2,
		D2D1_EXTEND_MODE_CLAMP,
		&GradientStops
	);
	RenderTarget->CreateLinearGradientBrush(
		D2D1::LinearGradientBrushProperties(
			D2D1::Point2F(x, y),
			D2D1::Point2F(x + width, y)),
		GradientStops,
		&LinearBrush
	);

	D2D1_RECT_F rect = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(width + x), static_cast<float>(height + y) };

	RenderTarget->FillRectangle(rect, LinearBrush);
	GradientStops->Release();
	LinearBrush->Release();
}

// draws at native resolution
void DrawBitmap(ID2D1Bitmap* bmp, const int& x, const int& y)
{
	RenderTarget->DrawBitmap(bmp, D2D1::RectF(static_cast<float>(x), static_cast<float>(y), bmp->GetSize().width + x, bmp->GetSize().height + y), 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, D2D1::RectF(0.0f, 0.0f, bmp->GetSize().width, bmp->GetSize().height));
}

// squeezes, compresses or expands the image to set variables.
void DrawBitmap(ID2D1Bitmap* bmp, const int& x, const int& y, const int& width, const int& height)
{
	RenderTarget->DrawBitmap(
		bmp,
		D2D1::RectF(static_cast<float>(x), static_cast<float>(y), static_cast<float>(width + x), static_cast<float>(height + y)),
		1.0f,  // Opacity
		D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		D2D1::RectF(0.0f, 0.0f, bmp->GetSize().width, bmp->GetSize().height)
	);
}

void DrawBitmap(ID2D1Bitmap* bmp, const int& x, const int& y, const int& width, const int& height, float imageposx, float imageposy, float imagewidth, float imageheight)
{
	if (imageposx < 0)
		imageposx = 0;
	if (imageposy < 0)
		imageposy = 0;
	if (imagewidth > bmp->GetSize().width)
		imagewidth = bmp->GetSize().width;
	if (imageheight > bmp->GetSize().height)
		imageheight = bmp->GetSize().height;

	RenderTarget->DrawBitmap(bmp, D2D1::RectF(static_cast<float>(x), static_cast<float>(y), static_cast<float>(width + x), static_cast<float>(height + y)), 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, D2D1::RectF(imageposx, imageposy, imagewidth, imageheight));
}



void DrawBitmap(ID2D1Bitmap* bmp, const int& x, const int& y, const int& width, const int& height, const float& rotation)
{
	D2D1_POINT_2F center = D2D1::Point2F(x + (width / 2), y + (height / 2));
	RenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(rotation, center));
	RenderTarget->DrawBitmap(bmp, D2D1::RectF(static_cast<float>(x), static_cast<float>(y), static_cast<float>(width + x), static_cast<float>(height + y)), 1.0f, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, D2D1::RectF(0.0f, 0.0f, bmp->GetSize().width, bmp->GetSize().height));
	RenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
}

void FilledTriangle(const int& x1, const int& y1, const int& x2, const int& y2, const int& x3, const int& y3, MyColour colour)
{
	// Define the points of the triangle
	D2D1_POINT_2F p1 = D2D1::Point2F(static_cast<FLOAT>(x1), static_cast<FLOAT>(y1));
	D2D1_POINT_2F p2 = D2D1::Point2F(static_cast<FLOAT>(x2), static_cast<FLOAT>(y2));
	D2D1_POINT_2F p3 = D2D1::Point2F(static_cast<FLOAT>(x3), static_cast<FLOAT>(y3));
	//render triangle in d2d1 using points

	Brush->SetColor(colour.Get());
	Brush->SetOpacity(colour.a);
	ID2D1PathGeometry* pathgeometry = NULL;
	Factory->CreatePathGeometry(&pathgeometry);
	ID2D1GeometrySink* sink = NULL;
	pathgeometry->Open(&sink);
	sink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);
	sink->AddLine(p2);
	sink->AddLine(p3);
	sink->EndFigure(D2D1_FIGURE_END_CLOSED);
	sink->Close();
	RenderTarget->FillGeometry(pathgeometry, Brush);
	pathgeometry->Release();
	sink->Release();
}

void CreateDirectXBitmap(const std::wstring& filename, ID2D1Bitmap** bmp)
{
	IWICImagingFactory* wicFactory = NULL;
	HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&wicFactory);


	IWICBitmapDecoder* wicDecoder = NULL;
	hr = wicFactory->CreateDecoderFromFilename(filename.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &wicDecoder);
	IWICBitmapFrameDecode* WicFrame = NULL;
	hr = wicDecoder->GetFrame(0, &WicFrame);
	IWICFormatConverter* WicConverter = NULL;
	hr = wicFactory->CreateFormatConverter(&WicConverter);
	hr = WicConverter->Initialize(WicFrame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0, WICBitmapPaletteTypeCustom);
	//  ID2D1Bitmap* bmp2;
	RenderTarget->CreateBitmapFromWicBitmap(WicConverter, NULL, bmp);

	wicFactory->Release();
	wicDecoder->Release();
	WicConverter->Release();
	WicFrame->Release();
}