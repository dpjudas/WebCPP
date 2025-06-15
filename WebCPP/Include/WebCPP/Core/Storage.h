#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include "JsonValue.h"

namespace web
{
	class LocalStorage
	{
	public:
		static JsonValue getJson(const std::string& key, const JsonValue& defaultValue = JsonValue::undefined());
		static double getNumber(const std::string& key, double defaultValue = 0);
		static std::string getString(const std::string& key, const std::string& defaultValue = {}) { return getItem(key, defaultValue); }
		static int getInt(const std::string& key, int defaultValue = 0) { return (int)getNumber(key, (double)defaultValue); }
		static unsigned int getUInt(const std::string& key, unsigned int defaultValue = 0) { return (unsigned int)getNumber(key, (double)defaultValue); }
		static int64_t getInt64(const std::string& key, int64_t defaultValue = 0) { return (int64_t)getNumber(key, (double)defaultValue); }
		static uint64_t getUInt64(const std::string& key, uint64_t defaultValue = 0) { return (uint64_t)getNumber(key, (double)defaultValue); }
		static float getFloat(const std::string& key, float defaultValue = 0) { return (float)getNumber(key, (double)defaultValue); }
		static double getDouble(const std::string& key, double defaultValue = 0) { return (double)getNumber(key, (double)defaultValue); }

		static void setJson(const std::string& key, const JsonValue& value);
		static void setNumber(const std::string& key, double value);
		static void setString(const std::string& key, const std::string& value) { setItem(key, value); }
		static void setInt(const std::string& key, int value) { setNumber(key, (double)value); }
		static void setUInt(const std::string& key, unsigned int value) { setNumber(key, (double)value); }
		static void setInt64(const std::string& key, int64_t value) { setNumber(key, (double)value); }
		static void setUInt64(const std::string& key, uint64_t value) { setNumber(key, (double)value); }

		static std::string getItem(const std::string& key, const std::string& defaultValue = {});
		static void setItem(const std::string& key, const std::string& value);

		static bool containsItem(const std::string& key);
		static void removeItem(const std::string& key);

		static void clear();
		static std::vector<std::string> keys();
	};

	class SessionStorage
	{
	public:
		static JsonValue getJson(const std::string& key, const JsonValue& defaultValue = JsonValue::undefined());
		static double getNumber(const std::string& key, double defaultValue = 0);
		static std::string getString(const std::string& key, const std::string& defaultValue = {}) { return getItem(key, defaultValue); }
		static int getInt(const std::string& key, int defaultValue = 0) { return (int)getNumber(key, (double)defaultValue); }
		static unsigned int getUInt(const std::string& key, unsigned int defaultValue = 0) { return (unsigned int)getNumber(key, (double)defaultValue); }
		static int64_t getInt64(const std::string& key, int64_t defaultValue = 0) { return (int64_t)getNumber(key, (double)defaultValue); }
		static uint64_t getUInt64(const std::string& key, uint64_t defaultValue = 0) { return (uint64_t)getNumber(key, (double)defaultValue); }
		static float getFloat(const std::string& key, float defaultValue = 0) { return (float)getNumber(key, (double)defaultValue); }
		static double getDouble(const std::string& key, double defaultValue = 0) { return (double)getNumber(key, (double)defaultValue); }

		static void setJson(const std::string& key, const JsonValue& value);
		static void setNumber(const std::string& key, double value);
		static void setString(const std::string& key, const std::string& value) { setItem(key, value); }
		static void setInt(const std::string& key, int value) { setNumber(key, (double)value); }
		static void setUInt(const std::string& key, unsigned int value) { setNumber(key, (double)value); }
		static void setInt64(const std::string& key, int64_t value) { setNumber(key, (double)value); }
		static void setUInt64(const std::string& key, uint64_t value) { setNumber(key, (double)value); }

		static std::string getItem(const std::string& key, const std::string& defaultValue = {});
		static void setItem(const std::string& key, const std::string& value);

		static bool containsItem(const std::string& key);
		static void removeItem(const std::string& key);

		static void clear();
		static std::vector<std::string> keys();
	};
}
