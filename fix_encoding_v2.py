#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
批量修复文件编码 - 直接使用GBK读取并转换为UTF-8
"""

import os
import sys
import glob

def convert_file_gbk_to_utf8(file_path):
    """尝试用GBK读取文件并转换为UTF-8"""
    try:
        # 方法1: 尝试GBK编码
        try:
            with open(file_path, 'r', encoding='gbk', errors='replace') as f:
                content = f.read()
            
            # 检查是否包含乱码替换字符
            if '\ufffd' in content:
                print(f"  {file_path}: GBK读取包含替换字符，尝试其他方法")
                return False
            
            # 保存为UTF-8
            with open(file_path, 'w', encoding='utf-8') as f:
                f.write(content)
            print(f"✓ {file_path}: GBK -> UTF-8 转换成功")
            return True
        except:
            pass
        
        # 方法2: 尝试GB2312
        try:
            with open(file_path, 'r', encoding='gb2312', errors='replace') as f:
                content = f.read()
            
            if '\ufffd' not in content:
                with open(file_path, 'w', encoding='utf-8') as f:
                    f.write(content)
                print(f"✓ {file_path}: GB2312 -> UTF-8 转换成功")
                return True
        except:
            pass
        
        # 方法3: 尝试UTF-8（可能已经是UTF-8但注释是乱码）
        try:
            with open(file_path, 'r', encoding='utf-8', errors='replace') as f:
                content = f.read()
            
            # 如果已经是UTF-8，检查是否有明显的GBK乱码模式
            # GBK乱码通常显示为类似 "" 的字符
            if '' in content or '' in content:
                # 尝试用latin1读取然后转换
                with open(file_path, 'rb') as f:
                    raw = f.read()
                try:
                    content = raw.decode('gbk')
                    with open(file_path, 'w', encoding='utf-8') as f:
                        f.write(content)
                    print(f"✓ {file_path}: 二进制GBK -> UTF-8 转换成功")
                    return True
                except:
                    pass
            
            print(f"- {file_path}: 已经是UTF-8或无法确定编码")
            return False
        except:
            pass
        
        return False
    except Exception as e:
        print(f"✗ {file_path}: 错误 - {e}")
        return False

def main():
    game_server_dir = 'GameServer'
    
    if not os.path.exists(game_server_dir):
        print(f"错误: 找不到目录 {game_server_dir}")
        return
    
    # 获取所有C++源文件
    files = []
    for ext in ['*.cpp', '*.h', '*.cc']:
        files.extend(glob.glob(os.path.join(game_server_dir, ext)))
    
    print(f"找到 {len(files)} 个文件")
    print("开始转换编码...\n")
    
    fixed_count = 0
    for file_path in sorted(files):
        if convert_file_gbk_to_utf8(file_path):
            fixed_count += 1
    
    print(f"\n完成! 转换了 {fixed_count} 个文件")

if __name__ == '__main__':
    main()

