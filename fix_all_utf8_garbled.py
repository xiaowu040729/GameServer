#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
修复所有UTF-8乱码（GBK被误读为UTF-8的情况）
"""

import os
import re

# 所有需要修复的乱码模式
fixes = [
    # NameGenerator.cpp
    (r'std::cout << "Redis杩炴帴鎴愬姛', 'std::cout << "Redis连接成功'),
    (r'// 浠嶳edis璇诲彇濮撴皬鍒楄〃', '// 从Redis读取姓氏列表'),
    (r'// 浠嶳edis璇诲彇鍚嶅瓧鍒楄〃', '// 从Redis读取名字列表'),
    (r'// 跳过第一行（说明文字）岋紙璇存槑鏂囧瓧锛', '// 跳过第一行（说明文字）'),
    (r'// 保存姓氏列表到Redis（使用RPUSH逐个添加）锛堜娇鐢≧PUSH閫愪釜娣诲姞锛', '// 保存姓氏列表到Redis（使用RPUSH逐个添加）'),
    (r'// 保存名字列表到Redis（使用RPUSH逐个添加）锛堜娇鐢≧PUSH閫愪釜娣诲姞锛', '// 保存名字列表到Redis（使用RPUSH逐个添加）'),
    (r'std::cout << "\[NameGenerator\] 鍒涘缓鏂板疄渚', 'std::cout << "[NameGenerator] 创建新实例'),
    (r'std::cerr << "鏃犳硶鍒嗛厤Redis杩炴帴', 'std::cerr << "无法分配Redis连接'),
    (r'std::cout << "\[NameGenerator\] 已经初始化锛岃烦杩囬噸澶嶅垵濮嬪寲', 'std::cout << "[NameGenerator] 已经初始化，跳过重复初始化'),
    (r'std::cout << "\[NameGenerator\] 初始化锛岄厤缃璺寰', 'std::cout << "[NameGenerator] 初始化，配置路径'),
    (r'std::cout << "浠嶳edis鎴愬姛鍔犺浇濮撳悕鏁版嵁', 'std::cout << "从Redis成功加载姓名数据'),
    (r'std::cout << "灏濊瘯浠庢枃浠跺姞杞藉撳悕鏁版嵁', 'std::cout << "尝试从文件加载姓名数据'),
    (r'std::cerr << "无法从文件加载姓名数据锛岃矾寰', 'std::cerr << "无法从文件加载姓名数据，路径'),
    (r'std::cout << "\[NameGenerator\] 初始化鎴愬姛', 'std::cout << "[NameGenerator] 初始化成功'),
    (r'std::cerr << "警告: 姓名数据为空锛岃繑鍥為粯璁ゅ \'test\'', 'std::cerr << "警告: 姓名数据为空，返回默认值 \'test\''),
    (r'std::cerr << "濮撴皬鏁伴噺', 'std::cerr << "姓氏数量'),
    (r'std::cerr << "鍚嶅瓧鏁伴噺', 'std::cerr << "名字数量'),
    (r'return "test";  // 如果数据未加载锛岃繑鍥為粯璁ゅ', 'return "test";  // 如果数据未加载，返回默认值'),
    (r' << ", 鍚嶅瓧绱㈠紩', ' << ", 名字索引'),
]

def fix_file(filepath):
    """修复单个文件"""
    if not os.path.exists(filepath):
        return False
    
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
        
        original = content
        
        # 应用所有修复
        for pattern, replacement in fixes:
            content = re.sub(pattern, replacement, content)
        
        # 清理重复的注释（如果修复后产生了重复）
        content = re.sub(r'/\*([^*]+)\*/[^*]*/\*([^*]+)\*/', r'/*\1\2*/', content)
        
        if content != original:
            with open(filepath, 'w', encoding='utf-8') as f:
                f.write(content)
            print(f"✓ 已修复: {filepath}")
            return True
        else:
            print(f"- 无需修复: {filepath}")
            return False
    except Exception as e:
        print(f"✗ 错误 {filepath}: {e}")
        return False

def main():
    files = [
        'GameServer/NameGenerator.cpp',
        'GameServer/NameGenerator.h',
    ]
    
    print("开始修复UTF-8乱码...\n")
    fixed = 0
    for f in files:
        if fix_file(f):
            fixed += 1
    
    print(f"\n完成! 修复了 {fixed} 个文件")

if __name__ == '__main__':
    main()

