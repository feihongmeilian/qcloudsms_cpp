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

class SmsSingleSender : public SmsBase
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
		json["msg"] = SmsSenderUtil::gbk2utf8(msg);
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

#endif
