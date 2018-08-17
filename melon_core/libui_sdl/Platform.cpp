/*
    Copyright 2016-2017 StapleButter

    This file is part of melonDS.

    melonDS is free software: you can redistribute it and/or modify it under
    the terms of the GNU General Public License as published by the Free
    Software Foundation, either version 3 of the License, or (at your option)
    any later version.

    melonDS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with melonDS. If not, see http://www.gnu.org/licenses/.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "../Platform.h"
#include "../Config.h"

#ifdef __WIN32__
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#define socket_t    SOCKET
	#define sockaddr_t  SOCKADDR
	#define pcap_dev_name description
#else
	#include <unistd.h>
	#include <arpa/inet.h>
	#include <netinet/in.h>
	#include <sys/select.h>
	#include <sys/socket.h>
	#define socket_t    int
	#define sockaddr_t  struct sockaddr
	#define closesocket close
	#define pcap_dev_name name
#endif

#ifndef INVALID_SOCKET
#define INVALID_SOCKET  (socket_t)-1
#endif


void Stop(bool internal);


namespace Platform
{


typedef struct
{
    SDL_Thread* ID;
    void (*Func)();

} ThreadData;

int ThreadEntry(void* data)
{
    ThreadData* thread = (ThreadData*)data;
    thread->Func();
    return 0;
}


socket_t MPSocket;
sockaddr_t MPSendAddr;
u8 PacketBuffer[2048];

#define NIFI_VER 1


void StopEmu()
{
    Stop(true);
}


void* Thread_Create(void (*func)())
{
    ThreadData* data = new ThreadData;
    data->Func = func;
    data->ID = SDL_CreateThread(ThreadEntry, "melonDS core thread", data);
    return data;
}

void Thread_Free(void* thread)
{
    delete (ThreadData*)thread;
}

void Thread_Wait(void* thread)
{
    SDL_WaitThread((SDL_Thread*)((ThreadData*)thread)->ID, NULL);
}


void* Semaphore_Create()
{
    return SDL_CreateSemaphore(0);
}

void Semaphore_Free(void* sema)
{
    SDL_DestroySemaphore((SDL_sem*)sema);
}

void Semaphore_Reset(void* sema)
{
    while (SDL_SemTryWait((SDL_sem*)sema) == 0);
}

void Semaphore_Wait(void* sema)
{
    SDL_SemWait((SDL_sem*)sema);
}

void Semaphore_Post(void* sema)
{
    SDL_SemPost((SDL_sem*)sema);
}


bool MP_Init()
{
    int opt_true = 1;
    int res;

#ifdef __WIN32__
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
    {
        return false;
    }
#endif // __WXMSW__

    MPSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (MPSocket < 0)
	{
		return false;
	}

	res = setsockopt(MPSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt_true, sizeof(int));
	if (res < 0)
	{
		closesocket(MPSocket);
		MPSocket = INVALID_SOCKET;
		return false;
	}

	sockaddr_t saddr;
	saddr.sa_family = AF_INET;
	*(u32*)&saddr.sa_data[2] = htonl(Config::SocketBindAnyAddr ? INADDR_ANY : INADDR_LOOPBACK);
	*(u16*)&saddr.sa_data[0] = htons(7064);
	res = bind(MPSocket, &saddr, sizeof(sockaddr_t));
	if (res < 0)
	{
		closesocket(MPSocket);
		MPSocket = INVALID_SOCKET;
		return false;
	}

	res = setsockopt(MPSocket, SOL_SOCKET, SO_BROADCAST, (const char*)&opt_true, sizeof(int));
	if (res < 0)
	{
		closesocket(MPSocket);
		MPSocket = INVALID_SOCKET;
		return false;
	}

	MPSendAddr.sa_family = AF_INET;
	*(u32*)&MPSendAddr.sa_data[2] = htonl(INADDR_BROADCAST);
	*(u16*)&MPSendAddr.sa_data[0] = htons(7064);

	return true;
}

void MP_DeInit()
{
    if (MPSocket >= 0)
        closesocket(MPSocket);

#ifdef __WIN32__
    WSACleanup();
#endif // __WXMSW__
}

int MP_SendPacket(u8* data, int len)
{
    if (MPSocket < 0)
        return 0;

    if (len > 2048-8)
    {
        printf("MP_SendPacket: error: packet too long (%d)\n", len);
        return 0;
    }

    *(u32*)&PacketBuffer[0] = htonl(0x4946494E); // NIFI
    PacketBuffer[4] = NIFI_VER;
    PacketBuffer[5] = 0;
    *(u16*)&PacketBuffer[6] = htons(len);
    memcpy(&PacketBuffer[8], data, len);

    int slen = sendto(MPSocket, (const char*)PacketBuffer, len+8, 0, &MPSendAddr, sizeof(sockaddr_t));
    if (slen < 8) return 0;
    return slen - 8;
}

int MP_RecvPacket(u8* data, bool block)
{
    if (MPSocket < 0)
        return 0;

    fd_set fd;
	struct timeval tv;

	FD_ZERO(&fd);
	FD_SET(MPSocket, &fd);
	tv.tv_sec = 0;
	tv.tv_usec = block ? 5000 : 0;

	if (!select(MPSocket+1, &fd, 0, 0, &tv))
    {
        return 0;
    }

    sockaddr_t fromAddr;
    socklen_t fromLen = sizeof(sockaddr_t);
    int rlen = recvfrom(MPSocket, (char*)PacketBuffer, 2048, 0, &fromAddr, &fromLen);
    if (rlen < 8+24)
    {
        return 0;
    }
    rlen -= 8;

    if (ntohl(*(u32*)&PacketBuffer[0]) != 0x4946494E)
    {
        return 0;
    }

    if (PacketBuffer[4] != NIFI_VER)
    {
        return 0;
    }

    if (ntohs(*(u16*)&PacketBuffer[6]) != rlen)
    {
        return 0;
    }

    memcpy(data, &PacketBuffer[8], rlen);
    return rlen;
}


}
