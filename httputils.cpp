#include "httputils.h"
#include "httplib.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>

#define CPPHTTPLIB_OPENSSL_SUPPORT

ApiResponse postSse(const std::string& domain, const std::string& uri, const httplib::Headers& headers, const std::string& body, StreamConsumer consumer) {
	httplib::Client cli(domain);

	auto res = cli.Post(uri, headers, body, "application/json; charset=utf-8");

	if (res) {
		std::string contentType = res->get_header_value("Content-Type");
		if (res->status == 200) {
			if (contentType.find("text/event-stream") != std::string::npos) {
				// TODO c++ 实现实时流式比较复杂，这里采用模拟实现（响应结束后处理）
				std::istringstream stream(res->body);
				std::string line;
				while (std::getline(stream, line)) {
					if (line.find("data:") == 0) {
						std::string data = line.substr(5);
						if (data != "" && data != "[DONE]") {
							consumer(data);
						}
					}
				}
			}
		}

		return { res->status, res->body, res->headers, contentType };
	}

	throw std::runtime_error("Request failed.");
}

ApiResponse postJson(const std::string& domain, const std::string& uri, const httplib::Headers& headers, const std::string& body) {
	httplib::Client cli(domain);

	auto res = cli.Post(uri, headers, body, "application/json; charset=utf-8");
	if (res) {
		std::string contentType = res->get_header_value("Content-Type");
		return { res->status, res->body, res->headers, contentType };
	}

	throw std::runtime_error("Request failed.");
}

void download(const std::string& domain, const std::string& uri, const std::string& save_path) {
	httplib::Client cli(domain);

	auto res = cli.Get(uri);
	if (res && res->status == 200) {
		std::ofstream out(save_path, std::ios::binary);
		out << res->body;
		out.close();
	}
	else {
		throw std::runtime_error("Request failed or file cannot be saved.");
	}
}