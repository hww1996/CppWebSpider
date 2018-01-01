#include<iostream>
#include<cstring>
#include"request.h"
namespace CppWebSpider{
	response requests::get(std::string host,std::string uri,uint16_t port,
			   std::unordered_map<std::string,std::string> headers,
			   std::unordered_map<std::string,std::string> data,
			   CookieJar *cookie,
			   int timeout){
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
		std::string sendmsg=
				"GET "+uri+" HTTP/1.1\r\n"+
				"Host: "+ host+"\r\n";

		sendmsg=make_header(sendmsg,host,uri,headers,cookie);

		response res=this->request_and_recv(host,uri,sockfd,sendmsg,cookie,timeout);
		close(sockfd);

		return res;
	}
	response requests::post(std::string host,std::string uri,uint16_t port,
				std::unordered_map<std::string,std::string> headers,
				std::string json_data,
				std::unordered_map<std::string,std::string> data,
				CookieJar *cookie,
				int timeout){
		int sockfd;
		if((sockfd=init(host,port))<0)
			return response(host,uri,"",cookie);;

		std::string post_data="";
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
		headers.insert({"Content-Length",std::to_string(data_size)});
		headers.insert({"Content-Type","application/x-www-form-urlencoded"});

		//base header
		std::string sendmsg=
				"POST "+uri+" HTTP/1.1\r\n"+
				"Host: "+ host+"\r\n";

		sendmsg=make_header(sendmsg,host,uri,headers,cookie);
		sendmsg+=post_data;

		response res=this->request_and_recv(host,uri,sockfd,sendmsg,cookie,timeout);
		close(sockfd);

		return res;
	}
	response requests::request_and_recv(std::string host,
							std::string uri,
							int sockfd,
							std::string headers,
							CookieJar *cookie,
							int timeout){
		//request

		ssize_t write_count=0;



		if((write_count=write(sockfd,headers.c_str(),headers.size()))<0){
			std::cerr<<"write error"<<std::endl;
			return response(host,uri,"",cookie);
		}
		if(write_count!=headers.size()){
			std::cerr<<"write error"<<std::endl;
			return response(host,uri,"",cookie);
		}

		//for the Server down.
		if(errno==EPIPE){
			std::cerr<<"Server Shutdown.Please Try again."<<std::endl;
			return response(host,uri,"",cookie);
		}


		//poll
		struct pollfd clientfds[1];

		clientfds[0].fd=sockfd;
		clientfds[0].events=POLLIN;

		int nready=0;
		ssize_t read_count=0;

		char buf[8192]={'\0'};
		std::string ans="";
		while(true){
			nready=poll(clientfds,1,timeout*1000);
			if(nready==0)
				break;
			else if(nready==-1){
				std::cerr<<"poll error."<<std::endl;
				return response(host,uri,"",cookie);
			}
			if(clientfds[0].revents&POLLIN){
				read_count=read(sockfd,buf,sizeof(buf)-1);
				if(read_count<=0){
					std::cerr<<"break here."<<std::endl;
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
			std::cerr<<"read error."<<std::endl;
			return response(host,uri,"",cookie);
		}

		//cout<<ans<<endl;
		//cout<<"-----------------------------"<<endl;

		return response(host,uri,ans,cookie);
	}

							
	std::string requests::make_header(std::string base_header,
					   std::string host,
					   std::string uri,
					   const std::unordered_map<std::string,std::string> &headers,
					   CookieJar *cookie){
		std::unordered_map<std::string,bool> base_usage{
				{"Connection",false},
				{"Accept",false},
		};
		std::unordered_map<std::string,std::string> base_values{
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
			std::string c=cookie->use(host,uri);
			if(!c.empty()){
				base_header=base_header+"Cookie: "+c+"\r\n";
			}
		}

		base_header+="\r\n";

		return base_header;

	}


	typedef struct sockaddr SA;
	int requests::init(std::string host,uint16_t port){
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
		std::cout<<ipLocation<<std::endl;
		inet_pton(hptr->h_addrtype,ipLocation,&addr.sin_addr);



		if(connect(sockfd,(const SA *)&addr,sizeof(addr))<0)
			return -1;
		return sockfd;
	}
}
