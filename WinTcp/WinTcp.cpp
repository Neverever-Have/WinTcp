#include "WinTcp.h"
#include "map"
#include "vector"
#include "sstream"
#include <algorithm>
#define WinTcp_Sleep(ms)  std::this_thread::sleep_for(std::chrono::milliseconds(ms))
static struct static_Request_Path_Data_t {
    E_RequestType RequestType;
    uintptr_t BackCall;
};
static std::map<std::string, static_Request_Path_Data_t> static_map_Path;
static uintptr_t GlobalBackCall, ErrorlBackCall;
CServer::CServer(const Initialize_data_t& Initialize_data) : m_ulPort(Initialize_data.m_culPort), m_pErrorBackCall(Initialize_data.m_pErrorBackCall) {
    ZeroMemory(&m_aHints, sizeof(m_aHints));
    m_aHints.ai_family = AF_INET;
    m_aHints.ai_socktype = SOCK_STREAM;
    m_aHints.ai_protocol = IPPROTO_TCP;
    m_aHints.ai_flags = AI_PASSIVE;
}
CServer::~CServer() {
    if (this->m_sListenSocket != INVALID_SOCKET) {
        closesocket(this->m_sListenSocket);
        this->m_sListenSocket = INVALID_SOCKET;
    }
    if (this->m_sClientSocket != INVALID_SOCKET) {
        closesocket(this->m_sClientSocket);
        this->m_sClientSocket = INVALID_SOCKET;
    }
    if (this->m_pResult) {
        delete this->m_pResult;
        this->m_pResult = nullptr;
    }
    WSACleanup();

}
E_ResultCode CServer::Initialize() {
    int iResult = WSAStartup(MAKEWORD(2, 2), &m_wsaData);
    if (iResult != 0) return E_ResultCode::WSAStartupFailed;
    iResult = getaddrinfo(nullptr, std::to_string(m_ulPort).c_str(), &m_aHints, &m_pResult);
    if (iResult != 0) {
        WSACleanup();
        return E_ResultCode::GetaddrInfoFailed;
    }
    m_sListenSocket = socket(m_pResult->ai_family, m_pResult->ai_socktype, m_pResult->ai_protocol);
    if (m_sListenSocket == INVALID_SOCKET) {
        freeaddrinfo(m_pResult);
        WSACleanup();
        return E_ResultCode::SocketError;
    }
    iResult = bind(m_sListenSocket, m_pResult->ai_addr, (int)m_pResult->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        freeaddrinfo(m_pResult);
        closesocket(m_sListenSocket);
        WSACleanup();
        return E_ResultCode::BindFailed;
    }
    freeaddrinfo(m_pResult);
    return (listen(m_sListenSocket, SOMAXCONN) == SOCKET_ERROR) ? (closesocket(m_sListenSocket), WSACleanup(), E_ResultCode::ListenFailed) : E_ResultCode::OK;
}
E_ResultCode CServer::PushGlobal(uintptr_t BackCall) {
    return (m_pGlobalsBackCall = GlobalBackCall = BackCall), E_ResultCode::OK;
}
E_ResultCode CServer::Run(uintptr_t GlobalsBackCall) {
    return (m_pGlobalsBackCall = GlobalsBackCall, m_iStatus = 0, std::thread(&CServer::m_fpRun, this).detach(), E_ResultCode::OK);
}
E_ResultCode CServer::PushPath(const E_RequestType& RequestType, const std::string& Path, uintptr_t BackCall) {
    static_map_Path[Path] = { RequestType, BackCall };
    return (static_map_Path.find(Path) != static_map_Path.end()) ? E_ResultCode::OK : E_ResultCode::Unknown;
}
void CServer::m_fpRun() {
    while (true) {
        m_sClientSocket = accept(m_sListenSocket, nullptr, nullptr);
        if (m_sClientSocket == INVALID_SOCKET) {
            reinterpret_cast<void(*)(int)>(m_pErrorBackCall)(-1);
            closesocket(m_sListenSocket);
            WSACleanup();
            return;
        }
        std::thread(&CServer::ClientHandler, this, &m_sClientSocket).detach();
    }
}
static RequestData_t parseHttpRequest(std::string& request) {
    request.erase(std::remove(request.begin(), request.end(), '\r'), request.end());
    RequestData_t httpRequest;
    std::istringstream stream(request);
    std::string line;
    std::getline(stream, line);
    std::istringstream requestLineStream(line);
    requestLineStream >> httpRequest.method >> httpRequest.path >> httpRequest.httpVersion;
    while (std::getline(stream, line) && !line.empty()) {
        auto pos = line.find(':');
        if (pos != std::string::npos) {
            std::string headerName = line.substr(0, pos);
            std::string headerValue = line.substr(pos + 1);
            headerValue.erase(0, headerValue.find_first_not_of(' '));
            httpRequest.headers[headerName] = headerValue;
        }
    }
    std::ostringstream bodyStream;
    while (std::getline(stream, line)) bodyStream << line << '\n';
    httpRequest.body = bodyStream.str();
    return httpRequest;
}
static std::string fpRequestTypeToString(E_RequestType type) {
    static const std::map<E_RequestType, std::string> RequestTypeToString = {
        {E_RequestType::GET, "GET"}, {E_RequestType::POST, "POST"}, {E_RequestType::HEAD, "HEAD"},
        {E_RequestType::PUT, "PUT"}, {E_RequestType::OPTIONS, "OPTIONS"}, {E_RequestType(5), "DELETE"},
        {E_RequestType::CONNECT, "CONNECT"}
    };
    auto it = RequestTypeToString.find(type);
    return it != RequestTypeToString.end() ? it->second : "UNKNOWN";
}
void CServer::ClientHandler(SOCKET* pClientSocket) {
    char recvbuf[1024];
    int iResult = recv(*pClientSocket, recvbuf, sizeof(recvbuf), 0);
    if (iResult > 0) {
        std::string receivedData(recvbuf, iResult);
        RequestData_t RequestData = parseHttpRequest(receivedData);
        ResponseData_t ResponseData;
        auto it = static_map_Path.find(RequestData.path);
        if (it != static_map_Path.end() && fpRequestTypeToString(it->second.RequestType) == RequestData.method)
            reinterpret_cast<void (*)(RequestData_t&, ResponseData_t&)>(it->second.BackCall)(RequestData, ResponseData);
        else if (GlobalBackCall)
            reinterpret_cast<void (*)(RequestData_t&, ResponseData_t&)>(GlobalBackCall)(RequestData, ResponseData);
        std::string httpResponse = "HTTP/1.1 " + std::to_string(ResponseData.status) + " " + ResponseData.str_status + "\r\n";
        for (auto& f : ResponseData.headers) httpResponse += f.first + ": " + f.second + "\r\n";
        if (ResponseData.Body_uc) {
            httpResponse += "\r\n";
            send(*pClientSocket, httpResponse.c_str(), httpResponse.length(), 0);
            send(*pClientSocket, (const char*)ResponseData.Body_uc, ResponseData.Body_ucSize, 0);
            delete[] ResponseData.Body_uc;
        }
        else {
            httpResponse += "\r\n" + ResponseData.Body;
            send(*pClientSocket, httpResponse.c_str(), httpResponse.length(), 0);
        }
    }
    closesocket(*pClientSocket);
}
