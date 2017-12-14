#include"CookieJar.h"
using namespace std;


void CookieJar::add(string host,vector<cookie_struct> cookie_list){
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

string CookieJar::use(string host,string uri){
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
void CookieJar::clear(){
    this->cookie_map.clear();
}
void CookieJar::add_helper(string host,cookie_struct cs){

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

string CookieJar::use_helper(string host,string uri){
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