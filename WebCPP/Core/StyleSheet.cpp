
#include "WebCPP/Core/StyleSheet.h"
#include "WebCPP/Core/Element.h"
#include <memory>

namespace web
{
	class StyleSheetImpl
	{
	public:
		StyleSheetImpl()
		{
			auto styleElement = std::make_unique<Element>("style");

			auto head = std::make_unique<Element>(JSValue::global("document")["head"]);
			head->appendChild(styleElement.get());

			stylesheet = styleElement->handle["sheet"];
		}

		int insertRule(const std::string& rule)
		{
			return stylesheet.call<int>("insertRule", rule);
		}

		void deleteRule(int index)
		{
			stylesheet.call<void>("deleteRule", index);
		}

		static StyleSheetImpl* get()
		{
			static StyleSheetImpl sheet;
			return &sheet;
		}

	private:
		JSValue stylesheet;
	};

	int StyleSheet::insertRule(const std::string& rule)
	{
		return StyleSheetImpl::get()->insertRule(rule);
	}

	void StyleSheet::deleteRule(int index)
	{
		StyleSheetImpl::get()->deleteRule(index);
	}
}
