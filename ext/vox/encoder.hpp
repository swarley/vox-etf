#include "erlpack/encoder.h"
#include "etf.hpp"

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
                rb_raise(rb_eArgError, "Unsupported serialization type");
                break;
            }
        }

        VALUE
        r_string(void)
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
            long l = FIX2LONG(fixnum);
            if (l > 0 && l <= 0xFF)
                erlpack_append_small_integer(erl_buff, (unsigned char)l);
            else
                erlpack_append_integer(erl_buff, l);
        }

        void encode_bignum(VALUE bignum)
        {
            size_t byte_count = rb_absint_size(bignum, NULL);
            if (byte_count <= 0xFF)
            {
                // id byte | n byte | sign byte | data
                uint8_t buff[3 + byte_count];
                buff[0] = term::small_big;
                buff[1] = byte_count;
                buff[2] = FIX2LONG(bignum) >= 0 ? 0 : 1;
                rb_integer_pack(bignum, buff + 3, byte_count, sizeof(uint8_t), 0, INTEGER_PACK_LITTLE_ENDIAN);
                erlpack_buffer_write(erl_buff, (const char *)buff, 3 + byte_count);
            }
            else
            {
                // id byte | 4 byte n | sign byte | data
                uint8_t buff[6 + byte_count];
                buff[0] = term::large_big;
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
            erlpack_append_list_header(erl_buff, RARRAY_LEN(array));
            uint32_t size = RARRAY_LEN(array);
            for (uint32_t index = 0; index < size; index++)
            {
                encode_object(RARRAY_AREF(array, index));
            }
        }

        void encode_symbol(VALUE symbol)
        {
            VALUE str = rb_sym2str(symbol);
            erlpack_append_atom_utf8(erl_buff, RSTRING_PTR(str), RSTRING_LEN(str));
        }

        void encode_string(VALUE string)
        {
            erlpack_append_binary(erl_buff, RSTRING_PTR(string), RSTRING_LEN(string));
        }

        void encode_hash(VALUE hash)
        {
            uint32_t size = RHASH_SIZE(hash);
            erlpack_append_map_header(erl_buff, size);
            VALUE keys = rb_funcall(hash, rb_intern("keys"), 0);

            for (uint32_t index = 0; index < size * 2; index += 2)
            {
                VALUE key = RARRAY_AREF(keys, index / 2);
                encode_object(key);
                encode_object(rb_hash_aref(hash, key));
            }
        }
    };
} // namespace etf