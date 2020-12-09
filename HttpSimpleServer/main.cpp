/*
 * Idea from https://github.com/tashaxing/CppHttpDemo
 * Idea from https://github.com/Bwar/CJsonObject
 */

#include <iostream>
#include <memory>
#include "http_server.h"
#include "ls_handler.h"

// 初始化HttpServer静态类成员
mg_serve_http_opts HttpServer::s_server_option;
std::string HttpServer::s_web_dir = "./web";
std::unordered_map<std::string, ReqHandler> HttpServer::s_handler_map;
std::unordered_set<mg_connection *> HttpServer::s_websocket_session_set;

int main(int argc, char *argv[])
{
	std::string port = (argc > 1) ? argv[1] : "7999"; // 传入启动端口，或默认7999

	auto http_server = std::shared_ptr<HttpServer>(new HttpServer);
	http_server->Init(port);
	// add handler
	http_server->AddHandler("/api/ls", handle_ls);
	http_server->Start();

	return 0;
}