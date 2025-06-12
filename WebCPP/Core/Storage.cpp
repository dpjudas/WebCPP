
#include "WebCPP/Core/Storage.h"
#include "WebCPP/Core/JSValue.h"

JsonValue LocalStorage::getJson(const std::string& key, const JsonValue& defaultValue)
{
	JSValue value = JSValue::global("localStorage").call<JSValue>("getItem", key);
	return value.isNull() ? defaultValue : JsonValue::parse(value.as<std::string>());
}

double LocalStorage::getNumber(const std::string& key, double defaultValue)
{
	return getJson(key, JsonValue::number(defaultValue)).to_double();
}

void LocalStorage::setJson(const std::string& key, const JsonValue& value)
{
	setItem(key, value.to_json());
}

void LocalStorage::setNumber(const std::string& key, double value)
{
	setJson(key, JsonValue::number(value));
}

std::string LocalStorage::getItem(const std::string& key, const std::string& defaultValue)
{
	JSValue value = JSValue::global("localStorage").call<JSValue>("getItem", key);
	return value.isNull() ? defaultValue : value.as<std::string>();
}

void LocalStorage::setItem(const std::string& key, const std::string& value)
{
	JSValue::global("localStorage").call<void>("setItem", key, value);
}

bool LocalStorage::containsItem(const std::string& key)
{
	JSValue value = JSValue::global("localStorage").call<JSValue>("getItem", key);
	return !value.isNull();
}

void LocalStorage::removeItem(const std::string& key)
{
	JSValue::global("localStorage").call<void>("removeItem", key);
}

void LocalStorage::clear()
{
	JSValue::global("localStorage").call<void>("clear");
}

std::vector<std::string> LocalStorage::keys()
{
	auto storage = JSValue::global("localStorage");
	int count = storage["length"].as<int>();
	std::vector<std::string> result;
	result.reserve(count);
	for (int i = 0; i < count; i++)
	{
		result.push_back(storage.call<std::string>("key", i));
	}
	return result;
}

/////////////////////////////////////////////////////////////////////////////

JsonValue SessionStorage::getJson(const std::string& key, const JsonValue& defaultValue)
{
	JSValue value = JSValue::global("sessionStorage").call<JSValue>("getItem", key);
	return value.isNull() ? defaultValue : JsonValue::parse(value.as<std::string>());
}

double SessionStorage::getNumber(const std::string& key, double defaultValue)
{
	return getJson(key, JsonValue::number(defaultValue)).to_double();
}

void SessionStorage::setJson(const std::string& key, const JsonValue& value)
{
	setItem(key, value.to_json());
}

void SessionStorage::setNumber(const std::string& key, double value)
{
	setJson(key, JsonValue::number(value));
}

std::string SessionStorage::getItem(const std::string& key, const std::string& defaultValue)
{
	JSValue value = JSValue::global("sessionStorage").call<JSValue>("getItem", key);
	return value.isNull() ? defaultValue : value.as<std::string>();
}

void SessionStorage::setItem(const std::string& key, const std::string& value)
{
	JSValue::global("sessionStorage").call<void>("setItem", key, value);
}

bool SessionStorage::containsItem(const std::string& key)
{
	JSValue value = JSValue::global("sessionStorage").call<JSValue>("getItem", key);
	return !value.isNull();
}

void SessionStorage::removeItem(const std::string& key)
{
	JSValue::global("sessionStorage").call<void>("removeItem", key);
}

void SessionStorage::clear()
{
	JSValue::global("sessionStorage").call<void>("clear");
}

std::vector<std::string> SessionStorage::keys()
{
	auto storage = JSValue::global("sessionStorage");
	int count = storage["length"].as<int>();
	std::vector<std::string> result;
	result.reserve(count);
	for (int i = 0; i < count; i++)
	{
		result.push_back(storage.call<std::string>("key", i));
	}
	return result;
}
