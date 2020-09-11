#pragma once

#include "./etf_reader.hpp"

#define ETF_VERSION 131

// Term IDs, first byte of any term.
enum term
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

VALUE decode(VALUE self, VALUE input);
inline VALUE decode_small_tuple(etf_reader *reader);
inline VALUE decode_large_tuple(etf_reader *reader);
inline VALUE decode_map(etf_reader *reader);
inline VALUE decode_list(etf_reader *reader);
inline VALUE decode_binary(etf_reader *reader);
inline VALUE decode_string(etf_reader *reader);
inline VALUE decode_atom(etf_reader *reader);
inline VALUE decode_small_atom(etf_reader *reader);
inline VALUE decode_small_bignum(etf_reader *reader);
inline VALUE decode_large_bignum(etf_reader *reader);

VALUE encode(VALUE self, VALUE input);

// Setup function for ruby FFI.
extern "C" void Init_etf();