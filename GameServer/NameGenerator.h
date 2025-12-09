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
    std::vector<std::string> first_names;  // 姓氏到楄〃
    std::vector<std::string> last_names;    // 名字到楄〃
    std::random_device rd;
    std::mt19937 gen;
    bool redis_connected;
    bool initialized;
    std::string config_path;
    
    NameGenerator();
    ~NameGenerator();
    
    // 连接Redis
    bool ConnectRedis(const char* host = "127.0.0.1", int port = 6379);
    
    // 从嶳edis读取姓名到楄〃
    bool LoadNamesFromRedis();
    
    // 从庢枃从惰诲彇姓名到楄〃
    bool LoadNamesFromFile();
    
    // 灏嗗撳悕到楄〃保存到癛edis
    bool SaveNamesToRedis();
    
public:
    // 鍗曚緥妯″紡
    static NameGenerator* GetInstance();
    static void DestroyInstance();
    
    // 鐢熸垚闅忔満姓名
    std::string GenerateRandomName();
    
    // 初始化，堜粠Redis或栨枃从跺姞斤級
    bool Initialize(const std::string& config_dir = "../config");
    
    // 妫鏌Redis连接鐘舵
    bool IsRedisConnected() const { return redis_connected; }
};

