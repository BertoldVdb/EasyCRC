/*
 * Copyright (c) 2017, Bertold Van den Bergh (vandenbergh@bertold.org)
 * All rights reserved.
 * This work has been developed to support research funded by
 * "Fund for Scientific Research, Flanders" (F.W.O.-Vlaanderen).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the author nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR DISTRIBUTOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "EasyCRC.h"

#include <iomanip>
#include <iostream>
#include <sstream>

#ifndef EASYCRCEXTRA_H_
#define EASYCRCEXTRA_H_

namespace EasyCRC {

template <typename CRCType> std::string exportLookupTableC(const Calculator<CRCType>& crc,
        unsigned int cols = 8,
        std::string name = "crcTable",
        std::string suffix = "") {
    using CRCResultTypeS = detail::CRCResultType<CRCType>;

    std::stringstream stream;
    stream << "static const uint"
           << 8*sizeof(CRCResultTypeS)
           << "_t " << name <<"[256] = {"
           << std::endl;

    for(unsigned int i = 0; i < 256; i++) {
        if(i % cols == 0) {
            stream << "    ";
        }

        stream << std::hex << "0x" << std::uppercase << std::setfill('0') <<
               std::setw(sizeof(CRCResultTypeS)*2) << std::right <<
               (int)crc.getLookupTableValue(i) << suffix;

        if(i != 255) {
            stream <<",";
        }

        if(i % cols == cols - 1 || i == 255) {
            stream << std::endl;
        } else {
            stream <<" ";
        }

    }

    stream << "};" << std::endl;
    return stream.str();
}

}

#endif
