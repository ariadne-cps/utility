/***************************************************************************
 *            utility/string.hpp
 *
 *  Copyright  2023  Pieter Collins
 *
 ****************************************************************************/

/*
 *  This file is part of Ariadne Utility.
 *
 *  Ariadne Utility is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Ariadne Utility is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Ariadne Utility.  If not, see <https://www.gnu.org/licenses/>.
 */

/** @file utility/string.hpp
 *  @brief Wrapper for string class
 */

#ifndef ARIADNE_UTILITY_STRING_HPP
#define ARIADNE_UTILITY_STRING_HPP

#include <sstream>
#include <string>

namespace Ariadne {

//! \brief A wrapper for the standard string class.
class String : public std::string {
  public:
    using std::string::string;
    /** @brief Construct a \c String from str. */
    String(const std::string& str) : std::string(str) {};
    /** @brief Construct a \c String with the specified characteristics. */
    String() = default;
};

/** @brief Return the null-terminated character sequence. */
inline const char* c_str(const String& str) {
    return str.c_str();
}
/** @brief Return the string representation. */
template<class T>
inline String to_string(const T& t) {
    std::stringstream ss;
    ss << t;
    return ss.str();
}
/** @brief Return the string representation. */
template<class T>
inline String to_str(const T& t) {
    return to_string(t);
}

/** @brief Return the display name of the type. */
template<class T>
String class_name();
/** @brief Return the display name of the type. */
template<>
inline String class_name<String>() { return "String"; }

} // namespace Ariadne

#endif /* ARIADNE_UTILITY_STRING_HPP */
