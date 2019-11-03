/*=======================================================
# Author       : mknight
# Last modified: 2019-11-03 16:16
# Email        : 824338670@qq.com
# Filename     : server_describe.cpp
# Description  : 
# =======================================================*/
#include "server_describe.h"
#include <fstream>
#include "tr_common/func_tools.h"
#include "tr_log/log_module.h"

ServerDes::ServerDes()
{

}

ServerDes::~ServerDes()
{
    
}

void ServerDes::Load()
{
    // server_des.json
    {
        Json::Reader reader;
        std::ifstream fs;
        Json::Value server_des_jv;
        fs.open("server_des.json", std::ios::binary);
        if (!fs.is_open())
        {
            TERROR("open server_des.json failed");
            return;
        }
        if (!reader.parse(fs, server_des_jv, false))
        {
            fs.close();
            TERROR("parse server_des.json failed");
            return;
        }
        TINFO("parce server_des.json success")
        if (server_des_jv.isMember("node_des"))
        {
            auto &node_des_jv = server_des_jv["node_des"];
            for (int32_t i = 0; i < node_des_jv.size(); ++i)
            {
                int32_t node_type = node_des_jv[i]["node_type"].asInt();
                std::string node_name = node_des_jv[i]["node_name"].asString();
                node_type_des_[node_type] = node_name;
            }
        }
        fs.close();
    }
    {
        // protocol_des.json
        Json::Reader reader;
        std::ifstream fs;
        Json::Value protocol_des_jv;
        fs.open("protocol_des.json", std::ios::binary);
        if (!fs.is_open())
        {
            TERROR("protocol_des.json failed");
            return;
        }
        if (!reader.parse(fs, protocol_des_jv, false))
        {
            fs.close();
            TERROR("parse protocol_des.json failed");
            return;
        }
        TINFO("parce protocol_des.json success")
        if (protocol_des_jv.isMember("proto_class_des"))
        {
            auto &msg_class_jv = protocol_des_jv["proto_class_des"];
            for (int32_t i = 0; i < msg_class_jv.size(); ++i)
            {
                int32_t msg_class = msg_class_jv[i]["msg_class"].asInt();
                std::string msg_class_name = msg_class_jv[i]["class_des"].asString();
                msg_class_des_[msg_class] = msg_class_name;
                // msg types
                auto & msg_type_jv = msg_class_jv[i]["msg_type"];
                for (int32_t j = 0; j < msg_type_jv.size(); ++j)
                {
                    int32_t msg_type = msg_type_jv[j]["type"].asInt();
                    std::string type_name = msg_type_jv[j]["des"].asString();
                    msg_type_des_[msg_class][msg_type] = type_name;
                }
            }
        }
        fs.close();
    }
}

std::string ServerDes::GetServerNodeName(int32_t route_node_type)
{
    std::string des_str = "unknow node name";
    auto it_node = node_type_des_.find(route_node_type);
    if (it_node != node_type_des_.end())
    {
        des_str = it_node->second;
    }
    return des_str;
}

std::string ServerDes::GetMsgClassName(int32_t msg_class)
{
    std::string des_str = "unknow msg_class name";
    auto it = msg_class_des_.find(msg_class);
    if (it != msg_class_des_.end())
    {
        des_str = it->second;
    }
    return des_str;
}

std::string ServerDes::GetMsgTypeName(int32_t msg_class, int32_t msg_type)
{
    std::string des_str = "unknow msg_type name";
    auto it = msg_type_des_.find(msg_class);
    if (it != msg_type_des_.end())
    {
        auto it_type = it->second.find(msg_type);
        if (it_type != it->second.end())
        {
            des_str = it_type->second;
        }
    }
    return des_str;
}