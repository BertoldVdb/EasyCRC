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

/* Include the types of CRC you want to use */
#include "EasyCRC/Types/crc32_c.h"
#include "EasyCRC/Types/crc16_a.h"
#include "EasyCRC/Types/crc8.h"

/* Only needed for table export */
#include "EasyCRC/EasyCRCExtra.h"

#include <cstdint>
#include <string>
#include <iostream>
#include <vector>

void testResult(uint64_t result, uint64_t correct) {
    std::cout<<std::setw(8)<<std::hex<<result<<" "<<std::dec;
    if(result == correct) {
        std::cout << "(CORRECT)" << std::endl;
    } else {
        std::cout << "(WRONG)\n" << std::endl;
    }

}

int main() {
    /*
     * Create the CRC object giving the CRC type as template parameter.
     * Load data in it using operator<<(uint8_t), operator<<(vector) or
     * loadBytes.
     * The result is obtained by calling result(). To reuse the object
     * you should first call reset to clear the internal state. If you
     * want to calculate an incremental CRC you can load some bytes,
     * get the result, load some more and request the result again.
     */
    std::cout << "32 Bit CRC (<< input):     ";
    EasyCRC::Calculator<EasyCRC::Type::CRC32_C> crc32;
    crc32 << 0x00;
    crc32 << 0x01;
    crc32 << 0x02;
    crc32 << 0x03;
    testResult(crc32.result(), 0xd9331aa3);

    std::cout << "32 Bit CRC (reuse):        ";
    crc32.reset();
    crc32 << 0x00;
    crc32 << 0x01;
    crc32 << 0x02;
    crc32 << 0x03;
    testResult(crc32.result(), 0xd9331aa3);

    std::cout << "16 Bit CRC (array input):  ";
    EasyCRC::Calculator<EasyCRC::Type::CRC16_A> crc16;
    uint8_t inputData[] = {0x00, 0x01, 0x02, 0x03};
    crc16.loadBytes(inputData, sizeof(inputData));
    testResult(crc16.result(), 0xdf7);

    std::cout << " 8 Bit CRC (vector input): ";
    EasyCRC::Calculator<EasyCRC::Type::CRC8> crc8;
    std::vector<uint8_t> inputVector;
    inputVector.push_back(0x00);
    inputVector.push_back(0x01);
    inputVector.push_back(0x02);
    inputVector.push_back(0x03);
    crc8 << inputVector;
    testResult(crc8.result(), 0x48);
    
    /* 
     * Using EasyCRCExtra.h you can generate a C file containing the lookup table, 
     * for example to deploy on an embedded system where you have a lot of flash
     * and little RAM
     */
     std::string tableC = EasyCRC::exportLookupTableC(crc8, 16);
     std::cout << std::endl << std::endl << tableC;
}


