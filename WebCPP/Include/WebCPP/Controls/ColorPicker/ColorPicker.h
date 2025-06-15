#pragma once

#include "../../Core/ResizeObserver.h"
#include "../../Core/View.h"

namespace web
{
	class TextLabel;
	class ColorPickerPopup;
	class ColorPickerSatValBox;
	class ColorPickerHueBox;
	class LineEdit;
	class LineEdit;
	class CheckboxLabel;
	struct RGBColorf;
	struct HSVColorf;

	class ColorPicker : public View
	{
	public:
		ColorPicker();

		void setChangeHandler(const std::function<void()>& handler);
		void setColor(int red, int green, int blue, bool noColor);

		int getRed() const { return red; }
		int getGreen() const { return green; }
		int getBlue() const { return blue; }
		bool isNoColor() const { return noColor; }

		void closePopup();

	private:
		void setupUi();
		void updateUi();
		void onClick(Event* e);
		void onFocusIn(Event* e);
		void onColorPickerChanged();

		std::function<void()> changeHandler;
		TextLabel* label = nullptr;

		bool noColor = true;
		int red = 0;
		int green = 0;
		int blue = 0;

		ColorPickerPopup* popup = nullptr;
	};

	class ColorPickerPopup : public View
	{
	public:
		ColorPickerPopup(ColorPicker* combo);

		void setChangeHandler(const std::function<void()>& handler);
		void setColor(int red, int green, int blue, bool noColor);

		int getRed();
		int getGreen();
		int getBlue();
		bool isNoColor();

	private:
		void setupUi();
		void updateRGBFromHSV();
		void updateHSVFromRGB();
		void updateColorCircle();
		void onSatValChanged();
		void onHueChanged();
		void onRgbChanged();
		void onNoColorChanged();
		void onKeyDown(Event* event);

		ColorPicker* combo = nullptr;
		ResizeObserver observer;

		std::function<void()> changeHandler;

		ColorPickerSatValBox* satvalBox = nullptr;
		ColorPickerHueBox* hueBox = nullptr;
		View* colorCircle = nullptr;
		LineEdit* red = nullptr;
		LineEdit* green = nullptr;
		LineEdit* blue = nullptr;
		CheckboxLabel* noColor = nullptr;
	};

	class ColorPickerSatValBox : public View
	{
	public:
		ColorPickerSatValBox();

		void setHue(float hue);
		void setValue(float value);
		void setSaturation(float saturation);

		float getHue() const { return hue; }
		float getValue() const { return value; }
		float getSaturation() const { return saturation; }

		void setChangeHandler(const std::function<void()>& handler);

	private:
		void updateUi();
		void updateDraggerPos(Event* e);
		void onPointerDown(Event* e);
		void onPointerMove(Event* e);
		void onPointerUp(Event* e);

		std::function<void()> changeHandler;
		float hue = 0.5f;
		float value = 0.5f;
		float saturation = 0.5f;
		bool pointerCaptureActive = false;

		View* saturationGradient = nullptr;
		View* valueGradient = nullptr;
		View* dragger = nullptr;
	};

	class ColorPickerHueBox : public View
	{
	public:
		ColorPickerHueBox();

		void setHue(float hue);
		float getHue() const { return hue; }

		void setChangeHandler(const std::function<void()>& handler);

	private:
		void updateUi();
		void updateDraggerPos(Event* e);
		void onPointerDown(Event* e);
		void onPointerMove(Event* e);
		void onPointerUp(Event* e);

		std::function<void()> changeHandler;
		float hue = 0.5f;
		bool pointerCaptureActive = false;

		View* dragger = nullptr;
	};

	struct HSVColorf
	{
		HSVColorf() = default;
		HSVColorf(float h, float s, float v) : h(h), s(s), v(v) {}

		float h = 0.0f;
		float s = 0.0f;
		float v = 0.0f;

		RGBColorf toRGB() const;
		std::string toCSSColor() const;
	};

	struct RGBColorf
	{
		RGBColorf() = default;
		RGBColorf(float r, float g, float b) : r(r), g(g), b(b) {}

		float r = 0.0f;
		float g = 0.0f;
		float b = 0.0f;

		HSVColorf toHSV() const;
		std::string toCSSColor() const;
	};
}
