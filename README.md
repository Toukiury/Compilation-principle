### 配置准备

**Windows用户**：

1. 安装 MSYS2（可选）

### Step 1: 准备目录结构

在项目根目录下创建必要的目录：

```bash
# 进入项目根目录
cd （项目根目录）

# 创建构建目录
mkdir build
mkdir bin
```

现在目录结构应该是：

```markdown
/
├── bin/       （空，用于存放生成的可执行文件）
├── build/     （空，用于存放编译中间文件）
└── file/       （源代码）
```

### **Step 2: 启动**终端

使用 MSYS2 的 MinGW 64-bit 终端

导航到项目目录：

```bash
cd  你的实际路径
```

### Step 3: 安装编译工具链

在 MSYS2 终端中执行：

```bash
# 更新软件包数据库
pacman -Sy

# 安装必备工具
pacman -S --needed mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-make flex bison
```

### Step 4: 生成构建系统

```bash
# 进入 build 目录
cd build

# 生成 Makefile
cmake .. -DCMAKE_INSTALL_PREFIX=../bin -G "MinGW Makefiles"
```

- `-DCMAKE_INSTALL_PREFIX=../bin`：指定可执行文件输出到 `bin` 目录
- `-G "MinGW Makefiles"`：明确指定生成适用于 MinGW 的构建文件

### Step 5: 编译项目

```bash
mingw32-make
```

检查 `bin/` 目录下是否生成了 `pascc.exe`（Windows）

### Step 6: 运行

```bash
./pascc -i <inputfile>
```

