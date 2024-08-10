#include <iostream>
#include "ServerSystem/WinTcp.h"
void backcall(RequestData_t& RequestData, ResponseData_t& ResponseData) {
    ResponseData.status = 200;
    ResponseData.str_status = "OK";
    ResponseData.Body_uc = new unsigned char[3];
    ResponseData.Body_uc[0] = 0x1;
    ResponseData.Body_uc[1] = 0x2;
    ResponseData.Body_uc[2] = 0x3;
    ResponseData.Body_ucSize=3;
    ResponseData.Body = "ok";
    ResponseData.headers["server"] = "OK";
}
void backcallGlobal(RequestData_t& RequestData, ResponseData_t& ResponseData) {
    printf("Global\n");
}
int main()
{
    Initialize_data_t c = {6024,0};
    CServer Server(c);
    Server.Initialize();
    Server.PushPath(E_RequestType::POST,"/123", (uintptr_t)backcall);
    Server.PushGlobal((uintptr_t)backcallGlobal);
    Server.Run();
    while (true)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
