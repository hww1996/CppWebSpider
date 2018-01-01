#include<iostream>
#include"StringAlgorithm.h"
#include"CookieJar.h"
#include"response.h"
#include"request.h"

int main() {
    CppWebSpider::requests r;
    CppWebSpider::CookieJar cj;
    /*r.post("192.168.1.242","/query",80,unordered_map<string,string>(),"",{
            {"fname","male"},
    });*/
    r.get("ids1.hfut.edu.cn","/amserver/UI/Login",80,{},{},&cj);
    r.post("ids1.hfut.edu.cn","/amserver/UI/Login",80,{},"", {
            {"IDToken0",""},
            {"IDToken1",""},//学号
            {"IDToken2",""},//密码
            {"IDButton","Submit"},
            {"goto",""},
            {"encoded","false"},
            {"inputCode",""},
            {"gx_charset","UTF-8"},
    },&cj);
    CppWebSpider::response res=r.get("my.hfut.edu.cn","/",80,{},{},&cj);
    std::cout<<res.get_content()<<std::endl;
    return 0;
}