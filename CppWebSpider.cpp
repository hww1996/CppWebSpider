#include <iostream>
#include<unistd.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<poll.h>
#include<cstring>
#include<unordered_map>
#include<vector>
using namespace std;

//除去target中左右有s字符串的地方
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
//split函数
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
//join函数
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
    void add(string host,vector<cookie_struct> cookie_list){
        size_t n=cookie_list.size();
        for(size_t i=0;i<n;i++){
			//判断是否有domain，假如没有的话就以当前的host为domain
            if(cookie_list[i].domain.empty()){
                add_helper(host,cookie_list[i]);
            }
            else{
                add_helper(cookie_list[i].domain,cookie_list[i]);
            }
        }
    }

	/*
		作用：返回当前的url能够使用的cookie，以name=value;name=value;...的方式
		参数：
			host	爬取的url的host
			uri		要爬取的资源
	*/
    string use(string host,string uri){
        size_t n=host.size();
        string ans="";
        for(size_t i=0;i<n;i++){
			//temp是从头开始将host减少，看其与那个cookie匹配的值；
            string temp=use_helper(host.substr(i),uri);
            if(ans.size()!=0&&temp.size()!=0)
                ans+=";";
            ans+=temp;
        }

        return ans;
    }
	
	//作用清除cookie，可以相当于将网页关闭，然后重新访问。
    void clear(){
        this->cookie_map.clear();
    }
private:
	/*
		作用：将cookie真正的添加到cookie_map里面
		参数：
			host 	爬取的url的host
			cs		想要添加到cookie_map的cookie
	*/
    void add_helper(string host,cookie_struct cs){
        if(cookie_map.find(host)==cookie_map.end()){

            cookie_map[host]=vector<cookie_struct>();
			//假如为空就不添加了
            if(cs.value.size()!=0)
                cookie_map[host].push_back(cs);

        }
        else{

            size_t j=0;
			//找到那个跟要添加的cookie重名的cookie，删除他，然后重新添加。
            while(j<cookie_map[host].size()){

                if(cookie_map[host][j].name==cs.name){
                    cookie_map[host].erase(j+cookie_map[host].begin());
                    break;
                }

                j++;
            }
			//假如value为空就不添加，相当于删除了这个cookie
            if(cs.value.size()!=0)
                cookie_map[host].push_back(cs);
        }
    }
	/*
		作用：根据当前的host和uri取出可用的cookie，并以以name=value;name=value;...的方式返回
		参数：
			host 	当前的host
			uri		当前的uri
	*/
    string use_helper(string host,string uri){
        string ans="";
        if(cookie_map.find(host)!=cookie_map.end()){
            vector<cookie_struct> vcst=cookie_map[host];
            for(size_t i=0;i<vcst.size();i++){
                size_t pos=0;
				//根据uri来查找匹配cookie中的path，看path是否为uri的前缀。
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
	//所有使用了这个CookieJar的cookie的保存容器
    unordered_map<string,vector<cookie_struct>> cookie_map;
};

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
    void parse(string content,CookieJar *cookie){
        size_t pos=0;
		//http的格式在正文和返回头之间的有一个\r\n
        if((pos=content.find("\r\n\r\n"))==content.npos){
            cerr<<"get information error"<<endl;
            return;
        }
        string header=content.substr(0,pos);
        parse_header(header,cookie);
        this->content=content.substr(pos+4);
        return;
    }
	/*
		作用：处理返回的http头部。
		参数：
			header	返回的http头部
			cookie	CookieJar的容器
	*/
    void parse_header(string header,CookieJar *cookie){
		//分行
        vector<string> sheader=split(header,"\r\n");
        if(sheader.size()==0)
            return;
        string first_line=sheader[0];
        vector<string> fvec=split(first_line," ");
		//假如第一行没有两空格以上的话，就为错误
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
			
			//假如不是Set-Cookie这种的话就全部丢进headers这个map里面，让操作者处理
            if(header_key!="Set-Cookie"&&header_key!="Set-cookie"&&header_key!="set-cookie"){
                this->headers[header_key]=header_value;
            }else{
				//cookie第一个一般为name=value，所以先split分号获得name=value这种东西。
                vector<string> cookie_split=split(header_value,";");
                if(cookie_split.size()==0)
                    continue;
                cookie_struct cs;
                size_t kvpos=0;
                string name="";
                string value="";
				//将name和value分出来
                if((kvpos=cookie_split[0].find("="))!=cookie_split[0].npos){
                    name=strip(cookie_split[0].substr(0,kvpos));
                    value=strip(cookie_split[0].substr(kvpos+1));
                }

                cs.name=name;
                cs.value=value;
				
                for(size_t j=1;j<cookie_split.size();j++){
					//将name和value分出来
                    if((kvpos=cookie_split[j].find("="))!=cookie_split[j].npos){
                        name=strip(cookie_split[j].substr(0,kvpos));
                        value=strip(cookie_split[j].substr(kvpos+1));
                    }
					//将支持的字段分出来
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
