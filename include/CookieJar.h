#ifndef __COOKIEJAR_H__
#define __COOKIEJAR_H__

#include<unordered_map>
#include"StringAlgorithm.h"
using namespace std;

//版本比较低的cookie的结构
struct cookie_struct {
    string name;
    string value;
    string expires;
    string path;
    string domain;
};
/*
1.cookie的存储的思想用了想chrome的方式，按domain来作为key来存放cookie
2.cookie的算法主要还是用了一篇博客中对cookie的描述，博客地址：
	http://bubkoo.com/2014/04/21/http-cookies-explained/
3.只支持有名字的cookie：
			Set-Cookie: name=value;...
	但是不支持：
			Set-Cookie: value;...
4.现在暂时不支持对时间(expires)的处理
*/
class CookieJar{
public:
    CookieJar():cookie_map(unordered_map<string,vector<cookie_struct> >()){}
	
    /*
		作用：添加cookie到cookie_map里面
		参数：
		host 		爬取的url的host
		cookie_list 从response里面parse以后得到的Set-Cookie的数组
	*/
    void add(string host,vector<cookie_struct> cookie_list);
	
    /*
		作用：返回当前的url能够使用的cookie，以name=value;name=value;...的方式
		参数：
		host	爬取的url的host
		uri		要爬取的资源
	*/
    string use(string host,string uri);

	//作用清除cookie，可以相当于将网页关闭，然后重新访问。
    void clear();
private:
	/*
		作用：将cookie真正的添加到cookie_map里面
		参数：
		host 	爬取的url的host
		cs		想要添加到cookie_map的cookie
	*/
    void add_helper(string host,cookie_struct cs);
	/*
		作用：根据当前的host和uri取出可用的cookie，并以以name=value;name=value;...的方式返回
		参数：
		host 	当前的host
		uri		当前的uri
	*/
    string use_helper(string host,string uri);
	//所有使用了这个CookieJar的cookie的保存容器
    unordered_map<string,vector<cookie_struct>> cookie_map;
};

#endif