/* Copyright (c) 2006-2012, Universities Space Research Association (USRA).
*  All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the Universities Space Research Association nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY USRA ``AS IS'' AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL USRA BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
* TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// Implementation of the Socket class.

#include "Socket.h"

#include "Debug.hh"

#include <cerrno>
#include <cstring>
#include <iostream>

#include <fcntl.h>
#include <arpa/inet.h>

#ifdef PLEXIL_VXWORKS
// VxWorks is not BSD!
#include <sockLib.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#endif

Socket::Socket() :
   m_sock (-1)
{
   
   memset (&m_addr,
       0,
       sizeof (m_addr));
   
}

Socket::~Socket()
{
   if (is_valid())
   {
      MARK;
      ::close (m_sock);
   }
}

bool Socket::create()
{
   m_sock = socket (AF_INET,
            SOCK_STREAM,
            0);
   
   if (! is_valid())
      return false;
   
   
   // TIME_WAIT - argh
   int on = 1;
   if (setsockopt (m_sock, SOL_SOCKET, SO_REUSEADDR, (char*) &on, sizeof (on)) == -1)
      return false;
   
   
   return true;
   
}

const Socket& Socket::operator << (const std::string& s) const
{
   if (! Socket::send (s))
   {
      throw SocketException ("Could not write to socket.");
   }
   
   return *this;
   
}

const Socket& Socket::operator << (const char& c) const
{
   char buff[] = {c, 0};
   std::string s(buff);
   
   if (! Socket::send (s))
   {
      throw SocketException ("Could not write to socket.");
   }
   
   return *this;
}

const Socket& Socket::operator >> (std::string& s) const
{
   if (! Socket::recv (s))
   {
      throw SocketException ("Could not read from socket.");
   }
   
   return *this;
}

bool Socket::bind (const uint16_t port)
{
   
   if (! is_valid())
   {
      return false;
   }
   
   
   
   m_addr.sin_family = AF_INET;
   m_addr.sin_addr.s_addr = INADDR_ANY;
   m_addr.sin_port = htons (port);
   
   int bind_return = ::bind (m_sock,
                 (struct sockaddr *) &m_addr,
                 sizeof (m_addr));
   
   
   if (bind_return == -1)
   {
      return false;
   }
   
   return true;
}


bool Socket::listen() const
{
   if (! is_valid())
   {
      return false;
   }
   
   int listen_return = ::listen (m_sock, MAXCONNECTIONS);
   
   if (listen_return == -1)
   {
      return false;
   }
   
   return true;
}


bool Socket::accept (Socket& new_socket) const
{
   MARK;
   int addr_length = sizeof (m_addr);
   new_socket.m_sock =
#ifdef PLEXIL_VXWORKS
     ::accept (m_sock, (sockaddr *) &m_addr, &addr_length);
#else
     ::accept (m_sock, (sockaddr *) &m_addr, (socklen_t *) &addr_length);
#endif
   
   if (new_socket.m_sock <= 0)
      return false;
   else
      return true;
}


bool Socket::send (const std::string s) const
{
   return ::send (m_sock, s.c_str(), s.size(), SOCK_MSG_NOSIGNAL) != -1;
}

bool Socket::send (char* msg, size_t size) const
{
   return ::send(m_sock, msg, size, SOCK_MSG_NOSIGNAL) != -1;
}

int Socket::recv (std::string& s) const
{
   char buf [ MAXRECV + 1 ];
   
   s = "";
   
   memset (buf, 0, MAXRECV + 1);
   
   int status = ::recv (m_sock, buf, MAXRECV, 0);
   
   if (status == -1)
   {
      std::cout << "status == -1   errno == " << errno << "  in Socket::recv\n";
      return 0;
   }
   else if (status == 0)
   {
      return 0;
   }
   else
   {
      s = buf;
      return status;
   }
}

// bool Socket::isOpen() const
// {
//    char buf[2];
   
//    int status = ::recv (m_sock, buf, 1, 0); //MSG_OOB | MSG_PEEK | MSG_WAITALL);
//    SHOW(status);
   
//    return true;

//    //int sval;
//    fd_set check_set;
//    struct timeval to;
   

//    FD_ZERO(&check_set);
//    FD_SET(m_sock,&check_set);

//    to.tv_sec = 0;
//    to.tv_usec = 1;
   
//    int ret_val = select(m_sock,&check_set,0,0,&to);
//    SHOW(ret_val);

//    return ret_val == 0;
//}

bool Socket::connect (const std::string host, const uint16_t port)
{
   if (! is_valid()) return false;
   
   m_addr.sin_family = AF_INET;
   m_addr.sin_port = htons (port);
   
   int status = inet_pton (AF_INET, host.c_str(), &m_addr.sin_addr);
   if (status != 1 || errno == EAFNOSUPPORT)
     return false;
   
   status = ::connect (m_sock, (sockaddr *) &m_addr, sizeof (m_addr));
   
   if (status == 0)
      return true;
   else
      return false;
}

void Socket::set_non_blocking (const bool b)
{
   
   int opts;
   
   opts = fcntl (m_sock,
         F_GETFL);
   
   if (opts < 0)
   {
      return;
   }
   
   if (b)
      opts = (opts | O_NONBLOCK);
   else
      opts = (opts & ~O_NONBLOCK);
   
   fcntl (m_sock,
      F_SETFL,opts);
   
}
