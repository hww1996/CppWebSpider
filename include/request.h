#ifndef __REQUEST_H__
#define __REQUEST_H__

#include<unistd.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<poll.h>

#include"StringAlgorithm.h"
#include"CookieJar.h"
#include"response.h"

using namespace std;

class requests{
public:
    requests()= default;
    response get(string host,string uri,uint16_t port,
               unordered_map<string,string> headers=unordered_map<string,string>(),
               unordered_map<string,string> data=unordered_map<string,string>(),
               CookieJar *cookie=NULL,
               int timeout=3);
    response post(string host,string uri,uint16_t port,
                unordered_map<string,string> headers=unordered_map<string,string>(),
                string json_data="",
                unordered_map<string,string> data=unordered_map<string,string>(),
                CookieJar *cookie=NULL,
                int timeout=3);
private:
    response request_and_recv(string host,
                            string uri,
                            int sockfd,
                            string headers,
                            CookieJar *cookie,
                            int timeout);

    //create the headers
    string make_header(string base_header,
                       string host,
                       string uri,
                       const unordered_map<string,string> &headers,
                       CookieJar *cookie);

    int init(string host,uint16_t port);
};


#endif