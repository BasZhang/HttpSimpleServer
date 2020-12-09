#pragma once

#include <iostream>
#include <memory>
#include "http_server.h"
#include "CJsonObject.hpp"

// �����г�ĳ�ļ��е��ļ���
bool handle_ls(std::string url, std::string body, mg_connection *c, OnRspCallback rsp_callback)
{
	using namespace std;
	using namespace neb;

	CJsonObject rJson(body);

	string path;
	if (!rJson.Get("path", path)) {
		rsp_callback(c, "[]");
		return false;
	}
	if (path.length() == 0) {
		rsp_callback(c, "[]");
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

	_finddata64i32_t file_info;
	//���ļ����Ҿ��
	int64 handle = _findfirst(path.c_str(), &file_info);

	if (-1 == handle) //����ֵΪ-1�����ʧ��
	{
		rsp_callback(c, "[]");
		return false;
	}
	int file_num = 0; // �ļ���
	CJsonObject wJson;
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
		wJson.Add(file_name);

	} while (!_findnext(handle, &file_info));  //����0�������
	//�ر��ļ����
	_findclose(handle);

	rsp_callback(c, wJson.ToString());

	return true;
}
