# WebCPP
WebCPP is a C++ view framework for WebAssembly.

The framework extends Emscripten with higher level classes for working and integrating with the browser DOM.
In WebCPP, you create view classes and those are represented automatically in the HTML DOM as elements. This
lets you effectively work with a browser the same way you'd do with traditional C++ view frameworks such as Qt,
WxWidgets and so on, except everything is native HTML DOM nodes and not faked via canvas elements.

Here's a small example of how to create a single page application (SPA) with WebCPP:

```c++
class MyView : public web::View
{
public:
	MyView() : web::View("myview-view")
	{
		auto label = std::make_shared<web::TextLabel>();
		label->setText("Hello World");

		auto layout = createFlowLayout();
		layout->addView(label);
	}
};

int main()
{
	auto router = std::make_unique<web::NavigationRouter>();
	router->addRoute<MyView>({});
	web::Navigation::init(std::move(router));
	return 0;
}
```

This will generate the following HTML:

```html
<!DOCTYPE html>
<html>
<body>
	<myview-view>
		<textlabel-view>Hello World</textlabel-view>
	</myview-view>
</body>
</html>
```

Please see the ExampleWebsite and ExampleWebserver folders for a more complete example.

## Prerequisites

In order to build WebCPP you need to have to things:

1) Install Emscripten (https://emscripten.org).

WebCPP expects `emcc` to be runnable from a command prompt.

2) Install CPPBuild (https://github.com/dpjudas/CPPBuild)

This is the build system used by WebCPP. It is similar to CMake, except it includes web specific target types
needed by WebCPP. In particular, it allows libraries to bundle CSS style sheets and Visual Studio to compile
both native C++ projects and web projects in the same solution.

## The View class

At the heart of WebCPP is the view class. It is used to build up a view tree of components on the C++ side.
Each view is associated with a single DOM element accessible via its `element` member variable.

The element type is specified in the constructor. It defaults to `basic-view`, but can represent any element
in HTML. By convention, views in WebCPP uses the `-view` postfix.

## The view layout

Each view has one layout associated with it. It is created by calling `createFlowLayout`, `createVBoxLayout`,
`createHBoxLayout` or `createGridLayout`.

The layout controls the layout strategy for the view. In HTML, the three major layout models are known as
flow, flex and grid:

- Flow layout is the original way HTML placed elements on a page. It stacks div elements on top of each
other, with span elements horizontally within the div elements.

- Flex layout stacks elements, either vertically or horizontally. Each element in the layout can be set to
stretch or shrink. This type of layout is closest to what traditional view frameworks uses.

- Grid layout defines a grid and places elements in the grid. It is useful for situations where a flex
layout is a poor fit for describing the relationship between elements.

Once you have created a layout you add views to it by calling `addView`. Each layout type has its
own set of functions designed to accommodate the needs of that particular model.

The following adds two child views with a flex layout:

```c++
class EditLineView : public web::View
{
public:
	EditLineView() : web::View("editline-view")
	{
		auto label = std::make_shared<web::TextLabel>();
		auto edit = std::make_shared<web::LineEdit>();

		label->setText("Name:");
		edit->setPlaceholder("Your name");

		auto layout = createHBoxLayout();
		layout->setGap(5);
		layout->addView(label);
		layout->addView(edit);
	}
};
```

## The Element class

This is the class for an actual DOM element on the page. Usually accessed via the `element` member on
`View`, but can also be created on its own. It provides services such as adding CSS classes or events
to a DOM element.

## JSValue

This is a typedef for `emscripten::val`, the lowest level type for interacting with Javascript and the
browser.

## Page navigation

The page can be accessed via `HtmlDocument`. `HtmlDocument::body()` is the root view for the entire page.

The `Navigation` and `NavigationRouter` classes provide a page routing system. They decompose URLs into
path parts and automatically creates or destroys views attached to the body element.

## Style sheets

CPPBuild includes a CSS compiler that automatically merges CSS files with @import directives and CSS
files from static libraries into a final CSS file for the website. The entire SPA package is built into
a .webpkg file, which is basically just a renamed .zip file.

## Async tasks

Emscripten has native C++20 coroutines support for Javascript promise objects via `JSValue`/`emscripten::val`.

WebCPP extends this with a `Task<ReturnType>` template and a `SendRequest` function that sends requests
back to the web server. This follows the Javascript promise model by executing all UI events on the UI
thread, so there's no need for mutexes.

```c++
// Send json to server. Wait for reply and update a text label:
web::task<> testApi()
{
	try
	{
		web::JsonValue request = web::JsonValue::object();
		request["ping"] = web::JsonValue::string("Ping/pong API test worked.");
	
		web::JsonValue response = co_await web::sendRequest("/api/test", request);
		pongLabel->setText(response["pong"].to_string());
	}
	catch (const std::exception& e)
	{
		pongLabel->setText(std::string("Ping/pong APIT test failed: ") + e.what());
	}
}
```
