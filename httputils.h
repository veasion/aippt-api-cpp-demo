#ifndef HTTP_UTILS_H
#define HTTP_UTILS_H

#include "httplib.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>

class ApiResponse {
public:
	int status;
	std::string text;
	httplib::Headers headers;
	std::string contentType;

	ApiResponse(int status, const std::string& text, const httplib::Headers& headers, const std::string& contentType)
		: status(status), text(text), headers(headers), contentType(contentType) {}
};

using StreamConsumer = std::function<void(const std::string&)>;

ApiResponse postSse(const std::string& domain, const std::string& uri, const httplib::Headers& headers, const std::string& body, StreamConsumer consumer);

ApiResponse postJson(const std::string& domain, const std::string& uri, const httplib::Headers& headers, const std::string& body);

void download(const std::string& domain, const std::string& uri, const std::string& save_path);

#endif