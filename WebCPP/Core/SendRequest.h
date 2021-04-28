#pragma once

#include <memory>
#include <vector>
#include <map>
#include <functional>
#include "JSValue.h"

void sendRequest(std::string url, JSValue request, std::function<void(JSValue response)> responseHandler, std::function<void(int statusCode, std::string contentType, std::string body)> errorHandler = {});
void setDefaultRequestErrorHandler(std::function<void(int statusCode, std::string contentType, std::string body)> errorHandler);
void callDefaultRequestErrorHandler(int statusCode, std::string contentType, std::string body);
