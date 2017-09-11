/* Copyright (c) 2006-2017, Universities Space Research Association (USRA).
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

//  Copyright Notices

//  This software was developed for use by the U.S. Government as
//  represented by the Administrator of the National Aeronautics and
//  Space Administration. No copyright is claimed in the United States
//  under 17 U.S.C. 105.

//  This software may be used, copied, and provided to others only as
//  permitted under the terms of the contract or other agreement under
//  which it was acquired from the U.S. Government.  Neither title to nor
//  ownership of the software is hereby transferred.  This notice shall
//  remain on all copies of the software.

/**
   @file TestData.cc
   @author Will Edgington

   @brief Numerous declarations related to testing.
*/

#ifndef PLEXIL_NO_ERROR_EXCEPTIONS
/* Contains the rest of this file */

#include "TestData.hh"

using PLEXIL::Error;

void TestData::failedCompare(const std::string& /* macro*/,
			     const std::string& one,
                             const std::string& two,
                             const std::string& file,
			     const int& line) {
  std::cerr << file << ':' << line << ": comparison failed: "
            << one << " is not equal to " << two << std::endl;
}

void TestData::unexpectedException(const std::string& /* macro */,
				   const Error& exception,
                                   const std::string& file, 
				   const int& line) {
  std::cerr << file << ':' << line << ": unexpected exception " << exception << std::endl;
}

void TestData::missingException(const std::string& /* macro */, 
				const std::string& msg,
                                const std::string& file, 
				const int& line) {
  std::cerr << file << ':' << line << ": unexpected success; " << msg << std::endl;
}

void TestData::missingException(const std::string& /* macro */,
				const Error& exception,
                                const std::string& file,
				const int& line) {
  std::cerr << file << ':' << line << ": unexpected success; expected exception: "
            << exception << std::endl;
}

void TestData::wrongException(const std::string& /* macro */,
                              const Error& caughtException,
                              const Error& expectedException,
                              const std::string& file,
			      const int& line) {
  std::cerr << file << ':' << line << ": unexpected exception "
            << caughtException << " is not the expected " << expectedException << std::endl;
}

void TestData::correctException(const std::string& /* macro */,
                                const Error& exception) {
  std::cout << "Caught expected exception " << exception << '\n';
}

#endif /* PLEXIL_NO_ERROR_EXCEPTIONS */
