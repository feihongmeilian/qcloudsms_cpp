/*
* Copyright (C) feihongmeilian (281006617@qq.com)
* License: MIT
*/

#ifndef SMS_SINGLE_SENDER_HPP
#define SMS_SINGLE_SENDER_HPP

#include "SmsBase.hpp"
#include "SmsSenderUtil.hpp"

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

class SmsSingleSender final : public SmsBase
{
public:
	SmsSingleSender(std::shared_ptr<asio::io_service> &io, int64_t appid, const std::string &appkey)
		: SmsBase(io, appid, appkey)
	{ }

	void send(int32_t type, const std::string &nationCode, const std::string &phoneNumber,
		const std::string &msg, const std::string &extend, const std::string &ext, RequestCallbackFunction &&cb)
	{
		const auto random = SmsSenderUtil::getRandom();
		const auto now = SmsSenderUtil::getCurrentTime();
		nlohmann::json json;
		json["tel"] = { { "nationcode", nationCode },{ "mobile", phoneNumber } };
		json["type"] = type;
		json["msg"] = msg;
		json["sig"] = SmsSenderUtil::calculateSignature(this->appkey_, random, now, phoneNumber);
		json["time"] = now;
		json["extend"] = extend;
		json["ext"] = ext;

		this->post(random, json, std::move(cb));
	}

	void sendWithParam(const std::string &nationCode, const std::string &phoneNumber, int32_t templateId,
		const std::vector<std::string> &parameters, const std::string &sign, const std::string &extend,
		const std::string &ext, RequestCallbackFunction &&cb)
	{
		const auto random = SmsSenderUtil::getRandom();
		const auto now = SmsSenderUtil::getCurrentTime();
		nlohmann::json json;
		json["tel"] = { { "nationcode", nationCode },{ "mobile", phoneNumber } };
		json["tpl_id"] = templateId;
		json["params"] = parameters;
		json["sig"] = SmsSenderUtil::calculateSignature(this->appkey_, random, now, phoneNumber);
		json["sign"] = sign;
		json["time"] = now;
		json["extend"] = extend;
		json["ext"] = ext;

		this->post(random, json, std::move(cb));
	}

protected:
	virtual std::string &path() override
	{
		static std::string path("/v5/tlssmssvr/sendsms");
		return path;
	}
};


class SmsSingleSenderResult final : public SmsResultBase
{
public:
	explicit SmsSingleSenderResult(const std::shared_ptr<HttpsClient::Response> &response)
		:SmsResultBase(response)
	{
		auto json = nlohmann::json::parse(response_->content);

		if (json == nullptr) return;

		result_ = json.at("result").get<int32_t>();
		errMsg_ = json.at("errmsg").get<std::string>();

		auto iter = json.find("ext");
		if (iter != json.end())
		{
			ext_ = iter->get<std::string>();
		}
		iter = json.find("sid");
		if (iter != json.end())
		{
			sid_ = iter->get<std::string>();
		}
		iter = json.find("fee");
		if (iter != json.end())
		{
			fee_ = iter->get<int32_t>();
		}
	}

	int32_t getResult() const { return result_; }
	int32_t getFee() const { return fee_; }
	const std::string &getErrMsg() const { return errMsg_; }
	const std::string &getExt() const { return ext_; }
	const std::string &getSid() const { return sid_; }

private:
	int32_t result_ = 0;
	std::string errMsg_;
	std::string ext_;
	std::string sid_;
	int32_t fee_ = 0;
};

#endif
