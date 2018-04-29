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

class SmsVoiceVerifyCodeSender final : public SmsBase
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


class SmsVoiceVerifyCodeSenderResult  final : public SmsResultBase
{
public:
	explicit SmsVoiceVerifyCodeSenderResult(const std::shared_ptr<HttpsClient::Response> &response)
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
		iter = json.find("callid");
		if (iter != json.end())
		{
			callId_ = iter->get<std::string>();
		}
	}

	int32_t getResult() const { return result_; }
	const std::string &getErrMsg() const { return errMsg_; }
	const std::string &getExt() const { return ext_; }
	const std::string &getCallId() const { return callId_; }

private:
	int32_t result_ = 0;
	std::string errMsg_;
	std::string ext_;
	std::string callId_;
};

#endif
