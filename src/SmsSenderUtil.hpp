/*
* Copyright (C) feihongmeilian (281006617@qq.com)
* License: MIT
*/

#ifndef SMS_SENDER_UTIL_HPP
#define SMS_SENDER_UTIL_HPP

#include <openssl/sha.h>

#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <random>
#include <memory>
#include <iomanip>

#if ( defined(WIN32) || defined(WIN64) )
#include <winnls.h>
#endif

class SmsSenderUtil
{
public:
	static int64_t getCurrentTime()
	{
		return std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()).time_since_epoch().count();;
	}

	static int64_t getRandom()
	{
		std::default_random_engine engine(getCurrentTime());
		std::uniform_int_distribution<unsigned> distribution(0, INT_MAX);
		return distribution(engine) % 900000L + 100000L;
	}

#if ( defined(WIN32) || defined(WIN64) )
	static std::string gbk2utf8(const std::string &gbk)
	{
		int len = MultiByteToWideChar(CP_ACP, 0, gbk.c_str(), -1, nullptr, 0);

		std::unique_ptr<wchar_t> wstr(new wchar_t[len + 1]);
		::memset(wstr.get(), 0, len + 1);
		MultiByteToWideChar(CP_ACP, 0, gbk.c_str(), -1, wstr.get(), len);
		len = WideCharToMultiByte(CP_UTF8, 0, wstr.get(), -1, nullptr, 0, nullptr, nullptr);

		std::unique_ptr<char> str(new char[len + 1]);
		::memset(str.get(), 0, len + 1);
		WideCharToMultiByte(CP_UTF8, 0, wstr.get(), -1, str.get(), len, nullptr, nullptr);
		
		std::string utf(str.get());
		return utf;
	}
#endif

	static std::string calculateSignature(const std::string &appkey, int64_t random, int64_t time, const std::string &phoneNumber)
	{
		std::stringstream ss;
		ss << "appkey=" << appkey << "&random=" << random << "&time=" << time << "&mobile=" << phoneNumber;
		return sha256(ss.str());
	}

	static std::string calculateSignature(const std::string &appkey, int64_t random, int64_t time,
		const std::vector<std::string> &phoneNumbers)
	{
		std::stringstream ss;
		ss << "appkey=" << appkey << "&random=" << random << "&time=" << time << "&mobile=";

		if (phoneNumbers.size() > 0)
		{
			ss <<phoneNumbers[0];
			for (int i = 1; i != phoneNumbers.size(); ++i)
			{
				ss << ",";
				ss << phoneNumbers[i];
			}
		}

		return sha256(ss.str());
	}

	static std::string calculateSignature(const std::string &appkey, int64_t random, int64_t time)
	{
		std::stringstream ss;
		ss << "appkey=" << appkey << "&random=" << random << "&time=" << time;
		return sha256(ss.str());
	}

private:
	static std::string sha256(const std::string &rawString)
	{
		std::stringstream ss;
		unsigned char md[SHA256_DIGEST_LENGTH];
		SHA256(reinterpret_cast<unsigned char *>(const_cast<char *>(rawString.data())), rawString.size(), md);
		for (int i = 0; i != SHA256_DIGEST_LENGTH; ++i)
		{
			ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(md[i]);
		}
		return ss.str();
	}
};

#endif