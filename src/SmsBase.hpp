/*
* Copyright (C) feihongmeilian (281006617@qq.com)
* License: MIT
*/

#ifndef SMS_BASE_HPP
#define SMS_BASE_HPP

#include <asio.hpp>
#include <client_https.hpp>
#include <nlohmann/json.hpp>

#include <functional>
#include <memory>
#include <string>

using HttpsClient = SimpleWeb::Client<SimpleWeb::HTTPS>;

class SmsBase
{
public:
	using RequestCallbackFunction = std::function<void(std::shared_ptr<HttpsClient::Response>, const SimpleWeb::error_code &)>;


	SmsBase(std::shared_ptr<asio::io_service> &io, int32_t appid, const std::string &appkey)
		:sms_connect_("yun.tim.qq.com", false)
	{
		this->appid_ = appid;
		this->appkey_ = appkey;
		this->sms_connect_.io_service = io;
	}

	virtual ~SmsBase() = default;


protected:
	void post(int64_t random, const nlohmann::json &content, RequestCallbackFunction &&cb) {

		SimpleWeb::CaseInsensitiveMultimap header;
		header.emplace("Conetent-Type", "application/json");

		std::stringstream ss;
		ss << "?sdkappid=" << this->appid_ << "&" << "random=" << random;
		const auto query = ss.str();

		this->sms_connect_.request("POST", path() + query, content.dump(), header, std::move(cb));
	}

	virtual std::string &path() = 0;

protected:
	int32_t appid_;
	std::string appkey_;
	HttpsClient sms_connect_;
};


class SmsResultBase
{
public:
	explicit SmsResultBase(const std::shared_ptr<HttpsClient::Response> &response)
		:response_(response) {}
	virtual ~SmsResultBase() = default;

	std::shared_ptr<HttpsClient::Response> getResponse() const
	{
		return response_;
	}

	std::string ToString() const
	{
		return response_->content.string();
	}

protected:
	std::shared_ptr<HttpsClient::Response> response_;
};

#endif
