#include <iostream>
#include<unistd.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<poll.h>
#include<cstring>
#include<unordered_map>
using namespace std;


class requests{
public:
    requests()= default;
    string get(string host,string uri,uint16_t port,
               unordered_map<string,string> headers=unordered_map<string,string>(),
               unordered_map<string,string> data=unordered_map<string,string>(),
               unordered_map<string,string> cookie=unordered_map<string,string>(),
               int timeout=3){
        int sockfd;
        if((sockfd=init(host,port))<0)
            return "";

        int data_count=0;
        if(data.size()!=0)
            uri+="?";
        for(auto iter=data.begin();iter!=data.end();++iter){
            if(data_count>0)
                uri+="&";
            uri=uri+iter->first+"="+iter->second;
            data_count++;
        }
        //base header
        string sendmsg=
                "GET "+uri+" HTTP/1.1\r\n"+
                "Host: "+ host+"\r\n";

        sendmsg=make_header(sendmsg,headers,cookie);

        //request
        write(sockfd,sendmsg.c_str(),sendmsg.size());


        //poll
        struct pollfd clientfds[1];

        clientfds[0].fd=sockfd;
        clientfds[0].events=POLLIN;

        int nready=0;
        ssize_t read_count=0;

        char buf[32]={'\0'};
        string ans="";
        while(true){
            nready=poll(clientfds,1,timeout*1000);
            if(nready==0)
                break;
            else if(nready==-1){
                cerr<<"poll error."<<endl;
                return "";
            }
            if(clientfds[0].revents&POLLIN){
                read_count=read(sockfd,buf,sizeof(buf)-1);
                if(read_count<=0){
                    break;
                }
                ans+=buf;
                read_count=0;
                memset(buf,0,sizeof(buf));
            }else{
                break;
            }

        }

        if(read_count<0){
            cerr<<"read error."<<endl;
            return "";
        }

        cout<<ans<<endl;

        close(sockfd);

        return ans;
    }
    string post(string host,string uri,uint16_t port,
                unordered_map<string,string> headers=unordered_map<string,string>(),
                string json_data="",
                unordered_map<string,string> data=unordered_map<string,string>(),
                unordered_map<string,string> cookie=unordered_map<string,string>(),
                int timeout=3){
        int sockfd;
        if((sockfd=init(host,port))<0)
            return "";

        string post_data="";
        if(json_data.size()!=0)
            post_data=json_data;
        else{
            int data_count=0;
            for(auto data_iter=data.begin();data_iter!=data.end();++data_iter){
                if(data_count>0)
                    post_data+="&";
                post_data=post_data+data_iter->first+"="+data_iter->second;
                data_count++;
            }
        }

        int data_size=post_data.size();
        headers.insert({"Content-Length",to_string(data_size)});
        headers.insert({"Content-Type","application/x-www-form-urlencoded"});
        //base header
        string sendmsg=
                "POST "+uri+" HTTP/1.1\r\n"+
                "Host: "+ host+"\r\n";

        sendmsg=make_header(sendmsg,headers,cookie);
        sendmsg+=post_data;

        //request
        write(sockfd,sendmsg.c_str(),sendmsg.size());


        //poll
        struct pollfd clientfds[1];

        clientfds[0].fd=sockfd;
        clientfds[0].events=POLLIN;

        int nready=0;
        ssize_t read_count=0;

        char buf[32]={'\0'};
        string ans="";
        while(true){
            nready=poll(clientfds,1,timeout*1000);
            if(nready==0)
                break;
            else if(nready==-1){
                cerr<<"poll error."<<endl;
                return "";
            }
            if(clientfds[0].revents&POLLIN){
                read_count=read(sockfd,buf,sizeof(buf)-1);
                if(read_count<=0){
                    break;
                }
                ans+=buf;
                read_count=0;
                memset(buf,0,sizeof(buf));
            }else{
                break;
            }

        }

        if(read_count<0){
            cerr<<"read error."<<endl;
            return "";
        }

        cout<<ans<<endl;

        close(sockfd);

        return ans;
    }
private:
    //create the headers
    string make_header(string base_header,
                       const unordered_map<string,string> &headers,
                       const unordered_map<string,string> &cookie){
        unordered_map<string,bool> base_usage{
                {"Connection",false},
                {"Accept",false},
                {"Accept-Encoding",false}
        };
        unordered_map<string,string> base_values{
                {"Connection","close"},
                {"Accept","*/*"},
                {"Accept-Encoding","gzip, deflate"}
        };

        for(auto iter=headers.begin();iter!=headers.end();++iter){
            if(base_usage.find(iter->first)!=base_usage.end()){
                base_usage[iter->first]=true;
            }
            base_header=base_header+iter->first+": "+iter->second+"\r\n";
        }

        for(auto value_iter=base_values.begin();value_iter!=base_values.end();++value_iter){
            if(!base_usage[value_iter->first]){
                base_header=base_header+value_iter->first+": "+value_iter->second+"\r\n";
            }
        }

        for(auto cookie_iter=cookie.begin();cookie_iter!=cookie.end();++cookie_iter){
            base_header=base_header+"Cookie: "+cookie_iter->first+"="+cookie_iter->second+"\r\n";
        }

        base_header+="\r\n";

        return base_header;

    }


    typedef struct sockaddr SA;
    int init(string host,uint16_t port){
        int sockfd=-1;

        hostent *hptr=gethostbyname(host.c_str());
        if(hptr==NULL)
            return -1;
        sockfd=socket(hptr->h_addrtype,SOCK_STREAM,0);

        struct sockaddr_in avest;
        bzero(&avest,sizeof(avest));
        avest.sin_family=hptr->h_addrtype;
        avest.sin_port=htons(port);

        char ipLocation[32]={'\0'};
        inet_ntop(hptr->h_addrtype,hptr->h_addr,ipLocation,sizeof(ipLocation));
        cout<<ipLocation<<endl;
        inet_pton(hptr->h_addrtype,ipLocation,&avest.sin_addr);


        if(connect(sockfd,(const SA *)&avest,sizeof(avest))<0)
            return -1;
        return sockfd;
    }
};

int main() {
    requests r;
    r.post("192.168.1.242","/query",80,unordered_map<string,string>(),"",{
            {"fname","male"},
    });
    return 0;
}