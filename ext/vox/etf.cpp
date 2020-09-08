#include "./etf_reader.hpp"
#include "./etf.hpp"
#include "stdio.h"
/****
 * Notes:
 *   We have to include self even on module level singletons
 ****/

// Decode an ETF term to a ruby object.
// Wrapper for {decode} that doesn't check for the version byte.
// @param data [char*] The term to be decoded.
// @return [Rice::Object] The decoded term as a ruby object.
Rice::Object
decode_term(etf_reader *reader)
{

    switch ((term)reader->read_8())
    {
    // ID [0]
    // INT [1]
    case term::small_integer:
        return to_ruby((uint8_t)reader->read_8());
    // ID [0]
    // INT [1-4]
    case term::integer:
        return to_ruby((int32_t)reader->read_32());
    // ID [0]
    // ARITY [1]
    // ELEMENTS [N]
    case term::small_tuple:
        return decode_small_tuple(reader);
    case term::large_tuple:
        return decode_large_tuple(reader);
    case term::map:
        return decode_map(reader);
    case term::nil:
        reader->read_8();
        return Rice::Array();
    case term::list:
        return decode_list(reader);
    case term::binary:
        return decode_binary(reader);
    case term::string:
        return decode_string(reader);
    case term::new_float:
        return to_ruby(reader->read_double());
    case term::atom:
    case term::atom_utf8:
        return decode_atom(reader);
    case term::small_atom:
    case term::small_atom_utf8:
        return decode_small_atom(reader);
    case term::small_big:
        return decode_small_bignum(reader);
    case term::large_big:
        return decode_large_bignum(reader);
    default:
        // TODO: Add term type to exception text
        throw Rice::Exception(rb_eArgError, "Unsupported term type");
    }

    return Rice::Nil;
}

inline Rice::Array
decode_small_tuple(etf_reader *reader)
{
    uint8_t arity = reader->read_8();
    Rice::Array arr;
    for (uint8_t index = 0; index < arity; index++)
    {
        arr.push(decode_term(reader));
    }
    return arr;
}

inline Rice::Array
decode_large_tuple(etf_reader *reader)
{
    uint32_t arity = reader->read_32();
    Rice::Array arr;
    for (uint32_t index = 0; index < arity; index++)
    {
        arr.push(decode_term(reader));
    }
    return arr;
}

inline Rice::Hash
decode_map(etf_reader *reader)
{
    uint32_t arity = reader->read_32();
    Rice::Hash hash;
    for (uint32_t index = 0; index < arity; index++)
    {
        Rice::Object key = decode_term(reader);
        Rice::Object value = decode_term(reader);

        if (key.rb_type() == RUBY_T_STRING)
            key = ((Rice::String)key).intern().to_sym();

        hash[key] = value;
    }
    return hash;
}

inline Rice::Array
decode_list(etf_reader *reader)
{
    uint32_t arity = reader->read_32();
    Rice::Array arr;
    for (uint32_t index = 0; index < (arity - 1); index++)
    {
        arr.push(decode_term(reader));
    }

    Rice::Object tail = decode_term(reader);
    if (tail.rb_type() != RUBY_T_ARRAY || ((Rice::Array)tail).size() > 0)
    {
        arr.push(tail);
    }

    return arr;
}

inline Rice::String
decode_binary(etf_reader *reader)
{
    uint32_t size = reader->read_32();
    std::string str = reader->read_string(size);
    return Rice::String(str);
}

inline Rice::String
decode_string(etf_reader *reader)
{
    uint16_t size = reader->read_16();
    std::string str = reader->read_string(size);
    return Rice::String(str);
}

inline Rice::Symbol
decode_atom(etf_reader *reader)
{
    uint16_t size = reader->read_16();
    std::string str = reader->read_string(size);
    return Rice::Symbol(str);
}

inline Rice::Symbol
decode_small_atom(etf_reader *reader)
{
    uint8_t size = reader->read_8();
    std::string str = reader->read_string(size);
    return Rice::Symbol(str);
}

inline Rice::Object
decode_small_bignum(etf_reader *reader)
{
    uint8_t size = reader->read_8();
    uint8_t sign = reader->read_8();
    uint8_t *data = (uint8_t *)reader->read_string(size).c_str();
    return rb_integer_unpack(data, size, 1, 0, INTEGER_PACK_LITTLE_ENDIAN | (sign * INTEGER_PACK_NEGATIVE));
}

inline Rice::Object
decode_large_bignum(etf_reader *reader)
{
    uint32_t size = reader->read_32();
    uint8_t sign = reader->read_8();
    uint8_t *data = (uint8_t *)reader->read_string(size).c_str();
    return rb_integer_unpack(data, size, 1, 0, INTEGER_PACK_LITTLE_ENDIAN | (sign * INTEGER_PACK_NEGATIVE));
}

// Decode an ETF term from a ruby string
// @param self [Object] Vox::ETF
// @param input [Rice::String] The ETF term to be decoded.
// @return [Rice::Object] Whatever the term is, as a ruby object.
Rice::Object decode(Rice::Object self, Rice::String input)
{
    char *data = (char *)input.c_str();
    if ((uint8_t)data[0] != ETF_VERSION)
    {
        throw Rice::Exception(rb_eArgError, "Unsupported ETF version");
    }
    // Add one to discard the version byte
    etf_reader *reader = new etf_reader(data + 1, data + input.length() + 1);
    return decode_term(reader);
}

/*
 Method called when the shared object is required in ruby.
 Sets up modules and binds methods.
*/
extern "C" void Init_etf()
{
    Rice::Module rb_mVox{Rice::define_module("Vox")};
    Rice::Module rb_mETF{rb_mVox.define_module("ETF")};
    rb_mETF.define_singleton_method("decode", &decode);
}