/*
 *   Copyright (C) 2009-2011,2013,2015,2016,2020,2024,2026 by Jonathan Naylor G4KLX
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#if !defined(UDPSocket_H)
#define UDPSocket_H

#include "Connection.h"
#include "RingBuffer.h"

#include <string>

#if !defined(_WIN32) && !defined(_WIN64)
#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#else
#include <ws2tcpip.h>
#include <Winsock2.h>
#endif

enum IPMATCHTYPE {
	IMT_ADDRESS_AND_PORT,
	IMT_ADDRESS_ONLY
};

class CUDPSocket : public IConnection {
public:
	CUDPSocket(const std::string& address, unsigned short port = 0U);
	CUDPSocket(unsigned short port = 0U);
	virtual ~CUDPSocket();

	bool setDestination(const std::string& address, unsigned short port);

	virtual bool open();

	virtual int16_t read(uint8_t* buffer, uint16_t length);
	virtual int16_t write(const uint8_t* buffer, uint16_t length);

	virtual void close();

	static void startup();
	static void shutdown();

	static int lookup(const std::string& hostName, unsigned short port, sockaddr_storage& address, unsigned int& addressLength);
	static int lookup(const std::string& hostName, unsigned short port, sockaddr_storage& address, unsigned int& addressLength, struct addrinfo& hints);

	static bool match(const sockaddr_storage& addr1, const sockaddr_storage& addr2, IPMATCHTYPE type = IMT_ADDRESS_AND_PORT);

	static bool isNone(const sockaddr_storage& addr);

private:
	std::string          m_localAddress;
	unsigned short       m_localPort;
	sockaddr_storage     m_address;
	unsigned int         m_addressLength;
	CRingBuffer<uint8_t> m_buffer;
#if defined(_WIN32) || defined(_WIN64)
	SOCKET               m_fd;
#else
	int                  m_fd;
#endif
};

#endif
