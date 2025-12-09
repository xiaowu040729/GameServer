#!/bin/bash

# 游戏服务器停止脚本
# 作者: Auto-generated
# 用途: 停止游戏服务器

# 设置颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 项目根目录
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PID_FILE="$PROJECT_ROOT/server.pid"
PORT=8899

# 打印带颜色的消息
print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# 通过PID文件停止服务器
stop_by_pid_file() {
    if [ ! -f "$PID_FILE" ]; then
        return 1
    fi
    
    PID=$(cat "$PID_FILE")
    
    if ! ps -p "$PID" > /dev/null 2>&1; then
        print_warn "PID文件中的进程 (PID: $PID) 不存在，清理PID文件"
        rm -f "$PID_FILE"
        return 1
    fi
    
    print_info "找到服务器进程 (PID: $PID)"
    print_info "正在停止服务器..."
    
    # 发送TERM信号
    kill "$PID" 2>/dev/null
    
    # 等待进程结束（最多等待10秒）
    for i in {1..10}; do
        if ! ps -p "$PID" > /dev/null 2>&1; then
            print_info "服务器已正常停止"
            rm -f "$PID_FILE"
            return 0
        fi
        sleep 1
        echo -n "."
    done
    echo ""
    
    # 如果还在运行，强制杀死
    if ps -p "$PID" > /dev/null 2>&1; then
        print_warn "进程未在10秒内退出，强制终止..."
        kill -9 "$PID" 2>/dev/null
        sleep 1
        
        if ! ps -p "$PID" > /dev/null 2>&1; then
            print_info "服务器已强制停止"
            rm -f "$PID_FILE"
            return 0
        else
            print_error "无法停止进程 (PID: $PID)"
            return 1
        fi
    fi
    
    rm -f "$PID_FILE"
    return 0
}

# 通过进程名停止服务器
stop_by_process_name() {
    # 查找GameServer进程
    PIDS=$(pgrep -f "GameServer" 2>/dev/null)
    
    if [ -z "$PIDS" ]; then
        return 1
    fi
    
    print_info "找到以下GameServer进程: $PIDS"
    
    for PID in $PIDS; do
        print_info "正在停止进程 (PID: $PID)..."
        
        # 发送TERM信号
        kill "$PID" 2>/dev/null
        
        # 等待进程结束
        for i in {1..5}; do
            if ! ps -p "$PID" > /dev/null 2>&1; then
                print_info "进程 (PID: $PID) 已停止"
                break
            fi
            sleep 1
        done
        
        # 如果还在运行，强制杀死
        if ps -p "$PID" > /dev/null 2>&1; then
            print_warn "强制终止进程 (PID: $PID)..."
            kill -9 "$PID" 2>/dev/null
            sleep 1
            
            if ! ps -p "$PID" > /dev/null 2>&1; then
                print_info "进程 (PID: $PID) 已强制停止"
            else
                print_error "无法停止进程 (PID: $PID)"
            fi
        fi
    done
    
    # 清理PID文件
    rm -f "$PID_FILE"
    return 0
}

# 通过端口停止服务器
stop_by_port() {
    if ! command -v lsof &> /dev/null && ! command -v fuser &> /dev/null && ! command -v netstat &> /dev/null && ! command -v ss &> /dev/null; then
        return 1
    fi
    
    PID=""
    
    # 尝试使用lsof
    if command -v lsof &> /dev/null; then
        PID=$(lsof -ti:$PORT 2>/dev/null)
    # 尝试使用fuser
    elif command -v fuser &> /dev/null; then
        PID=$(fuser $PORT/tcp 2>/dev/null | awk '{print $1}')
    # 尝试使用ss
    elif command -v ss &> /dev/null; then
        PID=$(ss -tlnp 2>/dev/null | grep ":$PORT " | grep -oP 'pid=\K[0-9]+' | head -1)
    # 尝试使用netstat
    elif command -v netstat &> /dev/null; then
        PID=$(netstat -tlnp 2>/dev/null | grep ":$PORT " | grep -oP '[0-9]+/.*' | grep -oP '^[0-9]+' | head -1)
    fi
    
    if [ -z "$PID" ]; then
        return 1
    fi
    
    print_info "找到占用端口 $PORT 的进程 (PID: $PID)"
    print_info "正在停止进程..."
    
    kill "$PID" 2>/dev/null
    
    # 等待进程结束
    for i in {1..5}; do
        if ! ps -p "$PID" > /dev/null 2>&1; then
            print_info "进程已停止"
            rm -f "$PID_FILE"
            return 0
        fi
        sleep 1
    done
    
    # 强制杀死
    if ps -p "$PID" > /dev/null 2>&1; then
        print_warn "强制终止进程..."
        kill -9 "$PID" 2>/dev/null
        sleep 1
        rm -f "$PID_FILE"
    fi
    
    return 0
}

# 主函数
main() {
    print_info "正在停止游戏服务器..."
    print_info "端口: $PORT"
    
    # 方法1: 通过PID文件停止
    if stop_by_pid_file; then
        print_info "服务器已成功停止"
        exit 0
    fi
    
    # 方法2: 通过进程名停止
    if stop_by_process_name; then
        print_info "服务器已成功停止"
        exit 0
    fi
    
    # 方法3: 通过端口停止
    if stop_by_port; then
        print_info "服务器已成功停止"
        exit 0
    fi
    
    # 所有方法都失败
    print_warn "未找到运行中的服务器进程"
    print_info "可能的原因："
    print_info "  1. 服务器未运行"
    print_info "  2. 服务器进程名称不是 'GameServer'"
    print_info "  3. 服务器未监听端口 $PORT"
    
    # 清理可能存在的过期PID文件
    if [ -f "$PID_FILE" ]; then
        print_info "清理过期的PID文件"
        rm -f "$PID_FILE"
    fi
    
    exit 0
}

# 执行主函数
main "$@"

