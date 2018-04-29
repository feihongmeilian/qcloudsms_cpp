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
#include <list>


class SmsMultiSender final : public SmsBase
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

class SmsMultiSenderResult final : public SmsResultBase
{
public:
	class Detail final
	{
	public:
		explicit Detail(const nlohmann::json &json)
		{
			result_ = json.at("result").get<int32_t>();
			errMsg_ = json.at("errmsg").get<std::string>();

			auto iter = json.find("mobile");
			if (iter != json.end())
			{
				mobile_ = iter->get<std::string>();
			}

			iter = json.find("nationcode");
			if (iter != json.end())
			{
				nationCode_ = iter->get<std::string>();
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
		const std::string &getErrMsg() const { return errMsg_; }
		const std::string &getMobile() const { return mobile_; }
		const std::string &getNationCode() const { return nationCode_; }
		const std::string &getSid() const { return sid_; }
		int32_t getFee() const { return fee_; }

	private:
		int32_t result_ = 0;
		std::string errMsg_;
		std::string mobile_;
		std::string nationCode_;
		std::string sid_;
		int32_t fee_ = 0;
	};

	explicit SmsMultiSenderResult(const std::shared_ptr<HttpsClient::Response> &response)
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

		iter = json.find("detail");
		if (iter != json.end())
		{
			for(auto item : iter->items())
			{
				details_.emplace_back(Detail(item.value()));
			}
		}
	}

	int32_t getResult() const { return result_; }
	const std::string &getErrMsg() const { return errMsg_; }
	const std::string &getExt() const { return ext_; }
	const std::list<Detail> &getDetails() const {return details_; }

private:
	int32_t result_ = 0;
	std::string errMsg_;
	std::string ext_;
	std::list<Detail> details_;
};

#endif