
#include "WebCPP/Controls/ColorPicker/ColorPicker.h"
#include "WebCPP/Controls/TextLabel/TextLabel.h"
#include "WebCPP/Controls/LineEdit/LineEdit.h"
#include "WebCPP/Controls/CheckboxLabel/CheckboxLabel.h"
#include "WebCPP/Core/HtmlDocument.h"
#include <cmath>

namespace web
{
	ColorPicker::ColorPicker() : View("colorpicker-view")
	{
		setupUi();

		element->setTabIndex(0);
		element->addEventListener("click", [=](Event* e) { onClick(e); });
		element->addEventListener("focusin", [=](Event* e) { onFocusIn(e); });

		label->element->setStyle("background-color", "transparent");
		label->setText("No Color");
	}

	void ColorPicker::setChangeHandler(const std::function<void()>& handler)
	{
		changeHandler = handler;
	}

	void ColorPicker::setColor(int newRed, int newGreen, int newBlue, bool newNoColor)
	{
		if (red == newRed && green == newGreen && blue == newBlue && noColor == newNoColor)
			return;

		red = newRed;
		green = newGreen;
		blue = newBlue;
		noColor = newNoColor;
		updateUi();
	}

	void ColorPicker::updateUi()
	{
		if (noColor)
		{
			label->element->setStyle("background-color", "transparent");
			label->setText("No Color");
		}
		else
		{
			label->element->setStyle("background-color", RGBColorf(red / 255.0f, green / 255.0f, blue / 255.0f).toCSSColor());
			label->setText("");
		}
	}

	void ColorPicker::onClick(Event* e)
	{
		if (!popup)
		{
			popup = std::make_shared<ColorPickerPopup>(this);
			popup->setChangeHandler([this]() { onColorPickerChanged(); });
			popup->setColor(red, green, blue, noColor);
			auto layer = popup->showPopupModal(true);
			layer->element->addEventListener("click", [=](Event* event) { event->stopPropagation(); closePopup(); });
		}
		else
		{
			closePopup();
		}
	}

	void ColorPicker::onFocusIn(Event* e)
	{
		addClass("focus");
	}

	void ColorPicker::onColorPickerChanged()
	{
		if (!popup)
			return;

		red = popup->getRed();
		green = popup->getGreen();
		blue = popup->getBlue();
		noColor = popup->isNoColor();
		updateUi();

		if (changeHandler)
			changeHandler();
	}

	void ColorPicker::closePopup()
	{
		if (popup)
		{
			popup->closeModal();
			popup = nullptr;
			setFocus();
		}
	}

	void ColorPicker::setupUi()
	{
		label = std::make_shared<TextLabel>();
		label->addClass("colorpicker-label");

		auto layout = createHBoxLayout();
		layout->addView(label, true, true);
	}

	/////////////////////////////////////////////////////////////////////////////

	ColorPickerPopup::ColorPickerPopup(ColorPicker* combo) : View("colorpickerpopup-view"), combo(combo)
	{
		setupUi();

		satvalBox->setChangeHandler([this]() { onSatValChanged(); });
		hueBox->setChangeHandler([this]() { onHueChanged(); });
		red->setChangeHandler([this](const std::string& text) { onRgbChanged(); });
		green->setChangeHandler([this](const std::string& text) { onRgbChanged(); });
		blue->setChangeHandler([this](const std::string& text) { onRgbChanged(); });
		noColor->setChangeHandler([this](bool checked) { onNoColorChanged(); });

		element->addEventListener("click", [=](Event* event) { event->stopPropagation(); });
		element->addEventListener("keydown", [=](Event* e) { onKeyDown(e); });
		element->setTabIndex(0);

		setDefaultFocused();

		// initial size and position
		const Rect& rect = combo->element->getBoundingClientRect();
		element->setStyle("left", std::to_string(rect.x) + "px");
		element->setStyle("top", std::to_string(rect.y + rect.height - 1) + "px");
		element->setStyle("width", std::to_string(rect.width - 2) + "px");

		observer.onResize = [=](std::vector<ResizeObserverEntry>) {
			const Rect& rect = combo->element->getBoundingClientRect();
			element->setStyle("left", std::to_string(rect.x) + "px");
			element->setStyle("top", std::to_string(rect.y + rect.height - 1) + "px");
			};
		observer.observe(combo->element.get());

		updateRGBFromHSV();
		updateColorCircle();
	}

	void ColorPickerPopup::setChangeHandler(const std::function<void()>& handler)
	{
		changeHandler = handler;
	}

	void ColorPickerPopup::setColor(int newRed, int newGreen, int newBlue, bool newNoColor)
	{
		red->setText(std::to_string(newRed));
		green->setText(std::to_string(newGreen));
		blue->setText(std::to_string(newBlue));
		noColor->setChecked(newNoColor);
		updateHSVFromRGB();
		updateColorCircle();
	}

	int ColorPickerPopup::getRed()
	{
		return std::atoi(red->getText().c_str());
	}

	int ColorPickerPopup::getGreen()
	{
		return std::atoi(green->getText().c_str());
	}

	int ColorPickerPopup::getBlue()
	{
		return std::atoi(blue->getText().c_str());
	}

	bool ColorPickerPopup::isNoColor()
	{
		return noColor->isChecked();
	}

	void ColorPickerPopup::updateRGBFromHSV()
	{
		HSVColorf hsv(hueBox->getHue(), satvalBox->getSaturation(), satvalBox->getValue());
		RGBColorf rgb = hsv.toRGB();
		red->setText(std::to_string((int)std::round(rgb.r * 255.0f)));
		green->setText(std::to_string((int)std::round(rgb.g * 255.0f)));
		blue->setText(std::to_string((int)std::round(rgb.b * 255.0f)));
	}

	void ColorPickerPopup::updateHSVFromRGB()
	{
		RGBColorf rgb(getRed() / 255.0f, getGreen() / 255.0f, getBlue() / 255.0f);
		HSVColorf hsv = rgb.toHSV();
		satvalBox->setHue(hsv.h);
		satvalBox->setSaturation(hsv.s);
		satvalBox->setValue(hsv.v);
		hueBox->setHue(hsv.h);
	}

	void ColorPickerPopup::updateColorCircle()
	{
		RGBColorf rgb(getRed() / 255.0f, getGreen() / 255.0f, getBlue() / 255.0f);
		colorCircle->element->setStyle("background-color", rgb.toCSSColor());
	}

	void ColorPickerPopup::onSatValChanged()
	{
		noColor->setChecked(false);
		updateRGBFromHSV();
		updateColorCircle();
		if (changeHandler)
			changeHandler();
	}

	void ColorPickerPopup::onHueChanged()
	{
		noColor->setChecked(false);
		satvalBox->setHue(hueBox->getHue());
		updateRGBFromHSV();
		updateColorCircle();
		noColor->setChecked(false);
		if (changeHandler)
			changeHandler();
	}

	void ColorPickerPopup::onRgbChanged()
	{
		noColor->setChecked(false);
		updateHSVFromRGB();
		updateColorCircle();
		if (changeHandler)
			changeHandler();
	}

	void ColorPickerPopup::onNoColorChanged()
	{
		if (changeHandler)
			changeHandler();
	}

	void ColorPickerPopup::onKeyDown(Event* event)
	{
		bool processed = true;

		const int keyCode = event->getKeyCode();
		if (keyCode == 27) // Escape
		{
			combo->closePopup();
		}
		else
		{
			processed = false;
		}

		if (processed)
		{
			event->preventDefault();
			event->stopPropagation();
		}
	}

	void ColorPickerPopup::setupUi()
	{
		satvalBox = std::make_shared<ColorPickerSatValBox>();
		hueBox = std::make_shared<ColorPickerHueBox>();

		auto form = std::make_shared<View>();
		form->addClass("colorpickerform");

		colorCircle = std::make_shared<View>();
		colorCircle->addClass("colorpickercircle");

		auto row = std::make_shared<View>();
		auto rowLayout = row->createHBoxLayout();
		rowLayout->setGap(10.0);
		rowLayout->setAlignItems(FlexAlignItems::center);
		rowLayout->addView(colorCircle);
		rowLayout->addView(hueBox, true, true);

		red = std::make_shared<LineEdit>();
		green = std::make_shared<LineEdit>();
		blue = std::make_shared<LineEdit>();

		auto labelred = std::make_shared<TextLabel>();
		labelred->setText("R");
		labelred->setCenterAlign();
		auto labelgreen = std::make_shared<TextLabel>();
		labelgreen->setText("G");
		labelgreen->setCenterAlign();
		auto labelblue = std::make_shared<TextLabel>();
		labelblue->setText("B");
		labelblue->setCenterAlign();

		noColor = std::make_shared<CheckboxLabel>();
		noColor->setText("No Color");

		red->setText(std::to_string(255));
		green->setText(std::to_string(255));
		blue->setText(std::to_string(255));

		red->addClass("colorpickeredit");
		green->addClass("colorpickeredit");
		blue->addClass("colorpickeredit");

		auto row2 = std::make_shared<View>();
		row2->element->setStyle("margin", "0 -5px");
		auto row2Layout = row2->createGridLayout();
		row2Layout->setColumns({ GridLayout::freeSpace, GridLayout::freeSpace, GridLayout::freeSpace });
		row2Layout->setGap(4.0);
		row2Layout->addView(red, 1, 1);
		row2Layout->addView(green, 2, 1);
		row2Layout->addView(blue, 3, 1);
		row2Layout->addView(labelred, 1, 2);
		row2Layout->addView(labelgreen, 2, 2);
		row2Layout->addView(labelblue, 3, 2);

		auto formLayout = form->createVBoxLayout();
		formLayout->setGap(7.0);
		formLayout->addView(row);
		formLayout->addView(row2);
		formLayout->addView(noColor);

		auto layout = createVBoxLayout();
		layout->addView(satvalBox);
		layout->addView(form);
	}

	/////////////////////////////////////////////////////////////////////////////

	ColorPickerSatValBox::ColorPickerSatValBox() : View("colorpickercolors-view")
	{
		saturationGradient = std::make_shared<View>();
		saturationGradient->addClass("colorpickercolors-sat");

		valueGradient = std::make_shared<View>();
		valueGradient->addClass("colorpickercolors-val");

		dragger = std::make_shared<View>();
		dragger->addClass("colorpickercolors-dragger");

		auto layout = createFlowLayout();
		layout->addView(saturationGradient);

		auto satLayout = saturationGradient->createFlowLayout();
		satLayout->addView(valueGradient);

		auto valLayout = valueGradient->createFlowLayout();
		valLayout->addView(dragger);

		element->addEventListener("pointerdown", [=](Event* e) { onPointerDown(e); });
		dragger->element->addEventListener("pointermove", [=](Event* e) { onPointerMove(e); });
		dragger->element->addEventListener("pointerup", [=](Event* e) { onPointerUp(e); });

		updateUi();
	}

	void ColorPickerSatValBox::setChangeHandler(const std::function<void()>& handler)
	{
		changeHandler = handler;
	}

	void ColorPickerSatValBox::setHue(float newHue)
	{
		if (hue != newHue)
		{
			hue = newHue;
			updateUi();
		}
	}

	void ColorPickerSatValBox::setValue(float newValue)
	{
		if (value != newValue)
		{
			value = newValue;
			updateUi();
		}
	}

	void ColorPickerSatValBox::setSaturation(float newSaturation)
	{
		if (saturation != newSaturation)
		{
			saturation = newSaturation;
			updateUi();
		}
	}

	void ColorPickerSatValBox::updateUi()
	{
		element->setStyle("background-color", HSVColorf(hue, 1.0f, 1.0f).toCSSColor());
		dragger->element->setStyle("background-color", HSVColorf(hue, saturation, value).toCSSColor());
		dragger->element->setStyle("left", "calc(" + std::to_string(saturation * 100.0f) + "% - 7px)");
		dragger->element->setStyle("top", "calc(" + std::to_string((1.0f - value) * 100.0f) + "% - 7px)");
	}

	void ColorPickerSatValBox::onPointerDown(Event* e)
	{
		e->stopPropagation();
		e->preventDefault();
		dragger->element->setPointerCapture(e->getPointerId());
		pointerCaptureActive = true;
		updateDraggerPos(e);
	}

	void ColorPickerSatValBox::onPointerMove(Event* e)
	{
		if (pointerCaptureActive)
			updateDraggerPos(e);
	}

	void ColorPickerSatValBox::onPointerUp(Event* e)
	{
		if (pointerCaptureActive)
			updateDraggerPos(e);
		pointerCaptureActive = false;
	}

	void ColorPickerSatValBox::updateDraggerPos(Event* e)
	{
		double x = e->clientX();
		double y = e->clientY();
		Rect box = element->getBoundingClientRect();
		if (box.width > 0.0f)
			saturation = (float)std::max(std::min((x - box.x) / box.width, 1.0), 0.0);
		if (box.height > 0.0f)
			value = 1.0f - (float)std::max(std::min((y - box.y) / box.height, 1.0), 0.0);
		updateUi();
		if (changeHandler)
			changeHandler();
	}

	/////////////////////////////////////////////////////////////////////////////

	ColorPickerHueBox::ColorPickerHueBox() : View("colorpickerhue-view")
	{
		dragger = std::make_shared<View>();
		dragger->addClass("colorpickerhue-dragger");

		auto layout = createFlowLayout();
		layout->addView(dragger);

		element->addEventListener("pointerdown", [=](Event* e) { onPointerDown(e); });
		dragger->element->addEventListener("pointermove", [=](Event* e) { onPointerMove(e); });
		dragger->element->addEventListener("pointerup", [=](Event* e) { onPointerUp(e); });

		updateUi();
	}

	void ColorPickerHueBox::setChangeHandler(const std::function<void()>& handler)
	{
		changeHandler = handler;
	}

	void ColorPickerHueBox::setHue(float newHue)
	{
		if (hue != newHue)
		{
			hue = newHue;
			updateUi();
		}
	}

	void ColorPickerHueBox::updateUi()
	{
		element->setStyle("background-color", HSVColorf(hue, 1.0f, 1.0f).toCSSColor());
		dragger->element->setStyle("background-color", HSVColorf(hue, 1.0f, 1.0f).toCSSColor());
		dragger->element->setStyle("left", "calc(" + std::to_string(hue * 100.0f) + "% - 7px)");
	}

	void ColorPickerHueBox::onPointerDown(Event* e)
	{
		e->stopPropagation();
		e->preventDefault();
		dragger->element->setPointerCapture(e->getPointerId());
		pointerCaptureActive = true;
		updateDraggerPos(e);
	}

	void ColorPickerHueBox::onPointerMove(Event* e)
	{
		if (pointerCaptureActive)
			updateDraggerPos(e);
	}

	void ColorPickerHueBox::onPointerUp(Event* e)
	{
		if (pointerCaptureActive)
			updateDraggerPos(e);
		pointerCaptureActive = false;
	}

	void ColorPickerHueBox::updateDraggerPos(Event* e)
	{
		double x = e->clientX();
		double y = e->clientY();
		Rect box = element->getBoundingClientRect();
		if (box.width > 0.0f)
			hue = (float)std::max(std::min((x - box.x) / box.width, 1.0), 0.0);
		updateUi();
		if (changeHandler)
			changeHandler();
	}

	/////////////////////////////////////////////////////////////////////////////

	RGBColorf HSVColorf::toRGB() const
	{
		float h6 = h * 6.0f;
		int i = (int)std::floor(h6);
		float f = h6 - i;
		float p = v * (1 - s);
		float q = v * (1 - f * s);
		float t = v * (1 - (1 - f) * s);
		int mod = i % 6;
		if (mod < 0) mod = 6 + mod;
		float rvalues[6] = { v, q, p, p, t, v };
		float gvalues[6] = { t, v, v, q, p, p };
		float bvalues[6] = { p, p, t, v, v, q };
		float r = rvalues[mod];
		float g = gvalues[mod];
		float b = bvalues[mod];
		return { r, g, b };
	}

	std::string HSVColorf::toCSSColor() const
	{
		return toRGB().toCSSColor();
	}

	HSVColorf RGBColorf::toHSV() const
	{
		float maxval = std::max(std::max(r, g), b);
		float minval = std::min(std::min(r, g), b);
		float v = maxval;
		float d = maxval - minval;
		float s = maxval == 0.0f ? 0.0f : d / maxval;
		float h = 0.0f;
		if (maxval != minval)
		{
			if (maxval == r)
				h = (g - b) / d + (g < b ? 6.0f : 0.0f);
			else if (maxval == g)
				h = (b - r) / d + 2.0f;
			else // if (maxval == b)
				h = (r - g) / d + 4.0f;
			h /= 6.0f;
		}
		return { h, s, v };
	}

	std::string RGBColorf::toCSSColor() const
	{
		return "rgb(" + std::to_string((int)std::round(r * 255.0f)) + "," + std::to_string((int)std::round(g * 255.0f)) + "," + std::to_string((int)std::round(b * 255.0f)) + ")";
	}
}
