//: ----------------------------------------------------------------------------
//: Copyright (C) 2015 Verizon.  All Rights Reserved.
//:
//: \file:    chargrp.h
//: \details: A binary predicate for checking if a char is in a group of chars 
//: \author:  Donnevan "Scott" Yeager
//: \date:    06/07/2013
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

#ifndef _CHARGRP_H_
#define _CHARGRP_H_

#include <stdint.h>
#include <string.h>

//static uint8_t s_BYTE_VALS[] = {1,2,4,8,16,32,64,128};

/**
  @brief Simple, lightweight class for testing the value of a char.

  By itself this class is not too usefull but when combined with string manipulation
  methods it comes in handy.

  I've gone back and forth with using a bit array vs a byte array vs a bool array
  and have not seen large enough gains (yet) from any to favour it for performance.
  So I'm currently using a bool array for cleanliness.

  @code
  chargrp spc_dot_slash(" ./");
  const char* path = "    ./helloworld.txt";

  while (spc_dot_slash.contains(*path))
      ++path;

  path should now equal "helloworld.txt"
  @endcode
 */
class chargrp
{
public:
        chargrp() : m_len(0)
        {
                clear();
        }

        explicit chargrp(const char* szGrp) : m_len(0)
        {
                clear();
                add(szGrp);
        }

        /**
          @brief Clears the interal memory.
          */
        void clear()
        {
                memset(m_vals, 0, sizeof(bool) * 256);
                m_len = 0;
        }

        /**
          @brief Adds the chars in szGrp to the cltn of chars.
          @param [in] szGrp Null terminated series of chars to add.
          */
        void add(const char* szGrp)
        {
                if (!szGrp) return;

                while (*szGrp)
                {
                        m_vals[uint8_t(*szGrp)] = true;
                        ++szGrp;
                        ++m_len;
                }
        }

        /**
          @brief Adds a single char to the cltn of chars.
          */
        void add(char val) { m_vals[uint8_t(val)] = true; ++m_len; }

        /**
          @brief Removes the chars in szGrp from the cltn.
          */
        void remove(const char* szGrp)
        {
                if (!szGrp) return;

                while (*szGrp)
                {
                        m_vals[uint8_t(*szGrp)] = false; 
                        ++szGrp;
                        --m_len;
                }
        }

        /**
          @brief Removes a single char from the cltn.
          */
        void remove(char val) { m_vals[uint8_t(val)] = false; --m_len; }

        /**
          @brief Checks if val is in the cltn.
          */
        inline bool contains(char val) const { return m_vals[uint8_t(val)]; }

        /**
          @brief Checks if val is in the cltn. Useful in methods requiring a predicate.
          */
        inline bool operator()(const char& val) const { return m_vals[uint8_t(val)]; }

        inline uint8_t length() const { return m_len; }

private:
        bool m_vals[256]; //!< One bool for each of the possible single byte values.
        uint8_t m_len;  //!< Number of chars in the group
};

#endif
