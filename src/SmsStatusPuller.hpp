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
#include <list>

class SmsStatusPuller final : public SmsBase
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

class SmsStatusPullReplyResult final : public SmsResultBase
{
public:
	class Reply final
	{
	public:
		explicit Reply(const nlohmann::json &json)
		{
			nationCode_ = json.at("nationCode").get<std::string>();
			mobile_ = json.at("mobile").get<std::string>();
			text_ = json.at("text").get<std::string>();
			sign_ = json.at("sign").get<std::string>();
			time_ = json.at("time").get<int64_t>();

			auto iter = json.find("extend");
			if (iter != json.end())
			{
				extend_ = iter->get<std::string>();
			}
		}

		const std::string &getExtend() const { return extend_; }
		const std::string &getNationCode() const { return nationCode_; }
		const std::string &getMobile() const { return mobile_; }
		const std::string &getText() const { return text_; }
		const std::string &getSign() const { return sign_; }
		int64_t getTime() const { return time_; }

	private:
		std::string extend_;
		std::string nationCode_;
		std::string mobile_;
		std::string text_;
		std::string sign_;
		int64_t time_ = 0;
	};

	explicit SmsStatusPullReplyResult(const std::shared_ptr<HttpsClient::Response> &response)
		:SmsResultBase(response)
	{
		auto json = nlohmann::json::parse(response_->content);

		if (json == nullptr) return;

		result_ = json.at("result").get<int32_t>();
		errMsg_ = json.at("errmsg").get<std::string>();

		auto iter = json.find("count");
		if (iter != json.end())
		{
			count_ = iter->get<int32_t>();
		}

		iter = json.find("data");
		if (iter != json.end())
		{
			for (auto item : iter->items())
			{
				replys_.emplace_back(Reply(item.value()));
			}
		}
	}

	int32_t getResult() const { return result_; }
	int32_t getCount() const { return count_; }
	const std::string &getErrMsg() const { return errMsg_; }
	const std::list<Reply> &getReplys() const { return replys_; }

private:
	int32_t result_;
	std::string errMsg_;
	int32_t count_;
	std::list<Reply> replys_;
};

class SmsStatusPullCallbackResult final : public SmsResultBase
{
public:
	class Callback final
	{
	public:
		explicit Callback(const nlohmann::json &json)
		{
			user_receive_time_ = json.at("user_receive_time").get<std::string>();
			nationCode_ = json.at("nationCode").get<std::string>();
			mobile_ = json.at("mobile").get<std::string>();
			report_status_ = json.at("report_status").get<std::string>();
			errMsg_ = json.at("errmsg").get<std::string>();
			description_ = json.at("description").get<std::string>();
			sid_ = json.at("sid").get<std::string>();
		}

		const std::string &getUserReceiveTime() const { return user_receive_time_; }
		const std::string &getNationCode() const { return nationCode_; }
		const std::string &getMobile() const { return mobile_; }
		const std::string &getReportStatus() const { return report_status_; }
		const std::string &getErrMsg() const { return errMsg_; }
		const std::string &getDescription() const { return description_; }
		const std::string &getSid() const { return sid_; }

	private:
		std::string user_receive_time_;
		std::string nationCode_;
		std::string mobile_;
		std::string report_status_;
		std::string errMsg_;
		std::string description_;
		std::string sid_;
	};

	explicit SmsStatusPullCallbackResult(const std::shared_ptr<HttpsClient::Response> &response)
		:SmsResultBase(response)
	{
		auto json = nlohmann::json::parse(response_->content);

		if (json == nullptr) return;

		result_ = json.at("result").get<int32_t>();
		errMsg_ = json.at("errmsg").get<std::string>();

		auto iter = json.find("count");
		if (iter != json.end())
		{
			count_ = iter->get<int32_t>();
		}

		iter = json.find("data");
		if (iter != json.end())
		{
			for (auto item : iter->items())
			{
				callbacks_.emplace_back(Callback(item.value()));
			}
		}
	}

	int32_t getResult() const { return result_; }
	int32_t getCount() const { return count_; }
	const std::string &getErrMsg() const { return errMsg_; }
	const std::list<Callback> &getCallbacks() const { return callbacks_; }

private:
	int32_t result_;
	std::string errMsg_;
	int32_t count_;
	std::list<Callback> callbacks_;
};

#endif
