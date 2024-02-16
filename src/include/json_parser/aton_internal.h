//: ----------------------------------------------------------------------------
//: Copyright (C) 2015 Verizon.  All Rights Reserved.
//:
//: \file:    aton_internal.h
//: \details: A templated const char* to number conversion function
//: \author:  Donnevan "Scott" Yeager
//: \date:    05/22/2014
//:
//:   Licensed under the Apache License, Version 2.0 (the "License");
//:   you may not use this file except in compliance with the License.
//:   You may obtain a copy of the License at
//:
//:       http://www.apache.org/licenses/LICENSE-2.0
//:
//:   Unless required by applicable law or agreed to in writing, software
//:   distributed under the License is distributed on an "AS IS" BASIS,
//:   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//:   See the License for the specific language governing permissions and
//:   limitations under the License.
//:

#ifndef ATON_INTERNAL_H
#define ATON_INTERNAL_H

#include <stdio.h>

#include <algorithm>
#include <stdint.h>
#include <limits.h>

#include <limits>

#ifndef likely
#define likely(x)   __builtin_expect((x),1)
#endif

#ifndef unlikely
#define unlikely(x) __builtin_expect((x),0)
#endif
static const uint8_t s_aton_conversion_table[256] = {
  UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,      // 0
  UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,      // 8
  UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,      // 16
  UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,      // 24
  UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,      // 32
  UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,   UCHAR_MAX,      // 40
 0,  1,  2,  3,  4,  5,  6,  7, 8, 9,                                                                        // 48
 UCHAR_MAX,  UCHAR_MAX,  UCHAR_MAX,  UCHAR_MAX,  UCHAR_MAX,  UCHAR_MAX,                                      // 58
 UCHAR_MAX,                                                                                                  // 64
// A,   B,   C,  D,  E,  F,  G,  H,  I,  J,  K,  L,  M,  N,  O,  P,  Q,  R,  S,  T,  U,  V,  W,  X,  Y,  Z  
  10,  11,  12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,  // 65
  UCHAR_MAX,  UCHAR_MAX,  UCHAR_MAX,  UCHAR_MAX,  UCHAR_MAX,                                                 // 91
 UCHAR_MAX,                                                                                                  // 96
// a,   b,   c,  d,  e,  f,  g,  h,  i,  j,  k,  l,  m,  n,  o,  p,  q,  r,  s,  t,  u,  v,  w,  x,  y,  z  
  10,  11,  12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,  // 97
 UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX,                                                      // 123
 UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX,                     // 128
 UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX,                     // 136
 UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX,                     // 144
 UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX,                     // 152
 UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX,                     // 160
 UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX,                     // 168
 UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX,                     // 176
 UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX,                     // 184
 UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX,                     // 192
 UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX,                     // 200
 UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX,                     // 208
 UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX,                     // 216
 UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX,                     // 224
 UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX,                     // 232
 UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX,                     // 240
 UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX, UCHAR_MAX                      // 248
};

static inline uint8_t get_char_val(char c)
{
        if (likely(c >= '0' && c <= '9'))
                return c - '0';
        if (c >= 'A' && c <= 'Z')
                return 10 + (c - 'A');
        if (c >= 'a' && c <= 'z')
                return 10 + (c - 'a');
        return std::numeric_limits<uint8_t>::max();
}

template<typename NUMB> bool aton_unsigned(NUMB& value, const char* str, size_t len, const char** remainder = NULL, uint64_t base = 10)
{
        if (unlikely(!str || !len))
        {
                value = 0;
                return false;
        }
        if (remainder) *remainder = NULL;
        uint64_t val = 0;
        if (unlikely(*str == '-'))
        {
                if (remainder) *remainder = "ATON_UNDERFLOW";
                value = 0;
                return false;
        }
        if (unlikely(*str == '+'))
        {
                ++str;
                --len;
        }
        size_t i = 0;
        for (; i < len; ++i)
        {
                uint8_t cv = s_aton_conversion_table[uint8_t(str[i])];
                if (unlikely(cv >= base))
                {
                        if (remainder) *remainder = str + i;
                        break;
                }
                if (unlikely(i > 10 && i == (size_t)std::numeric_limits<NUMB>::digits10))
                {
                        uint64_t mx = uint64_t(std::numeric_limits<NUMB>::max());
                        if (val > mx / 10 ||
                            mx - (val * base) < cv)
                        {
                                if (remainder) *remainder = "ATON_OVERFLOW";
                                value = std::numeric_limits<NUMB>::max();
                                return false;
                        }
                }
                if (likely(i != 0)) val *= base;
                val += cv;
        }
        if (unlikely(i > (size_t)std::numeric_limits<NUMB>::digits10 + 1 ||
            (i > (size_t)std::numeric_limits<NUMB>::digits10 && val > uint64_t(std::numeric_limits<NUMB>::max()))))
        {
                if (remainder) *remainder = "ATON_OVERFLOW";
                value = std::numeric_limits<NUMB>::max();
                return false;
        }
        value = NUMB(val);
        return true;
}

template<typename NUMB> bool aton_signed(NUMB& value, const char* str, size_t len, const char** remainder = NULL, uint64_t base = 10)
{
        if (unlikely(!str || !len))
        {
                value = 0;
                return false;
        }
        if (remainder) *remainder = NULL;
        uint64_t val = 0;
        bool negative = *str == '-';
        if (negative || *str == '+')
        {
                ++str;
                --len;
        }

        size_t i = 0;
        for (; i < len; ++i)
        {
                uint8_t cv = s_aton_conversion_table[uint8_t(str[i])];
                if (unlikely(cv >= base))
                {
                        if (remainder) *remainder = str + i;
                        break;
                }
                if (likely(i != 0)) val *= base;
                val += cv;
        }

        if (likely(!negative))
        {
                if (unlikely(i > (size_t)std::numeric_limits<NUMB>::digits10 + 1 ||
                             (i > (size_t)std::numeric_limits<NUMB>::digits10 && val > uint64_t(std::numeric_limits<NUMB>::max()))))
                {
                        if (remainder) *remainder = "ATON_OVERFLOW";
                        value = NUMB(std::numeric_limits<NUMB>::max());
                        return false;
                }

                value = NUMB(val);
                return true;
        }

        int64_t ret(val);
        if (negative) ret *= -1;
        if (i > (size_t)std::numeric_limits<NUMB>::digits10 + 1 ||
            ret < int64_t(std::numeric_limits<NUMB>::min()))
        {
                if (remainder) *remainder = "ATON_UNDERFLOW";
                value = NUMB(std::numeric_limits<NUMB>::min());
                return false;
        }
        value = NUMB(ret);
        return true;
}

template<typename NUMB> bool aton_float(NUMB& value, const char* str, size_t len, const char** remainder = NULL)
{
        if (unlikely(!str || !len))
        {
                value = 0.0;
                return false;
        }
        if (remainder) *remainder = NULL;
        uint64_t lval = 0;
        uint64_t rval = 0;
        uint64_t rdiv = 1;
        bool negative = *str == '-';
        if (negative || *str == '+')
        {
                ++str;
                --len;
        }
        ssize_t dot_pos = -1;
        size_t i = 0;
        for (; i < len; ++i)
        {
                char c = str[i];
                if ('.' == c)
                {
                        dot_pos = i;
                        ++i;
                        break;
                }
                uint8_t cv = s_aton_conversion_table[uint8_t(c)];
                //uint8_t cv = get_char_val(c);
                if (unlikely(cv >= 10))
                {
                        if (remainder) *remainder = str + i;
                        break;
                }
                if (i != 0) lval *= 10;
                lval += cv;
        }

        double val(lval);

        // now handle everything after the dot
        if (-1 != dot_pos)
        {
                for (; i < len; ++i)
                {
                        char c = str[i];
                        uint8_t cv = s_aton_conversion_table[uint8_t(c)];
                        //uint8_t cv = get_char_val(c);
                        if (unlikely(cv >= 10))
                        {
                                if (remainder) *remainder = str + i;
                                break;
                        }
                        if (i - dot_pos > 1) rval *= 10.0;
                        rval += cv;
                        rdiv *= 10;
                }
                val += double(rval) / rdiv;
        }

        if (!negative && val > double(std::numeric_limits<NUMB>::max()))
        {
                if (remainder) *remainder = "ATON_OVERFLOW";
                value = NUMB(std::numeric_limits<NUMB>::max());
                return false;
        }

        if (!negative)
        {
                value = NUMB(val);
                return true;
        }

        val *= -1;
        if (val < double(-std::numeric_limits<NUMB>::max()))
        {
                if (remainder) *remainder = "ATON_UNDERFLOW";
                value = NUMB(std::numeric_limits<NUMB>::min());
                return false;
        }
        value = NUMB(val);
        return true;
}

//
// NOTE: You should not be here, you should be looking in aton.h or subbuffer::aton
//

/**
  @brief A templated const char* + length to number conversion function.
  This allows a string like "1234567890" to be split into "12345" and "67890"
  then be converted to 12345 and 67890 without the need for temporary buffers

  @code
        const char* str = "123 N 1st St";
        uint32_t val = 0;
        if (!aton<uint32_t>(val, str, strlen(str)))
        {
                // error logic
        }
        // val now contains 123
  @endcode
 */
template<typename NUMB> bool aton_internal(NUMB& val, const char* str, size_t len, const char** remainder = NULL, uint64_t base = 10);
template<> inline bool aton_internal<uint8_t>(uint8_t& val, const char* str, size_t len, const char** remainder, uint64_t base) { return aton_unsigned(val, str, len, remainder, base); }
template<> inline bool aton_internal<uint16_t>(uint16_t& val, const char* str, size_t len, const char** remainder, uint64_t base) { return aton_unsigned(val, str, len, remainder, base); }
template<> inline bool aton_internal<uint32_t>(uint32_t& val, const char* str, size_t len, const char** remainder, uint64_t base) { return aton_unsigned(val, str, len, remainder, base); }
template<> inline bool aton_internal<uint64_t>(uint64_t& val, const char* str, size_t len, const char** remainder, uint64_t base) { return aton_unsigned(val, str, len, remainder, base); }

template<> inline bool aton_internal<int8_t>(int8_t& val, const char* str, size_t len, const char** remainder, uint64_t base)  { return aton_signed(val, str, len, remainder, base); }
template<> inline bool aton_internal<int16_t>(int16_t& val, const char* str, size_t len, const char** remainder, uint64_t base) { return aton_signed(val, str, len, remainder, base); }
template<> inline bool aton_internal<int32_t>(int32_t& val, const char* str, size_t len, const char** remainder, uint64_t base) { return aton_signed(val, str, len, remainder, base); }
template<> inline bool aton_internal<int64_t>(int64_t& val, const char* str, size_t len, const char** remainder, uint64_t base) { return aton_signed(val, str, len, remainder, base); }
template<> inline bool aton_internal<double>(double& val, const char* str, size_t len, const char** remainder, uint64_t /*base*/) { return aton_float(val, str, len, remainder); }
template<> inline bool aton_internal<float>(float& val, const char* str, size_t len, const char** remainder, uint64_t /*base*/) { return aton_float(val, str, len, remainder); }


#endif
