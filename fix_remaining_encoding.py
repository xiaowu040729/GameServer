#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
修复剩余文件中的UTF-8乱码
"""

import os
import re

def fix_file(filepath, fixes):
    """修复单个文件"""
    if not os.path.exists(filepath):
        return False
    
    try:
        # 读取文件（处理BOM）
        with open(filepath, 'rb') as f:
            raw = f.read()
        
        # 移除BOM
        if raw.startswith(b'\xef\xbb\xbf'):
            raw = raw[3:]
        
        # 尝试UTF-8解码
        try:
            content = raw.decode('utf-8')
        except:
            # 如果失败，尝试GBK
            content = raw.decode('gbk', errors='ignore')
        
        original = content
        
        # 应用修复
        for pattern, replacement in fixes:
            content = re.sub(pattern, replacement, content)
        
        if content != original:
            # 保存为UTF-8（无BOM）
            with open(filepath, 'w', encoding='utf-8', newline='') as f:
                f.write(content)
            print(f"✓ 已修复: {filepath}")
            return True
        else:
            print(f"- 无需修复: {filepath}")
            return False
    except Exception as e:
        print(f"✗ 错误 {filepath}: {e}")
        return False

# main.cpp的修复
main_fixes = [
    (r'锘#include', '#include'),
    (r'// 閫氳繃 Player 缁ф壙', '// 通过 Player 继承'),
    (r'/\*鍒濆嬪寲濮撳悕鐢熸垚鍣', '/*初始化姓名生成器（从Redis或文件加载姓名数据）*/'),
    (r'// 浣跨敤鐩稿逛簬鍙鎵ц屾枃浠剁殑璺寰', '// 使用相对于可执行文件的路径，或者绝对路径'),
    (r'// 鍋囪惧彲鎵ц屾枃浠跺湪GameServer鐩褰', '// 假设可执行文件在GameServer目录，config在项目根目录'),
    (r'// 濡傛灉浠庨」鐩鏍圭洰褰曡繍琛', '// 如果从项目根目录运行，使用相对路径'),
    (r'// 濡傛灉浠嶨ameServer鐩褰曡繍琛', '// 如果从GameServer目录运行，使用../config'),
    (r'std::cerr << "濮撳悕鐢熸垚鍣ㄥ垵濮嬪寲澶辫触', 'std::cerr << "姓名生成器初始化失败，将使用默认姓名'),
    (r'std::cerr << "灏濊瘯鐨勮矾寰', 'std::cerr << "尝试的路径'),
    (r'/\*鍒濆嬪寲妗嗘灦\*/', '/*初始化框架*/'),
    (r'/\*娣诲姞鐩戝惉閫氶亾锛岀洃鍚瑃cp璇锋眰\*/', '/*添加监听通道，监听tcp请求*/'),
    (r'/\*杩愯屾嗘灦\*/', '/*运行框架*/'),
    (r'/\*娑堥櫎鍒濆嬪寲\*/', '/*消除初始化*/'),
    (r'/\*娓呯悊濮撳悕鐢熸垚鍣\*/', '/*清理姓名生成器*/'),
]

# NameGenerator.cpp的修复
namegen_fixes = [
    (r'std::cout << "浠嶳edis鍔犺浇濮撳悕鏁版嵁: 濮撴皬 "', 'std::cout << "从Redis加载姓名数据: 姓氏 "'),
    (r' << " 涓, 鍚嶅瓧 "', ' << " 个, 名字 "'),
    (r' << " 涓" << std::endl', ' << " 个" << std::endl'),
    (r'// 璇诲彇濮撴皬鏂囦欢', '// 读取姓氏文件'),
    (r'std::cerr << "鏃犳硶鎵撳紑鏂囦欢: "', 'std::cerr << "无法打开文件: "'),
    (r'// 鍘婚櫎棣栧熬绌虹櫧瀛楃', '// 去除首尾空白字符'),
    (r'// 璇诲彇鍚嶅瓧鏂囦欢', '// 读取名字文件'),
    (r'// 璺宠繃绗涓琛', '// 跳过第一行（说明文字）'),
    (r'std::cerr << "濮撳悕鏂囦欢涓虹┖鎴栬诲彇澶辫触"', 'std::cerr << "姓名文件为空或读取失败"'),
    (r'std::cout << "浠庢枃浠跺姞杞藉撳悕鏁版嵁: 濮撴皬 "', 'std::cout << "从文件加载姓名数据: 姓氏 "'),
    (r'// 娓呯┖Redis涓鐨勬棫鏁版嵁', '// 清空Redis中的旧数据'),
    (r'// 淇濆瓨濮撴皬鍒楄〃鍒癛edis', '// 保存姓氏列表到Redis（使用RPUSH逐个添加）'),
    (r'// 淇濆瓨鍚嶅瓧鍒楄〃鍒癛edis', '// 保存名字列表到Redis（使用RPUSH逐个添加）'),
    (r'std::cout << "\[NameGenerator\] 宸茬粡鍒濆嬪寲', 'std::cout << "[NameGenerator] 已经初始化'),
    (r'std::cout << "\[NameGenerator\] 鍒濆嬪寲', 'std::cout << "[NameGenerator] 初始化'),
    (r'std::cerr << "鏃犳硶浠庢枃浠跺姞杞藉撳悕鏁版嵁', 'std::cerr << "无法从文件加载姓名数据'),
    (r'std::cout << "\[NameGenerator\] 鍒濆嬪寲鎴愬姛', 'std::cout << "[NameGenerator] 初始化成功'),
    (r'std::cerr << "璀﹀憡: 濮撳悕鏁版嵁涓虹┖', 'std::cerr << "警告: 姓名数据为空'),
    (r'// 濡傛灉鏁版嵁鏈鍔犺浇', '// 如果数据未加载'),
]

# AOI.h的修复
aoi_h_fixes = [
    (r'/\*鐜╁\*/', '/*玩家*/'),
    (r'/\*杩斿洖鍧愭爣浣嶇疆\*/', '/*返回坐标位置*/'),
    (r'/\*涓涓鏍煎瓙閲屽彲鑳芥湁澶氫釜鐜╁\*/', '/*一个格子里可能有多个玩家*/'),
    (r'/\*Area Of Intrest绠楁硶\*/', '/*Area Of Interest算法*/'),
    (r'/\*鍙浠ョ湅鍋氫竴鍧楀湴鍥\*/', '/*可以看作一块地图*/'),
    (r'/\*鍦板浘鐨勭浉鍏崇粏鑺\*/', '/*地图的相关细节*/'),
    (r'/\*涓鍧楀湴鍥鹃噷鏈夊氫釜鏍煎瓙\*/', '/*一块地图里有多个格子*/'),
    (r'/\*娣诲姞鐜╁\*/', '/*添加玩家*/'),
    (r'/\*鍒犻櫎鐜╁\*/', '/*删除玩家*/'),
    (r'/\*鑾峰彇鍛ㄥ洿鐜╁\*/', '/*获取周围玩家*/'),
]

# NameGenerator.h的修复
namegen_h_fixes = [
    (r'// 鍒濆嬪寲', '// 初始化'),
]

def main():
    files_to_fix = [
        ('GameServer/main.cpp', main_fixes),
        ('GameServer/NameGenerator.cpp', namegen_fixes),
        ('GameServer/NameGenerator.h', namegen_h_fixes),
        ('GameServer/AOI.h', aoi_h_fixes),
    ]
    
    print("开始修复剩余文件的乱码...\n")
    fixed = 0
    for filepath, fixes in files_to_fix:
        if fix_file(filepath, fixes):
            fixed += 1
    
    print(f"\n完成! 修复了 {fixed} 个文件")

if __name__ == '__main__':
    main()

