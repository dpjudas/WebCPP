#pragma once

#include "../../Core/View.h"

namespace web
{
	class SvgElementView : public View
	{
	public:
		SvgElementView(std::string elementType) : View(xmlns, std::move(elementType)) {}

		void clear();
		void addElement(std::string tag, std::map<std::string, std::string> attributes);
		void addElement(std::string tag, std::string text, std::map<std::string, std::string> attributes);

	private:
		JSValue createElement(std::string tag, std::map<std::string, std::string> attributes);
		JSValue createElement(std::string tag, std::string text, std::map<std::string, std::string> attributes);

		static const std::string xmlns;
	};

	class SvgBox : public SvgElementView
	{
	public:
		SvgBox();

		void setViewbox(double x, double y, double width, double height);
		void addView(SvgElementView* view);
	};

	enum class SvgFillRule
	{
		nonzero,
		evenodd
	};

	class SvgAnchor : public SvgElementView
	{
	public:
		SvgAnchor() : SvgElementView("a") {}

		void setDownload(const std::string& url) { element->setAttribute("download", url); }
		void setHref(const std::string& url) { element->setAttribute("href", url); }
		void setTarget(const std::string& target) { element->setAttribute("target", target); }
		void setReferrerPolicy(const std::string& policy) { element->setAttribute("referrerpolicy", policy); }
	};

	class SvgAnimate : public SvgElementView
	{
	public:
		SvgAnimate() : SvgElementView("animate") {}
	};

	class SvgAnimateMotion : public SvgElementView
	{
	public:
		SvgAnimateMotion() : SvgElementView("animateMotion") {}
	};

	class SvgAnimateTransform : public SvgElementView
	{
	public:
		SvgAnimateTransform() : SvgElementView("animateTransform") {}
	};

	class SvgCircle : public SvgElementView
	{
	public:
		SvgCircle() : SvgElementView("circle") {}

		void setX(const std::string& x) { element->setAttribute("cx", x); }
		void setX(double x) { setX(std::to_string(x)); }
		void setY(const std::string& y) { element->setAttribute("cy", y); }
		void setY(double y) { setY(std::to_string(y)); }
		void setRadius(const std::string& radius) { element->setAttribute("r", radius); }
		void setRadius(double radius) { setRadius(std::to_string(radius)); }
		void setPathLength(double length) { element->setAttribute("pathLength", std::to_string(length)); }

		void setStroke(const std::string& value) { element->setAttribute("stroke", value); }
		void setStrokeDashArray(const std::string& value) { element->setAttribute("stroke-dasharray", value); }
		void setStrokeLineCap(const std::string& value) { element->setAttribute("stroke-linecap", value); }
		void setStrokeLineJoin(const std::string& value) { element->setAttribute("stroke-linejoin", value); }
		void setStrokeMiterLimit(const std::string& value) { element->setAttribute("stroke-miterlimit", value); }
		void setStrokeWidth(const std::string& value) { element->setAttribute("stroke-width", value); }
		void setStrokeWidth(double value) { setStrokeWidth(std::to_string(value)); }

		void setFill(const std::string& value) { element->setAttribute("fill", value); }
	};

	class SvgClipPath : public SvgElementView
	{
	public:
		SvgClipPath() : SvgElementView("clipPath") {}
	};

	class SvgDefs : public SvgElementView
	{
	public:
		SvgDefs() : SvgElementView("defs") {}
	};

	class SvgDesc : public SvgElementView
	{
	public:
		SvgDesc() : SvgElementView("desc") {}
	};

	class SvgEllipse : public SvgElementView
	{
	public:
		SvgEllipse() : SvgElementView("ellipse") {}

		void setX(const std::string& x) { element->setAttribute("cx", x); }
		void setX(double x) { setX(std::to_string(x)); }
		void setY(const std::string& y) { element->setAttribute("cy", y); }
		void setY(double y) { setY(std::to_string(y)); }
		void setRadiusX(const std::string& radius) { element->setAttribute("rx", radius); }
		void setRadiusX(double radius) { setRadiusX(std::to_string(radius)); }
		void setRadiusY(const std::string& radius) { element->setAttribute("ry", radius); }
		void setRadiusY(double radius) { setRadiusY(std::to_string(radius)); }
		void setPathLength(double length) { element->setAttribute("pathLength", std::to_string(length)); }

		void setStroke(const std::string& value) { element->setAttribute("stroke", value); }
		void setStrokeDashArray(const std::string& value) { element->setAttribute("stroke-dasharray", value); }
		void setStrokeLineCap(const std::string& value) { element->setAttribute("stroke-linecap", value); }
		void setStrokeLineJoin(const std::string& value) { element->setAttribute("stroke-linejoin", value); }
		void setStrokeMiterLimit(const std::string& value) { element->setAttribute("stroke-miterlimit", value); }
		void setStrokeWidth(const std::string& value) { element->setAttribute("stroke-width", value); }
		void setStrokeWidth(double value) { setStrokeWidth(std::to_string(value)); }

		void setFill(const std::string& value) { element->setAttribute("fill", value); }
	};

	class SvgForeignObject : public SvgElementView
	{
	public:
		SvgForeignObject() : SvgElementView("foreignObject") {}
	};

	class SvgGroup : public SvgElementView
	{
	public:
		SvgGroup() : SvgElementView("g") {}
	};

	class SvgImage : public SvgElementView
	{
	public:
		SvgImage() : SvgElementView("image") {}

		void setX(const std::string& x) { element->setAttribute("x", x); }
		void setX(double x) { setX(std::to_string(x)); }
		void setY(const std::string& y) { element->setAttribute("y", y); }
		void setY(double y) { setY(std::to_string(y)); }

		void setWidth(const std::string& width) { element->setAttribute("width", width); }
		void setWidth(double width) { setWidth(std::to_string(width)); }
		void setHeight(const std::string& height) { element->setAttribute("height", height); }
		void setHeight(double height) { setHeight(std::to_string(height)); }

		void setHref(const std::string& url) { element->setAttribute("href", url); }
		void setPreserveAspectRatio(const std::string& value) { element->setAttribute("preserveAspectRatio", value); }
		void setCrossOrigin(const std::string& value) { element->setAttribute("crossorigin", value); }
		void setDecoding(const std::string& value) { element->setAttribute("decoding", value); }
	};

	class SvgLine : public SvgElementView
	{
	public:
		SvgLine() : SvgElementView("line") {}

		void setX1(const std::string& x) { element->setAttribute("x1", x); }
		void setX1(double x) { setX1(std::to_string(x)); }
		void setX2(const std::string& x) { element->setAttribute("x2", x); }
		void setX2(double x) { setX2(std::to_string(x)); }
		void setY1(const std::string& y) { element->setAttribute("y1", y); }
		void setY1(double y) { setY1(std::to_string(y)); }
		void setY2(const std::string& y) { element->setAttribute("y2", y); }
		void setY2(double y) { setY2(std::to_string(y)); }
		void setPathLength(double length) { element->setAttribute("pathLength", std::to_string(length)); }

		void setStroke(const std::string& value) { element->setAttribute("stroke", value); }
		void setStrokeDashArray(const std::string& value) { element->setAttribute("stroke-dasharray", value); }
		void setStrokeLineCap(const std::string& value) { element->setAttribute("stroke-linecap", value); }
		void setStrokeLineJoin(const std::string& value) { element->setAttribute("stroke-linejoin", value); }
		void setStrokeMiterLimit(const std::string& value) { element->setAttribute("stroke-miterlimit", value); }
		void setStrokeWidth(const std::string& value) { element->setAttribute("stroke-width", value); }
		void setStrokeWidth(double value) { setStrokeWidth(std::to_string(value)); }
	};

	class SvgLinearGradient : public SvgElementView
	{
	public:
		SvgLinearGradient() : SvgElementView("linearGradient") {}
	};

	class SvgMarker : public SvgElementView
	{
	public:
		SvgMarker() : SvgElementView("marker") {}
	};

	class SvgMask : public SvgElementView
	{
	public:
		SvgMask() : SvgElementView("mask") {}
	};

	class SvgMotionPath : public SvgElementView
	{
	public:
		SvgMotionPath() : SvgElementView("mpath") {}
	};

	class SvgPath : public SvgElementView
	{
	public:
		SvgPath() : SvgElementView("path") {}

		void setData(const std::string& d) { element->setAttribute("d", d); }
		void setPathLength(double length) { element->setAttribute("pathLength", std::to_string(length)); }

		void setStroke(const std::string& value) { element->setAttribute("stroke", value); }
		void setStrokeDashArray(const std::string& value) { element->setAttribute("stroke-dasharray", value); }
		void setStrokeLineCap(const std::string& value) { element->setAttribute("stroke-linecap", value); }
		void setStrokeLineJoin(const std::string& value) { element->setAttribute("stroke-linejoin", value); }
		void setStrokeMiterLimit(const std::string& value) { element->setAttribute("stroke-miterlimit", value); }
		void setStrokeWidth(const std::string& value) { element->setAttribute("stroke-width", value); }
		void setStrokeWidth(double value) { setStrokeWidth(std::to_string(value)); }

		void setFill(const std::string& value) { element->setAttribute("fill", value); }
		void setFillRule(SvgFillRule rule) { element->setAttribute("fill-rule", rule == SvgFillRule::evenodd ? "evenodd" : "nonzero"); }
	};

	class SvgPattern : public SvgElementView
	{
	public:
		SvgPattern() : SvgElementView("pattern") {}
	};

	class SvgPolygon : public SvgElementView
	{
	public:
		SvgPolygon() : SvgElementView("polygon") {}

		void setPoints(const std::string& points) { element->setAttribute("points", points); }
		void setPathLength(double length) { element->setAttribute("pathLength", std::to_string(length)); }

		void setStroke(const std::string& value) { element->setAttribute("stroke", value); }
		void setStrokeDashArray(const std::string& value) { element->setAttribute("stroke-dasharray", value); }
		void setStrokeLineCap(const std::string& value) { element->setAttribute("stroke-linecap", value); }
		void setStrokeLineJoin(const std::string& value) { element->setAttribute("stroke-linejoin", value); }
		void setStrokeMiterLimit(const std::string& value) { element->setAttribute("stroke-miterlimit", value); }
		void setStrokeWidth(const std::string& value) { element->setAttribute("stroke-width", value); }
		void setStrokeWidth(double value) { setStrokeWidth(std::to_string(value)); }

		void setFill(const std::string& value) { element->setAttribute("fill", value); }
		void setFillRule(SvgFillRule rule) { element->setAttribute("fill-rule", rule == SvgFillRule::evenodd ? "evenodd" : "nonzero"); }
	};

	class SvgPolyline : public SvgElementView
	{
	public:
		SvgPolyline() : SvgElementView("polyline") {}

		void setPoints(const std::string& points) { element->setAttribute("points", points); }
		void setPathLength(double length) { element->setAttribute("pathLength", std::to_string(length)); }

		void setStroke(const std::string& value) { element->setAttribute("stroke", value); }
		void setStrokeDashArray(const std::string& value) { element->setAttribute("stroke-dasharray", value); }
		void setStrokeLineCap(const std::string& value) { element->setAttribute("stroke-linecap", value); }
		void setStrokeLineJoin(const std::string& value) { element->setAttribute("stroke-linejoin", value); }
		void setStrokeMiterLimit(const std::string& value) { element->setAttribute("stroke-miterlimit", value); }
		void setStrokeWidth(const std::string& value) { element->setAttribute("stroke-width", value); }
		void setStrokeWidth(double value) { setStrokeWidth(std::to_string(value)); }

		void setFill(const std::string& value) { element->setAttribute("fill", value); }
		void setFillRule(SvgFillRule rule) { element->setAttribute("fill-rule", rule == SvgFillRule::evenodd ? "evenodd" : "nonzero"); }
	};

	class SvgRadialGradient : public SvgElementView
	{
	public:
		SvgRadialGradient() : SvgElementView("radialGradient") {}
	};

	class SvgRect : public SvgElementView
	{
	public:
		SvgRect() : SvgElementView("rect") {}

		void setX(const std::string& x) { element->setAttribute("x", x); }
		void setX(double x) { setX(std::to_string(x)); }
		void setY(const std::string& y) { element->setAttribute("y", y); }
		void setY(double y) { setY(std::to_string(y)); }

		void setWidth(const std::string& width) { element->setAttribute("width", width); }
		void setWidth(double width) { setWidth(std::to_string(width)); }
		void setHeight(const std::string& height) { element->setAttribute("height", height); }
		void setHeight(double height) { setHeight(std::to_string(height)); }

		void setCornerRadiusX(const std::string& x) { element->setAttribute("rx", x); }
		void setCornerRadiusX(double x) { setCornerRadiusX(std::to_string(x)); }
		void setCornerRadiusY(const std::string& y) { element->setAttribute("ry", y); }
		void setCornerRadiusY(double y) { setCornerRadiusY(std::to_string(y)); }

		void setPathLength(double length) { element->setAttribute("pathLength", std::to_string(length)); }

		void setStroke(const std::string& value) { element->setAttribute("stroke", value); }
		void setStrokeDashArray(const std::string& value) { element->setAttribute("stroke-dasharray", value); }
		void setStrokeLineCap(const std::string& value) { element->setAttribute("stroke-linecap", value); }
		void setStrokeLineJoin(const std::string& value) { element->setAttribute("stroke-linejoin", value); }
		void setStrokeMiterLimit(const std::string& value) { element->setAttribute("stroke-miterlimit", value); }
		void setStrokeWidth(const std::string& value) { element->setAttribute("stroke-width", value); }
		void setStrokeWidth(double value) { setStrokeWidth(std::to_string(value)); }

		void setFill(const std::string& value) { element->setAttribute("fill", value); }
	};

	class SvgSet : public SvgElementView
	{
	public:
		SvgSet() : SvgElementView("set") {}
	};

	class SvgStop : public SvgElementView
	{
	public:
		SvgStop() : SvgElementView("stop") {}
	};

	class SvgSymbol : public SvgElementView
	{
	public:
		SvgSymbol() : SvgElementView("symbol") {}
	};

	enum class SvgLengthAdjust
	{
		spacing,
		spacingAndGlyphs
	};

	enum class SvgTextAlignment
	{
		start,
		middle,
		end
	};

	class SvgText : public SvgElementView
	{
	public:
		SvgText() : SvgElementView("text") {}

		void setX(const std::string& x) { element->setAttribute("x", x); }
		void setX(double x) { setX(std::to_string(x)); }
		void setY(const std::string& y) { element->setAttribute("y", y); }
		void setY(double y) { setY(std::to_string(y)); }

		void setDeltaX(const std::string& dx) { element->setAttribute("dx", dx); }
		void setDeltaX(double dx) { setDeltaX(std::to_string(dx)); }
		void setDeltaY(const std::string& dy) { element->setAttribute("dy", dy); }
		void setDeltaY(double dy) { setDeltaY(std::to_string(dy)); }

		void setRotate(const std::string& glyphRotateList) { element->setAttribute("rotate", glyphRotateList); }

		void setLengthAdjust(SvgLengthAdjust v) { element->setAttribute("textLength", v == SvgLengthAdjust::spacing ? "spacing" : "spacingAndGlyphs"); }
		void setTextLength(double length) { element->setAttribute("textLength", std::to_string(length)); }

		void setText(const std::string& text) { clear(); element->handle.call<void>("appendChild", JSValue::global("document").call<JSValue>("createTextNode", text)); }

		void setTextAnchor(SvgTextAlignment value) { element->setAttribute("text-anchor", value == SvgTextAlignment::start ? "start" : (value == SvgTextAlignment::middle) ? "middle" : "end"); }
		void setAlignmentBaseline(SvgTextAlignment value) { element->setAttribute("alignment-baseline", value == SvgTextAlignment::start ? "start" : (value == SvgTextAlignment::middle) ? "middle" : "end"); }

		void setStroke(const std::string& value) { element->setAttribute("stroke", value); }
		void setStrokeDashArray(const std::string& value) { element->setAttribute("stroke-dasharray", value); }
		void setStrokeLineCap(const std::string& value) { element->setAttribute("stroke-linecap", value); }
		void setStrokeLineJoin(const std::string& value) { element->setAttribute("stroke-linejoin", value); }
		void setStrokeMiterLimit(const std::string& value) { element->setAttribute("stroke-miterlimit", value); }
		void setStrokeWidth(const std::string& value) { element->setAttribute("stroke-width", value); }
		void setStrokeWidth(double value) { setStrokeWidth(std::to_string(value)); }

		void setFill(const std::string& value) { element->setAttribute("fill", value); }
		void setFillRule(SvgFillRule rule) { element->setAttribute("fill-rule", rule == SvgFillRule::evenodd ? "evenodd" : "nonzero"); }
	};

	class SvgTextPath : public SvgElementView
	{
	public:
		SvgTextPath() : SvgElementView("textPath") {}

		void setStroke(const std::string& value) { element->setAttribute("stroke", value); }
		void setStrokeDashArray(const std::string& value) { element->setAttribute("stroke-dasharray", value); }
		void setStrokeLineCap(const std::string& value) { element->setAttribute("stroke-linecap", value); }
		void setStrokeLineJoin(const std::string& value) { element->setAttribute("stroke-linejoin", value); }
		void setStrokeMiterLimit(const std::string& value) { element->setAttribute("stroke-miterlimit", value); }
		void setStrokeWidth(const std::string& value) { element->setAttribute("stroke-width", value); }
		void setStrokeWidth(double value) { setStrokeWidth(std::to_string(value)); }

		void setFill(const std::string& value) { element->setAttribute("fill", value); }
		void setFillRule(SvgFillRule rule) { element->setAttribute("fill-rule", rule == SvgFillRule::evenodd ? "evenodd" : "nonzero"); }
	};

	class SvgTitle : public SvgElementView
	{
	public:
		SvgTitle() : SvgElementView("title") {}

		void setText(const std::string& text) { clear(); element->handle.call<void>("appendChild", JSValue::global("document").call<JSValue>("createTextNode", text)); }
	};

	class SvgTextSpan : public SvgElementView
	{
	public:
		SvgTextSpan() : SvgElementView("tspan") {}

		void setStroke(const std::string& value) { element->setAttribute("stroke", value); }
		void setStrokeDashArray(const std::string& value) { element->setAttribute("stroke-dasharray", value); }
		void setStrokeLineCap(const std::string& value) { element->setAttribute("stroke-linecap", value); }
		void setStrokeLineJoin(const std::string& value) { element->setAttribute("stroke-linejoin", value); }
		void setStrokeMiterLimit(const std::string& value) { element->setAttribute("stroke-miterlimit", value); }
		void setStrokeWidth(const std::string& value) { element->setAttribute("stroke-width", value); }
		void setStrokeWidth(double value) { setStrokeWidth(std::to_string(value)); }

		void setFill(const std::string& value) { element->setAttribute("fill", value); }
		void setFillRule(SvgFillRule rule) { element->setAttribute("fill-rule", rule == SvgFillRule::evenodd ? "evenodd" : "nonzero"); }
	};

	class SvgUse : public SvgElementView
	{
	public:
		SvgUse() : SvgElementView("use") {}
	};
}
