#include "ruby.h"
#include "encoder.hpp"
#include "decoder.hpp"
#include "etf.hpp"

VALUE decode(VALUE self, VALUE input)
{
    Check_Type(input, T_STRING);

    etf::decoder decoder(input);
    return decoder.decode_term();
}

VALUE encode(VALUE self, VALUE input)
{
    etf::encoder enc;
    enc.encode_object(input);
    return enc.r_string();
}

/*
 Method called when the shared object is required in ruby.
 Sets up modules and binds methods.
*/
extern "C" void Init_etf()
{
    VALUE mVox = rb_define_module("Vox");
    VALUE mETF = rb_define_module_under(mVox, "ETF");
    rb_define_singleton_method(mETF, "decode", reinterpret_cast<VALUE (*)(...)>(decode), 1);
    rb_define_singleton_method(mETF, "encode", reinterpret_cast<VALUE (*)(...)>(encode), 1);
}