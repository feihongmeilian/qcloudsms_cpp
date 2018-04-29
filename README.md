# qcloudsms_cpp
腾讯云短信的C++sdk实现版

基于asio和Simple-Web-Server实现的腾讯短信云的C++版本SDK，支持non-boost asio库


使用cmake编译工程时要引入asio和Simple-Web-Server，只需在CMakeLists.txt文件加入如下几行：

add_definitions(-DASIO_STANDALONE)

add_definitions(-DUSE_STANDALONE_ASIO)

include_directories(../deps/asio/include)

include_directories(../deps/Simple-Web-Server)

(假设deps目录是第三方库的存放目录）



使用例子如下：


    #include "SmsSingleSender.hpp"
    #include "SmsMultiSender.hpp"
    #include <string>
    #include <asio.hpp>

    int main() {
        auto io_context = std::make_shared<asio::io_service>();
  
	    int appid = [填自己的];
	    std::string appkey = [填自己的];
	    std::string nationcode("86");
	    std::string phoneNumber([填自己的];);
	    int type = 0;

	    std::string msg0([填自己的];);
    #if ( defined(WIN32) || defined(WIN64) )
            std::string msg = SmsSenderUtil::gbk2utf8(msg0);
    #else
            std::string msg(std::move(msg0));
    #endif
	    SmsSingleSender sender(io_context, appid, appkey);
	    sender.sendWithParam(nationcode, phoneNumber, 113462, { "654321","1" }, "", "", "",[](std::shared_ptr<SimpleWeb::Client<SimpleWeb::HTTPS>::Response> response, const SimpleWeb::error_code &ec) {
                if (!ec)
                {
                    SmsSingleSenderResult result(response);
                    std::cout << "result:" << result.getResult() << std::endl;
                    std::cout << "errMsg:" << result.getErrMsg() << std::endl;
                    std::cout << "ext:" << result.getExt() << std::endl;
                    std::cout << "sid:" << result.getSid() << std::endl;
                    std::cout << "fee:" << result.getFee() << std::endl;
                }
	    });


	    sender.send(type, nationcode, phoneNumber, msg, "", "", [](std::shared_ptr<SimpleWeb::Client<SimpleWeb::HTTPS>::Response> response, const SimpleWeb::error_code &ec) {
                if (!ec)
                {
                    SmsSingleSenderResult result(response);
                    std::cout << "result:" << result.getResult() << std::endl;
                    std::cout << "errMsg:" << result.getErrMsg() << std::endl;
                    std::cout << "ext:" << result.getExt() << std::endl;
                    std::cout << "sid:" << result.getSid() << std::endl;
                    std::cout << "fee:" << result.getFee() << std::endl;
                }
	    });
	
	    SmsMultiSender msender(io_context, appid, appkey);
	    msender.sendWithParam(nationcode, { phoneNumber }, 113462, { "567894","10" }, "", "", "", [](std::shared_ptr<SimpleWeb::Client<SimpleWeb::HTTPS>::Response> response, const SimpleWeb::error_code &ec) {
                if (!ec)
                {
                    SmsMultiSenderResult result(response);
                    std::cout << "result:" << result.getResult() << std::endl;
                    std::cout << "errMsg:" << result.getErrMsg() << std::endl;
                    std::cout << "ext:" << result.getExt() << std::endl;
                }
	    });
	    io_context->run();
    }
