#include "NameGenerator.h"
#include <iostream>
#include <algorithm>
#include <cstring>

// 静态成员初始化
NameGenerator* NameGenerator::instance = nullptr;

NameGenerator::NameGenerator() : gen(rd()), redis_connected(false), redis_ctx(nullptr), initialized(false) {
    config_path = "../config";
}

NameGenerator::~NameGenerator() {
    if (redis_ctx != nullptr) {
        redisFree(redis_ctx);
        redis_ctx = nullptr;
    }
}

NameGenerator* NameGenerator::GetInstance() {
    if (instance == nullptr) {
        instance = new NameGenerator();
        std::cout << "[NameGenerator] 创建新实例" << std::endl;
    }
    return instance;
}

void NameGenerator::DestroyInstance() {
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}

bool NameGenerator::ConnectRedis(const char* host, int port) {
    if (redis_ctx != nullptr) {
        redisFree(redis_ctx);
    }
    
    redis_ctx = redisConnect(host, port);
    if (redis_ctx == nullptr || redis_ctx->err) {
        if (redis_ctx != nullptr) {
            std::cerr << "Redis连接错误: " << redis_ctx->errstr << std::endl;
            redisFree(redis_ctx);
            redis_ctx = nullptr;
        } else {
            std::cerr << "无法分配Redis连接" << std::endl;
        }
        redis_connected = false;
        return false;
    }
    
    redis_connected = true;
    std::cout << "Redis连接成功: " << host << ":" << port << std::endl;
    return true;
}

bool NameGenerator::LoadNamesFromRedis() {
    if (!redis_connected || redis_ctx == nullptr) {
        return false;
    }
    
    first_names.clear();
    last_names.clear();
    
    // 从Redis读取姓氏列表
    redisReply* reply = (redisReply*)redisCommand(redis_ctx, "LRANGE first_names 0 -1");
    if (reply != nullptr && reply->type == REDIS_REPLY_ARRAY) {
        for (size_t i = 0; i < reply->elements; i++) {
            if (reply->element[i]->type == REDIS_REPLY_STRING) {
                first_names.push_back(std::string(reply->element[i]->str, reply->element[i]->len));
            }
        }
        freeReplyObject(reply);
    } else {
        if (reply != nullptr) freeReplyObject(reply);
        return false;
    }
    
    // 从Redis读取名字列表
    reply = (redisReply*)redisCommand(redis_ctx, "LRANGE last_names 0 -1");
    if (reply != nullptr && reply->type == REDIS_REPLY_ARRAY) {
        for (size_t i = 0; i < reply->elements; i++) {
            if (reply->element[i]->type == REDIS_REPLY_STRING) {
                last_names.push_back(std::string(reply->element[i]->str, reply->element[i]->len));
            }
        }
        freeReplyObject(reply);
    } else {
        if (reply != nullptr) freeReplyObject(reply);
        return false;
    }
    
    if (first_names.empty() || last_names.empty()) {
        return false;
    }
    
    std::cout << "从Redis加载姓名数据: 姓氏 " << first_names.size() 
              << " 个, 名字 " << last_names.size() << " 个" << std::endl;
    return true;
}

bool NameGenerator::LoadNamesFromFile() {
    first_names.clear();
    last_names.clear();
    
    // 读取姓氏文件
    std::string first_name_file = config_path + "/first_name.txt";
    std::ifstream first_file(first_name_file);
    if (!first_file.is_open()) {
        std::cerr << "无法打开文件: " << first_name_file << std::endl;
        return false;
    }
    
    std::string line;
    while (std::getline(first_file, line)) {
        // 去除首尾空白字符
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        if (!line.empty()) {
            first_names.push_back(line);
        }
    }
    first_file.close();
    
    // 读取名字文件
    std::string last_name_file = config_path + "/last_name.txt";
    std::ifstream last_file(last_name_file);
    if (!last_file.is_open()) {
        std::cerr << "无法打开文件: " << last_name_file << std::endl;
        return false;
    }
    
    // 跳过第一行（说明文字）
    std::getline(last_file, line);
    
    while (std::getline(last_file, line)) {
        // 去除首尾空白字符
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        if (!line.empty()) {
            last_names.push_back(line);
        }
    }
    last_file.close();
    
    if (first_names.empty() || last_names.empty()) {
        std::cerr << "姓名文件为空或读取失败" << std::endl;
        return false;
    }
    
    std::cout << "从文件加载姓名数据: 姓氏 " << first_names.size() 
              << " 个, 名字 " << last_names.size() << " 个" << std::endl;
    return true;
}

bool NameGenerator::SaveNamesToRedis() {
    if (!redis_connected || redis_ctx == nullptr) {
        return false;
    }
    
    // 清空Redis中的旧数据
    redisReply* reply = (redisReply*)redisCommand(redis_ctx, "DEL first_names last_names");
    if (reply != nullptr) {
        freeReplyObject(reply);
    }
    
    // 保存姓氏列表到Redis（使用RPUSH逐个添加）
    if (!first_names.empty()) {
        for (const auto& name : first_names) {
            reply = (redisReply*)redisCommand(redis_ctx, "RPUSH first_names %s", name.c_str());
            if (reply != nullptr) {
                freeReplyObject(reply);
            }
        }
    }
    
    // 保存名字列表到Redis（使用RPUSH逐个添加）
    if (!last_names.empty()) {
        for (const auto& name : last_names) {
            reply = (redisReply*)redisCommand(redis_ctx, "RPUSH last_names %s", name.c_str());
            if (reply != nullptr) {
                freeReplyObject(reply);
            }
        }
    }
    
    std::cout << "姓名数据宸蹭繚瀛樺埌Redis" << std::endl;
    return true;
}

bool NameGenerator::Initialize(const std::string& config_dir) {
    // 濡傛灉已经初始化囷紝鐩存帴斿洖
    if (initialized && !first_names.empty() && !last_names.empty()) {
        std::cout << "[NameGenerator] 已经初始化，跳过重复初始化" << std::endl;
        return true;
    }
    
    config_path = config_dir;
    
    std::cout << "[NameGenerator] 初始化，配置路径: " << config_path << std::endl;
    
    // 尝试连接Redis
    ConnectRedis();
    
    // 尝试从嶳edis加载
    if (redis_connected && LoadNamesFromRedis()) {
        std::cout << "从Redis成功加载姓名数据" << std::endl;
        return true;
    }
    
    // 濡傛灉Redis加载失败，屼粠文件加载
    std::cout << "尝试从文件加载姓名数据..." << std::endl;
    if (!LoadNamesFromFile()) {
        std::cerr << "无法从文件加载姓名数据，路径: " << config_path << std::endl;
        return false;
    }
    
    // 濡傛灉Redis连接成功，屽皢数据保存到癛edis
    if (redis_connected) {
        SaveNamesToRedis();
    }
    
    std::cout << "[NameGenerator] 初始化成功，屽撴皬数量: " << first_names.size() 
              << ", 名字数量: " << last_names.size() << std::endl;
    initialized = true;
    return true;
}

std::string NameGenerator::GenerateRandomName() {
    if (first_names.empty() || last_names.empty()) {
        std::cerr << "警告: 姓名数据为空，返回默认值 'test'" << std::endl;
        std::cerr << "姓氏数量: " << first_names.size() << ", 名字数量: " << last_names.size() << std::endl;
        std::cerr.flush();
        return "test";  // 如果数据未加载，返回默认值
    }
    
    // 闅忔満閫夋嫨姓氏鍜屽悕瀛
    std::uniform_int_distribution<> first_dist(0, first_names.size() - 1);
    std::uniform_int_distribution<> last_dist(0, last_names.size() - 1);
    
    int first_idx = first_dist(gen);
    int last_idx = last_dist(gen);
    
    std::string name = first_names[first_idx] + last_names[last_idx];
    std::cout << "鐢熸垚闅忔満姓名: " << name << " (姓氏索引: " << first_idx 
              << ", 名字索引: " << last_idx << ")" << std::endl;
    std::cout.flush();
    return name;
}

