/**
 * @file
 *
 * This file contains code lifted from
 *  https://github.com/ClausKlein/minijson_writer/
 *
 * This is a fork of minijson_writer created by ClausKlein. Until piwcsprwmodel
 * switches to that fork of minijson_writer, this file will remain to add some
 * functionality to the library.
 *
 * @par License
 * &copy; ClausKlein (https://github.com/ClausKlein/)
 *
 * The content of this file in particular is licensed under the 3-clause BSD
 * license as specified in
 *   https://github.com/ClausKlein/minijson_writer/blob/develop/LICENSE.txt
 */

#ifndef STRING_VIEW_WRITER_H
#define STRING_VIEW_WRITER_H

#include <iomanip>
#include <iostream>
#include <string_view>

/**
 * Prints out a JSON-quoted string.
 *
 * @author ClausKlein https://github.com/ClausKlein/
 */
inline void write_quoted_string(std::ostream &stream,
                                const std::string_view str) {
    stream << std::hex << std::right << std::setfill('0');
    stream << '"';

    for (auto c : str) {
        switch (c) {
        case '"':
            stream << "\\\"";
            break;

        case '\\':
            stream << "\\\\";
            break;

        case '\n':
            stream << "\\n";
            break;

        case '\r':
            stream << "\\r";
            break;

        case '\t':
            stream << "\\t";
            break;

        default:
            // TODO: that may also work CK! if (std::iscntrl(c))
            // NOTE: this writes "\0" too! CK
            if ((c >= 0 && c < 32) || c == 127) {
                stream << "\\u";
                stream.flush();
                stream << std::setw(4) << static_cast<unsigned>(c);
            } else {
                stream << c;
            }
            break;
        }
    }

    stream << '"';
    stream.flush();
    stream << std::dec;
}

#endif // STRING_VIEW_WRITER_H
