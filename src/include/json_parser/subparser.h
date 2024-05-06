//: ----------------------------------------------------------------------------
//: Copyright (C) 2015 Verizon.  All Rights Reserved.
//:
//: \file:    subparser.h
//: \details: Parses a subbuffer into little subbuffers.
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

#ifndef _SUBPARSER_H_
#define _SUBPARSER_H_

#include "subbuffer.h"

#include <vector>

/**
  @brief Parses the provided subbuffer without changing the underlying string/values.

  Simpler than using strtok and less complicated than rolling your own.
 */
class subparser
{
private:
        enum delimiter_type_t { CHAR_DELIM, SUBBUF_DELIM, CHARGRP_DELIM };
public:
        /**
          @brief Controls how empty fields are treated.
         */
        enum skip_empty_t
        {
                SKIP_EMPTY,  //!< Ignore empty fields, skip over them.
                RETURN_EMPTY //!< Return empty fields, allowing the caller to know if a field was blank.
        };

        /**
          @brief Parse using a char as the delimiter.

          This is the normal and most performant way of parsing with subparser.

          @param [in] sb        The subbuffer to parse
          @param [in] delimiter Char to use as the delimiter.
          @param [in] skip      How to treat empty fields.

          @code
                  subbuffer line("1,2,3,4,,5,6,7,8,9,0,");
                  // line has two empty fields, between 4 and 5 and after 0
                  subparser numbs(line, ',', subparser::SKIP_EMPTY);
                  while (!numbs.empty())
                  {
                          subbuffer n(numbs.next());
                          ...
                  }
          @endcode
         */
        subparser(subbuffer sb, char delimiter, skip_empty_t skip) :
                m_buff(sb),
                m_delimiter_char(delimiter),
                m_delimiter_sub(0, 0),
                m_delimiter_cg(NULL),
                m_empty(false),
                m_type(CHAR_DELIM),
                m_skip_empty(skip)
        { this->skip(); }

        /**
          @brief Parse using a subbuffer as the delimiter.

          @param [in] sb        The subbuffer to parse
          @param [in] delimiter Subbuffer to use as the delimiter.
          @param [in] skip      How to treat empty fields.

          @code
                  subbuffer line("1.-.2.-.3.-.4.-.5");
                  // use ".-." as the delimiter
                  subparser numbs(line, CONST_SUBBUF(".-."), subparser::SKIP_EMPTY);
                  while (!numbs.empty())
                  {
                          subbuffer n(numbs.next());
                          ...
                  }
          @endcode
         */
        subparser(subbuffer sb, subbuffer delimiter, skip_empty_t skip) :
                m_buff(sb),
                m_delimiter_char(0),
                m_delimiter_sub(delimiter),
                m_delimiter_cg(NULL),
                m_empty(false),
                m_type(SUBBUF_DELIM),
                m_skip_empty(skip)
        { this->skip(); }

        /**
          @brief Parse using a chargrp as the delimiter.

          Provides the flexibility of allowing the delimiter to be any of the chars in the chargrp.
          That flexibility a small performance penalty, so only use it if needed.

          @param [in] sb        The subbuffer to parse
          @param [in] delimiter Chargrp to use as the delimiter.
          @param [in] skip      How to treat empty fields.

          @code
                  subbuffer line("1.2,3-4(5");
                  // mixed delimiters, use chargrp to specifiy all the possible delimiters
                  chargrp cg(".,-(");
                  subparser numbs(line, cg, subparser::SKIP_EMPTY);
                  while (!numbs.empty())
                  {
                          subbuffer n(numbs.next());
                          ...
                  }
          @endcode
         */
        subparser(subbuffer sb, const chargrp& delimiter, skip_empty_t skip) :
                m_buff(sb),
                m_delimiter_char(0),
                m_delimiter_sub(0, 0),
                m_delimiter_cg(&delimiter),
                m_empty(false),
                m_type(CHARGRP_DELIM),
                m_skip_empty(skip)
        { this->skip(); }

        /**
          @brief Check if there are any remaining fields.
         */
        bool empty()
        {
                if (SKIP_EMPTY == m_skip_empty) return m_buff.empty();
                return m_empty;
        }

        /**
          @brief Finds and returns the next field in the subbuffer.
                 Any empty subbuffer is returned when the subbuffer has been fully parsed.
          */
        subbuffer next()
        {
                skip();
                if (m_empty) return subbuffer(0, 0);
                switch (m_type)
                {
                case CHAR_DELIM:
                {
                        subbuffer sb = m_buff.before(m_delimiter_char);
                        m_empty = ((m_buff.length() == sb.length()) || m_buff.empty());
                        m_buff.advance(sb.length() + 1);
                        return sb;
                }
                case SUBBUF_DELIM:
                {
                        subbuffer sb = m_buff.before(m_delimiter_sub);
                        m_empty = ((m_buff.length() == sb.length()) || m_buff.empty());
                        m_buff.advance(sb.length() + m_delimiter_sub.length());
                        return sb;
                }
                case CHARGRP_DELIM:
                {
                        subbuffer sb = m_buff.before(*m_delimiter_cg);
                        m_empty = ((m_buff.length() == sb.length()) || m_buff.empty());
                        m_buff.advance(sb.length() + 1);
                        return sb;
                }
                }
                return subbuffer(0, 0);
        }

        /**
          @brief A convenience method for splitting a subbuffer out into a vector of subbuffers.

          @param [out] vect The vector to be filled. Caller is reponsible for clearing vect if desired.

          @code
                  subbuffer line("1,2,3,4,5");
                  vector<subbuffer> vals;
                  subparser numbs(line, ',', subparser::SKIP_EMPTY);
                  numbs.split(vals);
                  // vals now contains { "1", "2", "3", "4", "5" }
          @endcode
         */
        size_t split(std::vector<subbuffer>& vect)
        {
                size_t cnt = 0;
                while (!this->empty())
                {
                        vect.push_back(next());
                        cnt++;
                }
                return cnt;
        }

        /**
          @brief Accessor for the remainder of the original subbuffer.
         */
        subbuffer sub()
        {
                skip();
                return m_buff;
        }
private:
        void skip()
        {
                if (m_skip_empty != SKIP_EMPTY) return;

                switch (m_type)
                {
                case CHAR_DELIM:
                        m_buff.ltrim(m_delimiter_char);
                        break;
                case SUBBUF_DELIM:
                        while (m_buff.starts_with(m_delimiter_sub))
                                m_buff.advance(m_delimiter_sub.length());
                        break;
                case CHARGRP_DELIM:
                        m_buff.ltrim(*m_delimiter_cg);
                        break;
                }
                m_empty = m_buff.empty();
        }

private:
        subbuffer m_buff;               //!< The buffer to be parsed, shrinks as we parse
        char      m_delimiter_char;
        subbuffer m_delimiter_sub;
        const chargrp*  m_delimiter_cg;
        bool      m_empty;

        delimiter_type_t m_type;        //!< Set in the constructor. Based on which type of delimiter was passed in.
        skip_empty_t     m_skip_empty;
};

#endif
