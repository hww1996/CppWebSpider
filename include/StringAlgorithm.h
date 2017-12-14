#ifndef _STRING_ALGORITHM_H__
#define _STRING_ALGORITHM_H__

#include<vector>
#include<string>

using namespace std;
//除去target中左右有s字符串的地方
string strip(string target,string s=" ");
//split函数
vector<string> split(string target,string s);
//join函数
string join(vector<string> string_list,string s);

#endif