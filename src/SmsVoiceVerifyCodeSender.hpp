/*
* Copyright (C) feihongmeilian (281006617@qq.com)
* License: MIT
*/

#ifndef SMS_VOICE_VERIFY_CODE_SENDER_HPP
#define SMS_VOICE_VERIFY_CODE_SENDER_HPP

#include "SmsBase.hpp"
#include "SmsSenderUtil.hpp"

#include <nlohmann/json.hpp>
#include <string>

class SmsVoiceVerifyCodeSender : public SmsBase
{
public:
	SmsVoiceVerifyCodeSender(std::shared_ptr<asio::io_service> &io, int32_t appid, const std::string &appkey)
		: SmsBase(io, appid, appkey)
	{ }

	void send(const std::string &nationCode, const std::string &phoneNumber, const std::string &msg,
		int32_t playtimes, const std::string &ext, RequestCallbackFunction &&cb)
	{
		const auto random = SmsSenderUtil::getRandom();
		const auto now = SmsSenderUtil::getCurrentTime();
		nlohmann::json json;
		json["tel"] = { { "nationcode", nationCode },{ "mobile", phoneNumber } };
		json["msg"] = SmsSenderUtil::gbk2utf8(msg);
		json["sig"] = SmsSenderUtil::calculateSignature(this->appkey_, random, now, phoneNumber);
		json["time"] = now;
		json["playtimes"] = playtimes;
		json["ext"] = ext;

		this->post(random, json, std::move(cb));
	}

protected:
	virtual std::string &path() override
	{
		static std::string path("/v5/tlsvoicesvr/sendvoice");
		return path;
	}
};

#endif
