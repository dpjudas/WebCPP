#pragma once

#include <string>
#include <functional>
#include "../../Core/Task.h"

namespace web
{
	enum class MessageBoxResult
	{
		okClicked,
		cancelClicked
	};

	class MessageBox
	{
	public:
		static task<> show(const std::string& title, const std::string& text, const std::string& okLabel = "OK");
		static task<MessageBoxResult> question(const std::string& title, const std::string& text, const std::string& okLabel = "OK", const std::string& cancelLabel = "Cancel");

		static void show(const std::string& title, const std::string& text, std::function<void()> okClicked, const std::string& okLabel = "OK");
		static void question(const std::string& title, const std::string& text, std::function<void()> okClicked, std::function<void()> cancelClicked = {}, const std::string& okLabel = "OK", const std::string& cancelLabel = "Cancel");
	};
}
