/********************************************************************\
 * cli.cpp -- UNIX command line interface                           *
 *                                                                  *
 * Copyright (C) 2009 Kenneth Laskoski                              *
 *                                                                  *
\********************************************************************/
/** @file cli.cpp
    @brief UNIX command line interface
    @author Copyright (C) 2009 Kenneth Laskoski

    Use, modification, and distribution are subject
    to the Boost Software License, Version 1.0.  (See accompanying file
    LICENSE_1_0.txt or a copy at <http://www.boost.org/LICENSE_1_0.txt>.)
*/

#include "kashmir/uuid_gen.h"
#include "kashmir/system/ccmd5.h"
#include "kashmir/system/ccsha1.h"
#include "kashmir/system/devrand.h"

#include <iostream>
#include <fstream>

#include <unistd.h>

namespace
{
    int n = 1;
    int v = 4;

    std::ostream* outp = &std::cout;
    std::ofstream ofile;

    void parse_cmd_line(int argc, char *argv[])
    {
        int ch;
        char *p;

        while ((ch = getopt(argc, argv, "n:o:v:")) != -1) {
            switch (ch) {
                case 'n':
                    n = strtoul(optarg, &p, 10);
                    if (*p != '\0' || n < 1)
                        std::cerr << "invalid argument for option 'n'\n";
                    break;
                case 'o':
                    ofile.open(optarg);
                    outp = &ofile;
                    break;
                case 'v':
                    v = strtoul(optarg, &p, 10);
                    if (*p == '\0' && (3 <= v && v <= 5))
                        break;
                    std::cerr << "invalid argument for option 'v'\n";
                    // fall through
                default:
                    exit(1);
            }

            argv += optind;
            argc -= optind;
        }
    }
}

int main(int argc, char *argv[])
{
    using kashmir::system::ccmd5;
    using kashmir::system::ccsha1;
    using kashmir::system::DevRand;

    parse_cmd_line(argc, argv);
    std::ostream& out = *outp;

    kashmir::uuid_t uuid;

    if (v == 3)
    {
        ccmd5 md5engine;
        out << kashmir::uuid::generate(md5engine, uuid, "") << '\n';
        return 0;
    }

    if (v == 5)
    {
        ccsha1 sha1engine;
        out << kashmir::uuid::generate(sha1engine, uuid, "") << '\n';
        return 0;
    }

    DevRand devrand;
    for (int i = 0; i < n; i++)
    {
        devrand >> uuid;
        out << uuid << '\n';
    }

    return 0;
}
