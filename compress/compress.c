/**********************************************************************************************/
/* compress.c      	                                                      					  */
/*                                                                       					  */
/* Copyright Paradigma Software, 1998-2026                                                    */
/* All Rights Reserved                                                   					  */
/**********************************************************************************************/
/**
    @brief Valentina Extension for ZIP compression.

    This file implements two functions:
    - `compress`: Compresses binary data using zlib and returns a BLOB with the size prepended.
    - `uncompress`: Uncompresses data previously compressed by the `compress` function.

    @note This extension uses zlib for compression and decompression.

    @note This extension is written in C and uses the Valentina Extension API.
*/

// STD:
#include <stdlib.h>

// ZLIB:
#include <zlib.h>

// VALENTINA DB EXTENSION:
//
// The only file you need include to create a Valentina extension in C/C++
//
#include <FBL_Extension.h>


/**********************************************************************************************/
/**
    @brief Compresses binary data using zlib and returns a BLOB with the size prepended.

    This function takes binary data as input, compresses it using zlib,
    and returns a BLOB that starts with a 64-bit integer indicating the size of the compressed data.

    @param ctx The Valentina context for the extension call

    @param args The arguments passed to the function.
                We have only one argument, which is the binary data to compress.

    @return A Valentina value containing the compressed binary data, or NULL on error.
*/
valentina_value_t compressFunc( valentina_context_t* ctx, valentina_value_t* args )
{
    // GET the binary data from the first argument:
    int64_t len;
    const void* pData = valentina_get_binary( args[ 0 ], &len );

    if( !pData )
        return NULL;


    // WORK with ZLIB:
    unsigned long  outLen = compressBound( len );
    unsigned char* pBuff  = malloc( outLen + 8 );

    compress( pBuff + 8, &outLen, pData, len );
    *((int64_t*) pBuff) = outLen;


    // Return the compressed data as a Valentina binary value
    return valentina_create_binary( ctx, pBuff, outLen + 8, free );
}


/**********************************************************************************************/
valentina_value_t uncompressFunc( valentina_context_t* ctx, valentina_value_t* args )
{
    // GET the binary data from the first argument:
    int64_t len;
    const unsigned char* pData = valentina_get_binary( args[ 0 ], &len );

    if( !pData || len < 8 )
    {
        valentina_throw_error( ctx, "Invalid data.", -1 );
        return NULL;
    }

    // WORK with ZLIB:
    unsigned long  outLen = *((int64_t*) pData);
    unsigned char* pBuff    = malloc( outLen );

    const int rc = uncompress( pBuff, &outLen, pData + 8, len - 8 );

    if( rc != Z_OK )
    {
        valentina_throw_error( ctx, "Invalid data", -1 );
        return NULL;
    }


    // Return the uncompressed data as a Valentina binary value:
    return valentina_create_binary( ctx, pBuff, outLen, free );
}


/**********************************************************************************************/
/**
    @brief Registers the compress() and uncompress() functions with the Valentina DB engine,
    allowing them to be used in SQL queries or other operations.

    This function is called once when the Valentina DB engine loads the extension.

    @param ctx The Valentina context for the extension initialization

    @note A single extension can register multiple functions.

    @note To initialize the `valentina_function_t` structure,
          designated initializer syntax from the C99 standard is used.
*/
VALENTINA_EXTENSION_DEFINE_ENTRY
void valentina_extension_init( valentina_context_t* ctx );
void valentina_extension_init( valentina_context_t* ctx )
{
    // Register the compress() function:
    valentina_create_function( ctx, (valentina_function_t) {
        .xFunc      = compressFunc,
        .name       = "compress",
        .resultType = VALENTINA_TYPE_BINARY,
        .minArg     = 1,
        .maxArg     = 1,
        .argDesc    = "data",
        .desc       = "The output is a BLOB that begins with a 64-bit integer that is the input size in bytes",
        .flags      = VALENTINA_FUNCTION_FLAG_IMMUTABLE });


    // Register the uncompress() function:
    valentina_create_function( ctx, (valentina_function_t) {
        .xFunc      = uncompressFunc,
        .name       = "uncompress",
        .resultType = VALENTINA_TYPE_BINARY,
        .minArg     = 1,
        .maxArg     = 1,
        .argDesc    = "data",
        .desc       = "Uncompress data previously compressed by 'compress' function.",
        .flags      = VALENTINA_FUNCTION_FLAG_IMMUTABLE });
}
