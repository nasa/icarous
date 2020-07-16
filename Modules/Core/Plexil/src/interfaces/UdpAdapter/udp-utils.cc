/* Copyright (c) 2006-2015, Universities Space Research Association (USRA).
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

#include "plexil-config.h"

#include "udp-utils.hh"

#include <cstring>
#include <cerrno>
#include <unistd.h> // for close()
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

namespace PLEXIL
{
  //
  // 32 bit versions of float and int conversions
  //

  long int float_to_long_int (float num)
  {
    //printf("\n\nsizeof(long int) = %d, sizeof(float) = %d\n\n", sizeof(long int), sizeof(float));
    float x = num;
    void* y = &x;
    return *(long int *) y;
  }

  float long_int_to_float (long int num)
  {
    long int x = num;
    void* y = &x;
    return *(float *) y;
  }

  int network_bytes_to_number(const unsigned char* buffer, int start_index, int total_bits, bool is_signed=true, bool debug)
  {
    int value = 0;
    int cursor = start_index;
    for (int i = total_bits - 8 ; i >= 0 ; i -= 8)
      {
        if (debug) printf("buffer[%d]==%d; shift>> %d bits; ", cursor, buffer[cursor], i);
        value += (buffer[cursor++] & 0xff) << i;
        if (debug) std::cout << "; value=" << value << std::endl;
      }
    if (is_signed && (value >= pow(2.0,total_bits)/2.0))
      {
        value -= (int) pow(2.0, total_bits);
      }
    return value;
  }

  void number_to_network_bytes(int number, unsigned char* buffer, int start_index, int total_bits, bool debug)
  {
    
    int cursor = start_index;
    for (int i = total_bits - 8 ; i >= 0 ; i -= 8)
      {
        if (debug) std::cout << "number=" << number << ": shift>> " << i << " bits; ";
        buffer[cursor++] = (int)((number >> i) & 0xff);
        if (debug) printf("buffer[%d]==%d\n", cursor, buffer[cursor - 1]);
      }
  }

  void encode_long_int(long int long_int, unsigned char* buffer, int start_index)
  // Encode a 32 bit integer (in network byte order)
  {
    number_to_network_bytes(htonl(long_int), buffer, start_index, 32, false);
  }

  void encode_short_int(long int long_int, unsigned char* buffer, int start_index)
  // Encode a 16 bit integer (in network byte order)
  {
    number_to_network_bytes(htons(long_int), buffer, start_index, 16, false);
  }

  long int decode_long_int(const unsigned char* buffer, int start_index)
  // Decode a 32 bit integer from the network bytes in host byte order
  {
    long int temp;
    temp = network_bytes_to_number(buffer, start_index, 32, false, false);
    return ntohl(temp);
  }

  short int decode_short_int(const unsigned char* buffer, int start_index)
  // Decode a 32 bit integer from the network bytes in host byte order
  {
    long int temp;
    temp = network_bytes_to_number(buffer, start_index, 16, false, false);
    return ntohs(temp);
  }

  void encode_float(float num, unsigned char* buffer, int start_index)
  // Encode a 32 bit float in network byte order
  {
    long int temp = htonl(float_to_long_int(num));
    number_to_network_bytes(temp, buffer, start_index, 32, false);
  }

  float decode_float(const unsigned char* buffer, int start_index)
  // Decode a 32 bit float from network byte order
  {
    // ntohl called in decode_long_int
    long int temp = decode_long_int(buffer, start_index);
    return long_int_to_float(temp);
  }

  void encode_string(const std::string& str, unsigned char* buffer, int start_index)
  {
    // Note that this DOES NOT encode a c string.  You can do that on your own.
    str.copy((char*)&buffer[start_index], str.length(), 0);
  }

  std::string decode_string(const unsigned char* buffer, int start_index, int length)
  {
    // This decoder stops at \0 or length, which ever comes first.  The \0 is never included.
    std::string str;
    for (int i = start_index ; i < start_index + length ; i++ )
      {
        unsigned char c = buffer[i];
        if (c == 0) break;
        str += c;
      }
    return str;
  }

  void print_buffer(const unsigned char* buffer, int bytes, bool fancy)
  {
    printf("#(");
    for (int i = 0 ; i < bytes ; i++)
      {
        if (i != 0) printf(" "); // print a space prior to the char to ease reading
        {
          if ((fancy==true) && ((32 < buffer[i]) || (buffer[i] > 126)))
            printf("%c", (unsigned int) buffer[i]);
          else
            printf("%d", (unsigned int) buffer[i]);
        }
      }
    printf(")\n");
  }

  int send_message_bind(int local_port, const char* peer_host, int peer_port, const char* buffer, size_t size, bool debug)
  {
    if (debug) printf("  send_message_bind(%d, %s, %d, buffer, %d) called\n", local_port, peer_host, peer_port, (int) size);
    // Set the local port
    struct sockaddr_in local_addr = {};
    memset((char *) &local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(local_port);
    // Set the peer port
    struct sockaddr_in peer_addr = {};
    memset((char *) &peer_addr, 0, sizeof(peer_addr));
    peer_addr.sin_port = htons(peer_port);
    peer_addr.sin_family = AF_INET;

    // handle either "localhost" or "127.0.0.1" addresses
    hostent *host_ip = gethostbyname(peer_host);
    if (host_ip == NULL) 
      {
        perror("send_message_connect: gethostbyname failed");
        return -1;
      }

    in_addr *network_ip_address = (in_addr*)host_ip->h_addr;
    std::string ip_addr = inet_ntoa(*network_ip_address);
    if (debug) printf("  send_message_bind: peer_host==%s, ip_addr==%s\n", peer_host, ip_addr.c_str());

    if (!inet_aton(ip_addr.c_str(), (struct in_addr *)&peer_addr.sin_addr.s_addr))
      {
        perror("inet_aton() returned -1 (peer_host bad IP address format?)");
        return -1;
      }

    int sock = socket(local_addr.sin_family, SOCK_DGRAM, 0);
    if (sock == -1)
      {
        perror("socket() returned -1");
        return -1;
      }

    int bind_err = bind(sock, (struct sockaddr *) &local_addr, sizeof(local_addr));
    if (bind_err < 0)
      {
        //char buf[50];
        //sprintf(buf, "send_message_bind: bind() returned -1 for %d", local_port);
        //perror(buf);
        perror("send_message_bind: bind() returned -1");
        return -1;
      }

    ssize_t bytes_sent = 0;
    bytes_sent = sendto(sock, buffer, size, 0, (struct sockaddr*) &peer_addr, sizeof(peer_addr));
    if (debug) printf("  send_message_bind: sent %ld bytes to %s:%d\n", (long) bytes_sent, peer_host, peer_port);
    close(sock);
    return bytes_sent;
  }

  int send_message_connect(const char* peer_host, int peer_port, const char* buffer, size_t size, bool debug)
  {
    struct sockaddr_in peer_addr = {};
    memset((char *) &peer_addr, 0, sizeof(peer_addr));
    peer_addr.sin_port = htons(peer_port);
    peer_addr.sin_family = AF_INET;

    // handle either "localhost" or "127.0.0.1" addresses
    hostent *host_ip = gethostbyname(peer_host);
    if (host_ip == NULL) 
      {
        perror("send_message_connect: gethostbyname failed");
        return -1;
      }

    in_addr *network_ip_address = (in_addr*)host_ip->h_addr;
    std::string ip_addr = inet_ntoa(*network_ip_address);

    if (!inet_aton(ip_addr.c_str(), (struct in_addr *)&peer_addr.sin_addr.s_addr))
      {
        perror("inet_aton() returned -1 (peer_host bad IP address format?)");
        return -1;
      }

    int sock = socket(peer_addr.sin_family, SOCK_DGRAM, 0);
    if (sock == -1)
      {
        perror("socket() returned -1");
        return -1;
      }

    int connect_err = connect(sock, (struct sockaddr *)&peer_addr, sizeof(peer_addr));
    if (connect_err < 0)
      {
        perror("connect() returned -1");
        return -1;
      }

    ssize_t bytes_sent = 0;
    bytes_sent = send(sock, buffer, size, 0);
    if (debug) printf("  send_message_connect: sent %ld bytes to %s:%d\n", (long)bytes_sent, peer_host, peer_port);
    close(sock);
    return bytes_sent;
  }

  int wait_for_input_on_thread(udp_thread_params* params)
  {
    int status;
    status = wait_for_input(params->local_port, params->buffer, params->size, params->sock, params->debug);
    return status;
  }

  int wait_for_input(int local_port, unsigned char* buffer, size_t size, int sock, bool debug)
  {
    if (debug)
      printf("  wait_for_input(%d, buffer, %d, %d) called\n", local_port, (int) size, sock);
    // Set up an appropriate local address (port)
    struct sockaddr_in local_addr = {};
    memset((char *) &local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(local_port);
    // Set up the storage for the peer address
    struct sockaddr_in peer_addr = {};
    memset((char *) &peer_addr, 0, sizeof(peer_addr));
    // Since the socket must be closed by the thread which spawned this thread, socket creating has
    // moved up to UdpAdapter::startUdpMessageReceiver
    // int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    // if (sock < 0) { perror("socket() returned -1"); return sock; }

    // Bind to the socket
    int bind_err = bind(sock, (struct sockaddr *) &local_addr, sizeof(local_addr));
    if (bind_err < 0) {
      char buf[50];
      sprintf(buf, "wait_for_input: bind() returned -1 for %d", local_port);
      perror(buf);
      return bind_err;
    }

    // Wait for input to become available and then read from the socket
    if (debug)
       printf("  wait_for_input calling recvfrom %s:%d on file %d\n",
	      inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port), sock);

    socklen_t slen = sizeof(struct sockaddr_in);
    int bytes_read = recvfrom(sock, buffer, size, 0, (struct sockaddr *) &peer_addr, &slen);
    if (bytes_read < 0) {
      char buf[80];
      sprintf(buf, "wait_for_input: recvfrom(%d) returned -1, errno %d", sock, errno);
      perror(buf);
      close(sock);
      return bytes_read;
    }
    if (debug) printf("  wait_for_input(%d, buffer, %d) received %d bytes from %s:%d\n",
                      local_port, (int) size, bytes_read,
                      inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port));
    close(sock);
    return 0;
  }
}

// EOF
