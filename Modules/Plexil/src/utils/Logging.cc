/* Copyright (c) 2006-2016, Universities Space Research Association (USRA).
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

#include "Logging.hh"
#include "Error.hh"
#include "lifecycle-utils.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>
#include <sstream>
#include <unistd.h> // for getpid(), isatty()

#ifdef HAVE_EXECINFO_H
#include <execinfo.h>
#endif

using PLEXIL::Error;

int Logging::ENABLE_LOGGING  = 0; // if != 0, enable messages to log file
int Logging::ENABLE_E_PROMPT = 0; // if != 0, enable error prompt messages
int Logging::ENABLE_W_PROMPT = 0; // if != 0, enable warning prompt messages

static const char *DEFAULT_LOG_FILE_NAME = "universalexec.log";

// Don't allocate until needed; if used, cleanup at exit
static char *FILE_NAME = NULL;   // global buffer
static size_t FILE_NAME_LEN = 0; // allocated size of above

// Locally defined functions
static const char* msg_type_name(int msg);
static void print_message(int msg_type, const char *fullmsg);
static const char *get_date_time();
static void prompt_user();
static void print_stack();
static void ensure_log_file_name();
static void purge();

void Logging::handle_message(int msg_type, const char * msg) 
{
  std::ostringstream msgbuf;
  msgbuf << msg_type_name(msg_type) << ": " << msg;
  print_message(msg_type, msgbuf.str().c_str());
}

void Logging::handle_message(int msg_type, const char * file, int offset, const char * msg) 
{
  std::ostringstream msgbuf;
  msgbuf << msg_type_name(msg_type);
  if (file)
    msgbuf << ": " << file;
  msgbuf << ":" << offset;
  if (msg)
    msgbuf << ": " << msg;
  print_message(msg_type, msgbuf.str().c_str());
}

void Logging::handle_message(int msg_type, const char * file, int line, int col, const char * msg) 
{
  std::ostringstream msgbuf;
  msgbuf << msg_type_name(msg_type);
  if (file)
    msgbuf << ": " << file;
  msgbuf << ":" << line << ":" << col;
  if (msg)
    msgbuf << ": " << msg;
  print_message(msg_type, msgbuf.str().c_str());
}

void Logging::print_to_log(char** run_command, int num) {
  std::ostringstream str;
  str << "user command: ";
  for (int i = 0; i < num; ++i)
    str << run_command[i] << " ";
  Logging::print_to_log(str.str().c_str());
}

void Logging::print_to_log(const char * fullmsg) 
{
  static bool sl_newSession = true;
  ensure_log_file_name();
  std::ofstream filestr(FILE_NAME, std::ios::app);
  std::streambuf *backup = std::cout.rdbuf();
  std::streambuf *psbuf = filestr.rdbuf();
  std::cout.rdbuf(psbuf);

  if (sl_newSession) {
    sl_newSession = false;
    std::cout << "================================================================================\n";
    std::cout << "Logging Session ID (PID): " << getpid() << "\n";
    std::cout << "================================================================================\n";
  }

  std::cout << get_date_time() << ": " << fullmsg << "\n";

  std::cout.rdbuf(backup);
  filestr.close();
}

void Logging::set_log_file_name(const char * fname) 
{
  if (!fname) {
    ensure_log_file_name();
    return;
  }

  // Ensure buffer is large enough
  size_t len = strlen(fname) + 1;
  if (!FILE_NAME || (FILE_NAME_LEN < len)) {
    static bool sl_allocated = false;
    char *oldFileName = FILE_NAME;
    FILE_NAME_LEN = len;
    FILE_NAME = (char *) new char[len];
    delete oldFileName;
    // If this is a first-time allocation, make sure it gets cleaned up.
    if (!sl_allocated) {
      plexilAddFinalizer(&purge);
      sl_allocated = true;
    }
  }
  strncpy(FILE_NAME, fname, FILE_NAME_LEN);
}

static const char* msg_type_name(int msg)
{
  switch (msg) {
  case Logging::LOG_ERROR:
    return "ERROR";
  case Logging::WARNING:
    return "WARNING";
  case Logging::INFO:
    return "INFO";
  default:
    return "UNKNOWN";
  }
}

static void print_message(int msg_type, const char *fullmsg)
{
  // write message to console
  Error::getStream() << fullmsg << std::endl;

  // write message to logfile if enabled
  if (Logging::ENABLE_LOGGING)
    Logging::print_to_log(fullmsg);

  switch (msg_type) {
  case Logging::LOG_ERROR:
    // prompt user
    if (Logging::ENABLE_E_PROMPT)
      prompt_user();
    break;

  case Logging::WARNING:
    // prompt user
    if (Logging::ENABLE_W_PROMPT)
      prompt_user();
    break;

  case Logging::INFO:
  default:
    // do nothing
    break;
  }
}

static void prompt_user() 
{
  do {
    char buf[16];
    Error::getStream() << " (pid:" << getpid()
                       << ") [E]xit, show [S]tack trace or [P]roceed: ";

    if (isatty(0) && isatty(1)) {
      if (!fgets(buf, 8, stdin))
	strcpy(buf, "E\n"); // go non-interactive if we don't get input
    }
    else
      strcpy(buf, "E\n");

    if ((buf[0] == 'E' || buf[0] == 'e') && buf[1] == '\n') {
      Error::getStream() << "aborting...\n";
      exit(0);
    }
    else if ((buf[0] == 'P' || buf[0] == 'p') && buf[1] == '\n')
      return;
    else if ((buf[0] == 'S' || buf[0] == 's') && buf[1] == '\n') {
      print_stack();
    }
  } while (1);
}

#define LOG_TIME_STRING_LEN 26

static const char *get_date_time() 
{
  static char sl_log_time[LOG_TIME_STRING_LEN] = "";
  time_t sl_rawtime;
  time(&sl_rawtime);
#ifdef HAVE_CTIME_R
#if defined(__VXWORKS__)
  static size_t sl_len = LOG_TIME_STRING_LEN;
  ctime_r(&sl_rawtime, sl_log_time, &sl_len);
#else
  ctime_r(&sl_rawtime, sl_log_time);
#endif
#else
  // *** TODO: do something sane if ctime_r() not available ***
#endif
  // Replace newline in result with null char
  char* retn = strchr(sl_log_time, '\n');
  if (retn != 0)
    *retn = '\0';
  return sl_log_time;
}

// Does nothing if runtime fails to support stack traces.
static void print_stack()
{
#ifdef HAVE_EXECINFO_H
  void *trace[16];
  int trace_size = backtrace(trace, 16);
  char **messages = backtrace_symbols(trace, trace_size);
  Error::getStream() << "Execution path:\n";
  if (Logging::ENABLE_LOGGING)
    Logging::print_to_log("Execution path:");
  for (int i = 0; i < trace_size; ++i) {
    Error::getStream() << messages[i] << "\n";
    if (Logging::ENABLE_LOGGING)
      Logging::print_to_log(messages[i]);
  }
  free(messages);
#endif
}

static void ensure_log_file_name()
{
  if (FILE_NAME == NULL)
    Logging::set_log_file_name(DEFAULT_LOG_FILE_NAME);
}

// Clean up any dynamic memory allocations
static void purge()
{
  char *filename = FILE_NAME;
  FILE_NAME = NULL;
  delete filename;
}
