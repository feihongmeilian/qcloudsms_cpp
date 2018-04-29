/*
* Copyright (C) feihongmeilian (281006617@qq.com)
* License: MIT
*/

#ifndef SMS_MOBILE_STATUS_PULLER_HPP
#define SMS_MOBILE_STATUS_PULLER_HPP

#include "SmsBase.hpp"
#include "SmsSenderUtil.hpp"
#include "SmsStatusPuller.hpp"

#include <nlohmann/json.hpp>
#include <string>

using SmsMobileStatusPullCallbackResult = SmsStatusPullCallbackResult;
using SmsMobileStatusPullReplyResult = SmsStatusPullReplyResult;

class SmsMobileStatusPuller final : public SmsBase
{
public:
	SmsMobileStatusPuller(std::shared_ptr<asio::io_service> &io, int32_t appid, const std::string &appkey)
		: SmsBase(io, appid, appkey)
	{ }

	void pullCallback(const std::string &nationCode, const std::string &mobile, int64_t beginTime,
		int64_t endTime, int32_t max, RequestCallbackFunction &&cb)
	{
		pull(0, nationCode, mobile, beginTime, endTime, max, std::move(cb));
	}

	void pullReply(const std::string &nationCode, const std::string &mobile, int64_t beginTime,
		int64_t endTime, int32_t max, RequestCallbackFunction &&cb)
	{
		pull(1, nationCode, mobile, beginTime, endTime, max, std::move(cb));
	}

protected:
	virtual std::string &path() override
	{
		static std::string path("/v5/tlssmssvr/pullstatus4mobile");
		return path;
	}

private:
	void pull(int32_t type, const std::string &nationCode, const std::string &mobile, int64_t beginTime,
		int64_t endTime, int32_t max, RequestCallbackFunction &&cb)
	{
		const auto random = SmsSenderUtil::getRandom();
		const auto now = SmsSenderUtil::getCurrentTime();
		nlohmann::json json;
		json["sig"] = SmsSenderUtil::calculateSignature(this->appkey_, random, now);
		json["type"] = type;
		json["time"] = now;
		json["max"] = max;
		json["begin_time"] = beginTime;
		json["end_time"] = endTime;
		json["nationcode"] = nationCode;
		json["mobile"] = mobile;

		this->post(random, json, std::move(cb));
	}
};

#endif

