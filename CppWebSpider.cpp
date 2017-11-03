#include <iostream>
#include<unistd.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<poll.h>
#include<cstring>
#include<unordered_map>
#include<vector>
using namespace std;

string strip(string target,string s=" "){
    size_t n=s.size(),m=target.size();
    if(m<n)
        return target;
    size_t start=0,i=0;
    while(i<m){
        for(size_t j=0;j<n;j++){
            if(s[j]!=target[i])
                break;
            i++;
        }
        if(i==start)
            break;
        if(i%n==0)
            start=i;
        else
            break;
    }
    size_t end=m;
    i=end-n;
    while(i>start){
        for(size_t j=0;j<n;j++){
            if(s[j]!=target[i])
                break;
            i++;
        }
        if(i==end)
            end-=n;
        else
            break;
        i=end-n;
    }
    string ans=target.substr(start,end-start);
    return ans;
}

vector<string> split(string target,string s){
    size_t pos=0,n=s.size();
    vector<string> string_list;
    while((pos=target.find(s))!=target.npos){
        if(pos!=0){
            string_list.push_back(target.substr(0,pos));
        }
        target=target.substr(pos+n);
    }
    if(target.size()!=0)
        string_list.push_back(target);
    return string_list;
}

string join(vector<string> string_list,string s){
    size_t n=string_list.size();
    if(n==0)
        return "";

    string ans=string_list[0];
    for(size_t i=1;i<n;i++){
        ans=ans+s+string_list[i];
    }

    return ans;
}

struct cookie_struct {
    string name;
    string value;
    string expires;
    string path;
    string domain;
};

class CookieJar{
public:
    CookieJar():cookie_map(unordered_map<string,vector<cookie_struct> >()){}


    //add the cookie
    void add(string host,vector<cookie_struct> cookie_list){
        size_t n=cookie_list.size();
        for(size_t i=0;i<n;i++){
            if(cookie_list[i].domain.empty()){
                add_helper(host,cookie_list[i]);
            }
            else{
                add_helper(cookie_list[i].domain,cookie_list[i]);
            }
        }
    }

    // return the cookie it need
    string use(string host,string uri){
        size_t n=host.size();
        string ans="";
        for(size_t i=0;i<n;i++){
            string temp=use_helper(host.substr(i),uri);
            if(ans.size()!=0&&temp.size()!=0)
                ans+=";";
            ans+=temp;
        }

        return ans;
    }
    void clear(){
        this->cookie_map.clear();
    }
private:
    void add_helper(string host,cookie_struct cs){

        if(cookie_map.find(host)==cookie_map.end()){

            cookie_map[host]=vector<cookie_struct>();
            if(cs.value.size()!=0)
                cookie_map[host].push_back(cs);

        }
        else{

            size_t j=0;

            while(j<cookie_map[host].size()){

                if(cookie_map[host][j].name==cs.name){
                    cookie_map[host].erase(j+cookie_map[host].begin());
                    break;
                }

                j++;
            }
            if(cs.value.size()!=0)
                cookie_map[host].push_back(cs);
        }
    }

    string use_helper(string host,string uri){
        string ans="";
        if(cookie_map.find(host)!=cookie_map.end()){
            vector<cookie_struct> vcst=cookie_map[host];
            for(size_t i=0;i<vcst.size();i++){
                size_t pos=0;
                if((pos=uri.find(vcst[i].path))!=uri.npos){
                    if(pos==0){
                        if(ans.size()!=0)
                            ans+=";";
                        ans=ans+vcst[i].name+"="+vcst[i].value;
                    }
                }
            }
        }
        return ans;
    }

    unordered_map<string,vector<cookie_struct>> cookie_map;
};

class response{
public:
    response(string host,string uri,string content,CookieJar *cookie):status_code(-1),
                                                    protocol(""),
                                                    host(host),
                                                    uri(uri),
                                                    content(""),
                                                    headers(unordered_map<string,string>())
    {
        if(content.size()!=0)
            parse(content,cookie);
    }
    int get_status_code() const{
        return this->status_code;
    }
    string get_content() const{
        return this->content;
    }
    string get_protocol() const{
        return this->protocol;
    }
    unordered_map<string,string> get_headers() const{
        return this->headers;
    };
private:
    void parse(string content,CookieJar *cookie){
        size_t pos=0;
        if((pos=content.find("\r\n\r\n"))==content.npos){
            cerr<<"get information error"<<endl;
            return;
        }
        string header=content.substr(0,pos);
        parse_header(header,cookie);
        this->content=content.substr(pos+4);
        return;
    }
    void parse_header(string header,CookieJar *cookie){
        vector<string> sheader=split(header,"\r\n");
        if(sheader.size()==0)
            return;
        string first_line=sheader[0];
        vector<string> fvec=split(first_line," ");
        if(fvec.size()<3){
            cerr<<"It's not http protocol."<<endl;
            return;
        }
        this->protocol=fvec[0];
        this->status_code=atoi(fvec[1].c_str());
        vector<cookie_struct> cookie_list;

        for(size_t i=1;i<sheader.size();i++){

            size_t header_pos=0;
            if((header_pos=sheader[i].find(":"))==sheader[i].npos){
                cerr<<"It's not http protocol."<<endl;
                return;
            }

            string header_key=sheader[i].substr(0,header_pos);
            string header_value=sheader[i].substr(header_pos+1);
            header_value=strip(header_value);

            if(header_key!="Set-Cookie"&&header_key!="Set-cookie"&&header_key!="set-cookie"){
                this->headers[header_key]=header_value;
            }else{
                vector<string> cookie_split=split(header_value,";");
                if(cookie_split.size()==0)
                    continue;
                cookie_struct cs;
                size_t kvpos=0;
                string name="";
                string value="";
                if((kvpos=cookie_split[0].find("="))!=cookie_split[0].npos){
                    name=strip(cookie_split[0].substr(0,kvpos));
                    value=strip(cookie_split[0].substr(kvpos+1));
                }

                cs.name=name;
                cs.value=value;

                for(size_t j=1;j<cookie_split.size();j++){
                    if((kvpos=cookie_split[j].find("="))!=cookie_split[j].npos){
                        name=strip(cookie_split[j].substr(0,kvpos));
                        value=strip(cookie_split[j].substr(kvpos+1));
                    }
                    if(name=="expires"||name=="Expires"){
                        cs.expires=value;
                    }else if(name=="path"||name=="Path"){
                        cs.path=value;
                    }else if(name=="domain"||name=="Domain"){
                        cs.domain=value;
                    }
                }
                if(cs.name.size()!=0)
                    cookie_list.push_back(cs);
            }
        }
        if(cookie!=NULL)
            cookie->add(this->host,cookie_list);
        return;
    }

    int status_code=-1;
    string protocol="";
    string host;
    string uri;
    string content="";
    unordered_map<string,string> headers;
};

class requests{
public:
    requests()= default;
    response get(string host,string uri,uint16_t port,
               unordered_map<string,string> headers=unordered_map<string,string>(),
               unordered_map<string,string> data=unordered_map<string,string>(),
               CookieJar *cookie=NULL,
               int timeout=3){
        int sockfd;
        if((sockfd=init(host,port))<0)
            return response(host,uri,"",cookie);

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

        sendmsg=make_header(sendmsg,host,uri,headers,cookie);

        response res=this->request_and_recv(host,uri,sockfd,sendmsg,cookie,timeout);
        close(sockfd);

        return res;
    }
    response post(string host,string uri,uint16_t port,
                unordered_map<string,string> headers=unordered_map<string,string>(),
                string json_data="",
                unordered_map<string,string> data=unordered_map<string,string>(),
                CookieJar *cookie=NULL,
                int timeout=3){
        int sockfd;
        if((sockfd=init(host,port))<0)
            return response(host,uri,"",cookie);;

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

        sendmsg=make_header(sendmsg,host,uri,headers,cookie);
        sendmsg+=post_data;

        response res=this->request_and_recv(host,uri,sockfd,sendmsg,cookie,timeout);
        close(sockfd);

        return res;
    }
private:
    response request_and_recv(string host,
                            string uri,
                            int sockfd,
                            string headers,
                            CookieJar *cookie,
                            int timeout){
        //request

        ssize_t write_count=0;



        if((write_count=write(sockfd,headers.c_str(),headers.size()))<0){
            cerr<<"write error"<<endl;
            return response(host,uri,"",cookie);
        }
        if(write_count!=headers.size()){
            cerr<<"write error"<<endl;
            return response(host,uri,"",cookie);
        }

        //for the Server down.
        if(errno==EPIPE){
            cerr<<"Server Shutdown.Please Try again."<<endl;
            return response(host,uri,"",cookie);
        }


        //poll
        struct pollfd clientfds[1];

        clientfds[0].fd=sockfd;
        clientfds[0].events=POLLIN;

        int nready=0;
        ssize_t read_count=0;

        char buf[8192]={'\0'};
        string ans="";
        while(true){
            nready=poll(clientfds,1,timeout*1000);
            if(nready==0)
                break;
            else if(nready==-1){
                cerr<<"poll error."<<endl;
                return response(host,uri,"",cookie);
            }
            if(clientfds[0].revents&POLLIN){
                read_count=read(sockfd,buf,sizeof(buf)-1);
                if(read_count<=0){
                    cerr<<"break here."<<endl;
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
            return response(host,uri,"",cookie);
        }

        //cout<<ans<<endl;
        //cout<<"-----------------------------"<<endl;

        return response(host,uri,ans,cookie);
    }

    //create the headers
    string make_header(string base_header,
                       string host,
                       string uri,
                       const unordered_map<string,string> &headers,
                       CookieJar *cookie){
        unordered_map<string,bool> base_usage{
                {"Connection",false},
                {"Accept",false},
        };
        unordered_map<string,string> base_values{
                {"Connection","keep-alive"},
                {"Accept","*/*"},
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

        if(cookie!=NULL){
            string c=cookie->use(host,uri);
            if(!c.empty()){
                base_header=base_header+"Cookie: "+c+"\r\n";
            }
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

        struct sockaddr_in addr;
        bzero(&addr,sizeof(addr));
        addr.sin_family=hptr->h_addrtype;
        addr.sin_port=htons(port);

        char ipLocation[32]={'\0'};
        inet_ntop(hptr->h_addrtype,hptr->h_addr,ipLocation,sizeof(ipLocation));
        cout<<ipLocation<<endl;
        inet_pton(hptr->h_addrtype,ipLocation,&addr.sin_addr);



        if(connect(sockfd,(const SA *)&addr,sizeof(addr))<0)
            return -1;
        return sockfd;
    }
};

int main() {
    requests r;
    CookieJar cj;
    /*r.post("192.168.1.242","/query",80,unordered_map<string,string>(),"",{
            {"fname","male"},
    });*/
    r.get("ids1.hfut.edu.cn","/amserver/UI/Login",80,{},{},&cj);
    response res=r.get("my.hfut.edu.cn","/",80,{},{},&cj);
    cout<<res.get_content()<<endl;
    return 0;
}
