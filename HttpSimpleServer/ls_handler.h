#pragma once

#include <iostream>
#include <memory>
#include "http_server.h"
#include "CJsonObject.hpp"
#include "charset.h"

/**
* @brief 处理列出某文件夹下的文件名
*
* @param url          请求url
* @param body         post请求体字符串
* @param c            http连接
* @param rsp_callback 返回数据发送回调，传入http连接和返回响应体字符串
*
* @return true        成功
* @return false      失败
*/
bool handle_ls(std::string url, std::string body, mg_connection *c, OnRspCallback rsp_callback)
{
	using namespace std;
	using namespace neb;

	auto bodyw = Utf8ToAnsi(body); // 转中文乱码
	CJsonObject rJson(bodyw);

	CJsonObject wJson("{}");
	wJson.ReplaceAdd("success", false);

	string path;
	if (!rJson.Get("path", path)) {
		wJson.ReplaceAdd("msg", "required param 'path' is not found");
		rsp_callback(c, wJson.ToString());
		return false;
	}
	if (path.length() == 0) {
		wJson.ReplaceAdd("msg", "request param 'path' error");
		rsp_callback(c, wJson.ToString());
		return false;
	}
	if (path.find_last_of("/", 0) == path.length() - 1 || path.find_last_of("\\", 0) == path.length() - 1)
	{
		path = path + "*";
	}
	else
	{
		path = path + "/*";
	}



	wJson.ReplaceAdd("success", true);
	wJson.AddEmptySubArray("data");

	_finddata64i32_t file_info;
	int64 handle = _findfirst(path.c_str(), &file_info); //打开文件查找句柄
	if (-1 == handle) //返回值为-1则查找失败
	{
		rsp_callback(c, wJson.ToString());
		return true;
	}
	int file_num = 0; // 文件数
	do
	{
		if (file_info.name[0] == '.') {
			continue;
		}
		//判断是否子目录
		string attribute;
		string file_name;
		if (file_info.attrib == _A_SUBDIR) //是目录
		{
			file_name = file_name.assign(file_info.name).append("/");
			attribute = "dir";
		}
		else
		{
			file_name = file_name.assign(file_info.name);
			attribute = "file";
			file_num++;
		}
		//输出文件信息并计数,文件名(带后缀)、文件最后修改时间、文件字节数(文件夹显示0)、文件是否目录
		// cout << file_info.name << ' ' << file_info.time_write << ' ' << file_info.size << ' ' << attribute << endl; //获得的最后修改时间是time_t格式的长整型，需要用其他方法转成正常时间显示
		
		wJson["data"].Add(file_name);

	} while (!_findnext(handle, &file_info));  //返回0则遍历完
	//关闭文件句柄
	_findclose(handle);

	auto jsonw = AnsiToUtf8(wJson.ToString()); // 转中文乱码
	rsp_callback(c, jsonw);

	return true;
}
