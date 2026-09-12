# Asset Studio — 无头素材渲染器
#
# 依赖：SDL2 (brew install sdl2) + zlib（macOS 自带）
# 注意：本目标每次都重新编译，避免游戏头文件或草稿改动后用到旧二进制。

GAME_DIR := ../../v2.0.0/multiple code files

CXX      := clang++
CXXFLAGS := -std=c++11 -O2 -Wall \
            -I/opt/homebrew/include -I/opt/homebrew/include/SDL2 -D_THREAD_SAFE \
            -I"$(GAME_DIR)" -Irenderer -Iregistry -Idrafts
LDFLAGS  := -L/opt/homebrew/lib -lSDL2 -lz

TARGET := assetstudio

.PHONY: all clean check scan server

all:
	@python3 drafts/gen_drafts.py
	@echo "  [CXX] $(TARGET)  (含游戏头文件，约 2-5 秒)"
	@$(CXX) $(CXXFLAGS) renderer/assetstudio.cpp -o $(TARGET) $(LDFLAGS) && echo "  [OK ] 编译完成"

# 一键启动工坊
server: all
	@python3 server.py --open

# 列出素材 + 渲染试点素材（冒烟测试）
check: all
	@./$(TARGET) --list --pretty | head -50
	@echo
	@./$(TARGET) --render ch2.player.nightelf --ascii --analysis

# 交叉检查：游戏源码里有哪些绘制入口还没登记
scan: all
	@python3 registry/scanner.py

clean:
	@rm -f $(TARGET)
	@rm -rf cache
	@echo "  [OK ] 已清理（含渲染缓存）"
