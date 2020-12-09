#pragma once

#include <iostream>
#include <memory>
#include "http_server.h"
#include "CJsonObject.hpp"
#include "charset.h"

/**
* @brief �����г�ĳ�ļ����µ��ļ���
*
* @param url          ����url
* @param body         post�������ַ���
* @param c            http����
* @param rsp_callback �������ݷ��ͻص�������http���Ӻͷ�����Ӧ���ַ���
*
* @return true        �ɹ�
* @return false      ʧ��
*/
bool handle_ls(std::string url, std::string body, mg_connection *c, OnRspCallback rsp_callback)
{
	using namespace std;
	using namespace neb;

	auto bodyw = Utf8ToAnsi(body); // ת��������
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
	int64 handle = _findfirst(path.c_str(), &file_info); //���ļ����Ҿ��
	if (-1 == handle) //����ֵΪ-1�����ʧ��
	{
		rsp_callback(c, wJson.ToString());
		return true;
	}
	int file_num = 0; // �ļ���
	do
	{
		if (file_info.name[0] == '.') {
			continue;
		}
		//�ж��Ƿ���Ŀ¼
		string attribute;
		string file_name;
		if (file_info.attrib == _A_SUBDIR) //��Ŀ¼
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
		//����ļ���Ϣ������,�ļ���(����׺)���ļ�����޸�ʱ�䡢�ļ��ֽ���(�ļ�����ʾ0)���ļ��Ƿ�Ŀ¼
		// cout << file_info.name << ' ' << file_info.time_write << ' ' << file_info.size << ' ' << attribute << endl; //��õ�����޸�ʱ����time_t��ʽ�ĳ����ͣ���Ҫ����������ת������ʱ����ʾ
		
		wJson["data"].Add(file_name);

	} while (!_findnext(handle, &file_info));  //����0�������
	//�ر��ļ����
	_findclose(handle);

	auto jsonw = AnsiToUtf8(wJson.ToString()); // ת��������
	rsp_callback(c, jsonw);

	return true;
}
