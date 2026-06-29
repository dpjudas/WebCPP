
#include "Main.h"
#include <WebCPP/Core/HtmlDocument.h>
#include <WebCPP/Core/Navigation.h>
#include <WebCPP/Core/View.h>
#include <WebCPP/Core/SendRequest.h>
#include <WebCPP/Controls/TextLabel/TextLabel.h>
#include <WebCPP/Controls/Button/Button.h>
#include <WebCPP/Controls/MessageBox/MessageBox.h>

// This is the view shown for the main page
class HomePage : public web::View
{
public:
	HomePage() : View("homepage-view")
	{
		addClass("fullscreenpage");

		auto label1 = std::make_shared<web::TextLabel>();
		label1->addClass("headline");
		label1->setText("Hello!");

		auto label2 = std::make_shared<web::TextLabel>();
		label2->setText("This is the home page.");

		auto button = std::make_shared<web::Button>();
		button->setText("Go to about page");
		button->pressed = std::bind_front(&HomePage::onButtonPressed, this);

		auto button2 = std::make_shared<web::Button>();
		button2->setText("Show message");
		button2->pressed = std::bind_front(&HomePage::onShowMessageBox, this);

		auto buttonBar = std::make_shared<web::View>();
		auto barLayout = buttonBar->createHBoxLayout();
		barLayout->setGap(5);
		barLayout->addView(button);
		barLayout->addView(button2);

		auto layout = createVBoxLayout();
		layout->setGap(5);
		layout->addView(label1);
		layout->addView(label2);
		layout->addView(buttonBar);
		layout->addView(pongLabel);

		// Start an async web request
		testApi();
	}

	void onButtonPressed()
	{
		web::Navigation::navigateTo("/about");
	}

	web::task<> onShowMessageBox()
	{
		bool result = co_await web::MessageBox::question("Question", "Are you tired of hearing about AI?", "YES", "OH YES");
		if (result)
		{
			web::MessageBox::show("Information", "You are right!");
		}
		else
		{
			web::MessageBox::show("Information", "You are SO RIGHT!");
		}
	}

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

	std::shared_ptr<web::TextLabel> pongLabel = std::make_shared<web::TextLabel>();
};

// This is the view shown for the /about page
class AboutPage : public web::View
{
public:
	AboutPage() : View("aboutpage-view")
	{
		addClass("fullscreenpage");

		auto label = std::make_shared<web::TextLabel>();
		label->setText("This is the about page.");

		auto button = std::make_shared<web::Button>();
		button->setText("Go to home page");
		button->pressed = []() { web::Navigation::navigateTo("/"); };

		auto buttonBar = std::make_shared<web::View>();
		auto barLayout = buttonBar->createHBoxLayout();
		barLayout->addView(button);

		auto layout = createVBoxLayout();
		layout->setGap(5);
		layout->addView(label);
		layout->addView(buttonBar);
	}
};

// This is the view shown for urls that do not match anything in NavigationRouter
class NotFoundPage : public web::View
{
public:
	NotFoundPage() : View("notfound-view")
	{
		addClass("fullscreenpage");

		auto label = std::make_shared<web::TextLabel>();
		label->setText("This is the not found page.");

		auto layout = createVBoxLayout();
		layout->addView(label);
	}
};

// The main function runs when the website is loaded.
// Note that the C++ runtime does not shut down when the function exits.
// Browser events and javascript can still interact with the program.
int main()
{
	// Set a data attribute on the body element
	web::HtmlDocument::body()->element->setAttribute("data-theme", "light");

	// Setup a router that handles page navigation
	auto router = std::make_unique<web::NavigationRouter>();
	router->setNotFoundPage<NotFoundPage>();
	router->addRoute<HomePage>({});
	router->addRoute<AboutPage>({ "about" });

	// Create the page view that the user initially navigated to
	web::Navigation::init(std::move(router));
	return 0;
}
