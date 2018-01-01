#include<cstring>
#include"StringAlgorithm.h"

namespace CppWebSpider{
	std::string strip(std::string target,std::string s){
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
		std::string ans=target.substr(start,end-start);
		return ans;
	}

	std::vector<std::string> split(std::string target,std::string s){
		size_t pos=0,n=s.size();
		std::vector<std::string> string_list;
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

	std::string join(std::vector<std::string> string_list,std::string s){
		size_t n=string_list.size();
		if(n==0)
			return "";

		std::string ans=string_list[0];
		for(size_t i=1;i<n;i++){
			ans=ans+s+string_list[i];
		}

		return ans;
	}
}