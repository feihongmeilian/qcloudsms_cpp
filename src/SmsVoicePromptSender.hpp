/*
* Copyright (C) feihongmeilian (281006617@qq.com)
* License: MIT
*/

#ifndef SMS_VOICE_PROMPT_SENDER_HPP
#define SMS_VOICE_PROMPT_SENDER_HPP

#include "SmsBase.hpp"
#include "SmsSenderUtil.hpp"

#include <nlohmann/json.hpp>
#include <string>

class SmsVoicePromptSender : public SmsBase
{
public:
	SmsVoicePromptSender(std::shared_ptr<asio::io_service> &io, int32_t appid, const std::string &appkey)
		: SmsBase(io, appid, appkey)
	{ }

	void send(const std::string &nationCode, const std::string &phoneNumber, int32_t prompttype, 
		const std::string &msg,int32_t playtimes, const std::string &ext, RequestCallbackFunction &&cb)
	{
		const auto random = SmsSenderUtil::getRandom();
		const auto now = SmsSenderUtil::getCurrentTime();
		nlohmann::json json;
		json["tel"] = { { "nationcode", nationCode },{ "mobile", phoneNumber } };
		json["prompttype"] = prompttype;
		json["promptfile"] = SmsSenderUtil::gbk2utf8(msg);
		json["playtimes"] = playtimes;
		json["sig"] = SmsSenderUtil::calculateSignature(this->appkey_, random, now, phoneNumber);
		json["time"] = now;
		json["ext"] = ext;

		this->post(random, json, std::move(cb));
	}

protected:
	virtual std::string &path() override
	{
		static std::string path("/v5/tlsvoicesvr/sendvoiceprompt");
		return path;
	}
};

#endif
