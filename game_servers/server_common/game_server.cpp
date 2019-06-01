/* ======================================================
 * Author       : mknight
 * Last modified: 2019-05-29 20:55
 * Email        : 824338670@qq.com
 * Filename     : game_server.cpp
 * Description  : 
 * ======================================================*/

#include "game_server.h"
#include "server_config.h"
GameServer::GameServer()
{
}
GameServer::~GameServer()
{

}

bool GameServer::Init()
{
	return true;
}

// 运行前的检查
bool GameServer::RunStepCheck()
{
	TBaseServer::RunStepCheck();
	return true;
}
// 正常运行
bool GameServer::RunStepRunning()
{
	return TBaseServer::RunStepRunning();
}

// 新的连接建立了
void GameServer::OnNewConnectComeIn(TConnection * new_connection)
{
	TBaseServer::OnNewConnectComeIn(new_connection);
}