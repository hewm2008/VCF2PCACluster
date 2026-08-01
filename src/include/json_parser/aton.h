//: ----------------------------------------------------------------------------
//: Copyright (C) 2015 Verizon.  All Rights Reserved.
//:
//: \file:    aton.h
//: \details: A templated subbuffer to number conversion function
//: \author:  Donnevan "Scott" Yeager 
//: \date:    06/06/2013
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

#ifndef ATON_H
#define ATON_H

#include "aton_internal.h"
#include "subbuffer.h"

/**
  @brief A templated subbuffer to number conversion function.
  This allows a string like "1234567890" to be split into "12345" and "67890"
  then be converted to 12345 and 67890 without the need for temporary buffers

  @code
	subbuffer data("123 N 1st St");
	uint32_t val = aton<uint32_t>(data);
	// val now contains 123
  @endcode
 */
template<typename NUMB> NUMB aton(subbuffer sb, subbuffer* remainder = NULL, uint64_t base = 10)
{
        const char* rem = NULL;
        NUMB val;
        bool ret = aton_internal<NUMB>(val, sb.begin(), sb.length(), &rem, base);
        // if ret == true then any remainder will be inside of sb
        // else rem will be a NULL terminated error string
        if (remainder) *remainder = ret ? subbuffer(rem, rem ? sb.length() - (rem - sb.begin()) : 0) : subbuffer(rem);
        return val;
}

#endif
