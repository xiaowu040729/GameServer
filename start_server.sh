#!/bin/bash

# 游戏服务器启动脚本
# 作者: Auto-generated
# 用途: 编译并启动游戏服务器

# 设置颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 项目根目录
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
GAME_SERVER_DIR="$PROJECT_ROOT/GameServer"
BINARY_NAME="GameServer"
BINARY_PATH="$GAME_SERVER_DIR/$BINARY_NAME"
PORT=8899
LOG_DIR="$PROJECT_ROOT/logs"
LOG_FILE="$LOG_DIR/server.log"
PID_FILE="$PROJECT_ROOT/server.pid"

# 创建日志目录
mkdir -p "$LOG_DIR"

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

# 检查依赖
check_dependencies() {
    print_info "检查编译依赖..."
    
    # 检查g++
    if ! command -v g++ &> /dev/null; then
        print_error "未找到 g++，请先安装: sudo apt-get install g++"
        exit 1
    fi
    
    # 检查pkg-config（用于查找库）
    if ! command -v pkg-config &> /dev/null; then
        print_warn "未找到 pkg-config，可能无法自动检测库路径"
    fi
    
    print_info "依赖检查完成"
}

# 检查并启动Redis服务器
check_and_start_redis() {
    print_info "检查Redis服务器..."
    
    # 检查Redis是否已安装
    if ! command -v redis-server &> /dev/null && ! command -v redis-cli &> /dev/null; then
        print_warn "未找到Redis，尝试使用systemctl检查..."
        
        # 尝试使用systemctl检查Redis服务
        if command -v systemctl &> /dev/null; then
            if systemctl list-unit-files | grep -q redis; then
                print_info "检测到Redis服务，尝试启动..."
                if systemctl is-active --quiet redis-server || systemctl is-active --quiet redis; then
                    print_info "Redis服务已在运行"
                    return 0
                else
                    print_info "启动Redis服务..."
                    sudo systemctl start redis-server 2>/dev/null || sudo systemctl start redis 2>/dev/null
                    sleep 2
                    if systemctl is-active --quiet redis-server || systemctl is-active --quiet redis; then
                        print_info "Redis服务启动成功"
                        return 0
                    fi
                fi
            fi
        fi
        
        print_error "Redis未安装或无法启动"
        print_info "请安装Redis: sudo apt-get install redis-server"
        print_warn "将继续运行，但姓名生成功能可能无法使用Redis缓存"
        return 1
    fi
    
    # 检查Redis是否正在运行
    if command -v redis-cli &> /dev/null; then
        if redis-cli ping &> /dev/null; then
            print_info "Redis服务器正在运行"
            return 0
        fi
    fi
    
    # 尝试启动Redis
    print_info "Redis服务器未运行，尝试启动..."
    
    # 方法1: 使用systemctl启动
    if command -v systemctl &> /dev/null; then
        if systemctl list-unit-files | grep -q redis; then
            if sudo systemctl start redis-server 2>/dev/null || sudo systemctl start redis 2>/dev/null; then
                sleep 2
                if redis-cli ping &> /dev/null 2>&1; then
                    print_info "Redis服务启动成功（通过systemctl）"
                    return 0
                fi
            fi
        fi
    fi
    
    # 方法2: 直接启动redis-server（后台运行）
    if command -v redis-server &> /dev/null; then
        print_info "尝试直接启动redis-server..."
        # 检查是否已有redis-server进程
        if pgrep -x redis-server > /dev/null; then
            print_info "Redis服务器进程已存在"
            sleep 1
            if redis-cli ping &> /dev/null 2>&1; then
                print_info "Redis服务器运行正常"
                return 0
            fi
        else
            # 启动redis-server（使用默认配置）
            nohup redis-server > "$LOG_DIR/redis.log" 2>&1 &
            REDIS_PID=$!
            sleep 2
            
            if redis-cli ping &> /dev/null 2>&1; then
                print_info "Redis服务器启动成功（PID: $REDIS_PID）"
                echo $REDIS_PID > "$PROJECT_ROOT/redis.pid"
                return 0
            else
                print_warn "Redis服务器启动可能失败，请检查日志: $LOG_DIR/redis.log"
                return 1
            fi
        fi
    fi
    
    print_warn "无法启动Redis服务器，将继续运行（姓名生成将使用文件模式）"
    return 1
}

# 编译服务器
compile_server() {
    print_info "开始编译服务器..."
    
    cd "$GAME_SERVER_DIR" || exit 1
    
    # 检查源文件
    SOURCE_FILES=(
        "main.cpp"
        "AOI.cpp"
        "GameChannel.cpp"
        "GameMsg.cpp"
        "GameProtocol.cpp"
        "GameRole.cpp"
        "NameGenerator.cpp"
        "msg.pb.cc"
    )
    
    for file in "${SOURCE_FILES[@]}"; do
        if [ ! -f "$file" ]; then
            print_error "源文件不存在: $file"
            exit 1
        fi
    done
    
    # 编译命令
    # 注意：需要根据实际的库路径调整 -L 和 -I 参数
    print_info "执行编译..."
    
    g++ -std=c++11 -pthread \
        -o "$BINARY_NAME" \
        "${SOURCE_FILES[@]}" \
        -lzinx -lprotobuf -lpthread -lhiredis \
        2>&1 | tee "$LOG_DIR/compile.log"
    
    if [ $? -eq 0 ]; then
        print_info "编译成功！"
        chmod +x "$BINARY_PATH"
    else
        print_error "编译失败，请查看日志: $LOG_DIR/compile.log"
        exit 1
    fi
}

# 检查端口是否被占用
check_port() {
    if command -v netstat &> /dev/null; then
        if netstat -tuln | grep -q ":$PORT "; then
            print_warn "端口 $PORT 已被占用"
            return 1
        fi
    elif command -v ss &> /dev/null; then
        if ss -tuln | grep -q ":$PORT "; then
            print_warn "端口 $PORT 已被占用"
            return 1
        fi
    fi
    return 0
}

# 检查服务器是否已在运行
check_running() {
    if [ -f "$PID_FILE" ]; then
        PID=$(cat "$PID_FILE")
        if ps -p "$PID" > /dev/null 2>&1; then
            print_warn "服务器已在运行 (PID: $PID)"
            return 1
        else
            # PID文件存在但进程不存在，删除过期的PID文件
            rm -f "$PID_FILE"
        fi
    fi
    return 0
}

# 启动服务器
start_server() {
    # 检查是否已运行
    if ! check_running; then
        exit 1
    fi
    
    # 检查并启动Redis
    check_and_start_redis
    
    # 检查端口
    if ! check_port; then
        print_error "请先停止占用端口 $PORT 的进程"
        exit 1
    fi
    
    # 检查可执行文件
    if [ ! -f "$BINARY_PATH" ]; then
        print_warn "可执行文件不存在，开始编译..."
        compile_server
    fi
    
    print_info "启动服务器..."
    print_info "端口: $PORT"
    print_info "日志文件: $LOG_FILE"
    
    # 启动服务器（后台运行）
    cd "$GAME_SERVER_DIR" || exit 1
    nohup "./$BINARY_NAME" >> "$LOG_FILE" 2>&1 &
    SERVER_PID=$!
    
    # 保存PID
    echo $SERVER_PID > "$PID_FILE"
    
    # 等待一下，检查进程是否成功启动
    sleep 1
    if ps -p $SERVER_PID > /dev/null 2>&1; then
        print_info "服务器启动成功！"
        print_info "PID: $SERVER_PID"
        print_info "查看日志: tail -f $LOG_FILE"
        print_info "停止服务器: $0 stop"
    else
        print_error "服务器启动失败，请查看日志: $LOG_FILE"
        rm -f "$PID_FILE"
        exit 1
    fi
}

# 停止服务器
stop_server() {
    if [ ! -f "$PID_FILE" ]; then
        print_warn "PID文件不存在，服务器可能未运行"
        return 1
    fi
    
    PID=$(cat "$PID_FILE")
    if ps -p "$PID" > /dev/null 2>&1; then
        print_info "停止服务器 (PID: $PID)..."
        kill "$PID"
        
        # 等待进程结束
        for i in {1..10}; do
            if ! ps -p "$PID" > /dev/null 2>&1; then
                break
            fi
            sleep 1
        done
        
        # 如果还在运行，强制杀死
        if ps -p "$PID" > /dev/null 2>&1; then
            print_warn "进程未正常退出，强制终止..."
            kill -9 "$PID"
        fi
        
        rm -f "$PID_FILE"
        print_info "服务器已停止"
    else
        print_warn "进程不存在，清理PID文件"
        rm -f "$PID_FILE"
    fi
    
    # 注意：不停止Redis，因为Redis可能是系统服务或其他应用在使用
    # 如果Redis是通过脚本启动的（有redis.pid文件），可以选择停止
    REDIS_PID_FILE="$PROJECT_ROOT/redis.pid"
    if [ -f "$REDIS_PID_FILE" ]; then
        REDIS_PID=$(cat "$REDIS_PID_FILE")
        if ps -p "$REDIS_PID" > /dev/null 2>&1; then
            print_info "检测到脚本启动的Redis进程 (PID: $REDIS_PID)"
            print_info "注意：Redis可能被其他应用使用，默认不停止"
            # 如果需要停止脚本启动的Redis，可以取消下面的注释
            # print_info "停止脚本启动的Redis..."
            # kill "$REDIS_PID" 2>/dev/null
            # rm -f "$REDIS_PID_FILE"
        else
            rm -f "$REDIS_PID_FILE"
        fi
    fi
}

# 查看服务器状态
status_server() {
    if [ -f "$PID_FILE" ]; then
        PID=$(cat "$PID_FILE")
        if ps -p "$PID" > /dev/null 2>&1; then
            print_info "服务器正在运行 (PID: $PID)"
            print_info "端口: $PORT"
            return 0
        else
            print_warn "PID文件存在但进程不存在"
            rm -f "$PID_FILE"
        fi
    fi
    print_info "服务器未运行"
    return 1
}

# 查看日志
view_logs() {
    if [ -f "$LOG_FILE" ]; then
        tail -f "$LOG_FILE"
    else
        print_warn "日志文件不存在: $LOG_FILE"
    fi
}

# 主函数
main() {
    case "${1:-start}" in
        start)
            check_dependencies
            start_server
            ;;
        stop)
            stop_server
            ;;
        restart)
            stop_server
            sleep 2
            check_dependencies
            start_server
            ;;
        status)
            status_server
            ;;
        logs)
            view_logs
            ;;
        compile)
            check_dependencies
            compile_server
            ;;
        redis)
            check_and_start_redis
            ;;
        *)
            echo "用法: $0 {start|stop|restart|status|logs|compile|redis}"
            echo ""
            echo "命令说明:"
            echo "  start   - 启动服务器（如果未编译则先编译，自动启动Redis）"
            echo "  stop    - 停止服务器"
            echo "  restart - 重启服务器"
            echo "  status  - 查看服务器状态"
            echo "  logs    - 查看服务器日志（实时）"
            echo "  compile - 仅编译服务器"
            echo "  redis   - 检查并启动Redis服务器"
            exit 1
            ;;
    esac
}

# 执行主函数
main "$@"

