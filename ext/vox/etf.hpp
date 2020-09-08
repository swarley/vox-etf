#pragma once

#include "rice/Module.hpp"
#include "rice/Class.hpp"
#include "rice/Hash.hpp"
#include "./etf_reader.hpp"

#define ETF_VERSION 131

// Term IDs, first byte of any term.
enum class term
{
    new_float = 70,
    bit_binary = 77,
    atom_cache = 82,
    new_pid = 88,
    new_port = 89,
    newer_reference = 90,
    small_integer = 97,
    integer = 98,
    // annoying
    etf_float = 99,
    atom = 100,
    reference = 101,
    port = 102,
    pid = 103,
    small_tuple = 104,
    large_tuple = 105,
    nil = 106,
    string = 107,
    list = 108,
    binary = 109,
    small_big = 110,
    large_big = 111,
    new_fun = 112,
    // annoying
    etf_export = 113,
    new_reference = 114,
    small_atom = 115,
    map = 116,
    fun = 117,
    atom_utf8 = 118,
    small_atom_utf8 = 119
};

Rice::Object decode(Rice::Object self, Rice::String input);
inline Rice::Array decode_small_tuple(etf_reader *reader);
inline Rice::Array decode_large_tuple(etf_reader *reader);
inline Rice::Hash decode_map(etf_reader *reader);
inline Rice::Array decode_list(etf_reader *reader);
inline Rice::String decode_binary(etf_reader *reader);
inline Rice::String decode_string(etf_reader *reader);
inline Rice::Symbol decode_atom(etf_reader *reader);
inline Rice::Symbol decode_small_atom(etf_reader *reader);
inline Rice::Object decode_small_bignum(etf_reader *reader);
inline Rice::Object decode_large_bignum(etf_reader *reader);

// Setup function for ruby FFI.
extern "C" void Init_etf();