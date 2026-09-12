#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
素材扫描器 —— 主游戏源码 vs 素材注册表 的交叉检查

作用：让「新增素材」这件事不会静默丢失。
  · 扫描游戏源码里所有的绘制入口（如 NightElf::draw、Ch2GateScene::drawDoor）
  · 与 registry/asset_registry.h 的登记项比对
  · 报出「源码里有、注册表里没有」的入口  → 提醒登记（这就是"自动读取新素材"）
  · 也报出「注册表里有、源码里已消失」的入口 → 防止你重构后注册表腐烂

用法：
    python3 scanner.py                  # 人读报告
    python3 scanner.py --json           # 机器读（给网页画廊用）
    python3 scanner.py --suggest        # 为未登记的入口生成注册项草稿
"""

import os
import re
import sys
import json

HERE = os.path.dirname(os.path.abspath(__file__))
TOOL_DIR = os.path.dirname(HERE)
GAME_DIR = os.path.normpath(os.path.join(TOOL_DIR, "..", "..", "v2.0.0", "multiple code files"))
REGISTRY = os.path.join(HERE, "asset_registry.h")

# 绘制入口的函数名模式：draw / drawEnemy / drawStars / drawHPBar ...
DRAW_FN = re.compile(r'\b(?:static\s+)?void\s+(draw[A-Za-z0-9_]*)\s*\(')
CLASS_DECL = re.compile(r'\b(?:class|struct)\s+([A-Za-z_][A-Za-z0-9_]*)\s*(?::[^;{]*)?\{')
# registry 里 source 字段形如 ".../player.h:215 (NightElf::draw)"
SRC_TOKEN = re.compile(r'\(([^)]*)\)')


def strip_comments(text):
    """去掉注释和字符串字面量，避免花括号计数被干扰"""
    out = []
    i, n = 0, len(text)
    while i < n:
        c = text[i]
        if c == '/' and i + 1 < n and text[i + 1] == '/':
            j = text.find('\n', i)
            i = n if j < 0 else j
        elif c == '/' and i + 1 < n and text[i + 1] == '*':
            j = text.find('*/', i + 2)
            i = n if j < 0 else j + 2
        elif c == '"' or c == "'":
            q = c
            i += 1
            while i < n and text[i] != q:
                if text[i] == '\\':
                    i += 1
                i += 1
            i += 1
        else:
            out.append(c)
            i += 1
    return ''.join(out)


def scan_file(path, relpath):
    """返回该文件里的绘制入口：[(Class::method 或 method, 行号)]"""
    try:
        raw = open(path, encoding='utf-8', errors='replace').read()
    except OSError:
        return []

    # 行号必须在剥离注释前统计，所以记录原文本的行边界
    lines = raw.split('\n')
    clean = strip_comments(raw)

    def line_of(pos):
        return clean.count('\n', 0, pos) + 1

    # 建立「位置 -> 所属类」映射：逐字符扫花括号，维护类栈
    owners = []            # [(start_pos, end_pos, classname)]
    stack = []             # [(brace_depth_entered, classname or None)]
    depth = 0
    pending_class = None

    for m in CLASS_DECL.finditer(clean):
        pending_class = m.group(1)
        # 找到该 class 的起始花括号
        brace = clean.find('{', m.end() - 1)
        if brace >= 0:
            owners.append([brace, None, pending_class])

    # 用简单的括号配对给每个类找结束位置
    for owner in owners:
        start = owner[0]
        d = 0
        i = start
        while i < len(clean):
            if clean[i] == '{':
                d += 1
            elif clean[i] == '}':
                d -= 1
                if d == 0:
                    owner[1] = i
                    break
            i += 1
        if owner[1] is None:
            owner[1] = len(clean)

    def owner_of(pos):
        best = None
        for s, e, name in owners:
            if s <= pos <= e:
                if best is None or (e - s) < best[1] - best[0]:
                    best = (s, e, name)
        return best[2] if best else None

    results = []
    for m in DRAW_FN.finditer(clean):
        fn = m.group(1)
        pos = m.start()
        cls = owner_of(pos)
        token = f"{cls}::{fn}" if cls else fn
        results.append((token, line_of(pos)))
    return results


def scan_game():
    """扫描整个游戏源码目录"""
    entries = []
    for root, dirs, files in os.walk(GAME_DIR):
        dirs[:] = [d for d in dirs if not d.startswith('.')]
        for f in sorted(files):
            if not f.endswith(('.h', '.cpp')):
                continue
            if 'archive' in f or 'deprecated' in f:
                continue
            full = os.path.join(root, f)
            rel = os.path.relpath(full, os.path.dirname(GAME_DIR))
            for token, line in scan_file(full, rel):
                entries.append({"token": token, "file": rel, "line": line})
    return entries


def read_registry_entries():
    """从注册表目录下所有头文件里抽出 reg("id", ...) 的 id 与 source 字段

    注意：登记项分散在 reg_players.h / reg_enemies.h / ... 等多个文件里，
    所以这里扫描整个 registry 目录，而不是只读 asset_registry.h。
    """
    out = []
    files = []
    if os.path.isdir(HERE):
        for name in sorted(os.listdir(HERE)):
            if name.endswith(".h") and not name.startswith("_"):
                files.append(os.path.join(HERE, name))
    # 草稿区也可能有登记项
    drafts = os.path.join(TOOL_DIR, "drafts")
    if os.path.isdir(drafts):
        for name in sorted(os.listdir(drafts)):
            d = os.path.join(drafts, name)
            if os.path.isdir(d) and not name.startswith((".", "_")):
                draw_h = os.path.join(d, "draw.h")
                if os.path.isfile(draw_h):
                    files.append(draw_h)

    for path in files:
        try:
            text = open(path, encoding="utf-8", errors="replace").read()
        except OSError:
            continue
        rel = os.path.relpath(path, TOOL_DIR)
        # C++ 允许把长字符串拆成相邻字面量跨行书写（"abc" "def"），
        # 先合并它们，否则正则匹配不到 .from("...") 的内容。
        text = re.sub(r'"\s*\n\s*"', '', text)
        text = re.sub(r'"\s+"', '', text)
        for m in re.finditer(
                r'\breg\(\s*"([^"]+)"\s*,\s*"([^"]*)"\s*,\s*"([^"]*)"\s*,\s*"([^"]*)"\s*\)', text):
            start = m.end()
            nxt = text.find("reg(", start)
            block = text[start: nxt if nxt > 0 else len(text)]
            src = ""
            sm = re.search(r'\.from\(\s*"([^"]*)"\s*\)', block)
            if sm:
                src = sm.group(1)
            tokens = []
            tm = SRC_TOKEN.search(src)
            if tm:
                tokens = [t.strip() for t in tm.group(1).split(",") if t.strip()]
            out.append({
                "id": m.group(1),
                "name": m.group(2),
                "chapter": m.group(3),
                "category": m.group(4),
                "source": src,
                "tokens": tokens,
                "file": rel,
            })
    return out


def main():
    game = scan_game()
    regs = read_registry_entries()

    covered_tokens = set()
    for r in regs:
        for t in r["tokens"]:
            covered_tokens.add(t)

    game_tokens = set(e["token"] for e in game)

    uncovered = [e for e in game if e["token"] not in covered_tokens]

    # 注册表指向了源码里已不存在的入口（重构后腐烂）
    stale = []
    for r in regs:
        for t in r["tokens"]:
            if t not in game_tokens:
                stale.append({"id": r["id"], "token": t})

    as_json = "--json" in sys.argv

    if as_json:
        print(json.dumps({
            "gameDir": GAME_DIR,
            "gameDrawCount": len(game),
            "registeredCount": len(regs),
            "registeredTokens": sorted(covered_tokens),
            "coveredCount": len([e for e in game if e["token"] in covered_tokens]),
            "uncovered": uncovered,
            "stale": stale,
        }, ensure_ascii=False, indent=2))
        return 0

    print("=" * 66)
    print("素材扫描器 —— 游戏源码 vs 素材注册表")
    print("=" * 66)
    print(f"游戏源码: {GAME_DIR}")
    print(f"绘制入口总数: {len(game)}")
    print(f"注册表登记项: {len(regs)}  (覆盖 {len(game) - len(uncovered)} 个入口)")
    print()

    if uncovered:
        print(f"⚠ 未登记的绘制入口 ({len(uncovered)} 个) —— 这些素材在画廊里看不到：")
        by_file = {}
        for e in uncovered:
            by_file.setdefault(e["file"], []).append(e)
        for f in sorted(by_file):
            print(f"\n  {f}")
            for e in sorted(by_file[f], key=lambda x: x["line"]):
                print(f"    {e['line']:5d}  {e['token']}")
    else:
        print("✓ 所有绘制入口都已登记")

    if stale:
        print(f"\n⚠ 注册表指向了已不存在的入口 ({len(stale)} 个) —— 源码重构后需要更新登记：")
        for s in stale:
            print(f"    {s['id']}  →  {s['token']}")
    print()

    if "--suggest" in sys.argv and uncovered:
        print("=" * 66)
        print("注册项草稿（复制到 registry/asset_registry.h 后补上绘制实现）")
        print("=" * 66)
        for e in uncovered:
            cid = f"{e['file'].split('/')[-1].replace('.h','')}.{e['token'].replace('::','.')}"
            print(f"""
    // TODO: 补绘制实现
    reg("{cid.lower()}", "{e['token']}", "?", "?")
        .from("{e['file']}:{e['line']} ({e['token']})")
        .tag("待分类")
        .drawFn(nullptr);""")

    return 0


if __name__ == "__main__":
    sys.exit(main())
