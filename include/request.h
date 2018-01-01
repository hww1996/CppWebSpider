#ifndef __REQUEST_H__
#define __REQUEST_H__

#include<unistd.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<poll.h>

#include"StringAlgorithm.h"
#include"CookieJar.h"
#include"response.h"

namespace CppWebSpider{
	class requests{
	public:
		requests()= default;
		response get(std::string host,std::string uri,uint16_t port,
				   std::unordered_map<std::string,std::string> headers=std::unordered_map<std::string,std::string>(),
				   std::unordered_map<std::string,std::string> data=std::unordered_map<std::string,std::string>(),
				   CookieJar *cookie=NULL,
				   int timeout=3);
		response post(std::string host,std::string uri,uint16_t port,
					std::unordered_map<std::string,std::string> headers=std::unordered_map<std::string,std::string>(),
					std::string json_data="",
					std::unordered_map<std::string,std::string> data=std::unordered_map<std::string,std::string>(),
					CookieJar *cookie=NULL,
					int timeout=3);
	private:
		response request_and_recv(std::string host,
								std::string uri,
								int sockfd,
								std::string headers,
								CookieJar *cookie,
								int timeout);

		//create the headers
		std::string make_header(std::string base_header,
						   std::string host,
						   std::string uri,
						   const std::unordered_map<std::string,std::string> &headers,
						   CookieJar *cookie);

		int init(std::string host,uint16_t port);
	};
}


#endif