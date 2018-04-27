/*
* Copyright (C) feihongmeilian (281006617@qq.com)
* License: MIT
*/

#ifndef SMS_STATUS_PULLER_HPP
#define SMS_STATUS_PULLER_HPP

#include "SmsBase.hpp"
#include "SmsSenderUtil.hpp"

#include <nlohmann/json.hpp>
#include <string>

class SmsStatusPuller : public SmsBase
{
public:
	SmsStatusPuller(std::shared_ptr<asio::io_service> &io, int32_t appid, const std::string &appkey)
		: SmsBase(io, appid, appkey)
	{ }

	void pullCallback(int32_t max, RequestCallbackFunction &&cb)
	{
		pull(0, max, std::move(cb));
	}

	void pullReply(int32_t max, RequestCallbackFunction &&cb)
	{
		pull(1, max, std::move(cb));
	}

protected:
	virtual std::string &path() override
	{
		static std::string path("/v5/tlssmssvr/pullstatus");
		return path;
	}

private:
	void pull(int32_t type, int32_t max, RequestCallbackFunction &&cb)
	{
		const auto random = SmsSenderUtil::getRandom();
		const auto now = SmsSenderUtil::getCurrentTime();
		nlohmann::json json;
		json["sig"] = SmsSenderUtil::calculateSignature(this->appkey_, random, now);
		json["time"] = now;
		json["type"] = type;
		json["max"] = max;

		this->post(random, json, std::move(cb));
	}
};

#endif
