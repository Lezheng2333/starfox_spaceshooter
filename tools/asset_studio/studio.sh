#!/usr/bin/env bash
# ============================================================
# Asset Studio 素材工坊 — 一键启动
#
#   ./studio.sh            编译 + 启动 + 打开浏览器
#   ./studio.sh --build    只编译
#   ./studio.sh --check    编译 + 冒烟自检（列出素材 + 渲染试点素材）
#   ./studio.sh --scan     交叉检查未登记的绘制入口
#   ./studio.sh --port N   指定端口
# ============================================================
set -euo pipefail

cd "$(dirname "$0")"

PORT=8791
MODE=serve

while [ $# -gt 0 ]; do
  case "$1" in
    --build) MODE=build ;;
    --check) MODE=check ;;
    --scan)  MODE=scan ;;
    --port)  PORT="$2"; shift ;;
    -h|--help) sed -n '2,12p' "$0" | sed 's/^# \{0,1\}//'; exit 0 ;;
    *) echo "未知参数: $1"; exit 1 ;;
  esac
  shift
done

if [ ! -f /opt/homebrew/include/SDL2/SDL.h ]; then
  echo "✗ 找不到 SDL2。请先安装：  brew install sdl2"
  exit 1
fi

case "$MODE" in
  build) make ;;
  check) make check ;;
  scan)  make scan ;;
  serve)
    make
    echo
    exec python3 server.py --port "$PORT" --open
    ;;
esac
