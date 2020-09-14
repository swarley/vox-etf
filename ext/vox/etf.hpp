#pragma once

#include "./extconf.h"
#define ETF_VERSION 131

VALUE decode(VALUE self, VALUE input);
VALUE encode(VALUE self, VALUE input);

// Setup function for ruby FFI.
extern "C" void Init_etf();