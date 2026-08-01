//: ----------------------------------------------------------------------------
//: Copyright (C) 2015 Verizon.  All Rights Reserved.
//:
//: \file:    json_parser.h
//: \details: Parses JSON text (DOM).
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

#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include "aton.h"
#include "json.h"

#include <map>
#include <vector>


// Predefine JSON_TRACE, JSON_WARNING and JSON_ERROR in a wrapper script if you want the traces, warnings and errors to use your
// internal logging

#ifndef JSON_TRACE
#define JSON_TRACE(fmt, x...) fprintf(stderr, "TRACE %s:%d: " fmt "\n", __FILE__, __LINE__, ## x)
#endif

#ifndef JSON_WARNING
#define JSON_WARNING(fmt, x...) fprintf(stderr, "WARN %s:%d: " fmt "\n", __FILE__, __LINE__, ## x)
#endif

#ifndef JSON_ERROR
#define JSON_ERROR(fmt, x...) fprintf(stderr, "ERROR %s:%d: " fmt "\n", __FILE__, __LINE__, ## x)
#endif

// every call to parse could end up recursing into another level of parsing
// if you recurse into parse enough times you'll overrun the stack.
// for easy math, assume that each value/object/array takes a minimum of two calls to parse
// one for the value and one for the real type (array/object)
// setting this at 500, means that there will be 500 calls to parse stacked on the stack
// or around 250 levels of nesting in the json.
// Feel free to bump it up higher but at some point you'll need to increase your stack size.

#ifndef JSON_MAX_PARSE_RECURSION
#define JSON_MAX_PARSE_RECURSION 500
#endif

/**
  @brief A set of classes for parsing JSON data.

  This parser is not intended to handle every corner case. What is is intended to do is parse
  normal JSON text in a rapid manner.
 */

namespace json
{
        enum val_type { UNSET, NUMBER, STRING, BOOL, OBJECT, ARRAY };
        static const chargrp braces(" []\n\t\r\f\v");
        static const chargrp brackets(" {}\n\t\r\f\v");
        static const chargrp spacecolon(" :\n\t\r\f\v");
        static const chargrp spacecomma(" ,\n\t\r\f\v");
        static const chargrp spacecommacolon(" ,:\n\t\r\f\v");
        static const chargrp space(" \n\t\r\f\v");
        class root;
        class object;
        class array;

        /**
          @brief The base class. Everything is a value.
         */
        class value
        {
                friend class object;
                friend class array;
        public:
                inline value() :m_type(UNSET), m_sval(), m_val() {}
                inline virtual ~value();


                /**
                  @brief Helper method for converting portions of the parsed JSON back into JSON text.
                 */
                inline void to_json(json_object& obj, subbuffer key) const;

                /**
                  @brief Helper method for converting portions of the parsed JSON back into JSON text.
                 */
                inline void to_json(json_array& arr) const;

                /**
                  @brief Helper method for converting portions of the parsed JSON back into JSON text.
                 */
                template<typename BUFF> void to_json(BUFF& json_text) const;
                inline bool parse(subbuffer& val, int32_t level);

                /**
                  @brief Only valid for STRING and BOOL values.
                  @returns The value for STRING values and "true" or "false" for BOOL values.
                 */
                inline subbuffer str() const
                {
                        if (m_type == STRING) return m_sval;
                        if (m_type == BOOL)
                        {
                                if (m_val.bval) return "true";
                                return "false";
                        }
                        return subbuffer(0, 0);
                }
                /**
                  @brief  Return a std::string object for STRING and BOOL values
                  @returns The value for STRING values and "true" or "false" for BOOL values.
                 */
                inline bool str(std::string& ao_string) const
                {
                        subbuffer l_sub = str();
                        ao_string.assign(l_sub.begin(), l_sub.length());
                        return l_sub.is_set();
                }

                /**
                   @brief  Perform limited unescapging of string values into the provided dest buffer
                   @details THIS FUNCTION IS ONLY PARTIALLY IMPLEMENTED.
                            We only correctly unescape \", \\ and \/ sequences.  Lots more to be added
                            http://www.ecma-international.org/publications/files/ECMA-ST/ECMA-404.pdf
                   @returns A temporary object representing the destination string with
                 */
                template <typename BUFF>
                inline subbuffer unescape(BUFF& dest) const
                {
                        dest.clear();
                        if (m_type == BOOL)
                        {
                                dest.assign(m_val.bval ? "true" : "false");
                        }
                        else if (m_type == STRING)
                        {
                                dest.reserve(m_sval.length());
                                const char* first = m_sval.begin();
                                const char* last = first + m_sval.length();
                                while (first != last)
                                {
                                        if (*first == '\\')  ++first;
                                        dest += *first++;
                                }
                        }
                        return subbuffer(dest.c_str(), dest.length());
                }

                /**
                  @brief Only valid for NUMBER and BOOL values.
                  @returns The value for NUMBER values and 1.0 or 0.0 for BOOL values.
                 */
                inline double numb(double default_val = 0.0) const
                {
                        if (m_type == NUMBER) return m_val.dval;
                        if (m_type == BOOL)
                        {
                                if (m_val.bval) return 1.0;
                                return 0.0;
                        }
                        return default_val;
                }

                /**
                  @brief Only valid for BOOL values.
                 */
                inline bool bval() const
                {
                        if (BOOL != m_type) return false;
                        return m_val.bval;
                }

                /**
                  @brief Only valid for OBJECT values.
                  @returns A pointer to the underlying OBJECT or NULL if it is not an OBJECT.
                 */
                inline object* to_object() const
                {
                        if (OBJECT != m_type) return NULL;
                        return m_val.oval;
                }

                /**
                  @brief Accessor for the values in an OBJECT.
                 */
                inline value& operator[] (subbuffer key);

                /**
                  @brief Accessor for the values in and ARRAY.
                 */
                inline value& operator[] (size_t key);

                /**
                  @brief Const accessor for the values in an OBJECT.
                 */
                inline const value& operator[] (subbuffer key) const;

                /**
                  @brief Const accessor for the values in and ARRAY.
                 */
                inline const value& operator[] (size_t key) const;

                /**
                  @brief Check of the existence of a key/value pair in an OBJECT.
                 */
                inline bool exists(subbuffer key) const;

                /**
                  @brief The number of elements in an ARRAY.
                 */
                inline size_t size() const;

                inline bool is_string() const { return m_type == STRING; }
                inline bool is_number() const { return m_type == NUMBER; }
                inline bool is_bool() const { return m_type == BOOL; }
                inline bool is_object() const { return m_type == OBJECT; }
                inline bool is_array() const { return m_type == ARRAY; }
                inline bool is_unset() const { return m_type == UNSET; }
                inline val_type get_type() const { return m_type; }

                inline subbuffer raw_subbuffer() const;
        protected:
                /**
                  @brief Reset the member variables to a pristine state.
                 */
                inline void clear();

        private:
                val_type m_type;
                subbuffer m_sval;
                union
                {
                        double dval;
                        bool   bval;
                        object* oval;
                        array*  aval;
                } m_val;
        };

        static value s_unset;

        /**
          @brief Represents a set of key/value pairs parsed from JSON text.
         */
        class object
        {
                friend class value;
        public:
                inline object() :m_vals(), m_unset(), m_sval() {}
                inline ~object() {}

                /**
                  @brief Check of the existence of a key/value pair.
                 */
                inline bool exists(subbuffer key) const { return m_vals.count(key); }

                /**
                  @brief An invalid call for OBJECT values.
                 */
                inline const value& operator[] (size_t /*key*/) const { return s_unset; }

                /**
                  @brief Find the value associated with the provided key. Remember to check "is_unset()" on the returned value.
                  @returns The value if found or an unset value if not found.
                 */
                inline const value& operator[] (subbuffer key)
                {
                        std::map<subbuffer, value>::iterator iter = m_vals.find(key);
                        if (iter == m_vals.end())
                        {
                                JSON_TRACE("failed to find value for key: '%.*s'\n", (int)key.length(), key.begin());
                                return m_unset;
                        }
                        return iter->second;
                }

                /**
                  @brief Covert the parsed JSON into a json_object for easy conversion back to JSON text.
                 */
                inline void to_json(json_object& obj)
                {
                        json_object subobj;
                        for (std::map<subbuffer, value>::iterator iter = m_vals.begin();
                             iter != m_vals.end();
                             ++iter)
                        {
                                iter->second.to_json(obj, iter->first);
                        }
                }

                inline bool parse(subbuffer& val, int32_t level)
                {
                        if (!val.starts_with('{'))
                        {
                                JSON_WARNING("object::parse, does not start with {\n");
                                return false;
                        }

                        if (JSON_MAX_PARSE_RECURSION < ++level)
                        {
                                JSON_ERROR("object::parse, too many levels of recursion (%d)\n", level);
                                return false;
                        }

                        m_sval = val;

                        // "key", val, "key", "val"......
                        val.advance(1);
                        val.ltrim(space);
                        while (!val.empty() && !val.starts_with('}'))
                        {
                                if (!val.starts_with('"'))
                                {
                                        JSON_WARNING("object::parse, invalid JSON, does not start with \", val: '%.*s', prev: '%.*s'\n",
                                                     SUBBUF_FORMAT(val.sub(0, 100)),
                                                     SUBBUF_FORMAT(subbuffer(val.begin() - 100, 100)));
                                        return false;
                                }
                                val.advance(1);
                                subbuffer key = val.before('"');
                                if (key.empty())
                                {
                                        JSON_WARNING("object::parse, invalid JSON, key is empty, '%.*s'\n", SUBBUF_FORMAT(m_sval));
                                        return false;
                                }

                                val.advance(key.length() + 1);
                                val.ltrim(space);
                                if (!val.starts_with(':'))
                                {
                                        JSON_WARNING("object::parse, invalid JSON, key:value not separated by colon, '%.*s'\n", SUBBUF_FORMAT(m_sval)); 
                                        return false;
                                }
                                val.ltrim(spacecolon);
                                // next char should be " or numeric or { or [ or null
                                if (val.at(0) == ',')
                                {
                                        val.trim(spacecomma);
                                        continue;
                                }
                                value v;
                                if (!v.parse(val, level))
                                {
                                        JSON_WARNING("object::parse, v failed to parse '%.*s'\n", SUBBUF_FORMAT(val.sub(0, 100)));
                                        return false;
                                }
                                m_vals[key] = v;
                                val.ltrim(space);
                                if (!val.starts_with(',') && !val.starts_with('}'))
                                {
                                        JSON_WARNING("object::parse, invalid JSON, missing comma (,) or right brace (}), val: '%.*s'\n",
                                                     SUBBUF_FORMAT(m_sval));
                                        return false;
                                }
                                val.ltrim(spacecomma);
                        }
                        if (val.starts_with('}')) val.advance(1);
                        // resize m_sval
                        m_sval.remove_from(m_sval.length() - val.length());
                        return true;
                }


                inline subbuffer raw_subbuffer() const { return m_sval; }

                inline std::map<subbuffer, value>::iterator begin() { return m_vals.begin(); }
                inline std::map<subbuffer, value>::iterator end() { return m_vals.end(); }

        private:
                inline void clear()
                {
                        for (std::map<subbuffer, value>::iterator iter = m_vals.begin();
                             iter != m_vals.end();
                             ++iter)
                        {
                                iter->second.clear();
                        }
                        m_vals.clear();
                }

                std::map<subbuffer, value> m_vals;
                value m_unset;
                subbuffer m_sval;
        };

        class array
        {
                friend class value;
                friend class object;
        public:
                inline array():m_vals(), m_sval() {}
                inline ~array() {}


                inline size_t size() const { return m_vals.size(); }

                inline void to_json(json_array& arr)
                {
                        for (std::vector<value>::iterator iter = m_vals.begin();
                             iter != m_vals.end();
                             ++iter)
                        {
                                iter->to_json(arr);
                        }
                }

                inline bool parse(subbuffer& val, int32_t level)
                {
                        if (!val.starts_with('['))
                        {
                                JSON_ERROR("array::parse, does not start with '['\n");
                                return false;
                        }

                        if (JSON_MAX_PARSE_RECURSION < ++level)
                        {
                                JSON_ERROR("object::parse, too many levels of recursion (%d)\n", level);
                                return false;
                        }

                        m_sval = val;

                        val.advance(1).ltrim(space);

                        while (!val.empty() && !val.starts_with(']'))
                        {
                                value v;
                                if (!v.parse(val, level)) return false;
                                m_vals.push_back(v);
                                if (m_vals.size() > 10000000)
                                {
                                        JSON_ERROR("m_vals.size > 10 million, should not be here\n");
                                        JSON_ERROR("orig val: %.*s\n", SUBBUF_FORMAT(m_sval.sub(0, 1000)));
                                        JSON_ERROR("val: %.*s\n", SUBBUF_FORMAT(val.sub(0, 100)));
                                        exit(0);
                                }
                                val.ltrim(spacecomma);
                        }
                        if (val.starts_with(']')) val.advance(1);
                        // trim m_sval down to just the array data
                        m_sval.remove_from(m_sval.length() - val.length());
                        return true;
                }

                inline value& operator[] (subbuffer /*key*/) const { return s_unset; }
                inline value& operator[] (size_t key)
                {
                        if (m_vals.size() <= key) return s_unset;
                        return m_vals[key];
                }

                inline subbuffer raw_subbuffer() const { return m_sval; }
        private:
                inline void clear()
                {
                        for (std::vector<value>::iterator iter = m_vals.begin();
                             iter != m_vals.end();
                             ++iter)
                        {
                                (*iter).clear();
                        }
                        m_vals.clear();
                }

                std::vector<value> m_vals;
                subbuffer m_sval;
        };

        size_t value::size() const
        {
                if (m_type == ARRAY) return m_val.aval->size();
                return 0;
        }

        subbuffer value::raw_subbuffer() const
        {
                switch (m_type)
                {
                case OBJECT:
                        return m_val.oval->raw_subbuffer();
                case ARRAY:
                        return m_val.aval->raw_subbuffer();
                default:
                        return m_sval;
                }
        }

        value& value::operator[] (subbuffer key)
        {
                if (m_type == OBJECT) return const_cast<value&>((*m_val.oval)[key]);
                return s_unset;
        }
        value& value::operator[] (size_t key)
        {
                if (m_type == ARRAY) return (*m_val.aval)[key];
                return s_unset;
        }
        const value& value::operator[] (subbuffer key) const
        {
                if (m_type == OBJECT) return (*m_val.oval)[key];
                return s_unset;
        }
        const value& value::operator[] (size_t key) const
        {
                if (m_type == ARRAY) return (*m_val.aval)[key];
                return s_unset;
        }

        bool value::exists(subbuffer key) const
        {
                if (OBJECT == m_type) return m_val.oval->exists(key);
                return false;
        }

        void value::clear()
        {
                if (m_type == OBJECT && m_val.oval)
                {
                        m_val.oval->clear();
                        delete m_val.oval;
                        m_val.oval = 0;
                }
                else if (m_type == ARRAY && m_val.aval)
                {
                        m_val.aval->clear();
                        delete m_val.aval;
                        m_val.aval = 0;
                }
                m_type = UNSET;
        }

        value::~value()
        {
                // clearing starts from the root object and only the root object.
                // create the root, make all the copies of values that you want and
                // they will be valid so long as the root exists.
        }

        template<typename BUFF> void value::to_json(BUFF& json_text) const
        {
                switch (m_type)
                {
                case OBJECT:
                {
                        json_object obj;
                        m_val.oval->to_json(obj);
                        obj.to_string(json_text);
                        break;
                }
                case ARRAY:
                {
                        json_array arr;
                        m_val.aval->to_json(arr);
                        arr.to_string(json_text);
                        break;
                }
                default:
                        break;
                }
        }

        void value::to_json(json_object& obj, subbuffer key) const
        {
                switch (m_type)
                {
                case OBJECT:
                {
                        json_object subobj;
                        m_val.oval->to_json(subobj);
                        obj.add(key, subobj);
                        break;
                }
                case ARRAY:
                {
                        json_array subarr;
                        m_val.aval->to_json(subarr);
                        obj.add(key, subarr);
                        break;
                }
                case STRING:
                        obj.add(key, m_sval);
                        break;
                case NUMBER:
                        obj.add(key, m_val.dval);
                        break;
                case BOOL:
                        obj.add(key, m_val.bval);
                        break;
                default:
                        break;
                }
        }

        void value::to_json(json_array& arr) const
        {
                switch (m_type)
                {
                case OBJECT:
                {
                        json_object subobj;
                        m_val.oval->to_json(subobj);
                        arr.add(subobj);
                        break;
                }
                case ARRAY:
                {
                        json_array subarr;
                        m_val.aval->to_json(subarr);
                        arr.add(subarr);
                        break;
                }
                case STRING:
                        arr.add(m_sval);
                        break;
                case NUMBER:
                        arr.add(m_val.dval);
                        break;
                case BOOL:
                        arr.add(m_val.bval);
                        break;
                default:
                        break;
                }
        }

        bool value::parse(subbuffer& val, int32_t level)
        {
                if (JSON_MAX_PARSE_RECURSION < ++level)
                {
                        JSON_ERROR("value::parse, too many levels of recursion (%d)\n", level);
                        return false;
                }

                //val.ltrim(spacecolon);
                val.ltrim(spacecommacolon);
                m_type = UNSET;
                if (val.starts_with('"'))
                {
                        val.advance(1);
                        // need to find an unescaped double quote
                        size_t dq = 0;
                        while (dq != subbuffer::npos)
                        {
                                dq = val.find('"', dq);
                                if (dq == 0)
                                {
                                        m_sval.clear();
                                        break;
                                }
                                else if (dq == subbuffer::npos)
                                {
                                        return false;
                                }
                                else if (val.at(dq - 1) == '\\')
                                {
                                        dq++;
                                        continue;
                                }
                                else
                                {
                                        m_sval = val.sub(0, dq);
                                        break;
                                }
                        }
                        m_type = STRING;
                        val.advance(m_sval.length() + 1);
                        return true;
                }
                else if (val.starts_with('['))
                {
                        m_val.aval = new array();
                        if (!m_val.aval->parse(val, level))
                        {
                                m_val.aval->clear();
                                delete m_val.aval;
                                m_val.aval = 0;
                                return false;
                        }
                        m_type = ARRAY;
                }
                else if (val.starts_with('{'))
                {
                        m_val.oval = new object;
                        if (!m_val.oval->parse(val, level))
                        {
                                m_val.oval->clear();
                                delete m_val.oval;
                                m_val.oval = 0;
                                return false;
                        }
                        m_type = OBJECT;
                }
                else if (val.starts_with(CONST_SUBBUF("true"), CASE_INSENSITIVE))
                {
                        m_val.bval = true;
                        m_type = BOOL;
                        val.advance(4);
                }
                else if (val.starts_with(CONST_SUBBUF("false"), CASE_INSENSITIVE))
                {
                        m_val.bval = false;
                        m_type = BOOL;
                        val.advance(5);
                }
                else if (isdigit(val.at(0)) || val.at(0) == '-')
                {
                        subbuffer rem;
                        m_val.dval = aton<double>(val, &rem);
                        if (!rem.empty() && !rem.starts_with(spacecomma) && !rem.starts_with(braces) && !rem.starts_with(brackets))
                        {
                                if (rem.starts_with("e+", CASE_INSENSITIVE))
                                {
                                        subbuffer rem2;
                                        int32_t mult = aton<int32_t>(rem.sub(2), &rem2);
                                        m_val.dval *= mult;
                                        rem = rem2;
                                }
                                else
                                {
                                        JSON_ERROR("value::parse, failed to parse full number value, val: '%.*s', rem: '%.*s'\n",
                                                   SUBBUF_FORMAT(val.sub(0, 100)), SUBBUF_FORMAT(rem.sub(0, 100)));
                                        m_type = UNSET;
                                        return false;
                                }
                        }

                        if (rem.empty())
                                m_sval = val;
                        else
                                m_sval = val.sub(0, rem.begin() - val.begin());

                        m_type = NUMBER;
                        val = rem;
                }
                else if (val.starts_with(subbuffer("null")))
                {
                        // this is a null
                        // skip over null
                        val.advance(4);
                }
                else
                {
                        m_type = UNSET;
                        // need to advance val past the next quote, comma, brace or bracket
                        static chargrp oops("\",]}");
                        size_t pos = val.find(oops);
                        if (subbuffer::npos == pos)
                        {
                                val.clear();
                        }
                        else
                        {
                                val.advance(pos + 1);
                        }
                        return false;
                }
                return true;
        }

        class root : public value
        {
        public:
                inline root()
                        : value(), m_is_valid(false)
                {}
                inline root(subbuffer val)
                        : value(), m_is_valid(false)
                {
                        int32_t level = 0;
                        m_is_valid = this->parse(val, level);
                }

                inline ~root()
                {
                        this->clear();
                }

                inline bool is_valid() const
                {
                        return m_is_valid;
                }
        private:
                bool m_is_valid;
        };
};

#endif
