//: ----------------------------------------------------------------------------
//: Copyright (C) 2015 Verizon.  All Rights Reserved.
//:
//: \file:    json.h
//: \details: Assists with building basic JSON text.
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
//:

#ifndef _JSON_H_
#define _JSON_H_

#include "subbuffer.h"

#include <string>
#include <stdio.h>

/**
 * @file json.h
 * @brief A set of light weight classes for building (not parsing) JSON data
 *
 * JSON has two basic structures, Objects and Arrays.
 *
 * It becomes a bit verbose on large data sets but is simple and easy to use for small
 * data sets.
 *
 * I intentionally kept the use pattern for these classes simple. That use pattern does
 * end up requiring at least one extra buffer copy when compared to building the json
 * by hand, but it also keeps it very simple and flexible.
 */

class json_array;

/**
 * @brief Used for building a JSON object.
 * {"name":"value","name":value}
 *
 * std::string buff; // a buffer from somewhere
 * json_object obj;
 * obj.add("age", 66);
 * obj.add("waist", 66);
 * obj.to_string(buff);  // buff is now {"age":66,"waist":66}
 *                       // obj is unchanged and you can continue to add key/value pairs
 *                       // or clear it and start again.
 */
class json_object
{
public:
        json_object():
                m_buff()
        {
                clear();
        }

        class json_friendly
        {
        public:
                json_friendly() :
                        m_last(' ') // something other than slash
                {}
                bool operator()(char c)
                {
                        bool ps = ('\\' == m_last);
                        bool ret = false;

                        switch (c)
                        {
                        case '\"':
                        case '\n':
                                ret = true;
                                break;
                        }
                        if (ps)
                                m_last = ' ';
                        else
                                m_last = c;

                        if (ps && !ret) return true;
                        if (!ps && ret) return true;
                        return false;
                }

                static void append(std::string& dest, subbuffer src)
                {
                        dest.reserve(dest.length() + (src.length() * 2));
                        json_friendly jf;
                        for (size_t i = 0; i < src.length(); i++)
                        {
                                if (jf(src[i])) dest += '\\';
                                dest += src[i];
                        }
                }
        private:
                char m_last;
        };

        template<typename ENCODER>
        json_object& add_encoded(subbuffer key, subbuffer val, ENCODER encode, bool quote_it = true)
        {
                add_key(key);
                if (quote_it || val.empty())
                {
                        m_buff += '\"';
                        encode(m_buff, val);
                        m_buff += '\"';
                }
                else
                        encode(m_buff, val);
                m_buff += ',';
                return *this;
        }

        json_object& add(subbuffer key, subbuffer val, bool quote_it = true)
        {
                add_key(key);
                if (quote_it || val.empty())
                {
                        m_buff += '\"';
                        json_friendly::append(m_buff, val);
                        m_buff += '\"';
                }
                else
                        m_buff.append(val.begin(), val.length());
                m_buff += ',';
                return *this;
        }

        json_object& add(subbuffer key, const char* val)
        {
                return add(key, subbuffer(val));
        }

        json_object& add(subbuffer key, int32_t val) { return add(key, int64_t(val)); }
        json_object& add(subbuffer key, uint32_t val) { return add(key, uint64_t(val)); }

        json_object& add(subbuffer key, int64_t val)
        {
                add_key(key);
                char buff[50];
                size_t len = snprintf(buff, sizeof(buff), "%ld", val);
                buff[sizeof(buff)-1] = '\0';
                m_buff.append(buff, len);
                m_buff += ',';
                return *this;
        }

        json_object& add(subbuffer key, uint64_t val)
        {
                add_key(key);
                char buff[50];
                size_t len = snprintf(buff, sizeof(buff), "%lu", val);
                buff[sizeof(buff)-1] = '\0';
                m_buff.append(buff, len);
                m_buff += ',';
                return *this;
        }

        /**
         * @brief Add a double value
         * @param [in] num_desc number of places after the decimal point to keep (MAX 12)
         */
        json_object& add(subbuffer key, double val, int num_desc = 2)
        {
                add_key(key);
                char buff[512];
                size_t len = snprintf(buff, sizeof(buff), "%.*f", num_desc, val);
                buff[sizeof(buff)-1] = '\0';
                m_buff.append(buff, len);
                m_buff += ',';
                return *this;
        }

        json_object& add(subbuffer key, bool val)
        {
                add_key(key);
                if (val)
                        m_buff.append("true", 4);
                else
                        m_buff.append("false", 5);
                m_buff += ',';
                return *this;
        }

        json_object& add(subbuffer key, json_object& obj)
        {
                add_key(key);
                obj.to_string(m_buff);
                m_buff += ',';
                return *this;
        }

        // defined at end of file
        json_object& add(subbuffer key, json_array& arr);

        template<typename BUFF> void to_string(BUFF& dest)
        {
                // play with a subbuffer so we don't effect m_buff
                subbuffer src(m_buff);
                src.rtrim(',');
                dest.append(src.begin(), src.length());
                dest += '}';
        }

        template<typename BUFF> void to_string(BUFF* dest)
        {
                if (!dest) return;
                to_string(*dest);
        }

        json_object& clear()
        {
                m_buff.clear();
                m_buff += '{';
                return *this;
        }

        bool empty() const { return m_buff.length() <= 1; }
private:
        void add_key(subbuffer name)
        {
                // @TODO get to the point where we can do
                // m_buff << '\"' << name << '\"' << ':';
                m_buff += '\"';
                m_buff.append(name.begin(), name.length());
                m_buff += '\"';
                m_buff += ':';
        }
        std::string m_buff;
};

/**
 * @brief Used for building a JSON array.
 * ["val1","val2", val3, true, false]
 */
class json_array
{
public:
        json_array():
                m_buff()
        {
                clear();
        }

        json_array& add(subbuffer val, bool quote_it = true)
        {
                if (quote_it) m_buff += '\"';
                if (quote_it)
                        json_object::json_friendly::append(m_buff, val);
                else
                        m_buff.append(val.begin(), val.length());
                if (quote_it) m_buff += '\"';
                m_buff += ',';
                return *this;
        }

        json_array& add(const char* val)
        {
                return add(subbuffer(val));
        }

        json_array& add(int32_t val) { return add(int64_t(val)); }
        json_array& add(uint32_t val) { return add(uint64_t(val)); }

        json_array& add(int64_t val)
        {
                char buff[50];
                size_t len = snprintf(buff, sizeof(buff), "%ld", val);
                buff[sizeof(buff)-1] = '\0';
                m_buff.append(buff, len);
                m_buff += ',';
                return *this;
        }

        json_array& add(uint64_t val)
        {
                char buff[50];
                size_t len = snprintf(buff, sizeof(buff), "%lu", val);
                buff[sizeof(buff)-1] = '\0';
                m_buff.append(buff, len);
                m_buff += ',';
                return *this;
        }

        /**
         * @brief Add a double value
         * @param [in] num_desc number of places after the decimal point to keep (MAX 12)
         */
        json_array& add(double val, int num_desc = 2)
        {
                char buff[512];
                size_t len = snprintf(buff, sizeof(buff), "%.*f", num_desc, val);
                buff[sizeof(buff)-1] = '\0';
                m_buff.append(buff, len);
                m_buff += ',';
                return *this;
        }

        json_array& add(bool val)
        {
                if (val)
                        m_buff.append("true", 4);
                else
                        m_buff.append("false", 5);
                m_buff += ',';
                return *this;
        }

        json_array& add(json_object& obj)
        {
                obj.to_string(m_buff);
                m_buff += ',';
                return *this;
        }

        json_array& add(json_array& arr)
        {
                arr.to_string(m_buff);
                m_buff += ',';
                return *this;
        }

        template<typename BUFF> void to_string(BUFF& dest) const
        {
                // play with a subbuffer so we don't effect m_buff
                subbuffer src(m_buff);
                src.rtrim(',');
                dest.append(src.begin(), src.length());
                dest += ']';
        }

        template<typename BUFF> void to_string(BUFF* dest) const
        {
                if (!dest) return;
                to_string(*dest);
        }

        json_array& clear()
        {
                m_buff.clear();
                m_buff += '[';
                return *this;
        }

        bool empty() const { return m_buff.length() <= 1; }
private:
        std::string m_buff;
};

inline json_object& json_object::add(subbuffer key, json_array& arr)
{
        add_key(key);
        arr.to_string(m_buff);
        m_buff += ',';
        return *this;
}

#endif
