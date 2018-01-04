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


#include <string>
#include <iomanip>
#include <atomic>
#include <thread>
#include <vector>

#ifndef EASYCRC_H_
#define EASYCRC_H_

namespace EasyCRC {
namespace detail {
template <typename CRCType> using CRCResultType = typename std::remove_cv<decltype(CRCType::polynomial)>::type;
}

template <typename CRCType> class Calculator {
public:
    using CRCResultTypeS = detail::CRCResultType<CRCType>;

    Calculator() {
        reset();
        initLookupTable();
    }

    void reset() {
        shiftReg_ = CRCType::initialValue;
    }

    void operator<<(uint8_t inputByte) {
        /* Reflect the input bytes if needed */
        if(CRCType::reflectInput) {
            inputByte = reflectByte(inputByte);
        }

        /* Look up pre-calculated result */
        shiftReg_ = (shiftReg_ << 8) ^ lookupTable_[shiftReg_ >>
                    (sizeof(CRCResultTypeS)-1)*8 ^ inputByte];
    }

    void operator<<(std::vector<uint8_t> vector) {
        loadBytes(vector.data(), vector.size());
    }

    void loadBytes(const uint8_t* bytes, size_t length) {
        for(unsigned int i=0; i<length; i++) {
            operator<< (bytes[i]);
        }
    }

    CRCResultTypeS result() const {
        CRCResultTypeS shiftReg = shiftReg_;

        if(CRCType::reflectOutput) {
            /*
             * If it is only a single byte we just reflect it, otherwise we need
             * to swap the reflected bytes as well.
             */
            if(sizeof(CRCResultTypeS) == 1) {
                shiftReg = reflectByte(shiftReg);
            } else {
                unsigned char* outputBytes = reinterpret_cast<unsigned char*>(&shiftReg);

                for(unsigned int i=0; i<sizeof(CRCResultTypeS)/2; i++) {
                    unsigned char tmp = reflectByte(outputBytes[i]);
                    outputBytes[i] = reflectByte(outputBytes[sizeof(CRCResultTypeS)-i-1]);
                    outputBytes[sizeof(CRCResultTypeS)-i-1] = tmp;
                }
            }
        }

        return shiftReg ^ CRCType::finalXOR;
    }

    CRCResultTypeS getLookupTableValue(uint8_t index) const {
        /* Return the value */
        return lookupTable_[index];
    }

private:
    CRCResultTypeS shiftReg_;

    static CRCResultTypeS lookupTable_[256];
    static std::atomic_flag lookupTableFirst_;
    static std::atomic<bool> lookupTableDone_;

    void initLookupTable() const {
        /* Is the table done? */
        if(lookupTableDone_.load(std::memory_order_acquire)) {
            return;
        }

        /* Check if we need to make it */
        if(!lookupTableFirst_.test_and_set()) {
            lookupTable_[0] = 0;
            for(unsigned int i=1; i<256; i++) {
                /* Apply polynomial division to shift register with the LSB and zeros */
                lookupTable_[i] = doPolynomialDivision(i);
            }

            lookupTableDone_.store(true, std::memory_order_release);
        } else {
            /* Wait if we are not first and the table is not yet built by the first thread */
            while(!lookupTableDone_.load(std::memory_order_acquire)) {
                std::this_thread::yield();
            }
        }
    }

    CRCResultTypeS doPolynomialDivision(CRCResultTypeS input) const {
        for(unsigned int i=0; i<sizeof(CRCResultTypeS)*8; i++) {
            bool bitOut = input >> (sizeof(CRCResultTypeS)*8-1);
            input <<= 1;
            if(bitOut) {
                input ^= CRCType::polynomial;
            }
        }
        return input;
    }

    uint8_t reflectByte(uint8_t x) const {
        x = (x & 0x0F) << 4 | (x & 0xF0) >> 4;
        x = (x & 0x33) << 2 | (x & 0xCC) >> 2;
        x = (x & 0x55) << 1 | (x & 0xAA) >> 1;
        return x;
    }
};

template <typename CRCType> detail::CRCResultType<CRCType> Calculator<CRCType>::lookupTable_[256];
template <typename CRCType> std::atomic_flag Calculator<CRCType>::lookupTableFirst_;
template <typename CRCType> std::atomic<bool> Calculator<CRCType>::lookupTableDone_ (false);

}


#endif /* CRCCALCULATOR_H_ */
