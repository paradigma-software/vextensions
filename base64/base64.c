/**********************************************************************************************/
/* base64.c       	                                                      					  */
/*                                                                       					  */
/* Copyright Paradigma Software, 1998-2026                                                    */
/* All Rights Reserved                                                   					  */
/**********************************************************************************************/
/**
    @brief Valentina Extension for Base64 Encoding and Decoding

    This file implements two functions:
    - `base64encode`: Converts binary data into Base64-encoded text.
    - `base64decode`: Converts Base64-encoded text back into binary data.

    These functions can be used in SQL queries or other operations within the Valentina database.

    @note This extension is written in C and uses the Valentina Extension API.
*/

// STD:
#include <stdlib.h>
#include <string.h>


// VALENTINA DB EXTENSION:
//
// The only file you need include to create a Valentina extension in C/C++
//
#include <FBL_Extension.h>

/**********************************************************************************************/
/*
 * BSD license
 *
 * Base64 encoding/decoding (RFC1341)
 * Copyright (c) 2005-2011, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

static const unsigned char base64_table[65] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


/**********************************************************************************************/
/**
 * base64_encode - Base64 encode
 * @src: Data to be encoded
 * @len: Length of the data to be encoded
 * @out_len: Pointer to output length variable, or %NULL if not used
 * Returns: Allocated buffer of out_len bytes of encoded data,
 * or %NULL on failure
 *
 * Caller is responsible for freeing the returned buffer. Returned buffer is
 * nul terminated to make it easier to use as a C string. The nul terminator is
 * not included in out_len.
 */
unsigned char * base64_encode(
    const unsigned char *src,
    size_t              len,
    size_t              *out_len )
{
    unsigned char *out, *pos;
    const unsigned char *end, *in;
    size_t olen;
    int line_len;

    olen = len * 4 / 3 + 4; /* 3-byte blocks to 4-byte */
    olen += olen / 72; /* line feeds */
    olen++; /* nul termination */
    if (olen < len)
        return NULL; /* integer overflow */
    out = malloc(olen);
    if (out == NULL)
        return NULL;

    end = src + len;
    in = src;
    pos = out;
    line_len = 0;
    while (end - in >= 3) {
        *pos++ = base64_table[in[0] >> 2];
        *pos++ = base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
        *pos++ = base64_table[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
        *pos++ = base64_table[in[2] & 0x3f];
        in += 3;
        line_len += 4;
        if (line_len >= 72) {
            *pos++ = '\n';
            line_len = 0;
        }
    }

    if (end - in) {
        *pos++ = base64_table[in[0] >> 2];
        if (end - in == 1) {
            *pos++ = base64_table[(in[0] & 0x03) << 4];
            *pos++ = '=';
        } else {
            *pos++ = base64_table[((in[0] & 0x03) << 4) |
                          (in[1] >> 4)];
            *pos++ = base64_table[(in[1] & 0x0f) << 2];
        }
        *pos++ = '=';
        line_len += 4;
    }

    *pos = '\0';
    if (out_len)
        *out_len = pos - out;
    return out;
}


/**********************************************************************************************/
/**
 * base64_decode - Base64 decode
 * @src: Data to be decoded
 * @len: Length of the data to be decoded
 * @out_len: Pointer to output length variable
 * Returns: Allocated buffer of out_len bytes of decoded data,
 * or %NULL on failure
 *
 * Caller is responsible for freeing the returned buffer.
 */
unsigned char * base64_decode(const unsigned char *src, size_t len,
                  size_t *out_len)
{
    unsigned char dtable[256], *out, *pos, block[4], tmp;
    size_t i, count, olen;
    int pad = 0;

    memset(dtable, 0x80, 256);
    for (i = 0; i < sizeof(base64_table) - 1; i++)
        dtable[base64_table[i]] = (unsigned char) i;
    dtable['='] = 0;

    count = 0;
    for (i = 0; i < len; i++) {
        if (dtable[src[i]] != 0x80)
            count++;
    }

    if (count == 0 || count % 4)
        return NULL;

    olen = count / 4 * 3;
    pos = out = malloc(olen);
    if (out == NULL)
        return NULL;

    count = 0;
    for (i = 0; i < len; i++) {
        tmp = dtable[src[i]];
        if (tmp == 0x80)
            continue;

        if (src[i] == '=')
            pad++;
        block[count] = tmp;
        count++;
        if (count == 4) {
            *pos++ = (block[0] << 2) | (block[1] >> 4);
            *pos++ = (block[1] << 4) | (block[2] >> 2);
            *pos++ = (block[2] << 6) | block[3];
            count = 0;
            if (pad) {
                if (pad == 1)
                    pos--;
                else if (pad == 2)
                    pos -= 2;
                else {
                    /* Invalid padding */
                    free(out);
                    return NULL;
                }
                break;
            }
        }
    }

    *out_len = pos - out;
    return out;
}


/**********************************************************************************************/
valentina_value_t base64encode( valentina_context_t* ctx, valentina_value_t* args )
{
    int64_t len;
    const void* data = valentina_get_binary( args[ 0 ], &len );

    if( !data )
        return NULL;

    size_t out_len;
    unsigned char* encoded = base64_encode( data, (size_t) len, &out_len );

    if( encoded )
        return valentina_create_text( ctx, (const char*) encoded, out_len, free );

    valentina_throw_error( ctx, "Not enough memory.", -1 );
    return NULL;
}


/**********************************************************************************************/
valentina_value_t base64decode( valentina_context_t* ctx, valentina_value_t* args )
{
    int64_t len;
    const void* data = valentina_get_binary( args[ 0 ], &len );

    size_t out_len;
    unsigned char* decoded = base64_decode( data, (size_t) len, &out_len );

    if( decoded )
        return valentina_create_binary( ctx, decoded, out_len, free );

    valentina_throw_error( ctx, "Not enough memory.", -1 );
    return NULL;
}


/**********************************************************************************************/
/**
    @brief Registers the base64encode() and base64decode() functions with the Valentina DB engine,
    allowing them to be used in SQL queries or other operations.

    This function is called once when the Valentina DB engine loads the extension.

    @param ctx The Valentina context for the function call

    @note A single extension can register multiple functions.

    @note To initialize the `valentina_function_t` structure,
          designated initializer syntax from the C99 standard is used.
*/
VALENTINA_EXTENSION_DEFINE_ENTRY
void valentina_extension_init( valentina_context_t* ctx );
void valentina_extension_init( valentina_context_t* ctx )
{
    // Register the base64encode() function:
    valentina_create_function( ctx, (valentina_function_t) {
        .xFunc      = base64encode,
        .name       = "base64encode",
        .resultType = VALENTINA_TYPE_TEXT,
        .minArg     = 1,
        .maxArg     = 1,
        .argDesc    = "data",
        .desc       = "Converts a binary into base64-encoded text." });


    // Register the base64decode() function:
    valentina_create_function( ctx, (valentina_function_t) {
        .xFunc      = base64decode,
        .name       = "base64decode",
        .resultType = VALENTINA_TYPE_BINARY,
        .minArg     = 1,
        .maxArg     = 1,
        .argDesc    = "data",
        .desc       = "Converts a base64-encoded text to binary." });
}
