#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <map>
#include <functional>
#pragma comment(lib, "Ws2_32.lib")

struct Initialize_data_t {
    unsigned long m_culPort;
    uintptr_t m_pErrorBackCall;
};

enum class E_ResultCode : short {
    Unknown = -1,
    OK,
    WSAStartupFailed,
    GetaddrInfoFailed,
    SocketError,
    BindFailed,
    ListenFailed
};

#undef DELETE
enum class E_RequestType {
    GET,
    POST,
    HEAD,
    PUT,
    OPTIONS,
    DELETE,
    CONNECT
};

#define DELETE (0x00010000L)

struct RequestData_t {
    std::string method;
    std::string path;
    std::string httpVersion;
    std::map<std::string, std::string> headers;
    std::string body;
};

struct ResponseData_t {
    int status = 200;
    std::string str_status = "OK";
    std::map<std::string, std::string> headers;
    std::string Body;
    unsigned char* Body_uc = nullptr;
    size_t Body_ucSize = 0;
};

class CServer {
public:
    CServer(const Initialize_data_t& Initialize_data);
    ~CServer();
    E_ResultCode Initialize();
    E_ResultCode Run(uintptr_t pGlobalsBackCall = NULL);
    E_ResultCode PushPath(const E_RequestType& RequestType, const std::string& Path, uintptr_t BackCall);
    E_ResultCode PushGlobal(uintptr_t BackCall);

private:
    int m_iStatus = 0;
    addrinfo m_aHints = {};
    addrinfo* m_pResult = nullptr;
    unsigned long m_ulPort = 0;
    WSADATA m_wsaData = {};
    SOCKET m_sListenSocket = INVALID_SOCKET;
    SOCKET m_sClientSocket = INVALID_SOCKET;
    uintptr_t m_pGlobalsBackCall = 0;
    uintptr_t m_pErrorBackCall = 0;

    void ClientHandler(SOCKET* pClientSocket);
    void m_fpRun();
};
