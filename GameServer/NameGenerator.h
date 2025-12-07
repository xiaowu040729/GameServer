#pragma once
#include <string>
#include <vector>
#include <random>
#include <hiredis/hiredis.h>
#include <fstream>
#include <sstream>

class NameGenerator {
private:
    static NameGenerator* instance;
    redisContext* redis_ctx;
    std::vector<std::string> first_names;  // 姓氏列表
    std::vector<std::string> last_names;    // 名字列表
    std::random_device rd;
    std::mt19937 gen;
    bool redis_connected;
    bool initialized;
    std::string config_path;
    
    NameGenerator();
    ~NameGenerator();
    
    // 连接Redis
    bool ConnectRedis(const char* host = "127.0.0.1", int port = 6379);
    
    // 从Redis读取姓名列表
    bool LoadNamesFromRedis();
    
    // 从文件读取姓名列表
    bool LoadNamesFromFile();
    
    // 将姓名列表保存到Redis
    bool SaveNamesToRedis();
    
public:
    // 单例模式
    static NameGenerator* GetInstance();
    static void DestroyInstance();
    
    // 生成随机姓名
    std::string GenerateRandomName();
    
    // 初始化（从Redis或文件加载）
    bool Initialize(const std::string& config_dir = "../config");
    
    // 检查Redis连接状态
    bool IsRedisConnected() const { return redis_connected; }
};

