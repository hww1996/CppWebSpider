#ifndef __RESPONSE_H__
#define __RESPONSE_H__


#include"StringAlgorithm.h"
#include"CookieJar.h"
using namespace std;

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
    response(string host,string uri,string content,CookieJar *cookie):status_code(-1),
                                                    protocol(""),
                                                    host(host),
                                                    uri(uri),
                                                    content(""),
                                                    headers(unordered_map<string,string>())
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
    string get_content() const{
        return this->content;
    }
	//返回爬取的协议
    string get_protocol() const{
        return this->protocol;
    }
	//返回一些response头
    unordered_map<string,string> get_headers() const{
        return this->headers;
    };
private:
	/*
		作用：处理爬虫返回的原始的的http数据
		参数：
		content	爬虫返回的原始的的http数据
		cookie	CookieJar的容器
	*/
    void parse(string content,CookieJar *cookie);
	/*
		作用：处理返回的http头部。
		参数：
		header	返回的http头部
		cookie	CookieJar的容器
	*/
    void parse_header(string header,CookieJar *cookie);

	//返回的状态码
    int status_code=-1;
	//使用的协议
    string protocol="";
	//爬取的url的host
    string host;
	//请求的资源
    string uri;
	//返回的html数据
    string content="";
	//一些http的返回头的数据
    unordered_map<string,string> headers;
};

#endif