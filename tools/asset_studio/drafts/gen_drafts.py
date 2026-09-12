#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
草稿自动发现器

扫描 drafts/<名字>/draw.h，生成 _generated.h，让每个草稿自动出现在画廊里。
每个草稿目录的 draw.h 必须定义一个 register_<名字>() 函数。

约定：
    drafts/nightelf_wide/draw.h   →   register_nightelf_wide()

新增草稿 = 新建一个目录 + 放 draw.h，然后 make，就自动出现了。
"""

import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(HERE, "_generated.h")


def sanitize(name):
    return re.sub(r"[^A-Za-z0-9_]", "_", name)


def main():
    dirs = []
    if os.path.isdir(HERE):
        for name in sorted(os.listdir(HERE)):
            if name.startswith(".") or name.startswith("_"):
                continue
            d = os.path.join(HERE, name)
            if os.path.isdir(d) and os.path.isfile(os.path.join(d, "draw.h")):
                dirs.append(name)

    lines = [
        "#pragma once",
        "",
        "// ============================================================",
        "// 本文件由 drafts/gen_drafts.py 自动生成，请勿手工修改。",
        "// 新增草稿 = 在 drafts/ 下新建目录并放入 draw.h，然后 make。",
        "// ============================================================",
        "",
    ]
    for name in dirs:
        lines.append('#include "%s/draw.h"' % name)
    lines.append("")
    lines.append("inline void registerDrafts() {")
    lines.append("    static bool built = false;")
    lines.append("    if (built) return;")
    lines.append("    built = true;")
    for name in dirs:
        lines.append("    register_%s();" % sanitize(name))
    lines.append("}")
    lines.append("")

    content = "\n".join(lines)

    old = ""
    if os.path.isfile(OUT):
        try:
            old = open(OUT, encoding="utf-8").read()
        except OSError:
            old = ""
    if old != content:
        with open(OUT, "w", encoding="utf-8") as f:
            f.write(content)
        print("  [GEN] drafts/_generated.h  (%d 个草稿%s)"
              % (len(dirs), (": " + ", ".join(dirs)) if dirs else ""))
    else:
        print("  [GEN] drafts/_generated.h  无变化 (%d 个草稿)" % len(dirs))
    return 0


if __name__ == "__main__":
    sys.exit(main())
