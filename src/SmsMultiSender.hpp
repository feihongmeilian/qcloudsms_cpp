/*
* Copyright (C) feihongmeilian (281006617@qq.com)
* License: MIT
*/

#ifndef SMS_MULTI_SENDER_HPP
#define SMS_MULTI_SENDER_HPP

#include "SmsBase.hpp"
#include "SmsSenderUtil.hpp"

#include <nlohmann/json.hpp>

#include <string>
#include <vector>


class SmsMultiSender : public SmsBase
{
public:
	SmsMultiSender(std::shared_ptr<asio::io_service> &io, int32_t appid, const std::string &appkey)
		: SmsBase(io, appid, appkey)
	{ }

	void send(int32_t type, const std::string &nationCode, const std::vector<std::string> &phoneNumbers,
	          const std::string &msg, const std::string &extend, const std::string &ext, RequestCallbackFunction &&cb)
	{
		const auto random = SmsSenderUtil::getRandom();
		const auto now = SmsSenderUtil::getCurrentTime();

		nlohmann::json json;
		json["tel"] = toTel(nationCode, phoneNumbers);
		json["type"] = type;
		json["msg"] = SmsSenderUtil::gbk2utf8(msg);
		json["sig"] = SmsSenderUtil::calculateSignature(this->appkey_, random, now, phoneNumbers);
		json["time"] = now;
		json["extend"] = extend;
		json["ext"] = ext;

		this->post(random, json, std::move(cb));
	}

	void sendWithParam(const std::string &nationCode, const std::vector<std::string> &phoneNumbers,
		int templateId, const std::vector<std::string> &parameters, const std::string &sign, 
		const std::string &extend, const std::string &ext, RequestCallbackFunction &&cb)
	{
		const auto random = SmsSenderUtil::getRandom();
		const auto now = SmsSenderUtil::getCurrentTime();

		nlohmann::json json;
		json["tel"] = toTel(nationCode, phoneNumbers);
		json["tpl_id"] = templateId;
		json["params"] = parameters;
		json["sig"] = SmsSenderUtil::calculateSignature(this->appkey_, random, now, phoneNumbers);
		json["sign"] = sign;
		json["time"] = now;
		json["extend"] = extend;
		json["ext"] = ext;

		this->post(random, json, std::move(cb));
	}

protected:
	virtual std::string &path() override
	{
		static std::string path("/v5/tlssmssvr/sendmultisms2");
		return path;
	}

private:
	std::vector<nlohmann::json> toTel(const std::string &nationCode, const std::vector<std::string> &phoneNumbers)
	{
		std::vector<nlohmann::json> vec;
		for (auto phoneNumber : phoneNumbers)
		{
			vec.push_back({ { "nationcode", nationCode }, { "mobile", phoneNumber } });
		}
		return vec;
	}
};

#endif