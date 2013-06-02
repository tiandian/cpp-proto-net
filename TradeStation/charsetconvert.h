#pragma once

#include <string>


void UTF_8ToUnicode(wchar_t* pOut,char *pText);  // 把UTF-8转换成Unicode
void UnicodeToUTF_8(char* pOut,wchar_t* pText);  //Unicode 转换成UTF-8
void UnicodeToGB2312(char* pOut,wchar_t uData);  // 把Unicode 转换成 GB2312 
void Gb2312ToUnicode(wchar_t* pOut,char *gbBuffer);// GB2312 转换成　Unicode
void GB2312ToUTF_8(std::string& pOut, const char *gb2312);//GB2312 转为 UTF-8
void UTF_8ToGB2312(std::string &pOut, const char *utf8);//UTF-8 转为 GB2312
