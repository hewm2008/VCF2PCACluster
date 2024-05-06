//: ----------------------------------------------------------------------------
//: Copyright (C) 2015 Verizon.  All Rights Reserved.
//:
//: \file:    subbuffer.h
//: \details: Object oriented handling of const char* + length (range).
//: \author:  Donnevan "Scott" Yeager
//: \date:    06/21/2013
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

#ifndef _SUBBUFFER_H_
#define _SUBBUFFER_H_

#include "aton_internal.h"
#include "chargrp.h"

#include <limits.h>
#include <stdio.h>

#ifndef likely
#define likely(x)   __builtin_expect((x),1)
#endif

#ifndef unlikely
#define unlikely(x) __builtin_expect((x),0)
#endif


#if __cplusplus > 199711L
#include <regex>
#endif

#include <string>


#define SUBBUF_LEN(x) ((x).begin()), ((x).length())

#define CONST_SUBBUF(x) subbuffer(x, sizeof(x) - 1)

/// use as in printf("%.*s", SUBBUF_FORMAT(mysubbuf))
#define SUBBUF_FORMAT(x) int((x).length()), ((x).begin())

/**
  @brief An enum for specifying case in comparisons
 */
enum case_sensitivity_t { CASE_INSENSITIVE, CASE_SENSITIVE };


static const signed char s_SUBBUF_FAST_TOLOWER_table[256] = {
  0,   1,   2,   3,   4,   5,   6,   7,
  8,   9,  10,  11,  12,  13,  14,  15,
 16,  17,  18,  19,  20,  21,  22,  23,
 24,  25,  26,  27,  28,  29,  30,  31,
 32,  33,  34,  35,  36,  37,  38,  39,
 40,  41,  42,  43,  44,  45,  46,  47,
 48,  49,  50,  51,  52,  53,  54,  55,
 56,  57,  58,  59,  60,  61,  62,  63,
 64,  97,  98,  99, 100, 101, 102, 103,
104, 105, 106, 107, 108, 109, 110, 111,
112, 113, 114, 115, 116, 117, 118, 119,
120, 121, 122,  91,  92,  93,  94,  95,
 96,  97,  98,  99, 100, 101, 102, 103,
104, 105, 106, 107, 108, 109, 110, 111,
112, 113, 114, 115, 116, 117, 118, 119,
120, 121, 122, 123, 124, 125, 126, 127,
-128, -127, -126, -125, -124, -123, -122, -121,
-120, -119, -118, -117, -116, -115, -114, -113,
-112, -111, -110, -109, -108, -107, -106, -105,
-104, -103, -102, -101, -100, -99, -98, -97,
-96, -95, -94, -93, -92, -91, -90, -89,
-88, -87, -86, -85, -84, -83, -82, -81,
-80, -79, -78, -77, -76, -75, -74, -73,
-72, -71, -70, -69, -68, -67, -66, -65,
-64, -63, -62, -61, -60, -59, -58, -57,
-56, -55, -54, -53, -52, -51, -50, -49,
-48, -47, -46, -45, -44, -43, -42, -41,
-40, -39, -38, -37, -36, -35, -34, -33,
-32, -31, -30, -29, -28, -27, -26, -25,
-24, -23, -22, -21, -20, -19, -18, -17,
-16, -15, -14, -13, -12, -11, -10,  -9,
 -8,  -7,  -6,  -5,  -4,  -3,  -2,  -1
};

#define SUBBUF_LOWERCASE_NOT_EQUAL(c, d)   (unsigned char)(c | 0x20) != (unsigned char)(d | 0x20)
#define TOLOWER(c)   s_SUBBUF_FAST_TOLOWER_table[(unsigned char)(c)]

struct caseless_compare
{
        inline bool operator() (const char a, const char b) const { return TOLOWER(a) == TOLOWER(b); }
};

struct caseless_less_than
{
        inline bool operator() (const char a, const char b) const { return TOLOWER(a) < TOLOWER(b); }
};
/**
  @brief Non-owned reference to a (possibly not null-terminated) substring

  Use to avoid unnecessary copies of substrings.

  Use to simplify complicated code.

  @note Remember that the source bytes are not copied into the subbuffer object and so
        the subbuffer is only valid for the lifetime of the original string.
 */
class subbuffer
{
public:
        static const size_t npos = ULONG_MAX;

public:
        /**
          @brief Constructor, the subbuffer is empty after this constructor.
         */
        subbuffer() :
                m_p(NULL), m_len(0)
        {}

        /**
          @brief Constructor
          @param [in] src A std::string that may or may not have content."
         */
        subbuffer(const std::string& src) :
                m_p(src.c_str()), m_len(src.length())
        {}

        /**
          @brief Constructor
          @param [in] p Pointer to zero or more chars."
          @param [in] len The number of chars that p points to. Defaults to subbuffer::npos."
         */
        subbuffer(const char* p, size_t len = npos)
                :
                m_p(p),
                m_len(m_p ? len : 0)
        {
                if (m_p && m_len == npos) m_len = strlen(m_p);
                if (m_len == npos) m_len = 0;
        }

        /**
          @brief Used to assign a new substring.
          @param [in] p Pointer to zero or more chars."
          @param [in] len The number of chars that p points to."
         */
        inline void set(const char* p, size_t len)
        {
                m_p = p;
                m_len = len;
                if (npos == m_len) m_len = 0;
        }

        /**
          @brief Used to assign a new substring.
          @param [in] sz Pointer to NULL terminated char array."
         */
        inline void set(const char* sz)
        {
                m_p = sz;
                m_len = (sz ? strlen(sz) : 0);
        }

        /**
          @brief Accessor for the underlying const char*

          @note Do not assume that the buffer is NULL terminated.

          This method was purposely named "begin" rather than "c_str" to
          remind the caller that the returned char* is not guaranteed to
          be NULL terminated.
         */
        inline const char* begin() const { return m_p; }

        /**
          @brief Access the underlying chars as an array.
          @param [in] i The index into the array.

          @note There is no bounds checking done in this method. If you need bounds checking use subbuffer::at(int i).
         */
        inline char operator[](int i) const { return m_p[i]; }

        /**
          @brief Accessor for the length of the buffer
         */
        inline size_t length() const { return m_len; }

        /**
          @brief Does the subbuffer have any characters in it?
          @returns false if the const ptr is NULL or has zero chars in it.
         */
        inline bool empty() const { return !m_len; }

        /**
          @brief Allows the distinction between having a pointer with zero length and having no pointer.
         */
        inline bool is_set() const { return m_p != NULL; }

        /**
          @brief Compares this subbuffer with the first len chars in ptr using char_case.
          @returns true if they match, false if they don't match.
         */
        inline bool equals(const char* ptr, size_t len) const
        {
                return m_len == len && std::equal(m_p, m_p + m_len, ptr);
        }

        /**
          @brief Compares this subbuffer with the first len chars in ptr using char_case.
          @param char_case CASE_SENSITIVE or CASE_INSENSITIVE.
          @returns true if they match, false if they don't match.
         */
        inline bool equals(const char* ptr, size_t len, case_sensitivity_t char_case) const
        {
                if (CASE_SENSITIVE == char_case) return this->equals(ptr, len);
                static caseless_compare cp;
                return m_len == len && std::equal(m_p, m_p + m_len, ptr, cp);
        }

        /**
          @brief Compares this subbuffer with sb.
          @returns true if they match, false if they don't match.
         */
        inline bool equals(const subbuffer& sb) const
        {
                return m_len == sb.m_len && std::equal(m_p, m_p + m_len, sb.m_p);
        }

        /**
          @brief Compares this subbuffer with sb using char_case.
          @returns true if they match, false if they don't match.
         */
        inline bool equals(const subbuffer& sb, case_sensitivity_t char_case) const
        {
                if (CASE_SENSITIVE == char_case) return this->equals(sb);
                static caseless_compare cp;
                return m_len == sb.m_len && std::equal(m_p, m_p + m_len, sb.m_p, cp);
        }

        inline bool equals_caseless(const subbuffer& sb) const
        {
                static caseless_compare cp;
                return m_len == sb.m_len && std::equal(m_p, m_p + m_len, sb.m_p, cp);
        }

#if __cplusplus > 199711L

        inline bool match(const std::regex& re) const
        {
                return m_len && std::regex_match(m_p, m_p + m_len, re);
        }

        inline bool search(const std::regex& re) const
        {
                return m_len && std::regex_search(m_p, m_p + m_len, re);
        }

#endif

        /**
          @brief Compares this subbuffer with sb using char_case.
          Does the comparison starting from the end of the strings to the front.
          So if your strings tend to differ near their ends this could save you some cpu cycles.
          @returns true if they match, false if they don't match-
                   starting from the last character in the string.
         */
        bool requals(const subbuffer& sb, case_sensitivity_t char_case = CASE_SENSITIVE) const
        {
                if (m_len != sb.m_len) return false;

                const char* sb_p  = sb.m_p + sb.m_len - 1;

                if (CASE_SENSITIVE == char_case)
                {
                        for (const char* start = m_p + m_len - 1; start >= m_p; start--, sb_p--)
                        {
                                // comparison order critical here
                                if (*start != *sb_p)
                                        return false;
                        }
                }
                else
                {
                        for (const char* start = m_p + m_len - 1; start >= m_p; start--, sb_p--)
                        {
                                // comparison order critical here
                                if (TOLOWER(*start) != TOLOWER(*sb_p))
                                        return false;
                        }
                }
                return true;
        }

        /**
          @brief Compares this subbuffer with c using char_case.
          @returns true if they match, false if they don't match.
         */
        inline bool equals(char c, case_sensitivity_t char_case = CASE_SENSITIVE) const
        {
                if (CASE_SENSITIVE == char_case) return 1 == m_len && c == m_p[0];

                return 1 == m_len && TOLOWER(c) == TOLOWER(m_p[0]);
        }

        /**
          @brief Compares this subbuffer with sb.
          @returns an integer less than, equal to, or greater than zero if this subbuffer
                   is found, respectively, to be less than, to match, or be greater than sb.
         */
        inline int compare(const subbuffer& sb, bool compare_length_first = false) const
        {
                if (!m_len) return (!sb.m_len) ?  0 : -1;
                if (!sb.m_len) return 1;
                
                if (compare_length_first && m_len != sb.m_len)
                        return m_len < sb.m_len ? -1 : 1; 

                if (*m_p != *sb.m_p) return *m_p - *sb.m_p;
                int ret = ::strncmp(m_p, sb.m_p, std::min(m_len, sb.m_len));
                return (ret ? ret : m_len < sb.m_len ? -1 : m_len > sb.m_len ? 1 : 0);
        }

        /**
          @brief Compares this subbuffer with sb using char_case.
          @returns an integer less than, equal to, or greater than zero if this subbuffer
                   is found, respectively, to be less than, to match, or be greater than sb.
         */
        inline int compare(const subbuffer& sb, case_sensitivity_t char_case, bool compare_length_first = false) const
        {
                if (likely(CASE_INSENSITIVE == char_case))
                        return compare_caseless(sb, compare_length_first);
                else return this->compare(sb, compare_length_first);
        }

        inline int compare_caseless(const subbuffer& sb, bool compare_length_first = false) const
        {
                if (!m_len) return (!sb.m_len) ?  0 : -1;
                if (!sb.m_len) return 1;

                if (compare_length_first && m_len != sb.m_len)
                        return m_len < sb.m_len ? -1 : 1;

                const char a = TOLOWER(*m_p);
                const char b = TOLOWER(*sb.m_p);
                if (a != b) return (a < b) ? -1 : 1;

                int ret = ::strncasecmp(m_p, sb.m_p, std::min(m_len, sb.m_len));
                return (ret ? ret : m_len < sb.m_len ? -1 : m_len > sb.m_len ? 1 : 0);
        }

        /**
          @brief Compares the first sb.length() bytes of this subbuffer.
          @returns true if they match, false if they don't match.
         */
        inline bool starts_with(const subbuffer& sb) const
        {
                return m_len >= sb.m_len && std::equal(m_p, m_p + sb.m_len, sb.m_p);
        }

        /**
          @brief Compares the first sb.length() bytes of this subbuffer using char_case.
          @returns true if they match, false if they don't match.
         */
        inline bool starts_with(const subbuffer& sb, case_sensitivity_t char_case) const
        {
                if (CASE_SENSITIVE == char_case)
                        return m_len >= sb.m_len && std::equal(m_p, m_p + sb.m_len, sb.m_p);
                return m_len >= sb.m_len && 0 == ::strncasecmp(m_p, sb.m_p, sb.m_len);
        }

        /**
          @brief Compares the first byte of this subbuffer using pred().
          The pred object needs to have a "bool operator()(char) const" method
          @returns true if they match, false if they don't match.
         */
        template<class BinaryPredicate> inline bool starts_with(const BinaryPredicate& pred) const
        {
                return m_len && pred(*m_p);
        }

        /**
          @brief Compares the first byte of this subbuffer.
          @returns true if they match, false if they don't match.
         */
        inline bool starts_with(char c) const
        {
                return m_len > 0 && c == m_p[0];
        }

        /**
          @brief Compares the last sb.length() bytes of this subbuffer using char_case.
          @returns true if they match, false if they don't match.
         */
        inline bool ends_with(const subbuffer& sb, case_sensitivity_t char_case = CASE_SENSITIVE) const
        {
                if (CASE_SENSITIVE == char_case)
                        return m_len >= sb.m_len && 0 == ::strncmp(m_p + (m_len - sb.m_len), sb.m_p, sb.m_len);
                return m_len >= sb.m_len && 0 == ::strncasecmp(m_p + (m_len - sb.m_len), sb.m_p, sb.m_len);
        }

        /**
          @brief Compares the last byte of this subbuffer using cg.in().
          @returns true if they match, false if they don't match.
         */
        inline bool ends_with(const chargrp& cg) const
        {
                return m_len > 0 && cg.contains(m_p[m_len - 1]);
        }

        /**
          @brief Compares the last byte of this subbuffer.
          @returns true if they match, false if they don't match.
         */
        inline bool ends_with(char c) const
        {
                return m_len > 0 && c == m_p[m_len - 1];
        }

        /**
          @brief An array of chars that have all been changed to upper case

          Tried combining this class and caseless_equal_to to pretty it up
          but that added 4 nanoseconds.
         */

        /**
          @brief A binary functor object used for passing to std::search
          Using an object instead of a function ptr in the str::search method
          shaved 15-20 nanoseconds off the timings.
         */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"  // ignored because binary_function has non-virtual dtor
        struct caseless_equal_to : std::binary_function <int,int,bool>
        {
#pragma GCC diagnostic pop                // back to default behaviour
                caseless_equal_to(char* tab) :
                        m_tab(tab)
                {
                }
                bool operator() (int i, int j) const
                {
                        return m_tab[i - CHAR_MIN] == m_tab[j - CHAR_MIN];
                }
                char* m_tab;
        };

        inline size_t find(const subbuffer& sb) const
        {
                if (!m_len || !sb.m_len || sb.m_len > m_len) return npos;

                const char* p = std::search(m_p, m_p + m_len, sb.m_p, sb.m_p + sb.m_len);
                return (p == m_p + m_len) ? npos : (const char*)p - m_p;
        }

        /**
          @brief Searches for sb in this subbuffer starting at pos and using char_case
          @returns subbuffer::npos if not found or the the offset if found.
         */
        size_t find(const subbuffer& sb, size_t pos, case_sensitivity_t char_case = CASE_SENSITIVE) const
        {
                if (!m_len || !sb.m_len) return npos;
                if (sb.m_len > (m_len - pos) || pos >= m_len) return npos;

                if (CASE_SENSITIVE == char_case)
                {
                        const char* p = std::search(m_p + pos, m_p + m_len, sb.m_p, sb.m_p + sb.m_len);
                        return (p == m_p + m_len) ? npos : (const char*)p - m_p;
                }

                static caseless_compare cp;
                const char* p = std::search(m_p + pos, m_p + m_len, sb.m_p, sb.m_p + sb.m_len, cp);
                return (p == m_p + m_len) ? npos : (const char*)p - m_p;
        }


        /**
          @brief Searches for val in this subbuffer
          @param [in] val The char to search for.
          @returns subbuffer::npos if not found or the the offset if found.
         */

        inline size_t find(char val) const
        {
                if (!m_len) return npos;

                const void* p = memchr(m_p, val, m_len);
                return p ? (const char*)p - m_p : npos;
        }

        /**
          @brief Searches for val in this subbuffer starting at pos and using char_case.
          @param [in] val The char to search for.
          @param [in] pos The index to start the search from. Defaults to 0.
          @param [in] char_case CASE_SENSITIVE (default) or CASE_INSENSITIVE
          @returns subbuffer::npos if not found or the the offset if found.
         */
        inline size_t find(char val, size_t pos, case_sensitivity_t char_case = CASE_SENSITIVE) const
        {
                if (!m_len || pos >= m_len) return npos;

                if (CASE_SENSITIVE == char_case)
                {
                        const void* p = memchr(m_p + pos, val, m_len - pos);
                        if (!p) return npos;
                        return (const char*)p - m_p;
                }

                caseless_compare fobj;
                for (size_t i = pos; i < m_len; ++i)
                {
                        if (fobj(m_p[i], val)) return i;
                }
                return npos;
        }

        /**
          @brief Apply func to all chars starting from pos.
          @param [in] pred Can be either "bool (*pred)(char c)" or an object with "bool operator()(char c)"
         */
        template<class BinaryPredicate> inline bool is_all(BinaryPredicate pred, size_t pos = 0) const
        {
                if (!m_len) return false;

                for (size_t s = pos; s < m_len; ++s)
                        if (!pred(m_p[s])) return false;
                return true;
        }

        /**
          @brief Searches for characters equal to val and returns the number of matches starting from pos.
          @param [in] val The char to count.
          @returns 0 if no matches were found.
         */
        size_t count(char val, size_t pos = 0) const
        {
                if (!m_len) return 0;
                size_t cnt = 0;
                for (size_t s = pos; s < m_len; ++s)
                        if (m_p[s] == val) ++cnt;
                return cnt;
        }

        /**
          @brief Searches for the first char in cg starting at pos.
          @returns subbuffer::npos if not found or the the offset if found.
         */
        size_t find(const chargrp& cg, size_t pos = 0) const
        {
                if (!m_len || pos >= m_len) return npos;
                for (size_t i = 0; i < m_len; ++i)
                        if (cg(m_p[i])) return i;
                return npos;

                //const char* pr = std::find_if(m_p + pos, m_p + m_len, cg);
                //return pr == m_p + m_len ? npos : pr - m_p;
        }

        /**
          @brief Searches for sb in this subbuffer, in reverse order.
          @returns subbuffer::npos if not found or the the offset if found.
         */
        inline size_t rfind(char val) const
        {
                if (!m_len) return npos;

                const void* p = memrchr(m_p, val, m_len);
                if (!p) return npos;
                return (const char*)p - m_p;
        }

        /**
          @brief Searches for sb in this subbuffer starting at pos and using char_case.
          @returns subbuffer::npos if not found or the the offset if found.
         */
        size_t rfind(char val, size_t pos, case_sensitivity_t char_case = CASE_SENSITIVE) const
        {
                if (!m_len) return npos;
                if (pos >= m_len) pos = m_len - 1;

                if (CASE_SENSITIVE == char_case)
                {
                        for (size_t i = pos; i <= pos; --i)
                                if (m_p[i] == val) return i;
                        return npos;
                }

                caseless_compare fobj;
                for (size_t i = pos; i <= pos; --i)
                        if (fobj(m_p[i], val)) return i;
                return npos;
        }

        /**
          @brief Searches for the last char in cg starting at pos.
          @returns subbuffer::npos if not found or the the offset if found.
         */
        size_t rfind(const chargrp& cg, size_t pos = npos) const
        {
                if (!m_len) return npos;

                if (pos >= m_len) pos = m_len - 1;

                for (size_t s = pos; s < m_len; --s)
                        if (cg.contains(m_p[s])) return s;
                return npos;
        }

        /**
          @brief Find the first char that is not a member of the given set.
          @returns subbuffer::npos if none or the the offset if found.
         */
        size_t first_not_in(const chargrp& cg, size_t pos = 0) const
        {
                if (empty()) return npos;

                for (size_t s = pos; s < m_len; ++s)
                        if (!cg.contains(m_p[s])) return s;
                return npos;
        }

        /**
          @brief Similar to find except that is is more explicit and easier to understand.
          @returns true if found, false if not found.
         */
        inline bool contains(const subbuffer& sb, case_sensitivity_t char_case = CASE_SENSITIVE) const
        {
                return npos != find(sb, 0, char_case);
        }


        /**
          @brief Similar to find except that is is more explicit and easier to understand.
          @returns true if found, false if not found.
         */
        inline bool contains(const chargrp& cg) const
        {
                return npos != find(cg);
        }

        /**
          @brief Checks if this subbuffer is contained within the passed in subbuffer.
          @returns true if it is, false if it is not.
         */
        bool is_sub_of(const subbuffer& sb) const
        {
                return (m_p >= sb.m_p && (m_p + m_len) <= (sb.m_p + sb.m_len));
        }

        /**
          @brief Accessor for the char at offset.
          @returns char(0) if subbuffer is empty or offset is out of bounds
         */
        inline char at(size_t offset) const
        {
                if (!m_p || offset >= m_len) return char(0);
                return m_p[offset];
        }

        /**
          @brief Builds/returns a subbuffer from this buffer.
         */
        inline subbuffer sub(size_t pos = 0, size_t len = npos) const
        {
                if (pos >= m_len) return subbuffer(m_p, 0);
                return subbuffer(m_p + pos, std::min(len, m_len - pos));
        }


        /**
          @brief Searches for val.
          @returns The substring before val if found, or the whole subbuffer if not found.
         */
        inline subbuffer before(char val) const
        {
                return sub(0, find(val));
        }

        /**
          @brief Searches for the first char in cg.
          @returns The initial substring if found, or the whole subbuffer if not found.
         */
        inline subbuffer before(const chargrp& cg) const
        {
                return sub(0, find(cg));
        }

        /**
          @brief Searches for the first str.
          @returns The initial substring if found, or the whole subbuffer if not found.
         */
        inline subbuffer before(const subbuffer& str) const
        {
                return sub(0, find(str));
        }

        /**
          @brief Searches for val from the back of the buffer.
          @returns The substring before val if found, or the whole subbuffer if not found.
         */
        inline subbuffer rbefore(char val) const
        {
                return sub(0, rfind(val));
        }

        /**
          @brief Searches for the last char in cg.
          @returns The substring if found, or the whole subbuffer if not found.
         */
        inline subbuffer rbefore(const chargrp& cg) const
        {
                return sub(0, rfind(cg));
        }

        /**
          @brief Searches for val.
          @returns The substring after val if found, or an empty subbuffer if not found.
         */
        inline subbuffer after(char val) const
        {
                const void* p = memchr(m_p, val, m_len);
                if (!p) return subbuffer(0, 0);
                return sub((const char*)p - m_p + 1);
        }

        /**
          @brief Searches for the first char in cg.
          @returns The substring if found, or an empty subbuffer if not found.
         */
        inline subbuffer after(const chargrp& cg) const
        {
                size_t pos = find(cg);
                if (subbuffer::npos == pos) return subbuffer(0, 0);
                return sub(pos + 1);
        }

        /**
          @brief Searches str.
          @returns The substring if found, or an empty subbuffer if not found.
         */
        inline subbuffer after(const subbuffer& str) const
        {
                size_t pos = find(str);
                if (subbuffer::npos == pos) return subbuffer(0, 0);
                return sub(pos + str.length());
        }

        /**
          @brief Searches for val from the back of the buffer.
          @returns The substring after val if found, or an empty subbuffer if not found.
         */
        inline subbuffer rafter(char val) const
        {
                size_t pos = rfind(val);
                if (subbuffer::npos == pos) return subbuffer(0, 0);
                return sub(pos + 1);
        }

        /**
          @brief Searches for the last char in sg.
          @returns The substring if found, or an empty subbuffer if not found.
         */
        inline subbuffer rafter(const chargrp& cg) const
        {
                size_t pos = rfind(cg);
                if (subbuffer::npos == pos) return subbuffer(0, 0);
                return sub(pos + 1);
        }

        /**
          @brief Removes all occurances of val from the left/front side of the buffer.
          @returns A reference to this subbuffer.
         */
        inline subbuffer& ltrim(char val)
        {
                if (!m_len || *m_p != val) return *this;
                ++m_p;
                --m_len;
                while (m_len && *m_p == val && --m_len)
                {
                        ++m_p;
                }
                return *this;
        }

        inline subbuffer& ltrim(char val, size_t max)
        {
                if (!m_len || *m_p != val) return *this;
                ++m_p;
                --m_len;
                --max;
                while (m_len && *m_p == val && max && --m_len)
                {
                        ++m_p;
                        --max;
                }
                return *this;
        }


        /**
          @brief Removes all chars that isfunc returns true for. Starting from the left/front side of the buffer.
          @returns A reference to this subbuffer.
         */
        subbuffer& ltrim(int (*isfunc)(int))
        {
                if (!m_len || !isfunc(*m_p)) return *this;
                while (m_len && isfunc(*m_p) && --m_len)
                {
                        ++m_p;
                }
                return *this;
        }

        /**
          @brief Removes all chars that pred(char) returns true for. Starting from the left/front side of the buffer.
          @returns A reference to this subbuffer.
         */
        template<class BinaryPredicate> subbuffer& ltrim(const BinaryPredicate& pred)
        {
                if (!m_len || !pred(*m_p)) return *this;
                while (m_len && pred(*m_p) && --m_len)
                {
                        ++m_p;
                }
                return *this;
        }

        /**
          @brief Removes all occurances of val from the right/back side of the buffer.
          @returns A reference to this subbuffer.
         */
        subbuffer& rtrim(char val)
        {
                if (!m_len) return *this;
                while (m_len && m_p[m_len - 1] == val)
                        --m_len;
                return *this;
        }

        /**
          @brief Removes all chars that isfunc returns true for. Starting from the right/back side of the buffer.
          @returns A reference to this subbuffer.
         */
        subbuffer& rtrim(int (*isfunc)(int))
        {
                if (!m_len) return *this;
                while (m_len && isfunc(m_p[m_len - 1]))
                        --m_len;
                return *this;
        }

        /**
          @brief Removes all chars that pred(char) returns true for. Starting from the right/back side of the buffer.
          @returns A reference to this subbuffer.
         */
        template<class BinaryPredicate> inline subbuffer& rtrim(BinaryPredicate pred)
        {
                if (!m_len) return *this;
                while (m_len && pred(m_p[m_len - 1]))
                        --m_len;
                return *this;
        }

        /**
          @brief Removes all occurances of val from the left/front and right/back side of the buffer.
          @returns A reference to this subbuffer.
         */
        inline subbuffer& trim(char val) { return ltrim(val).rtrim(val); }

        /**
          @brief Removes all occurances of chars in cg from the left/front and right/back side of the buffer.
          @returns A reference to this subbuffer.
         */
        inline subbuffer& trim(const chargrp& cg) { return ltrim(cg).rtrim(cg); }

        /**
          @brief Removes all chars from the left/front and right/back side of the buffer while isfunc(char) returns true.
          @returns A reference to this subbuffer
         */
        inline subbuffer& trim(int (*isfunc)(int)) { return ltrim(isfunc).rtrim(isfunc); }

        /**
          @brief Removes all chars from the left/front and right/back side of the buffer while pred(char) returns true.
          @returns A reference to this subbuffer
         */
        template<class BinaryPredicate> inline subbuffer& trim(BinaryPredicate pred) { return ltrim(pred).rtrim(pred); }

        /**
          @brief Advances the const char* and reduces the length.
          @returns A reference to this subbuffer.
         */
        subbuffer& advance(size_t bytes)
        {
                if (!m_len) return *this;
                if (bytes >= m_len) return clear();

                m_p += bytes;
                m_len -= bytes;
                return *this;
        }

        /**
          @brief Remove the end of the buffer starting from and including pos.
         */
        subbuffer& remove_from(size_t pos)
        {
                if (pos >= m_len) return *this;
                m_len = pos;
                return *this;
        }

        /**
          @brief Zeroes out the member vars.
          @returns A reference to this subbuffer.
         */
        inline subbuffer& clear()
        {
                m_p = 0;
                m_len = 0;
                return *this;
        }

        /**
          @brief Copy at most len bytes from the memory pointed to by the subbuffer into dest.
         */
        char* copy_to(char* dest, size_t len = subbuffer::npos)
        {
                if (!m_len || !dest || !len) return dest;

                if (len > m_len) len = m_len;
                memcpy(dest, m_p, len);
                dest[len] = 0;
                return dest;
        }

        /**
          @brief Performs an alphanumeric to numeric conversion using the requested base.
          @param [out] val The numeric value to fill.
          @param [out] remainder If the whole subbuffer could not be converted, this is set to the rest.
          @param [in] base Defaults to 10.
         */
        template<typename NUMB> bool aton(NUMB& val, subbuffer* remainder = NULL, uint64_t base = 10) const
        {
                const char* rem = NULL;
                bool ret = aton_internal<NUMB>(val, m_p, m_len, &rem, base);
                // if ret == true then any remainder will be inside of sb
                // else rem will be a NULL terminated error string
                if (remainder) *remainder = ret ? subbuffer(rem, rem ? this->length() - (rem - this->begin()) : 0) : subbuffer(rem);
                return ret;
        }

        inline uint64_t hash(uint64_t res = 0, case_sensitivity_t char_case = CASE_SENSITIVE) const
        {
                const uint64_t seed = 131; // 31 131 1313 13131 131313 etc..
                const char* buff = m_p;
                if (CASE_SENSITIVE == char_case)
                {
                        for (size_t i = 0; i < m_len; ++i)
                                res = (res * seed) + buff[i];
                }
                else
                {
                        for (size_t i = 0; i < m_len; ++i)
                                res = (res * seed) + TOLOWER(buff[i]);
                }
                res &= 0x7fffffffffffffffUL;
                return res;
        }

        inline bool operator< (const subbuffer& sb) const
        {
                return std::lexicographical_compare(m_p, m_p + m_len, sb.m_p, sb.m_p + sb.m_len);
        }

        inline bool operator== (const subbuffer& val) const { return equals(val); }

private:
        const char* m_p;
        size_t m_len;
};

/**
  @brief struct for using subbuffer in std collections.
  buffer equals function
  */
struct subbuffer_equals
{
        inline bool operator()(const subbuffer& x, const subbuffer& y) const
        {
                return (x.equals(y));
        }
};

struct subbuffer_hash
{
        inline size_t operator()(const subbuffer& sb) const
        {
                return sb.hash();
        }
};



#endif
