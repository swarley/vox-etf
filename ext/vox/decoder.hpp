#pragma once
#include <zlib.h>
#include "ruby.h"
#include "erlpack/sysdep.h"
#include "erlpack/constants.h"

/* This code is highly derivative of discord's erlpack decoder
 * targeting Javascript.
 * 
 * 
 *   MIT License
 *
 *    Copyright (c) 2017 Discord
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in all
 *   copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *   SOFTWARE.
 */

namespace etf
{
    class decoder
    {
    public:
        decoder(VALUE str)
            : data((const uint8_t *)RSTRING_PTR(str)), size(RSTRING_LEN(str)), offset(0)
        {
            if (read8() != FORMAT_VERSION)
                rb_raise(rb_eArgError, "Invalid version: %i", ETF_VERSION);
        }

        decoder(const uint8_t *str, size_t data_size, bool skip_version = false) : data(str), size(data_size), offset(0)
        {
            if (skip_version)
                return;
            if (read8() != FORMAT_VERSION)
                rb_raise(rb_eArgError, "Invalid version: %i", ETF_VERSION);
        }

        VALUE decode_term()
        {
            if (offset >= size)
            {
                rb_raise(rb_eRangeError, "Decoding beyond the end of the buffer");
                return Qnil;
            }

            const uint8_t type = read8();
            switch (type)
            {
            case SMALL_INTEGER_EXT:
                return decode_small_integer();
            case INTEGER_EXT:
                return decode_integer();
            case FLOAT_EXT:
                return decode_float();
            case NEW_FLOAT_EXT:
                return decode_new_float();
            case ATOM_EXT:
            case ATOM_UTF8_EXT:
                return decode_atom();
            case SMALL_ATOM_EXT:
            case SMALL_ATOM_UTF8_EXT:
                return decode_small_atom();
            case SMALL_TUPLE_EXT:
                return decode_small_tuple();
            case LARGE_TUPLE_EXT:
                return decode_large_tuple();
            case NIL_EXT:
                return decode_nil();
            case STRING_EXT:
                return decode_string_as_list();
            case LIST_EXT:
                return decode_list();
            case MAP_EXT:
                return decode_map();
            case BINARY_EXT:
                return decode_binary_as_string();
            case SMALL_BIG_EXT:
                return decode_small_bignum();
            case LARGE_BIG_EXT:
                return decode_large_bignum();
            case COMPRESSED:
                return decode_compressed();
            default:
                rb_raise(rb_eArgError, "Unsupported type identifier `%i' found", type);
                return Qnil;
            }
        }

    private:
        const uint8_t *data;
        const size_t size;
        size_t offset;

        uint8_t read8(void)
        {
            if (offset + sizeof(uint8_t) > size)
            {
                rb_raise(rb_eRangeError, "Reading a byte passes the end of the buffer");
                return 0;
            }
            uint8_t val = *reinterpret_cast<const uint8_t *>(data + offset);
            offset += sizeof(uint8_t);
            return val;
        }

        uint16_t read16()
        {
            if (offset + sizeof(uint16_t) > size)
            {
                rb_raise(rb_eRangeError, "Reading two bytes passes the end of the buffer");
                return 0;
            }

            uint16_t val = _erlpack_be16(*reinterpret_cast<const uint16_t *>(data + offset));
            offset += sizeof(uint16_t);
            return val;
        }

        uint32_t read32()
        {
            if (offset + sizeof(uint32_t) > size)
            {
                rb_raise(rb_eRangeError, "Reading four bytes passes the end of the buffer");
                return 0;
            }

            uint32_t val = _erlpack_be32(*reinterpret_cast<const uint32_t *>(data + offset));
            offset += sizeof(uint32_t);
            return val;
        }

        uint64_t read64()
        {
            if (offset + sizeof(uint64_t) > size)
            {
                rb_raise(rb_eRangeError, "Reading eight bytes passes the end of the buffer");
                return 0;
            }

            uint64_t val = _erlpack_be64(*reinterpret_cast<const uint64_t *>(data + offset));
            offset += sizeof(uint64_t);
            return val;
        }

        VALUE decode_small_integer()
        {
            return INT2FIX(read8());
        }

        VALUE decode_integer()
        {
            return INT2NUM(read32());
        }

        VALUE decode_array(int32_t length)
        {
            VALUE values[length];

            for (int32_t index = 0; index < length; index++)
                values[index] = decode_term();
            return rb_ary_new_from_values(length, values);
        }

        VALUE decode_list()
        {
            const uint32_t length = read32();
            VALUE array = decode_array(length);
            const uint8_t tail = read8();

            if (tail != NIL_EXT)
            {
                rb_raise(rb_eArgError, "List doesn't end with `NIL`, but it must!");
                return Qnil;
            }

            return array;
        }

        VALUE decode_tuple(uint32_t length)
        {
            return decode_array(length);
        }

        VALUE decode_nil()
        {
            return rb_ary_new();
        }

        VALUE decode_map()
        {
            const uint32_t length = read32();
            VALUE hash = rb_hash_new();

            for (uint32_t index = 0; index < length; index++)
            {
                const VALUE key = decode_term();
                const VALUE value = decode_term();

                rb_hash_aset(hash, key, value);
            }

            return hash;
        }

        const char *read_string(uint32_t length)
        {
            if (offset + length > size)
            {
                rb_raise(rb_eRangeError, "Reading sequence past the end of the buffer");
                return 0;
            }

            const uint8_t *str = data + offset;
            offset += length;
            return (const char *)str;
        }

        VALUE process_atom(const char *atom, uint16_t length)
        {
            if (atom == NULL)
            {
                return Qnil;
            }

            if (length >= 3 && length <= 5)
            {
                if (length == 3 && strncmp(atom, "nil", 3) == 0)
                    return Qnil;
                // Is this actually a possible to receive from discord?
                // It's defined in the js erlpack decoder but I'm not sure.
                else if (length == 4 && strncmp(atom, "null", 4) == 0)
                    return Qnil;
                else if (length == 4 && strncmp(atom, "true", 4) == 0)
                    return Qtrue;
                else if (length == 5 && strncmp(atom, "false", 5) == 0)
                    return Qfalse;
            }

            return ID2SYM(rb_intern2(atom, length));
        }

        VALUE decode_atom()
        {
            uint16_t length = read16();
            const char *atom = read_string(length);
            return process_atom(atom, length);
        }

        VALUE decode_small_atom()
        {
            uint8_t length = read8();
            const char *atom = read_string(length);
            return process_atom(atom, length);
        }

        VALUE decode_float()
        {
            const uint8_t FLOAT_LENGTH = 31;
            const char *float_string = read_string(FLOAT_LENGTH);

            if (float_string == NULL)
                return Qnil;

            double number;
            char buff[FLOAT_LENGTH + 1] = {0};
            memcpy(buff, float_string, FLOAT_LENGTH);

            uint32_t count = sscanf(buff, "%lf", &number);
            if (count != 1)
            {
                rb_raise(rb_eArgError, "Invalid float encoded.");
                return Qnil;
            }

            return DBL2NUM(number);
        }

        VALUE decode_new_float()
        {
            uint64_t u64 = read64();
            double dbl;
            memcpy(&dbl, &u64, sizeof(double));

            return DBL2NUM(dbl);
        }

        VALUE decode_big(uint32_t length)
        {
            const uint8_t sign = read8();
            const char *buff = read_string(length);

            int flags = INTEGER_PACK_LITTLE_ENDIAN | (sign * INTEGER_PACK_NEGATIVE);
            return rb_integer_unpack(buff, length, 1, 0, flags);
        }

        VALUE decode_small_bignum()
        {
            const uint8_t length = read8();
            return decode_big(length);
        }

        VALUE decode_large_bignum()
        {
            const uint32_t length = read32();
            return decode_big(length);
        }

        VALUE decode_binary_as_string()
        {
            const uint32_t length = read32();
            const char *str = read_string(length);

            if (str == NULL)
                return Qnil;

            return rb_str_new(str, length);
        }

        VALUE decode_string_as_list()
        {
            const uint16_t length = read16();

            if (offset + length > size)
            {
                rb_raise(rb_eRangeError, "Reading sequence past the end of the buffer");
                return Qnil;
            }

            VALUE values[length];
            for (uint16_t index = 0; index < length; index++)
                values[index] = INT2FIX(read8());
            return rb_ary_new_from_values(length, values);
        }

        VALUE decode_small_tuple()
        {
            return decode_tuple(read8());
        }

        VALUE decode_large_tuple()
        {
            return decode_tuple(read32());
        }

        VALUE decode_compressed()
        {
            const uint32_t decompressed_size = read32();

            unsigned long source_size = decompressed_size;
            uint8_t *out_buffer = (uint8_t *)malloc(decompressed_size);
            const int ret = uncompress(out_buffer, &source_size, (const unsigned char *)(data + offset), (uLong)(size - offset));

            offset += source_size;
            if (ret != Z_OK)
            {
                free(out_buffer);
                rb_raise(rb_eArgError, "Failed to uncompress compressed item");
                return Qnil;
            }

            decoder decompressed(out_buffer, decompressed_size, true);
            VALUE value = decompressed.decode_term();
            free(out_buffer);
            return value;
        }
    };
} // namespace etf