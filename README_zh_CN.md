# 语言
- [简体中文](README_zh_CN.md)
- [English](README.md)

# C++ TCP 服务器项目

这个项目是一个用 C++ 实现的简单 TCP 服务器，能够处理客户端连接并处理请求。服务器支持基本的类 HTTP 请求，并且可以向客户端发送二进制数据，比如文件或图片。

## 功能

- 使用线程处理多个客户端连接。
- 支持基本的类 HTTP GET 请求。
- 能够向客户端发送二进制数据（例如文件或图片）。
- 针对常见网络问题的错误处理。
- 灵活的结构，可以自定义回调函数。

## 开始使用

### 先决条件

- Windows 操作系统
- Visual Studio（或其他支持 C++11 或更高版本的 C++ 编译器）
- Windows Sockets API (Winsock)

### 安装步骤

1. 克隆仓库：
   ```sh
   git clone https://github.com/yourusername/tcp-server.git
   cd tcp-server
   ```

2. 在你喜欢的 C++ IDE（如 Visual Studio）中打开项目。

3. 确保项目设置中已链接 `Ws2_32.lib`，因为该项目依赖于 Windows Sockets API。

### 构建项目

1. 在 Visual Studio 中打开解决方案文件。
2. 构建解决方案 (`Ctrl+Shift+B`) 或从菜单中选择 `构建`。
3. 根据你的构建配置，输出的可执行文件将生成在 `Debug` 或 `Release` 文件夹中。

### 运行服务器

1. 执行编译生成的二进制文件。
2. 服务器默认监听 `127.0.0.1` 的 `1024` 端口。
3. 你可以使用浏览器或 `curl` 等工具连接到服务器：
   ```sh
   curl http://127.0.0.1:1024/
   ```

### 项目结构

- `WinTcp.h` 和 `WinTcp.cpp`: 服务器的核心实现，处理套接字初始化、客户端连接和请求处理。
- `CServer`: 负责服务器操作的主要类，包括初始化、运行服务器和处理客户端请求。
- `RequestData_t` & `ResponseData_t`: 表示请求和响应数据的结构体。
- `E_RequestType`: 表示不同类型的类 HTTP 请求的枚举类。

## 使用

### 自定义服务器

要自定义服务器行为，你可以修改 `CServer` 类及其方法。例如，可以更改 `ClientHandler` 中请求的处理方式，或添加对其他请求类型的支持。

### 发送文件

要向客户端发送文件（如图片、文档），请修改 `ClientHandler` 函数，以读取文件内容并将其作为二进制流发送。确保在 HTTP 响应头中设置正确的 MIME 类型。

### 错误处理

服务器使用 `E_ResultCode` 枚举来处理各种错误，例如 `WSAStartupFailed`、`SocketError` 和 `BindFailed`。你可以扩展它以覆盖更多情况或自定义错误消息。

## 贡献

欢迎贡献代码！请提交问题或拉取请求以提交你的改进。

## 许可证

该项目采用 自定义 许可证 - 详见 [LICENSE](LICENSE) 文件。
