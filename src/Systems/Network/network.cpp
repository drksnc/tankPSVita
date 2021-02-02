#include "network.h"
#include "sdl2/sdl.h"
#include "psp2/sysmodule.h"

#define PORT 3600
static char NetParamMem[16 * 1024];

CNetwork::CNetwork()
{

}

CNetwork::~CNetwork()
{
    sceNetCtlTerm();
    sceNetTerm();

    if (sceSysmoduleIsLoaded(SCE_SYSMODULE_NET) == SCE_SYSMODULE_LOADED)
        sceSysmoduleUnloadModule(SCE_SYSMODULE_NET);
}

void CNetwork::Init()
{
    if (sceSysmoduleIsLoaded(SCE_SYSMODULE_NET) != SCE_SYSMODULE_LOADED)
        sceSysmoduleLoadModule(SCE_SYSMODULE_NET);

    SceNetInitParam initparam;
	initparam.memory = NetParamMem;
	initparam.size = sizeof(NetParamMem);
	initparam.flags = 0;

	sceNetInit(&initparam);
    sceNetCtlInit();

    sceNetCtlInetGetInfo(SCE_NETCTL_INFO_GET_IP_ADDRESS, &m_info);
    sceNetInetPton(SCE_NET_AF_INET, m_info.ip_address, &m_local_address);
}

void CNetwork::Connect(std::string ip_addr)
{
    m_client_socket = sceNetSocket("TankVitaNet_CL", SCE_NET_AF_INET, SCE_NET_SOCK_DGRAM, SCE_NET_IPPROTO_UDP);
   
	m_server_addr.sin_family = SCE_NET_AF_INET;
	m_server_addr.sin_port   = sceNetHtons(PORT);
    sceNetInetPton(SCE_NET_AF_INET, ip_addr.c_str(), &m_server_addr.sin_addr);
    sceNetConnect(m_client_socket, (SceNetSockaddr*)&m_server_addr, sizeof(m_server_addr));
    SDL_LogMessage(0, SDL_LogPriority::SDL_LOG_PRIORITY_INFO, "Trying connect to [%s]:[%i]", ip_addr.c_str(), PORT);
}

void CNetwork::CreateServer()
{
    m_server_socket = sceNetSocket("TankVitaNet_SV", SCE_NET_AF_INET, SCE_NET_SOCK_DGRAM, SCE_NET_IPPROTO_UDP);
    m_server_addr.sin_family = SCE_NET_AF_INET;
	m_server_addr.sin_addr.s_addr = sceNetHtonl(m_local_address.s_addr);
	m_server_addr.sin_port = sceNetHtons(PORT);

    sceNetBind(m_server_socket, (SceNetSockaddr*)&m_server_addr, sizeof(m_server_addr));
    sceNetListen(m_server_socket, 128);

    SDL_LogMessage(0, SDL_LogPriority::SDL_LOG_PRIORITY_INFO, "Server Created on PORT [%i]", PORT);
}

void CNetwork::Listen()
{
    SceNetSockaddrIn clientaddr;
    unsigned int claddrlen = sizeof(clientaddr); 

    int new_cl_sock = sceNetAccept(m_server_socket, (SceNetSockaddr*)&clientaddr, &claddrlen);
    char remote_ip[16];
    if (new_cl_sock >= 0)
    {
        sceNetInetNtop(SCE_NET_AF_INET, &clientaddr.sin_addr.s_addr, remote_ip, sizeof(remote_ip));
        SDL_LogMessage(0, SDL_LogPriority::SDL_LOG_PRIORITY_INFO, "NEW CONNECTION %i %s", new_cl_sock, remote_ip);
    }
}