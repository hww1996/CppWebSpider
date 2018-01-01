#ifndef __RESPONSE_H__
#define __RESPONSE_H__


#include"StringAlgorithm.h"
#include"CookieJar.h"

namespace CppWebSpider{
	/*
		处理放回的数据
	*/

	class response{
	public:
		/*
			作用：构造函数
			参数：
			host	当前爬取的url的host
			uri		想要爬取的资源
			content	爬虫返回的原始的的http数据
			cookie	CookieJar的容器
		*/
		response(std::string host,std::string uri,std::string content,CookieJar *cookie):status_code(-1),
														protocol(""),
														host(host),
														uri(uri),
														content(""),
														headers(std::unordered_map<std::string,std::string>())
		{
			//假如爬虫返回的原始的的http数据不为空才parse
			if(content.size()!=0)
				parse(content,cookie);
		}
		//返回爬取的status_code
		int get_status_code() const{
			return this->status_code;
		}
		//返回爬取的内容
		std::string get_content() const{
			return this->content;
		}
		//返回爬取的协议
		std::string get_protocol() const{
			return this->protocol;
		}
		//返回一些response头
		std::unordered_map<std::string,std::string> get_headers() const{
			return this->headers;
		};
	private:
		/*
			作用：处理爬虫返回的原始的的http数据
			参数：
			content	爬虫返回的原始的的http数据
			cookie	CookieJar的容器
		*/
		void parse(std::string content,CookieJar *cookie);
		/*
			作用：处理返回的http头部。
			参数：
			header	返回的http头部
			cookie	CookieJar的容器
		*/
		void parse_header(std::string header,CookieJar *cookie);

		//返回的状态码
		int status_code=-1;
		//使用的协议
		std::string protocol="";
		//爬取的url的host
		std::string host;
		//请求的资源
		std::string uri;
		//返回的html数据
		std::string content="";
		//一些http的返回头的数据
		std::unordered_map<std::string,std::string> headers;
	};
}

#endif