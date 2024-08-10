# Language
- [简体中文](README_zh_CN.md)
- [English](README.md)

# C++ TCP Server Project

This project is a C++ implementation of a simple TCP server that handles client connections and processes requests. The server supports basic HTTP-like requests and can send binary data, such as files or images, back to the client.

## Features

- Handles multiple client connections using threads.
- Supports basic HTTP-like GET requests.
- Can send binary data (e.g., files or images) to the client.
- Error handling for common networking issues.
- Flexible structure with customizable callbacks.

## Getting Started

### Prerequisites

- Windows OS
- Visual Studio (or another C++ compiler that supports C++11 or higher)
- Windows Sockets API (Winsock)

### Installation

1. Clone the repository:
   ```sh
   git clone https://github.com/yourusername/tcp-server.git
   cd tcp-server
   ```

2. Open the project in your preferred C++ IDE (e.g., Visual Studio).

3. Ensure that `Ws2_32.lib` is linked in your project settings, as this project depends on the Windows Sockets API.

### Building the Project

1. Open the solution file in Visual Studio.
2. Build the solution (`Ctrl+Shift+B`) or select `Build` from the menu.
3. The output executable will be generated in the `Debug` or `Release` folder, depending on your build configuration.

### Running the Server

1. Execute the compiled binary. 
2. By default, the server listens on `127.0.0.1` at port `1024`.
3. You can connect to the server using a browser or a tool like `curl`:
   ```sh
   curl http://127.0.0.1:1024/
   ```

### Project Structure

- `WinTcp.h` and `WinTcp.cpp`: Core server implementation, handling socket initialization, client connections, and request processing.
- `CServer`: The main class responsible for server operations, including initialization, running the server, and handling client requests.
- `RequestData_t` & `ResponseData_t`: Structures representing the request and response data.
- `E_RequestType`: Enum class representing different types of HTTP-like requests.

## Usage

### Customizing the Server

To customize the server behavior, you can modify the `CServer` class and its methods. For example, you can change the way requests are processed in `ClientHandler`, or add support for additional request types.

### Sending Files

To send files (e.g., images, documents) to the client, modify the `ClientHandler` function to read the file content and send it as a binary stream. Ensure the correct MIME type is set in the HTTP response header.

### Error Handling

The server uses an `E_ResultCode` enum to handle various errors like `WSAStartupFailed`, `SocketError`, and `BindFailed`. You can extend this to cover more cases or customize error messages.

## Contributing

Contributions are welcome! Please open an issue or submit a pull request with your improvements.

## License

This project is licensed under the Custom License - see the [LICENSE](LICENSE) file for details.
