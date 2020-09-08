#pragma once

#include "rice/Exception.hpp"
#include "endian.h"

class etf_reader
{
private:
    const char *end;
    char *cursor;
    void check_bounds(size_t size);

public:
    etf_reader(char *begin, char *end_ptr);
    uint8_t read_8(void);
    uint16_t read_16(void);
    uint32_t read_32(void);
    std::string read_string(uint32_t size);
    double read_double();
};

void etf_reader::check_bounds(size_t size)
{
    if ((cursor + size) > end)
        throw Rice::Exception(rb_eRangeError, "Out of bounds");
}

etf_reader::etf_reader(char *begin, char *end_ptr)
{
    end = end_ptr;
    cursor = (char *)begin;
}

uint8_t etf_reader::read_8(void)
{
    check_bounds(1);
    return (uint8_t)*cursor++;
}

uint16_t etf_reader::read_16(void)
{
    check_bounds(2);
    uint16_t ret = (cursor[1] << 0) | (cursor[0] << 8);
    cursor += 2;
    return ret;
}

uint32_t etf_reader::read_32(void)
{
    check_bounds(4);
    uint32_t ret = (cursor[3] << 0) | (cursor[2] << 8) | (cursor[1] << 16) | (cursor[0] << 24);
    cursor += 4;
    return ret;
}

std::string etf_reader::read_string(uint32_t size)
{
    check_bounds(size);
    std::string ret{cursor, size};
    cursor += size;
    return ret;
}

union double_swapper
{
    char buff[8];
    uint64_t u;
    double d;
};

double etf_reader::read_double()
{
    check_bounds(8);

    // Fix byte order
    char buff[8] = {cursor[7], cursor[6], cursor[5], cursor[4], cursor[3], cursor[2], cursor[1], cursor[0]};
    double d;
    memcpy(&d, &buff, 8);

    return d;
}
