/**********************************************************************************************/
/* ocr.cpp        	                                                      					  */
/*                                                                       					  */
/* Copyright Paradigma Software, 1998-2026                                                    */
/* All Rights Reserved                                                   					  */
/**********************************************************************************************/

// STD:
#include <cstring>
#include <filesystem>
#include <string>

// SYSTEM:
#if defined( _WIN32 )
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include <windows.h>
#else
#include <cstdlib>
    #include <dlfcn.h>
#endif

// Tesseract OCR:
#include <tesseract/baseapi.h>
#include <allheaders.h>

// VALENTINA DB EXTENSION:
#include "FBL_Extension.h"

/**********************************************************************************************/
namespace {

/**********************************************************************************************/
const char kModuleAddressAnchor = 0;

/**********************************************************************************************/
void delete_string( void* p )
{
    delete[] static_cast<char*>( p );
}

/**********************************************************************************************/
std::string tessdata_path_next_to( const std::string& modulePath )
{
    const size_t pos = modulePath.find_last_of( "/\\" );
    if( pos == std::string::npos )
        return "tessdata";

    return modulePath.substr( 0, pos + 1 ) + "tessdata";
}

/**********************************************************************************************/
bool directory_exists( const std::string& path )
{
    std::error_code error;
    return std::filesystem::is_directory( path, error );
}

/**********************************************************************************************/
std::string real_module_path( const std::string& modulePath )
{
#if defined( _WIN32 )
    (void) modulePath;
    return {};
#else
    char* const resolved = realpath( modulePath.c_str(), nullptr );
    if( !resolved )
        return {};

    std::string result = resolved;
    free( resolved );
    return result;
#endif
}

/**********************************************************************************************/
std::string find_tessdata_path( const std::string& modulePath )
{
    const std::string directPath = tessdata_path_next_to( modulePath );
    if( directory_exists( directPath ) )
        return directPath;

    const std::string realPath = real_module_path( modulePath );
    if( !realPath.empty() && realPath != modulePath )
    {
        const std::string realTessdataPath = tessdata_path_next_to( realPath );
        if( directory_exists( realTessdataPath ) )
            return realTessdataPath;
    }

    return directPath;
}

/**********************************************************************************************/
std::string module_tessdata_path()
{
#if defined( _WIN32 )
    HMODULE module = nullptr;
    const auto address = reinterpret_cast<LPCWSTR>( &kModuleAddressAnchor );
    const DWORD flags = GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;
    if( !GetModuleHandleExW( flags, address, &module ) )
        return {};

    std::wstring modulePathW;
    DWORD size = MAX_PATH;
    while( true )
    {
        modulePathW.resize( size );
        const DWORD length = GetModuleFileNameW( module, modulePathW.data(), size );
        if( length == 0 )
            return {};

        if( length < size - 1 )
        {
            modulePathW.resize( length );
            break;
        }

        size *= 2;
    }

    const int pathSize = WideCharToMultiByte(
        CP_UTF8, 0, modulePathW.c_str(), -1, nullptr, 0, nullptr, nullptr );
    if( pathSize <= 1 )
        return {};

    std::string modulePath( static_cast<size_t>( pathSize ), '\0' );
    WideCharToMultiByte(
        CP_UTF8, 0, modulePathW.c_str(), -1, modulePath.data(), pathSize, nullptr, nullptr );
    modulePath.pop_back();
#else
    Dl_info info = {};
    if( dladdr( &kModuleAddressAnchor, &info ) == 0 || !info.dli_fname )
        return {};

    std::string modulePath = info.dli_fname;
#endif

    return find_tessdata_path( modulePath );
}

/**********************************************************************************************/
enum class OcrOutputFormat
{
    Text,
    Html,
    Tsv
};

/**********************************************************************************************/
valentina_value_t run_ocr( valentina_context_t* ctx, valentina_value_t* args, OcrOutputFormat format, const char* functionName )
{
    const int32_t argc = valentina_arg_count( ctx );

    // Argument 0 (required): image data in PNG, JPEG or BMP format
    if( valentina_is_null( args[ 0 ] ) )
    {
        const std::string error = std::string( functionName ) + ": image data is NULL.";
        valentina_throw_error( ctx, error.c_str(), -1 );
        return nullptr;
    }

    int64_t     len  = 0;
    const void* data = valentina_get_binary( args[ 0 ], &len );

    if( !data || len <= 0 )
    {
        const std::string error = std::string( functionName ) +
            ": image data must be a non-empty binary value.";
        valentina_throw_error( ctx, error.c_str(), -1 );
        return nullptr;
    }

    // Argument 1 (optional): language string, default "eng"
    std::string lang = "eng";

    if( argc >= 2 && !valentina_is_null( args[ 1 ] ) )
    {
        const char* value = valentina_get_text( ctx, args[ 1 ], nullptr );
        if( value && value[ 0 ] != '\0' )
            lang = value;
    }

    // Argument 2 (optional): page segmentation mode, default PSM_AUTO (3)
    tesseract::PageSegMode psm = tesseract::PSM_AUTO;
    if( argc >= 3 && !valentina_is_null( args[ 2 ] ) )
    {
        const int64_t value = valentina_get_int64( args[ 2 ] );
        if( value < 0 || value > 13 )
        {
            const std::string error = std::string( functionName ) +
                ": page segmentation mode must be in range 0..13.";
            valentina_throw_error( ctx, error.c_str(), -1 );
            return nullptr;
        }

        psm = static_cast<tesseract::PageSegMode>( value );
    }

    // Argument 3 (optional): OCR engine mode, default OEM_DEFAULT (3)
    tesseract::OcrEngineMode oem = tesseract::OEM_DEFAULT;
    if( argc >= 4 && !valentina_is_null( args[ 3 ] ) )
    {
        const int64_t value = valentina_get_int64( args[ 3 ] );
        if( value < 0 || value > 3 )
        {
            const std::string error = std::string( functionName ) +
                ": OCR engine mode must be in range 0..3.";
            valentina_throw_error( ctx, error.c_str(), -1 );
            return nullptr;
        }

        oem = static_cast<tesseract::OcrEngineMode>( value );
    }

    // Load image from memory via Leptonica:
    PIX* pix = pixReadMem( reinterpret_cast<const l_uint8*>( data ), static_cast<size_t>( len ) );
    if( !pix )
    {
        const std::string error = std::string( functionName ) + ": failed to decode image data.";
        valentina_throw_error( ctx, error.c_str(), -1 );
        return nullptr;
    }

    // Initialize and run Tesseract:
    tesseract::TessBaseAPI api;
    const std::string tessdataPath = module_tessdata_path();
    if( api.Init( tessdataPath.empty() ? nullptr : tessdataPath.c_str(), lang.c_str(), oem ) != 0 )
    {
        pixDestroy( &pix );

        const std::string error = std::string( functionName ) +
            ": failed to initialize Tesseract for lang '" + lang +
            "' using tessdata path '" + tessdataPath + "'.";
        valentina_throw_error( ctx, error.c_str(), -1 );
        
        return nullptr;
    }

    api.SetPageSegMode( psm );
    api.SetImage( pix );

    char* text = nullptr;
    switch( format )
    {
        case OcrOutputFormat::Text:
            text = api.GetUTF8Text();
            break;

        case OcrOutputFormat::Html:
            text = api.GetHOCRText( 0 );
            break;

        case OcrOutputFormat::Tsv:
            text = api.GetTSVText( 0 );
            break;
    }

    pixDestroy( &pix );
    api.End();

    if( !text )
    {
        const std::string error = std::string( functionName ) + ": Tesseract returned no output.";
        valentina_throw_error( ctx, error.c_str(), -1 );
        return nullptr;
    }

    return valentina_create_text( ctx, text, static_cast<int64_t>( strlen( text ) ), delete_string );
}

/**********************************************************************************************/
valentina_value_t ocrTextFunc( valentina_context_t* ctx, valentina_value_t* args )
{
    return run_ocr( ctx, args, OcrOutputFormat::Text, "ocr_text" );
}

/**********************************************************************************************/
valentina_value_t ocrHtmlFunc( valentina_context_t* ctx, valentina_value_t* args )
{
    return run_ocr( ctx, args, OcrOutputFormat::Html, "ocr_html" );
}

/**********************************************************************************************/
valentina_value_t ocrTsvFunc( valentina_context_t* ctx, valentina_value_t* args )
{
    return run_ocr( ctx, args, OcrOutputFormat::Tsv, "ocr_tsv" );
}

/**********************************************************************************************/
void register_ocr_function(
    valentina_context_t* ctx,
    const char* name,
    const char* desc,
    valentina_value_t (*callback)( valentina_context_t* ctx, valentina_value_t* args ) )
{
    valentina_function_t func = {};
    func.name       = name;
    func.resultType = VALENTINA_TYPE_TEXT;
    func.minArg     = 1;
    func.maxArg     = 4;
    func.argDesc    = "data, lang, psm, oem";
    func.desc       = desc;
    func.flags      = VALENTINA_FUNCTION_FLAG_IMMUTABLE;
    func.xFunc      = callback;

    valentina_create_function( ctx, func );
}

/**********************************************************************************************/
} // namespace


/**********************************************************************************************/
VALENTINA_EXTENSION_DEFINE_ENTRY
void valentina_extension_init( valentina_context_t* ctx );
void valentina_extension_init( valentina_context_t* ctx )
{
    const char* textDesc =
        "Recognizes text from PNG, JPEG, or BMP image bytes using Tesseract OCR "
        "and returns plain UTF-8 text. "
        "lang is an optional Tesseract language code, default 'eng'; bundled languages include "
        "'ukr', 'eng', 'pol', 'deu', 'fra', 'ita', 'spa', 'por', 'chi_sim', 'chi_tra', 'jpn', "
        "and 'kor'. Multiple languages can be combined with '+', for example 'eng+ukr'. "
        "psm is an optional page segmentation mode in range 0..13, default 3 (PSM_AUTO). "
        "oem is an optional OCR engine mode in range 0..3, default 3 (OEM_DEFAULT).";

    const char* htmlDesc =
        "Recognizes text from PNG, JPEG, or BMP image bytes using Tesseract OCR "
        "and returns hOCR markup, an HTML-like format with recognized text and layout coordinates. "
        "lang is an optional Tesseract language code, default 'eng'; multiple languages can be "
        "combined with '+', for example 'eng+ukr'. "
        "psm is an optional page segmentation mode in range 0..13, default 3 (PSM_AUTO). "
        "oem is an optional OCR engine mode in range 0..3, default 3 (OEM_DEFAULT).";

    const char* tsvDesc =
        "Recognizes text from PNG, JPEG, or BMP image bytes using Tesseract OCR "
        "and returns TSV output with OCR hierarchy, bounding boxes, confidence, and text. "
        "lang is an optional Tesseract language code, default 'eng'; multiple languages can be "
        "combined with '+', for example 'eng+ukr'. "
        "psm is an optional page segmentation mode in range 0..13, default 3 (PSM_AUTO). "
        "oem is an optional OCR engine mode in range 0..3, default 3 (OEM_DEFAULT).";

    register_ocr_function( ctx, "ocr_text", textDesc, ocrTextFunc );
    register_ocr_function( ctx, "ocr_html", htmlDesc, ocrHtmlFunc );
    register_ocr_function( ctx, "ocr_tsv",  tsvDesc,  ocrTsvFunc );
}
