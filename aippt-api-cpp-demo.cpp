
#include <iostream>
#include <iostream>
#include "httputils.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main()
{
	// 官网 https://docmee.cn
	// 开放平台 https://docmee.cn/open-platform/api

	// 填写你的API-KEY
	std::string apiKey = "{YOUR API-KEY}";

	// 第三方用户ID（数据隔离）
	std::string userId = "test";
	std::string subject = "AI未来的发展";

	// 创建token
	std::string token = createApiToken(apiKey, userId);

	// 直接生成PPT
	std::cout << "正在生成PPT...\n";
	directGeneratePptx(token, subject);
}

std::string createApiToken(std::string& apiKey, std::string& uid) {
	httplib::Headers headers = {
		{"Api-Key", apiKey}
	};
	std::string body = "{\"uid\":\"" + uid + "\"}";
	HttpResponse resp = postJson("https://docmee.cn", "/user/createApiToken", headers, body);
	if (resp.status != 200) {
		throw std::runtime_error("创建apiToken失败，httpStatus=" + std::to_string(resp.status));
	}
	auto jsonData = json::parse(resp.text);
	int code = jsonData["code"];
	if (code != 0) {
		std::string message = jsonData["message"];
		throw std::runtime_error("创建apiToken异常：" + message);
	}
	std::string token = jsonData["data"]["token"];
	return token;
}

void directGeneratePptx(std::string& token, std::string& subject) {
	httplib::Headers headers = {
		{"token", token}
	};
	std::string body = "{\"stream\": true, \"subject\": \"" + subject + "\", \"pptxProperty\": false}";
	StreamConsumer consumer = [](const std::string& data) {
		auto jsonData = json::parse(data);
		int status = jsonData["status"];
		if (status == -1) {
			// 异常
			std::string error = jsonData["error"];
			throw std::runtime_error("生成PPT异常：" + error);
		}
		if (jsonData.contains("text")) {
			std::string text = jsonData["text"];
			std::cout << text;
		}
		if (status == 4 && jsonData.contains("result")) {
			// 生成完成
			auto pptInfo = jsonData["result"];
			std::cout << "\npptId: " + pptInfo["id"] + "\n";
			std::cout << "ppt主题: " + pptInfo["subject"] + "\n";
			std::cout << "ppt下载链接: " + pptInfo["fileUrl"] + "\n";
		}
	};
	HttpResponse resp = postSse("https://docmee.cn", "/api/ppt/directGeneratePptx", headers, body, consumer);
	if (resp.status != 200) {
		throw std::runtime_error("生成PPT失败，httpStatus=" + std::to_string(resp.status));
	}
	if (resp.contentType.find("application/json") != std::string::npos) {
		auto jsonData = json::parse(resp.text);
		std::string message = jsonData["message"];
		throw std::runtime_error("生成PPT异常：" + message);
	}
}