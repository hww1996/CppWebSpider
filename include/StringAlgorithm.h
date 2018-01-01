#ifndef _STRING_ALGORITHM_H__
#define _STRING_ALGORITHM_H__

#include<vector>
#include<string>

namespace CppWebSpider{
	//除去target中左右有s字符串的地方
	std::string strip(std::string target,std::string s=" ");
	//split函数
	std::vector<std::string> split(std::string target,std::string s);
	//join函数
	std::string join(std::vector<std::string> string_list,std::string s);
}


#endif