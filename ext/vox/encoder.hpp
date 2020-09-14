#include "erlpack/encoder.h"
#include "erlpack/constants.h"
#include "etf.hpp"
#include "ruby.h"

namespace etf
{
    class encoder
    {
    public:
        encoder()
        {
            erl_buff = (erlpack_buffer *)malloc(sizeof(erl_buff));
            erl_buff->buf = (char *)malloc(sizeof(char) * 128);
            erl_buff->length = 0;
            erl_buff->allocated_size = 128;
            erlpack_append_version(erl_buff);
        }

        ~encoder()
        {
            free(erl_buff->buf);
            free(erl_buff);
        }

        void encode_object(VALUE input)
        {
            switch (TYPE(input))
            {
            case T_TRUE:
                encode_true();
                break;
            case T_FALSE:
                encode_false();
                break;
            case T_NIL:
                encode_nil();
                break;
            case T_FLOAT:
                encode_float(input);
                break;
            case T_BIGNUM:
                encode_bignum(input);
                break;
            case T_FIXNUM:
                encode_fixnum(input);
                break;
            case T_SYMBOL:
                encode_symbol(input);
                break;
            case T_STRING:
                encode_string(input);
                break;
            case T_ARRAY:
                encode_array(input);
                break;
            case T_HASH:
                encode_hash(input);
                break;
            default:
                if (rb_respond_to(input, rb_intern("to_hash")))
                {
                    VALUE hash = rb_funcall(input, rb_intern("to_hash"), 0);
                    Check_Type(hash, T_HASH);
                    encode_hash(hash);
                }
                else
                {
                    rb_raise(rb_eArgError, "Unsupported serialization type");
                }

                break;
            }
        }

        VALUE
        r_string()
        {
            return rb_str_new(erl_buff->buf, erl_buff->length);
        }

    private:
        erlpack_buffer *erl_buff;

        void encode_true()
        {
            erlpack_append_true(erl_buff);
        }

        void encode_false()
        {
            erlpack_append_false(erl_buff);
        }

        void encode_nil()
        {
            erlpack_append_nil(erl_buff);
        }

        void encode_fixnum(VALUE fixnum)
        {
            uint32_t n = NUM2UINT(fixnum);
            if (n > 0 && n <= UINT8_MAX)
                erlpack_append_small_integer(erl_buff, (uint8_t)n);
            else
                erlpack_append_integer(erl_buff, n);
        }

        void encode_bignum(VALUE bignum)
        {
            size_t byte_count = rb_absint_size(bignum, NULL);
            if (byte_count <= 0xFF)
            {
                // id byte | n byte | sign byte | data
                uint8_t buff[3 + byte_count];
                buff[0] = SMALL_BIG_EXT;
                buff[1] = byte_count;
                buff[2] = FIX2LONG(bignum) >= 0 ? 0 : 1;
                rb_integer_pack(bignum, buff + 3, byte_count, sizeof(uint8_t), 0, INTEGER_PACK_LITTLE_ENDIAN);
                erlpack_buffer_write(erl_buff, (const char *)buff, 3 + byte_count);
            }
            else
            {
                // id byte | 4 byte n | sign byte | data
                uint8_t buff[6 + byte_count];
                buff[0] = LARGE_BIG_EXT;
                _erlpack_store32(buff + 1, byte_count);
                buff[5] = RBIGNUM_SIGN(bignum) ? 0 : 1;
                rb_integer_pack(bignum, buff + 6, byte_count, sizeof(uint8_t), 0, INTEGER_PACK_LITTLE_ENDIAN);
                erlpack_buffer_write(erl_buff, (const char *)buff, 6 + byte_count);
            }
        }

        void encode_float(VALUE rfloat)
        {
            erlpack_append_double(erl_buff, NUM2DBL(rfloat));
        }

        void encode_array(VALUE array)
        {
            uint64_t size = RARRAY_LEN(array);
            if (size == 0)
            {
                erlpack_append_nil_ext(erl_buff);
                return;
            }
            else if (size > UINT32_MAX)
            {
                rb_raise(rb_eRangeError, "Array size is too large to fit into a 32 bit integer.");
                return;
            }

            erlpack_append_list_header(erl_buff, size);
            for (size_t index = 0; index < size; index++)
            {
                encode_object(RARRAY_AREF(array, index));
            }

            erlpack_append_nil_ext(erl_buff);
        }

        void encode_symbol(VALUE symbol)
        {
            encode_string(rb_sym2str(symbol));
        }

        void encode_string(VALUE string)
        {
            erlpack_append_binary(erl_buff, RSTRING_PTR(string), RSTRING_LEN(string));
        }

        void encode_hash(VALUE hash)
        {
            uint64_t size = RHASH_SIZE(hash);
            if (size > UINT32_MAX)
            {
                rb_raise(rb_eRangeError, "Hash size is too large to fit into a 32 bit integer");
                return;
            }

            erlpack_append_map_header(erl_buff, size);
            VALUE keys = rb_funcall(hash, rb_intern("keys"), 0);

            for (uint32_t index = 0; index < size * 2; index += 2)
            {
                VALUE key = RARRAY_AREF(keys, index / 2);
                // Normalize keys to strings because discord
                encode_object(key);
                encode_object(rb_hash_aref(hash, key));
            }
        }
    };
} // namespace etf