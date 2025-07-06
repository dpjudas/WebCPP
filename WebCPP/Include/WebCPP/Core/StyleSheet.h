#pragma once

#include <string>

namespace web
{
	class StyleSheet
	{
	public:
		static int insertRule(const std::string& rule);
		static void deleteRule(int index);
	};
}
