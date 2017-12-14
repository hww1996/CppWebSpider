#include<iostream>
#include"response.h"
using namespace std;


void response::parse(string content,CookieJar *cookie){
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


void response::parse_header(string header,CookieJar *cookie){
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