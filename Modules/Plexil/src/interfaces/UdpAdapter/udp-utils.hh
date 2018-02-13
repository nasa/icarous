/* Copyright (c) 2006-2013, Universities Space Research Association (USRA).
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

#include <stdio.h>
#include <iostream>           // cout
#include <math.h>             // pow()
#include <arpa/inet.h>        // htonl
#include <netdb.h>            // gethostbyname

namespace PLEXIL
{

  struct udp_thread_params
  {
    int local_port;
    unsigned char* buffer;
    size_t size;
    int sock;
    bool debug;
  };

  long int float_to_long_int (float num);
  float long_int_to_float (long int num);
  int network_bytes_to_number(const unsigned char* buffer, int start_index, int total_bits, bool is_signed, bool debug=false);
  void number_to_network_bytes(int number, unsigned char* buffer, int start_index, int total_bits, bool debug=false);
  void encode_long_int(long int num, unsigned char* buffer, int start_index);
  void encode_short_int(long int num, unsigned char* buffer, int start_index);
  long int decode_long_int(const unsigned char* buffer, int start_index);
  short int decode_short_int(const unsigned char* buffer, int start_index);
  void encode_float(float num, unsigned char* buffer, int start_index);
  float decode_float(const unsigned char* buffer, int start_index);
  void encode_string(const std::string& str, unsigned char* buffer, int start_index);
  std::string decode_string(const unsigned char* buffer, int start_index, int length);
  int udp_tests(void);
  void print_buffer(const unsigned char* buffer, int bytes, bool fancy=false);

  int send_message_connect(const char* peer_host, int peer_port, const char* buffer, size_t size, bool debug=false);
  int send_message_bind(int local_port, const char* peer_host, int peer_port, const char* buffer, size_t size, bool debug=false);
  int wait_for_input_on_thread(udp_thread_params* params);
  int wait_for_input(int local_port, unsigned char* buffer, size_t size, int sock, bool debug=false);
}

// EOF
