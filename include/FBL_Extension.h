/**********************************************************************************************/
/* FBL_Extension.h 	                                                      					  */
/*                                                                       					  */
/* Copyright (c) 1998-2026 Paradigma Software                                                 */
/* SPDX-License-Identifier: MIT                                                               */
/*                                                                                            */
/* Permission is hereby granted, free of charge, to any person obtaining a copy of this         */
/* software and associated documentation files (the "Software"), to deal in the Software        */
/* without restriction, including without limitation the rights to use, copy, modify, merge,    */
/* publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons   */
/* to whom the Software is furnished to do so, subject to the following conditions:             */
/*                                                                                            */
/* The above copyright notice and this permission notice shall be included in all copies or     */
/* substantial portions of the Software.                                                       */
/*                                                                                            */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          */
/* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR     */
/* PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE   */
/* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR         */
/* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER       */
/* DEALINGS IN THE SOFTWARE.                                                                  */
/**********************************************************************************************/
/**
 * @file FBL_Extension.h
 * @brief Valentina Database Engine Extension API
 *
 * This file defines the comprehensive API for creating and managing extensions
 * for the Valentina database engine. Extensions allow developers to extend
 * the database functionality with custom SQL functions, procedures, and
 * external data source integration.
 *
 * @note This file is part of the Valentina C++ Application Development Kit (ADK),
 *        but notice that it is independent from any VSDK header.
 *
 * @note This file the only one you need to include in your extension code.
 *        You do NOT need add to extension project FBL_Extension*.cpp files.
 *
 * @note Check Valentina Wiki for additional documentation on extension development:
 *          https://valentina-db.com/dokuwiki/doku.php?id=valentina:products:vcomponents:vkernel:extensions:extensions
 *
 *
 * @section supported_languages Supported Extension Languages
 *
 * Extensions can be implemented in multiple programming languages:
 * - **C/C++**: Compiled native extensions for maximum performance
 * - **Python**: Script-based extensions for rapid development
 * - **JavaScript**: Dynamic scripting with modern language features
 *
 *
 *
 * @section loading_mechanisms Extension Loading
 *
 * Extensions are loaded dynamically by the Valentina database engine through two methods:
 *
 * **1. Automatic Loading:**
 *   - Extensions placed in the `vcomponents/vextensions` folder are automatically discovered.
 *   - Sub-folders are NOT scanned, so you can use own sub-folders for manual loading.
 *   - The engine loads these extensions at startup.
 *   - The engine looks for a specific entry point function named `valentina_extension_init`.
 *
 *   @see valentina_extensions_location() and valentina_user_extensions_location()
 *
 *
 * **2. Manual Loading via SQL:**
 * ```sql
 * SELECT load_extension( 'absOrRelativePath/to/extension' [, 'entryFuncName'] );
 * ```
 * - Supports both absolute and relative paths.
 * - Relative paths are resolved using default extension directories.
 * - Optional entry point parameter for custom initialization functions.
 *
 *
 *
 * @section api_overview API Overview
 *
 * The extension API provides:
 * - **Function Registration**: Custom scalar and aggregate SQL functions
 * - **Procedure Registration**: SQL procedures with cursor return capabilities
 * - **External Data Sources**: Integration with external data through cursor interface
 * - **Value Management**: Creation and manipulation of Valentina data types
 * - **Memory Management**: Context-aware memory allocation and cleanup
 * - **Error Handling**: Comprehensive error reporting and exception management
 *
 *
 *
 * @section examples Usage Examples
 *
 * For practical examples and tutorials, see:
 * - VSDK/Examples/Extensions directory
 * - Valentina Wiki documentation for extension development
 *
 *
 *
 * @note This header defines C-compatible API functions that can be used from both C and C++ code.
 * @note All API functions are thread-safe unless explicitly documented otherwise.
 */

#pragma once

// STD:
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


/**********************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


/**********************************************************************************************/
/**
    Version of the valentina_api table layout.
*/
#define VALENTINA_API_VERSION 1


/**********************************************************************************************/
// Macros
//

/**
    Visibility of an extension exported functions (defined in an extension).
*/
#ifdef __cplusplus // i.e. for extension written in C++  code
#   ifdef __GNUC__
#       define VALENTINA_EXTENSION_API extern "C" __attribute__((visibility("default")))
#   else // __GNUC__
#       define VALENTINA_EXTENSION_API extern "C" __declspec(dllexport)
#   endif // __GNUC__
#else // __cplusplus, i.e. for extension written in C code
#   ifdef __GNUC__
#       define VALENTINA_EXTENSION_API __attribute__((visibility("default")))
#   else // __GNUC__
#       define VALENTINA_EXTENSION_API __declspec(dllexport)
#   endif // __GNUC__
#endif // __cplusplus


/**
 * Defines the extension entry-point boilerplate.
 *
 * Use this macro in exactly one source file of an extension, immediately before
 * the `valentina_extension_init()` function definition. It defines the shared
 * API table pointer, exports the helper used by the engine to bind that table,
 * and applies the export attributes to the entry point that follows.
 */
#define VALENTINA_EXTENSION_DEFINE_ENTRY \
    const valentina_api* g_valentina_api_ptr = NULL; \
    \
    VALENTINA_EXTENSION_API void valentina_extension_bind_api( const valentina_api* api ); \
    \
    VALENTINA_EXTENSION_API void valentina_extension_bind_api( const valentina_api* api ) \
    { \
        g_valentina_api_ptr = api; \
    } \
    VALENTINA_EXTENSION_API



/**********************************************************************************************/
/**********************************************************************************************/
//
// Integration Types
// Types to communicate with Valentina DB engine.
//
/**********************************************************************************************/
/**********************************************************************************************/


// Forward declarations for C++ compilation
#ifdef __cplusplus
namespace fbl
{
    struct I_Value;

    namespace vsql
    {
        struct I_Cursor;
    }
}
#endif // __cplusplus


/**
 * @brief Handle to a Valentina database cursor.
 *
 * This type represents a cursor that can iterate over rows of data in the Valentina
 * database system. Cursors are used to access query results, external data sources,
 * and procedure return values. They provide a consistent interface for row-by-row
 * data access regardless of the underlying data source.
 *
 * In C++ compilations, this maps to the native Valentina cursor interface for optimal
 * performance. In C compilations, it's treated as an opaque handle.
 *
 * @note Cursors are managed by the Valentina engine and should not be manually freed.
 * @note Use valentina_destroy_cursor() for explicit cleanup when needed.
 */
#ifdef __cplusplus
    typedef fbl::vsql::I_Cursor     valentina_cursor_t;
#else // __cplusplus
    typedef void                    valentina_cursor_t;
#endif // __cplusplus


/**
 * @brief Handle to a Valentina value object.
 *
 * This type represents a value in the Valentina type system, capable of holding
 * any supported data type including scalars, arrays, and complex types. Values
 * are immutable once created and are automatically managed by the context system.
 *
 * In C++ compilations, this maps directly to the native Valentina value pointer
 * for seamless integration. In C compilations, it's treated as an opaque handle.
 *
 * @note Values are context-managed and automatically freed when the context is destroyed.
 * @note Use valentina_destroy_value() for explicit cleanup when needed.
 */
#ifdef __cplusplus
    using valentina_value_t = fbl::I_Value*;
#else // __cplusplus
    typedef void* valentina_value_t;
#endif // __cplusplus


/**
 * @brief Compact date representation (32-bit).
 *
 * This structure represents a date using bit fields for space-efficient storage.
 * The packed format allows dates to be stored in exactly 32 bits while maintaining
 * reasonable range and precision for most applications.
 *
 * @note Valid day range  : 1-31 (5 bits)
 * @note Valid month range: 1-12 (4 bits)
 * @note Valid year range : approximately -4,194,304 to +4,194,303 (23 bits signed)
 * @note No validation is performed on field values during assignment.
 */
typedef struct                          // size 32 bits
{
    unsigned day	: 5;    /// Day of month (1-31)
    unsigned month	: 4;    /// Month (1-12)
    signed   year	: 23;   /// Year (signed, wide range)
}
valentina_date_t;


/**
 * @brief Compact date and time representation (64-bit).
 *
 * This structure represents a complete date and time using bit fields for
 * space-efficient storage. The packed format stores both date and time
 * components in exactly 64 bits with millisecond precision.
 *
 * @note Valid milliseconds range: 0-999 (10 bits, though 15 bits allocated)
 * @note Valid seconds range: 0-59 (6 bits)
 * @note Valid minutes range: 0-59 (6 bits)
 * @note Valid hours range  : 0-23 (5 bits)
 * @note Valid day range    : 1-31 (5 bits)
 * @note Valid month range  : 1-12 (4 bits)
 * @note Valid year range   : approximately -4,194,304 to +4,194,303 (23 bits signed)
 */
typedef struct                          // size 64 bits
{
    unsigned milliseconds	: 15;   /// Milliseconds (0-999, extra bits for future use)
    unsigned seconds        : 6;    /// Seconds (0-59)
    unsigned minutes        : 6;    /// Minutes (0-59)
    unsigned hours          : 5;    /// Hours (0-23)
    unsigned day            : 5;    /// Day of month (1-31)
    unsigned month          : 4;    /// Month (1-12)
    signed   year           : 23;   /// Year (signed, wide range)
}
valentina_datetime_t;


/**
 * @brief Compact time representation (32-bit).
 *
 * This structure represents time of day using bit fields for space-efficient
 * storage. The packed format stores time components in exactly 32 bits with
 * millisecond precision, suitable for time-only values.
 *
 * @note Valid milliseconds range: 0-999 (10 bits, though 15 bits allocated)
 * @note Valid seconds range: 0-59 (6 bits)
 * @note Valid minutes range: 0-59 (6 bits)
 * @note Valid hours range: 0-23 (5 bits)
 * @note No date component is stored in this structure.
 */
typedef struct                          // size 32 bits
{
    unsigned milliseconds	: 15;   /// Milliseconds (0-999, extra bits for future use)
    unsigned seconds        : 6;    /// Seconds (0-59)
    unsigned minutes        : 6;    /// Minutes (0-59)
    unsigned hours          : 5;    /// Hours (0-23)
}
valentina_time_t;



/**********************************************************************************************/
/**********************************************************************************************/
//
// EXTENSION
//
/**********************************************************************************************/
/**********************************************************************************************/


/**********************************************************************************************/
// Constants, which you can use in your extension code.
//

/** Result codes that indicate the success or failure of operations. */
typedef enum
{
// Extension to VDB:

    VALENTINA_DONE                              = -1,   /// Extension operation has completed, no more data.

// VDB to Extension:

    VALENTINA_OK                                = 0,    /// Operation completed successfully.

// Registration errors:

    VALENTINA_ERR_ARG_NULL                      = 1,    /// Some argument is NULL, but should not.
    VALENTINA_ERR_ARG_NAME_INVALID              = 2,    /// Improper name ( e.g. empty, ...).
    VALENTINA_ERR_ARG_NAME_EXISTS               = 3,    /// A function/procedure with such name already exists.

    VALENTINA_ERR_ARG_DATA_INCOMPLETE           = 4,    /// Provided data doesn't have all required members.
    VALENTINA_ERR_ARG_DATA_INVALID              = 5,    /// Invalid data, e.g. wrong JSON given.
    VALENTINA_ERR_ARG_FUNC_RES_TYPE_INVALID     = 6,    /// Function has an invalid result type.
    VALENTINA_ERR_ARG_NOT_FUNCTION              = 7,    /// The script must return a function.
    VALENTINA_ERR_ARG_UNKNOWN_SCRIPT_LANGUAGE   = 8,    /// Unknown name of a script language

// Runtime:

    VALENTINA_ERR_NO_MEMORY                     = 9,    /// Unable to allocate memory.
    VALENTINA_ERR_SQL                           = 10,   /// Error occurred during SQL query execution.

// Other:

    VALENTINA_ERR_INTERNAL                      = 11,   /// Internal unexpected error.
    VALENTINA_ERR_UNKNOWN                       = 255   /// Unknown error occurred.
}
ValentinaResultCode;


/** Functions flags. */
typedef enum
{
    VALENTINA_FUNCTION_FLAG_IMMUTABLE = 1,  /// The function cannot modify the database and always returns the same result
                                            /// when given the same argument values.

    VALENTINA_FUNCTION_FLAG_STABLE    = 2   /// The function cannot modify the database, and within a single table scan it will
                                            /// consistently return the same result for the same argument values,
                                            /// but its result could change across SQL statements.
}
ValentinaFunctionFlag;


/** Value types. */
typedef enum
{
    VALENTINA_TYPE_ARRAY    = 1,    /// Array of values.
    VALENTINA_TYPE_BOOL     = 2,    /// Boolean value.
    VALENTINA_TYPE_BINARY   = 3,    /// Binary value.
    VALENTINA_TYPE_DATE     = 4,    /// Date value.
    VALENTINA_TYPE_DATETIME = 5,    /// Date with time value.
    VALENTINA_TYPE_DOUBLE   = 6,    /// Double-precision floating-point value.
    VALENTINA_TYPE_INT64    = 7,    /// 64-bit integer value.
    VALENTINA_TYPE_MONEY    = 8,    /// Currency value.
    VALENTINA_TYPE_NULL     = 9,    /// Null value.
    VALENTINA_TYPE_TEXT     = 10,   /// Text string value.
    VALENTINA_TYPE_TIME     = 11,   /// Time value.
    VALENTINA_TYPE_UINT64   = 12,   /// 64-bit unsigned integer value.
    VALENTINA_TYPE_VARIANT  = 13    /// Variant type that can hold any value.
}
ValentinaValueType;



/**********************************************************************************************/
// Extension Types
//
// valentina_context_t - is used widely throughout the extension API.
// Other types you can use depending on your extension needs.
//


/**
 * @brief Execution context for extension functions and procedures.
 *
 * This opaque structure represents the execution context for a function or procedure
 * call within the Valentina database engine. It provides access to function arguments,
 * error handling, memory management, and other contextual information needed during
 * extension execution.
 *
 * The context is automatically created by the Valentina engine when calling extension
 * functions and procedures, and is passed as the first parameter to all extension
 * callback functions. It manages the lifetime of allocated resources and provides
 * thread-safe access to engine services.
 *
 * @note This is an opaque type - its internal structure is not exposed to extensions.
 * @note The context is only valid during the execution of the extension function/procedure.
 * @note All memory allocated through the context is automatically freed when the context is destroyed.
 */
typedef struct valentina_context_t valentina_context_t;


/**
 * @brief Definition structure for a custom SQL function in Valentina database engine.
 *
 * This structure describes a user-defined SQL function that can be registered with
 * the Valentina database engine. It supports both scalar functions (that return a single
 * value for each row) and aggregate functions (that accumulate values across multiple rows).
 *
 * For scalar functions, only the xFunc callback needs to be implemented.
 * For aggregate functions, all - xInit, xStep, and xFinish callbacks must be implemented.
 *
 * @note char* strings should be UTF-8 encoded.
 * @note Function names must be unique within their scope (global or database-specific).
 * @note All string pointers should point to null-terminated strings with lifetime
 *       extending beyond the function registration call.
 * @note For aggregate functions, xFunc should be set to NULL.
 *
 * @see valentina_create_function()
 * @see ValentinaFunctionFlag enum for function behaviour flags.
 */
typedef struct
{
    const char*         name;              /// The unique name of the function as it will appear in SQL
    const char*         category;          /// The category/group this function belongs to (optional)
    ValentinaValueType  resultType;        /// The data type that this function returns
    int32_t             minArg;            /// The minimum number of arguments the function accepts (0 to 255)
    int32_t             maxArg;            /// The maximum number of arguments the function accepts (0 to 255)
    const char*         argDesc;           /// Description of the function arguments format and types
    const char*         desc;              /// Human-readable description of what the function does
    uint64_t            flags;             /// Function behavior flags (see VALENTINA_FUNCTION_FLAG_*)
    void*               userData;          /// Custom user data accessible from function context


// Scalar functions:

                        /**
                         * @brief Scalar function implementation.
                         *
                         * This function pointer should be implemented for scalar functions that return
                         * a single value for each row.
                         *
                         * For aggregate functions it is usually set to NULL.
                         *
                         * @param ctx The Valentina context for the function call
                         * @param args Array of input arguments passed to the function
                         * @return The computed result value
                         */
    valentina_value_t   (*xFunc)( valentina_context_t* ctx, valentina_value_t* args );


// Aggregating functions:

                        /**
                         * @brief Aggregate function init implementation.
                         *
                         * This function is called at the start of aggregate group
                         * to setup the initial state of the accumulated state.
                         *
                         * @param ctx The Valentina context for the function call
                         */
    void                (*xInit)( valentina_context_t* ctx );

                        /**
                         * @brief Aggregate function step implementation.
                         *
                         * This function is called once for each row when processing aggregate functions.
                         * It should accumulate the input values into the aggregate state.
                         *
                         * @param ctx The Valentina context for the function call
                         * @param args Array of input arguments for the current row
                         *
                         * @see valentina_aggregate_data()
                         */
    void                (*xStep)( valentina_context_t* ctx, valentina_value_t* args );

                        /**
                         * @brief Aggregate function finalization implementation.
                         *
                         * This function is called once at the end of aggregate group
                         * to compute and return the final result from the accumulated state.
                         *
                         * @param ctx The Valentina context for the function call
                         * @return The final aggregate result value
                         */
    valentina_value_t   (*xFinish)( valentina_context_t* ctx );
}
valentina_function_t;


/**
 * @brief Definition structure for a custom SQL procedure in Valentina database engine.
 *
 * This structure describes a user-defined SQL procedure that can be registered with
 * the Valentina database engine. Unlike functions, procedures can return cursors
 * (result sets) and are typically used for more complex operations that involve
 * data retrieval and manipulation.
 *
 * Procedures are invoked in SQL using CALL statements and can return tabular data
 * through cursors, making them suitable for implementing custom data sources,
 * computed tables, or complex business logic.
 *
 * @note char* strings should be UTF-8 encoded.
 * @note Procedure names must be unique within their scope (global or database-specific).
 * @note All string pointers should point to null-terminated strings with lifetime
 *       extending beyond the procedure registration call.
 * @note The xProc function must be implemented and should return a valid cursor
 *       or NULL on error.
 *
 * @see valentina_create_procedure()
 * @see ValentinaFunctionFlag enum for procedure behavior flags.
 */
typedef struct
{
    const char*         name;              /// The unique name of the procedure as it will appear in SQL CALL statements
    int32_t             minArg;            /// The minimum number of arguments the procedure accepts (0 to 255)
    int32_t             maxArg;            /// The maximum number of arguments the procedure accepts (0 to 255)
    const char*         argDesc;           /// Human-readable description of expected argument types and format
    const char*         desc;              /// Detailed description of the procedure's purpose and behavior
    uint64_t            flags;             /// Procedure behavior flags (see VALENTINA_FUNCTION_FLAG_* constants)
    void*               userData;          /// Custom user data that will be accessible from the procedure context

    /// TODO(I.N.): specify columns comparator for sorted data to create indexes into Valentina Engine

                        /**
                         * @brief Procedure implementation function.
                         *
                         * This function pointer must be implemented to define the procedure's behavior.
                         * It should execute the procedure logic and return a cursor containing the results.
                         *
                         * @param ctx The Valentina context for the procedure call
                         * @param args Array of input arguments passed to the procedure

                         * @return You can return cursor with results,
                                    or NULL if your procedure does not return any data.
                                    Errors should be thrown as exceptions using valentina_throw_error.
                         */
    valentina_cursor_t* (*xProc)( valentina_context_t* ctx, valentina_value_t* args );
}
valentina_procedure_t;


/**
 * @brief Definition structure for a script-based SQL procedure in Valentina database engine.
 *
 * This structure describes a user-defined SQL procedure that is implemented using a
 * scripting language rather than compiled C/C++ code. Script procedures provide a more
 * flexible way to implement business logic without requiring compilation, making them
 * suitable for rapid prototyping and dynamic functionality.
 *
 * The script code is executed by the Valentina engine's embedded scripting runtime,
 * which supports multiple languages like Python and JavaScript. The procedure can
 * return cursors just like native procedures.
 *
 * @note char* strings should be UTF-8 encoded.
 * @note Procedure names must be unique within their scope (global or database-specific).
 * @note All string pointers should point to null-terminated strings with lifetime
 *       extending beyond the procedure registration call.
 * @note The scripting language must be supported by the Valentina engine runtime.
 * @note Script code should be syntactically correct for the specified language.
 *
 * @see valentina_create_procedure_from_script()
 * @see ValentinaFunctionFlag enum for script behaviour flags.
 */
typedef struct
{
    const char*         name;              /// The unique name of the script procedure as it will appear in SQL CALL statements
    const char*         lang;              /// The scripting language identifier (e.g., "Python", "JavaScript")
    const char*         code;              /// The complete source code that implements the procedure logic
    int32_t             minArg;            /// The minimum number of arguments the procedure accepts (0 to 255)
    int32_t             maxArg;            /// The maximum number of arguments the procedure accepts (0 to 255)
    const char*         argDesc;           /// Human-readable description of expected argument types and format
    const char*         desc;              /// Detailed description of the procedure's purpose and behaviour
    uint64_t            flags;             /// Procedure behavior flags (see VALENTINA_FUNCTION_FLAG_* constants)
}
valentina_script_procedure_t;


/**
 * @brief Interface to an (!) EXTERNAL DATA (!) cursor for Valentina database engine,
 *        allowing integration with external data sources.
 *        For example, this can be used to build cursor OVER file systems, REST APIs,
 *        or any other data source that can be represented as a table.
 *
 * This structure provides a complete interface for implementing external data sources
 * that can be integrated with Valentina database engine. It defines metadata about
 * the data structure (columns, types) and function pointers for all necessary
 * operations including navigation, modification, and transaction management.
 *
 * @note Required members: columnCount, flags, xColumnValue,
 *                     AND either {xMove/xRowCount} or {xNext}.
 *       All other function pointers may be set to NULL if the corresponding functionality
 *       is not needed or supported by the external data source.
 *
 * @note All implemented function pointers must be properly initialized before using the cursor.
 * @note The cursor implementation should handle concurrent access appropriately.
 */
typedef struct
{
// Metadata about the external data source

    void*               cursor;
    uint64_t            flags;

    int32_t             columnCount;
    const char**        columnNames;
    ValentinaValueType* columnTypes;


// function pointers:

        // cursor management:

                        /**
                         * @brief Closes the cursor and releases any associated resources.
                         *
                         * This function should be implemented to properly clean up the cursor
                         * and release any resources it holds. It is called when the cursor is no longer needed.
                         *
                         * @param ctx The Valentina context for the operation
                         * @param cursor The cursor to close
                         */
    void                (*xClose)( valentina_context_t* ctx, void* cursor );

        // column access:

                        /**
                         * @brief Retrieves the value of a specific column for the current row.
                         *
                         * This function must be implemented to return the value of the specified column
                         * at the current cursor position. It's one of the core functions that enables
                         * data access from the external data source.
                         *
                         * @param ctx The Valentina context for the operation
                         * @param cursor The cursor instance
                         * @param index The zero-based column index (0 to columnCount-1)
                         *
                         * @return The value at the specified column,
                         *         or NULL if the column is null or index is invalid
                         *
                         * @note This function is required and must be implemented.
                         * @note The index must be within the range [0, columnCount-1].
                         * @note The returned value should match the type specified in columnTypes[index].
                         */
    valentina_value_t   (*xColumnValue)( valentina_context_t* ctx, void* cursor, int32_t index );

        // navigation:

                        /**
                         * @brief Moves the cursor to a specific row position.
                         *
                         * This function positions the cursor at the specified row number, enabling
                         * random access to data. Implementation of this function is optional but
                         * recommended for performance when dealing with large datasets.
                         *
                         * @param ctx The Valentina context for the operation
                         * @param cursor The cursor instance
                         * @param row The zero-based row number to move to
                         *
                         * @return VALENTINA_OK on success,
                         *          or an appropriate error code
                         *
                         * @note Either xMove/xRowCount or xNext must be implemented.
                         * @note Row numbers are zero-based (0 to rowCount-1).
                         * @note After successful move, xColumnValue should return values for the new row.
                         */
    ValentinaResultCode (*xMove)( valentina_context_t* ctx, void* cursor, int64_t row );

                        /**
                         * @brief Advances the cursor to the next row.
                         *
                         * This function moves the cursor forward by one row, implementing sequential
                         * access to the data. This is the primary navigation method for cursors
                         * that don't support random access.
                         *
                         * @param ctx The Valentina context for the operation
                         * @param cursor The cursor instance
                         *
                         * @return VALENTINA_OK if moved successfully,
                         *         VALENTINA_DONE if no more rows,
                         *         or an appropriate error code
                         *
                         * @note Either xMove/xRowCount or xNext must be implemented.
                         * @note Return VALENTINA_DONE when there are no more rows to read.
                         * @note The cursor should initially be positioned before the first row.
                         */
    ValentinaResultCode (*xNext)( valentina_context_t* ctx, void* cursor );

                        /**
                         * @brief Returns the total number of rows in the cursor.
                         *
                         * This function provides the total count of rows available in the external
                         * data source. It's used for optimization and progress reporting.
                         *
                         * @param ctx The Valentina context for the operation
                         * @param cursor The cursor instance
                         *
                         * @return The total number of rows.
                         *
                         * @note Either xMove/xRowCount or xNext must be implemented.
                         * @note This count should remain consistent during cursor lifetime.
                         */
    int64_t             (*xRowCount)( valentina_context_t* ctx, void* cursor );

        // record modification:

                        /**
                         * @brief Deletes the current row from the external data source.
                         *
                         * This function removes the row at the current cursor position from the
                         * underlying data source. Implementation is optional and only needed
                         * if the data source supports row deletion.
                         *
                         * @param ctx The Valentina context for the operation
                         * @param cursor The cursor instance
                         *
                         * @return VALENTINA_OK on successful deletion,
                         *          or an appropriate error code
                         *
                         * @note This function is optional; set to NULL if deletion is not supported.
                         * @note After deletion, cursor position behavior is implementation-specific.
                         * @note May require transaction support depending on the data source.
                         */
    ValentinaResultCode (*xDelete)( valentina_context_t* ctx, void* cursor );

                        /**
                         * @brief Inserts a new row into the external data source.
                         *
                         * This function adds a new row to the data source with the specified column
                         * values. The indexes array specifies which columns are being set.
                         *
                         * @param ctx The Valentina context for the operation
                         * @param cursor The cursor instance
                         * @param count The number of columns being inserted
                         * @param indexes Array of column indexes being set (0-based)
                         * @param values Array of values corresponding to the specified columns
                         * @param outRow Pointer to receive the new row number (optional, can be NULL)
                         *
                         * @return VALENTINA_OK on successful insertion,
                         *         or an appropriate error code
                         *
                         * @note This function is optional; set to NULL if insertion is not supported.
                         * @note The indexes array specifies which columns from columnNames are being set.
                         * @note Values must match the types specified in columnTypes for the corresponding columns.
                         */
    ValentinaResultCode (*xInsert)( valentina_context_t* ctx, void* cursor, int32_t count, int32_t* indexes, valentina_value_t* values, int64_t* outRow );

                        /**
                         * @brief Updates specific columns of the current row.
                         *
                         * This function modifies the values of specified columns in the current row
                         * of the external data source.
                         *
                         * @param ctx The Valentina context for the operation
                         * @param cursor The cursor instance
                         * @param count The number of columns being updated
                         * @param indexes Array of column indexes being updated (0-based)
                         * @param values Array of new values for the specified columns
                         *
                         * @return VALENTINA_OK on successful update,
                         *          or an appropriate error code
                         *
                         * @note This function is optional; set to NULL if updates are not supported.
                         * @note The indexes array specifies which columns are being modified.
                         * @note Values must match the types specified in columnTypes for the corresponding columns.
                         */
    ValentinaResultCode (*xUpdate)( valentina_context_t* ctx, void* cursor, int64_t count, int64_t* indexes, valentina_value_t* values );

                        /**
                         * @brief Synchronizes pending changes to the external data source.
                         *
                         * This function flushes any pending modifications (inserts, updates, deletes)
                         * to the underlying data source, ensuring data consistency.
                         *
                         * @param ctx The Valentina context for the operation
                         * @param cursor The cursor instance
                         *
                         * @return VALENTINA_OK on successful synchronization,
                         *          or an appropriate error code
                         *
                         * @note This function is optional; set to NULL if synchronization is not needed.
                         * @note Called when Valentina needs to ensure all changes are persisted.
                         * @note Implementation depends on the nature of the external data source.
                         */
    ValentinaResultCode (*xSync)( valentina_context_t* ctx, void* cursor );

        // transaction management:

                        /**
                         * @brief Begins a new transaction on the external data source.
                         *
                         * This function starts a transaction context for the cursor, allowing
                         * multiple operations to be grouped together for atomicity.
                         *
                         * @param ctx The Valentina context for the operation
                         * @param cursor The cursor instance
                         *
                         * @return VALENTINA_OK on successful transaction start,
                         *          or an appropriate error code
                         *
                         * @note All three transaction functions (xBegin, xCommit, xRollback) are optional.
                         * @note Set to NULL if the data source doesn't support transactions.
                         * @note If implemented, all three transaction functions should be provided.
                         */
    ValentinaResultCode (*xBegin)( valentina_context_t* ctx, void* cursor );

                        /**
                         * @brief Commits the current transaction on the external data source.
                         *
                         * This function commits all changes made within the current transaction,
                         * making them permanent in the external data source.
                         *
                         * @param ctx The Valentina context for the operation
                         * @param cursor The cursor instance
                         *
                         * @return VALENTINA_OK on successful commit,
                         *          or an appropriate error code
                         *
                         * @note This function is optional; set to NULL if transactions are not supported.
                         * @note Should only be called after a successful xBegin call.
                         * @note After commit, a new transaction must be started for further operations.
                         */
    ValentinaResultCode (*xCommit)( valentina_context_t* ctx, void* cursor );

                        /**
                         * @brief Rolls back the current transaction on the external data source.
                         *
                         * This function cancels all changes made within the current transaction,
                         * reverting the data source to its state before the transaction began.
                         *
                         * @param ctx The Valentina context for the operation
                         * @param cursor The cursor instance
                         *
                         * @return VALENTINA_OK on successful rollback,
                         *          or an appropriate error code
                         *
                         * @note This function is optional; set to NULL if transactions are not supported.
                         * @note Should only be called after a successful xBegin call.
                         * @note After rollback, a new transaction must be started for further operations.
                         */
    ValentinaResultCode (*xRollback)( valentina_context_t* ctx, void* cursor );
}
valentina_external_cursor_t;



/**********************************************************************************************/
// Registration Methods
//
//  Use these methods to register functions and procedures.
//  Usually in the entry-point function of your extension DLL or script.


                                    /**
                                     * Adds an SQL function that can return any type of value
                                     *  except cursors.
                                     *
                                     * If the ctx parameter is NULL, the function will be global
                                     *  and available to all databases.
                                     * Otherwise, the function will be visible only in the database
                                     *  specified by ctx.
                                     *
                                     * @return VALENTINA_OK on success,
                                     *  or an appropriate error code if the function could not be created.
                                     */
static inline ValentinaResultCode  valentina_create_function(
                                        valentina_context_t* ctx,  // set NULL for global function
                                        valentina_function_t func );

                                    /**
                                     * Adds an SQL procedure.
                                     *      It can return cursors, errors or nothing.
                                     *
                                     * If the ctx parameter is NULL, the procedure will be global
                                     *  and available to all databases.
                                     * Otherwise, the procedure will be visible only in the database
                                     *  specified by ctx.
                                     *
                                     * @return VALENTINA_OK on success,
                                     *  or an appropriate error code if the function could not be created.
                                     */
static inline ValentinaResultCode  valentina_create_procedure(
                                        valentina_context_t*  ctx,
                                        valentina_procedure_t proc ); // set NULL for global procedure

                                    /**
                                     * Adds an SQL procedure from a script
                                     *      It can return cursors, errors or nothing.
                                     *
                                     * If the ctx parameter is NULL, the procedure will be global
                                     *  and available to all databases.
                                     * Otherwise, the procedure will be visible only in the database
                                     *  specified by ctx.
                                     *
                                     * @return VALENTINA_OK on success,
                                     *  or an appropriate error code if the function could not be created.
                                     */
static inline ValentinaResultCode  valentina_create_procedure_from_script(
                                        valentina_context_t*         ctx,
                                        valentina_script_procedure_t proc ); // set NULL for global procedure



/**********************************************************************************************/
// Extension Locations API
//

/**
 * @brief Gets the application-specific extensions directory path.
 *
 * Returns the path to the "vcomponents/vextensions" folder that is located near or within
 * the current application. Each application has its own dedicated vcomponents directory
 * containing extensions that are specific to that application instance. This location is
 * typically used for extensions that are bundled with or installed directly for a particular
 * application.
 *
 * @return A null-terminated string containing the absolute path to the application's
 *         vcomponents/vextensions directory. The returned pointer is valid for the lifetime of the
 *         application and should not be freed by the caller. Always returns a valid path.
 *
 * @note Each application maintains its own separate vcomponents folder.
 * @note The path format is platform-specific.
 */
static inline const char*  valentina_extensions_location( void );


/**
 * @brief Gets the system-wide user extensions directory path.
 *
 * Returns the path to the extensions directory located in the user's HOME directory.
 * This location is accessible to all Valentina applications running under the current
 * user account, providing a centralised place for user-installed extensions. This
 * location is particularly useful when there's no possibility to write extensions
 * directly into the application's directory due to permission restrictions.
 *
 * @return A null-terminated string containing the absolute path to the user's
 *         system-wide extensions directory. The returned pointer is valid for the
 *         lifetime of the application and should not be freed by the caller.
 *         Always returns a valid path.
 *
 *      WIN: "C:\Users\<username>\AppData\Roaming\com.paradigmasoft.vextensions"
 *      MAC: "/Library/Application Support/com.paradigmasoft.vextensions"
 *      LIN: "/home/username/.local/lib/com.paradigmasoft.vextensions"
 *
 * @note This directory is shared among all Valentina applications for the current user.
 * @note Extensions in this location are available to all applications.
 * @note Used when application directory is read-only or write-protected.
 */
static inline const char*  valentina_user_extensions_location( void );


/**********************************************************************************************/
// Handling the function/procedure context during a call.
//

                                    /**
                                     * @brief Gets the number of arguments passed to a user-defined function or procedure.
                                     *
                                     * This function returns the count of arguments that were passed to the currently
                                     * executing user-defined function or procedure. It can be used within the
                                     * implementation of xFunc, xInit, xStep, xFinish, or xProc callbacks to determine
                                     * how many arguments were provided by the SQL caller.
                                     *
                                     * This is particularly useful for functions or procedures that accept a variable
                                     * number of arguments (where minArg != maxArg), allowing the implementation
                                     * to handle different argument counts appropriately.
                                     *
                                     * @param ctx The Valentina context for the function or procedure call
                                     * @return The number of arguments passed to the function/procedure, or 0 if ctx is NULL
                                     *
                                     * @note The returned count will always be within the range [minArg, maxArg] as
                                     *       specified in the function/procedure definition.
                                     * @note This function should only be called from within function/procedure implementations.
                                     *
                                     * @see valentina_get_xxx() functions below for accessing argument values.
                                     */
static inline int32_t              valentina_arg_count( valentina_context_t* ctx );


    // Special Data Access:

                                    /**
                                     * @brief Allocates or retrieves persistent storage for aggregate function state.
                                     *
                                     * This function provides a mechanism for aggregate functions to maintain state
                                     * across multiple calls during SQL aggregate processing. On the first call,
                                     * it allocates a buffer of the specified size and initializes it to zero.
                                     * On subsequent calls with the same context, it returns the same buffer,
                                     * allowing aggregate functions to accumulate data across rows.
                                     * For new groups, a new buffer is allocated.
                                     *
                                     * The buffer is automatically managed by the Valentina engine and is freed
                                     * when the aggregate context is destroyed. This is typically used in
                                     * conjunction with the xInit, xStep, and xFinish callbacks of aggregate functions.
                                     *
                                     * @param ctx The Valentina context for the function call
                                     * @param len The size in bytes of the buffer to allocate (only used on first call)
                                     * @return Pointer to the persistent buffer, or NULL on allocation failure or invalid context
                                     *
                                     * @note The buffer is zero-initialized only on the first allocation.
                                     * @note The same buffer is returned for all subsequent calls within the same context.
                                     * @note Memory is automatically freed when the context is destroyed.
                                     * @note This function is primarily intended for use within aggregate function implementations.
                                     * @note Returns NULL if ctx is NULL or len is 0 on the first call.
                                     *
                                     * @see valentina_function_t::xInit
                                     * @see valentina_function_t::xStep
                                     * @see valentina_function_t::xFinish
                                     *
                                     * @example
                                     *    struct myfunc_context_t { data_you_may_need };
                                     *    myfunc_context_t* mc = valentina_aggregate_data( ctx, sizeof(myfunc_context_t) );
                                     */
static inline void*                valentina_aggregate_data(
                                        valentina_context_t* ctx,
                                        int64_t              len );


                                    /**
                                     * @brief Retrieves user-defined data associated with the function or procedure.
                                     *
                                     * This function returns the custom user data pointer that was specified in the
                                     * userData field when the function or procedure was registered with the Valentina
                                     * engine. This allows implementations to access custom state, configuration,
                                     * or any other data needed for the function's operation.
                                     *
                                     * The user data is persistent across all calls to the same function or procedure
                                     * and can be used to store function-specific state, configuration parameters,
                                     * or references to external resources.
                                     *
                                     * @param ctx The Valentina context for the function or procedure call
                                     * @return The user data pointer that was specified during function/procedure registration,
                                     *         or NULL if no user data was provided or ctx is NULL
                                     *
                                     * @note The returned pointer is exactly what was provided during registration.
                                     * @note The lifetime and management of the user data is the responsibility of the extension.
                                     * @note This data is shared across all invocations of the same function/procedure.
                                     */
static inline void*                valentina_user_data( valentina_context_t* ctx );


    // Error handling:

                                    /**
                                     * @brief Retrieves the error message from the most recent operation.
                                     *
                                     * This function returns a human-readable error message describing the last
                                     * error that occurred within the current function or procedure context.
                                     * It can be used to get detailed information about what went wrong during
                                     * the execution of extension code or Valentina API calls.
                                     *
                                     * The error message is automatically managed by the Valentina engine and
                                     * remains valid until the context is destroyed or a new error occurs.
                                     *
                                     * @param ctx The Valentina context for the function or procedure call
                                     * @return A null-terminated string containing the error message, or NULL if
                                     *         no error occurred or ctx is NULL
                                     *
                                     * @note The returned string is owned by the context and should not be freed.
                                     * @note This function is typically called after other API functions return error codes.
                                     */
static inline const char*          valentina_error_message( valentina_context_t* ctx );


                                    /**
                                     * @brief Sets an error condition and terminates function execution.
                                     *
                                     * This function is used to report an error condition from within a user-defined
                                     * function or procedure implementation. It sets the error message in the context
                                     * and signals to the Valentina engine that the function execution should be
                                     * terminated with an error.
                                     *
                                     * After calling this function, the function implementation should immediately
                                     * return, as the error state has been set and normal execution cannot continue.
                                     * The error message will be propagated to the SQL caller.
                                     *
                                     * @param ctx The Valentina context for the function or procedure call
                                     * @param err A null-terminated UTF-8 encoded error message string
                                     * @param len The length of the error message in bytes, or -1 if null-terminated

                                     * @return Always returns NULL, making it suitable for use in return statements:
                                     *         `return valentina_throw_error(ctx, "Error message", -1);`
                                     */
static inline void*                valentina_throw_error(
                                        valentina_context_t* ctx,
                                        const char*          err,
                                        int64_t              len );




/**********************************************************************************************/
// SQL Query Execution API
//

                                    /**
                                     * @brief Executes a SQL query on the current Valentina database.
                                     *
                                     * This function executes a SQL query string against the currently active
                                     * Valentina database. It can be used for any SQL statement
                                     * (INSERT, UPDATE, DELETE, SELECT, etc.) except for SELECT/SHOW statements
                                     * that return recordset results.
                                     *
                                     * The query string should be a valid SQL statement, and it will be executed
                                     * immediately. If the query modifies data (INSERT, UPDATE, DELETE), the number
                                     * of affected rows will be returned in outCount if it is not NULL.
                                     *
                                     * @param ctx The Valentina context for the operation
                                     * @param query The SQL query string to execute
                                     * @param len The length of the query string in bytes, or -1 if null-terminated
                                     * @param outCount Pointer to receive the number of affected rows (optional, can be NULL)
                                     * @return VALENTINA_OK on success, or an appropriate error code
                                     *
                                     * @note For SELECT queries that return results, use valentina_sql_select instead.
                                     * @note The outCount parameter receives the number of rows affected by the operation.
                                     */
static inline ValentinaResultCode  valentina_sql_execute(
                                            valentina_context_t* ctx,
                                            const char*          query,
                                            int64_t              len,
                                            int64_t*             outCount );

                                    /**
                                     * @brief Executes a parameterized SQL query with bound values
                                     *          on the current Valentina database.
                                     *
                                     * This function executes a SQL query that contains parameter placeholders (typically '?')
                                     * with the provided bind values substituted for the placeholders. This is the secure
                                     * way to execute SQL queries with dynamic values, preventing SQL injection attacks
                                     * and ensuring proper type handling.
                                     *
                                     * The query can be any valid SQL statement (INSERT, UPDATE, DELETE, SELECT, etc.)
                                     * except SELECT statements. For SELECT queries with parameters, use valentina_sql_select_bind.
                                     *
                                     * @param ctx The Valentina context for the operation
                                     * @param query The SQL query string with parameter placeholders (typically '?')
                                     * @param len The length of the query string in bytes, or -1 if null-terminated
                                     * @param binds Array of values to bind to the query parameters
                                     * @param bindsCount The number of bind values in the binds array
                                     * @param outCount Pointer to receive the number of affected rows (optional, can be NULL)
                                     * @return VALENTINA_OK on success, or an appropriate error code
                                     *
                                     * @note The number of bind values must match the number of parameter placeholders in the query.
                                     * @note Bind values are substituted in order of appearance in the query.
                                     * @note This function is safer than concatenating values directly into SQL strings.
                                     * @note For SELECT queries, use valentina_sql_select_bind instead.
                                     * @note The outCount parameter receives the number of rows affected by the operation.
                                     */
static inline ValentinaResultCode  valentina_sql_execute_bind(
                                            valentina_context_t* ctx,
                                            const char*          query,
                                            int64_t              len,
                                            valentina_value_t*   binds,
                                            int64_t              bindsCount,
                                            int64_t*             outCount );


/**********************************************************************************************/
// Cursor API
//
// Your extension can use this API to create a native Valentina cursor
// from various data sources:
//   * result of SELECT query to the current Valentina database
//   * data that you have in memory
//   * JSON data that you have in memory
//   * external implementation of cursor
//

    // Select() to Valentina DB:

                                    /** Executes SQL query for the current Valentina database.
                                        Returns result cursor, if any. */
static inline ValentinaResultCode  valentina_sql_select(
                                        valentina_context_t* ctx,
                                        const char*          query,
                                        int64_t              len,
                                        valentina_cursor_t** outCursor );

static inline ValentinaResultCode  valentina_sql_select_bind(
                                        valentina_context_t* ctx,
                                        const char*          query,             // SQL query with '?' or ':n' placeholders
                                        int64_t              len,               // length of query in bytes.
                                        valentina_value_t*   binds,             // valentina_value_t[] array
                                        int64_t              bindsCount,
                                        valentina_cursor_t** outCursor );


/**********************************************************************************************/
// External Cursor API

                                    /** Creates a native valentina cursor from passed data. */
static inline ValentinaResultCode  valentina_create_cursor_from_data(
                                        valentina_context_t* ctx,
                                        int64_t              columnCount,
                                        int64_t              rowCount,
                                        char**               columnNames,
                                        valentina_value_t**  rows,
                                        valentina_cursor_t** outCursor );

                                    /** Creates a native valentina cursor around of JSON data. */
static inline ValentinaResultCode  valentina_create_cursor_from_json(
                                        valentina_context_t* ctx,
                                        const char*          json,
                                        int64_t              len,
                                        valentina_cursor_t** outCursor );

                                    /** Creates a native valentina cursor
                                     *  around of valentina_external_cursor_t implementation. */
static inline ValentinaResultCode  valentina_create_cursor_from_external_cursor(
                                        valentina_context_t*         ctx,
                                        valentina_external_cursor_t* externalCursor,
                                        valentina_cursor_t**         outCursor );


/**********************************************************************************************/
// Valentina Cursor Destruction

                                    /** This function releases the cursor manually.
                                     *  If this function is not called, the cursor will be automatically released
                                     *  when the associated context is destroyed. */
static inline void                 valentina_destroy_cursor(
                                        valentina_context_t* ctx,
                                        valentina_cursor_t*  cursor );


/**********************************************************************************************/
// Value API

        // -------------------------
        // Value Factories:
        //
        // Values returned by these functions belong to the context
        // and will be automatically freed upon exiting the function.

static inline valentina_value_t    valentina_create_array    ( valentina_context_t* ctx, int32_t type, valentina_value_t* items, int64_t count );
static inline valentina_value_t    valentina_create_binary   ( valentina_context_t* ctx, void* data, int64_t len, void(*deleter)(void*) );
static inline valentina_value_t    valentina_create_bool     ( valentina_context_t* ctx, bool value );
static inline valentina_value_t    valentina_create_date     ( valentina_context_t* ctx, valentina_date_t date );
static inline valentina_value_t    valentina_create_datetime ( valentina_context_t* ctx, valentina_datetime_t datetime );
static inline valentina_value_t    valentina_create_double   ( valentina_context_t* ctx, double value );
static inline valentina_value_t    valentina_create_int64    ( valentina_context_t* ctx, int64_t value );
static inline valentina_value_t    valentina_create_money    ( valentina_context_t* ctx, const char*     str, int64_t len, void(*deleter)(void*) );
static inline valentina_value_t    valentina_create_money16  ( valentina_context_t* ctx, const uint16_t* str, int64_t len, void(*deleter)(void*) );
static inline valentina_value_t    valentina_create_text     ( valentina_context_t* ctx, const char*     str, int64_t len, void(*deleter)(void*) );
static inline valentina_value_t    valentina_create_text16   ( valentina_context_t* ctx, const uint16_t* str, int64_t len, void(*deleter)(void*) );
static inline valentina_value_t    valentina_create_time     ( valentina_context_t* ctx, valentina_time_t time );
static inline valentina_value_t    valentina_create_uint64   ( valentina_context_t* ctx, uint64_t value );


        // -------------------------
        // Value Destructors:

                                    /** This function releases the value (created via 'valentina_create_*') manually.
                                     * If this function is not called, the value will be automatically released
                                     * when the associated context is destroyed. */
static inline void                 valentina_destroy_value(
                                        valentina_context_t* ctx,
                                        valentina_value_t    value );


        // -------------------------
        // Value Accessors:

static inline bool                 valentina_is_null         ( valentina_value_t value );
static inline ValentinaValueType   valentina_type_of         ( valentina_value_t value );
//
static inline int64_t              valentina_get_array_count ( valentina_value_t value );
static inline valentina_value_t    valentina_get_array_item  ( valentina_value_t value, int64_t index );
static inline const void*          valentina_get_binary      ( valentina_value_t value, int64_t* outLen );
static inline int32_t              valentina_get_bool        ( valentina_value_t value );
static inline valentina_date_t     valentina_get_date        ( valentina_value_t value );
static inline valentina_datetime_t valentina_get_datetime    ( valentina_value_t value );
static inline double               valentina_get_double      ( valentina_value_t value );
static inline int64_t              valentina_get_int64       ( valentina_value_t value );
static inline valentina_time_t     valentina_get_time        ( valentina_value_t value );
static inline uint64_t             valentina_get_uint64      ( valentina_value_t value );

                                    /** UTF8 and UTF16 text accessors. */
static inline char*                valentina_get_text        ( valentina_context_t* ctx, valentina_value_t value, int64_t* outLen );
static inline uint16_t*            valentina_get_text16      ( valentina_context_t* ctx, valentina_value_t value, int64_t* outLen );


/**********************************************************************************************/
// Memory Management API:
//

                                    /**
                                     * @brief Allocates a zero-initialized block of memory managed by the context.
                                     *
                                     * This function allocates a block of memory of the specified size and initializes
                                     * all bytes to zero. The allocated memory is automatically managed by the Valentina
                                     * context, which means it will be automatically freed when the context is destroyed,
                                     * preventing memory leaks even if valentina_free() is not called explicitly.
                                     *
                                     * This is the recommended way to allocate memory within extension functions and
                                     * procedures, as it provides automatic cleanup and integrates with Valentina's
                                     * memory management system.
                                     *
                                     * @param ctx The Valentina context that will own and manage the allocated memory
                                     * @param size The size in bytes of the memory block to allocate
                                     * @return Pointer to the allocated and zero-initialized memory block, or NULL on failure
                                     *
                                     * @note The returned memory is zero-initialized (all bytes set to 0).
                                     * @note Memory ownership belongs to the context and will be automatically freed.
                                     * @note For manual memory release, use valentina_free() with the same context.
                                     * @note Returns NULL if ctx is NULL or size is 0.
                                     * @note The allocated memory remains valid until the context is destroyed or explicitly freed.
                                     */
static inline void*                valentina_calloc(
                                        valentina_context_t* ctx,
                                        size_t               size );

                                    /**
                                     * @brief Manually releases memory owned by the context.
                                     *
                                     * This function explicitly frees memory that was allocated by valentina_calloc(),
                                     * valentina_get_text(), or valentina_get_text16() within the same context.
                                     *
                                     * While calling this function is optional (as all context-owned memory is
                                     * automatically freed when the context is destroyed), it can be useful for
                                     * early cleanup in long-running functions or when dealing with large allocations.
                                     *
                                     * This function provides fine-grained control over memory lifetime while still
                                     * maintaining the safety of automatic cleanup. It's particularly useful when
                                     * you need to free large temporary buffers before the function completes.
                                     *
                                     * @param ctx The Valentina context that owns the memory to be freed
                                     * @param ptr Pointer to the memory block to free (must have been allocated by this context)
                                     *
                                     * @note Only memory allocated by valentina_calloc(), valentina_get_text(), or
                                     *       valentina_get_text16() with the same context should be passed to this function.
                                     * @note Passing NULL for ptr is safe and will be ignored.
                                     * @note If this function is not called, memory will be automatically freed when the context is destroyed.
                                     * @note After calling this function, the pointer should be considered invalid.
                                     */
static inline void                 valentina_free(
                                        valentina_context_t* ctx,
                                        void*                ptr );


/**********************************************************************************************/
// API Table
//
// This table can be passed to extensions when direct symbol imports are not desired.
// It mirrors the public functions declared above.
//
typedef struct valentina_api
{
    uint32_t              version;           /// VALENTINA_API_VERSION used by this table layout.

    ValentinaResultCode   (*create_function)(
                                valentina_context_t* ctx,
                                valentina_function_t func );

    ValentinaResultCode   (*create_procedure)(
                                valentina_context_t*  ctx,
                                valentina_procedure_t proc );

    ValentinaResultCode   (*create_procedure_from_script)(
                                valentina_context_t*         ctx,
                                valentina_script_procedure_t proc );

    const char*           (*extensions_location)( void );
    const char*           (*user_extensions_location)( void );

    int32_t               (*arg_count)( valentina_context_t* ctx );

    void*                 (*aggregate_data)(
                                valentina_context_t* ctx,
                                int64_t              len );

    void*                 (*user_data)( valentina_context_t* ctx );
    const char*           (*error_message)( valentina_context_t* ctx );

    void*                 (*throw_error)(
                                valentina_context_t* ctx,
                                const char*          err,
                                int64_t              len );

    ValentinaResultCode   (*sql_execute)(
                                valentina_context_t* ctx,
                                const char*          query,
                                int64_t              len,
                                int64_t*             outCount );

    ValentinaResultCode   (*sql_execute_bind)(
                                valentina_context_t* ctx,
                                const char*          query,
                                int64_t              len,
                                valentina_value_t*   binds,
                                int64_t              bindsCount,
                                int64_t*             outCount );

    ValentinaResultCode   (*sql_select)(
                                valentina_context_t* ctx,
                                const char*          query,
                                int64_t              len,
                                valentina_cursor_t** outCursor );

    ValentinaResultCode   (*sql_select_bind)(
                                valentina_context_t* ctx,
                                const char*          query,
                                int64_t              len,
                                valentina_value_t*   binds,
                                int64_t              bindsCount,
                                valentina_cursor_t** outCursor );

    ValentinaResultCode   (*create_cursor_from_data)(
                                valentina_context_t* ctx,
                                int64_t              columnCount,
                                int64_t              rowCount,
                                char**               columnNames,
                                valentina_value_t**  rows,
                                valentina_cursor_t** outCursor );

    ValentinaResultCode   (*create_cursor_from_json)(
                                valentina_context_t* ctx,
                                const char*          json,
                                int64_t              len,
                                valentina_cursor_t** outCursor );

    ValentinaResultCode   (*create_cursor_from_external_cursor)(
                                valentina_context_t*         ctx,
                                valentina_external_cursor_t* externalCursor,
                                valentina_cursor_t**         outCursor );

    void                  (*destroy_cursor)(
                                valentina_context_t* ctx,
                                valentina_cursor_t*  cursor );

    valentina_value_t     (*create_array)( valentina_context_t* ctx, int32_t type, valentina_value_t* items, int64_t count );
    valentina_value_t     (*create_binary)( valentina_context_t* ctx, void* data, int64_t len, void(*deleter)(void*) );
    valentina_value_t     (*create_bool)( valentina_context_t* ctx, bool value );
    valentina_value_t     (*create_date)( valentina_context_t* ctx, valentina_date_t date );
    valentina_value_t     (*create_datetime)( valentina_context_t* ctx, valentina_datetime_t datetime );
    valentina_value_t     (*create_double)( valentina_context_t* ctx, double value );
    valentina_value_t     (*create_int64)( valentina_context_t* ctx, int64_t value );
    valentina_value_t     (*create_money)( valentina_context_t* ctx, const char* str, int64_t len, void(*deleter)(void*) );
    valentina_value_t     (*create_money16)( valentina_context_t* ctx, const uint16_t* str, int64_t len, void(*deleter)(void*) );
    valentina_value_t     (*create_text)( valentina_context_t* ctx, const char* str, int64_t len, void(*deleter)(void*) );
    valentina_value_t     (*create_text16)( valentina_context_t* ctx, const uint16_t* str, int64_t len, void(*deleter)(void*) );
    valentina_value_t     (*create_time)( valentina_context_t* ctx, valentina_time_t time );
    valentina_value_t     (*create_uint64)( valentina_context_t* ctx, uint64_t value );

    void                  (*destroy_value)(
                                valentina_context_t* ctx,
                                valentina_value_t    value );

    bool                  (*is_null)( valentina_value_t value );
    ValentinaValueType    (*type_of)( valentina_value_t value );

    int64_t               (*get_array_count)( valentina_value_t value );
    valentina_value_t     (*get_array_item)( valentina_value_t value, int64_t index );
    const void*           (*get_binary)( valentina_value_t value, int64_t* outLen );
    int32_t               (*get_bool)( valentina_value_t value );
    valentina_date_t      (*get_date)( valentina_value_t value );
    valentina_datetime_t  (*get_datetime)( valentina_value_t value );
    double                (*get_double)( valentina_value_t value );
    int64_t               (*get_int64)( valentina_value_t value );
    valentina_time_t      (*get_time)( valentina_value_t value );
    uint64_t              (*get_uint64)( valentina_value_t value );

    char*                 (*get_text)( valentina_context_t* ctx, valentina_value_t value, int64_t* outLen );
    uint16_t*             (*get_text16)( valentina_context_t* ctx, valentina_value_t value, int64_t* outLen );

    void*                 (*calloc)(
                                valentina_context_t* ctx,
                                size_t               size );

    void                  (*free)(
                                valentina_context_t* ctx,
                                void*                ptr );
}
valentina_api;


/**********************************************************************************************/
extern const valentina_api* g_valentina_api_ptr;


/**********************************************************************************************/
static inline ValentinaResultCode valentina_create_function(
    valentina_context_t* ctx,
    valentina_function_t func )
{
    return g_valentina_api_ptr->create_function( ctx, func );
}


/**********************************************************************************************/
static inline ValentinaResultCode valentina_create_procedure(
    valentina_context_t*  ctx,
    valentina_procedure_t proc )
{
    return g_valentina_api_ptr->create_procedure( ctx, proc );
}


/**********************************************************************************************/
static inline ValentinaResultCode valentina_create_procedure_from_script(
    valentina_context_t*         ctx,
    valentina_script_procedure_t proc )
{
    return g_valentina_api_ptr->create_procedure_from_script( ctx, proc );
}


/**********************************************************************************************/
static inline const char* valentina_extensions_location( void )
{
    return g_valentina_api_ptr->extensions_location();
}


/**********************************************************************************************/
static inline const char* valentina_user_extensions_location( void )
{
    return g_valentina_api_ptr->user_extensions_location();
}


/**********************************************************************************************/
static inline int32_t valentina_arg_count( valentina_context_t* ctx )
{
    return g_valentina_api_ptr->arg_count( ctx );
}


/**********************************************************************************************/
static inline void* valentina_aggregate_data(
    valentina_context_t* ctx,
    int64_t              len )
{
    return g_valentina_api_ptr->aggregate_data( ctx, len );
}


/**********************************************************************************************/
static inline void* valentina_user_data( valentina_context_t* ctx )
{
    return g_valentina_api_ptr->user_data( ctx );
}


/**********************************************************************************************/
static inline const char* valentina_error_message( valentina_context_t* ctx )
{
    return g_valentina_api_ptr->error_message( ctx );
}


/**********************************************************************************************/
static inline void* valentina_throw_error(
    valentina_context_t* ctx,
    const char*          err,
    int64_t              len )
{
    return g_valentina_api_ptr->throw_error( ctx, err, len );
}


/**********************************************************************************************/
static inline ValentinaResultCode valentina_sql_execute(
    valentina_context_t* ctx,
    const char*          query,
    int64_t              len,
    int64_t*             outCount )
{
    return g_valentina_api_ptr->sql_execute( ctx, query, len, outCount );
}


/**********************************************************************************************/
static inline ValentinaResultCode valentina_sql_execute_bind(
    valentina_context_t* ctx,
    const char*          query,
    int64_t              len,
    valentina_value_t*   binds,
    int64_t              bindsCount,
    int64_t*             outCount )
{
    return g_valentina_api_ptr->sql_execute_bind( ctx, query, len, binds, bindsCount, outCount );
}


/**********************************************************************************************/
static inline ValentinaResultCode valentina_sql_select(
    valentina_context_t* ctx,
    const char*          query,
    int64_t              len,
    valentina_cursor_t** outCursor )
{
    return g_valentina_api_ptr->sql_select( ctx, query, len, outCursor );
}


/**********************************************************************************************/
static inline ValentinaResultCode valentina_sql_select_bind(
    valentina_context_t* ctx,
    const char*          query,
    int64_t              len,
    valentina_value_t*   binds,
    int64_t              bindsCount,
    valentina_cursor_t** outCursor )
{
    return g_valentina_api_ptr->sql_select_bind( ctx, query, len, binds, bindsCount, outCursor );
}


/**********************************************************************************************/
static inline ValentinaResultCode valentina_create_cursor_from_data(
    valentina_context_t* ctx,
    int64_t              columnCount,
    int64_t              rowCount,
    char**               columnNames,
    valentina_value_t**  rows,
    valentina_cursor_t** outCursor )
{
    return g_valentina_api_ptr->create_cursor_from_data( ctx, columnCount, rowCount, columnNames, rows, outCursor );
}


/**********************************************************************************************/
static inline ValentinaResultCode valentina_create_cursor_from_json(
    valentina_context_t* ctx,
    const char*          json,
    int64_t              len,
    valentina_cursor_t** outCursor )
{
    return g_valentina_api_ptr->create_cursor_from_json( ctx, json, len, outCursor );
}


/**********************************************************************************************/
static inline ValentinaResultCode valentina_create_cursor_from_external_cursor(
    valentina_context_t*         ctx,
    valentina_external_cursor_t* externalCursor,
    valentina_cursor_t**         outCursor )
{
    return g_valentina_api_ptr->create_cursor_from_external_cursor( ctx, externalCursor, outCursor );
}


/**********************************************************************************************/
static inline void valentina_destroy_cursor(
    valentina_context_t* ctx,
    valentina_cursor_t*  cursor )
{
    g_valentina_api_ptr->destroy_cursor( ctx, cursor );
}


/**********************************************************************************************/
static inline valentina_value_t valentina_create_array( valentina_context_t* ctx, int32_t type, valentina_value_t* items, int64_t count )
{
    return g_valentina_api_ptr->create_array( ctx, type, items, count );
}


/**********************************************************************************************/
static inline valentina_value_t valentina_create_binary( valentina_context_t* ctx, void* data, int64_t len, void(*deleter)(void*) )
{
    return g_valentina_api_ptr->create_binary( ctx, data, len, deleter );
}


/**********************************************************************************************/
static inline valentina_value_t valentina_create_bool( valentina_context_t* ctx, bool value )
{
    return g_valentina_api_ptr->create_bool( ctx, value );
}


/**********************************************************************************************/
static inline valentina_value_t valentina_create_date( valentina_context_t* ctx, valentina_date_t date )
{
    return g_valentina_api_ptr->create_date( ctx, date );
}


/**********************************************************************************************/
static inline valentina_value_t valentina_create_datetime( valentina_context_t* ctx, valentina_datetime_t datetime )
{
    return g_valentina_api_ptr->create_datetime( ctx, datetime );
}


/**********************************************************************************************/
static inline valentina_value_t valentina_create_double( valentina_context_t* ctx, double value )
{
    return g_valentina_api_ptr->create_double( ctx, value );
}


/**********************************************************************************************/
static inline valentina_value_t valentina_create_int64( valentina_context_t* ctx, int64_t value )
{
    return g_valentina_api_ptr->create_int64( ctx, value );
}


/**********************************************************************************************/
static inline valentina_value_t valentina_create_money( valentina_context_t* ctx, const char* str, int64_t len, void(*deleter)(void*) )
{
    return g_valentina_api_ptr->create_money( ctx, str, len, deleter );
}


/**********************************************************************************************/
static inline valentina_value_t valentina_create_money16( valentina_context_t* ctx, const uint16_t* str, int64_t len, void(*deleter)(void*) )
{
    return g_valentina_api_ptr->create_money16( ctx, str, len, deleter );
}


/**********************************************************************************************/
static inline valentina_value_t valentina_create_text( valentina_context_t* ctx, const char* str, int64_t len, void(*deleter)(void*) )
{
    return g_valentina_api_ptr->create_text( ctx, str, len, deleter );
}


/**********************************************************************************************/
static inline valentina_value_t valentina_create_text16( valentina_context_t* ctx, const uint16_t* str, int64_t len, void(*deleter)(void*) )
{
    return g_valentina_api_ptr->create_text16( ctx, str, len, deleter );
}


/**********************************************************************************************/
static inline valentina_value_t valentina_create_time( valentina_context_t* ctx, valentina_time_t time )
{
    return g_valentina_api_ptr->create_time( ctx, time );
}


/**********************************************************************************************/
static inline valentina_value_t valentina_create_uint64( valentina_context_t* ctx, uint64_t value )
{
    return g_valentina_api_ptr->create_uint64( ctx, value );
}


/**********************************************************************************************/
static inline void valentina_destroy_value(
    valentina_context_t* ctx,
    valentina_value_t    value )
{
    g_valentina_api_ptr->destroy_value( ctx, value );
}


/**********************************************************************************************/
static inline bool valentina_is_null( valentina_value_t value )
{
    return g_valentina_api_ptr->is_null( value );
}


/**********************************************************************************************/
static inline ValentinaValueType valentina_type_of( valentina_value_t value )
{
    return g_valentina_api_ptr->type_of( value );
}


/**********************************************************************************************/
static inline int64_t valentina_get_array_count( valentina_value_t value )
{
    return g_valentina_api_ptr->get_array_count( value );
}


/**********************************************************************************************/
static inline valentina_value_t valentina_get_array_item( valentina_value_t value, int64_t index )
{
    return g_valentina_api_ptr->get_array_item( value, index );
}


/**********************************************************************************************/
static inline const void* valentina_get_binary( valentina_value_t value, int64_t* outLen )
{
    return g_valentina_api_ptr->get_binary( value, outLen );
}


/**********************************************************************************************/
static inline int32_t valentina_get_bool( valentina_value_t value )
{
    return g_valentina_api_ptr->get_bool( value );
}


/**********************************************************************************************/
static inline valentina_date_t valentina_get_date( valentina_value_t value )
{
    return g_valentina_api_ptr->get_date( value );
}


/**********************************************************************************************/
static inline valentina_datetime_t valentina_get_datetime( valentina_value_t value )
{
    return g_valentina_api_ptr->get_datetime( value );
}


/**********************************************************************************************/
static inline double valentina_get_double( valentina_value_t value )
{
    return g_valentina_api_ptr->get_double( value );
}


/**********************************************************************************************/
static inline int64_t valentina_get_int64( valentina_value_t value )
{
    return g_valentina_api_ptr->get_int64( value );
}


/**********************************************************************************************/
static inline valentina_time_t valentina_get_time( valentina_value_t value )
{
    return g_valentina_api_ptr->get_time( value );
}


/**********************************************************************************************/
static inline uint64_t valentina_get_uint64( valentina_value_t value )
{
    return g_valentina_api_ptr->get_uint64( value );
}


/**********************************************************************************************/
static inline char* valentina_get_text( valentina_context_t* ctx, valentina_value_t value, int64_t* outLen )
{
    return g_valentina_api_ptr->get_text( ctx, value, outLen );
}


/**********************************************************************************************/
static inline uint16_t* valentina_get_text16( valentina_context_t* ctx, valentina_value_t value, int64_t* outLen )
{
    return g_valentina_api_ptr->get_text16( ctx, value, outLen );
}


/**********************************************************************************************/
static inline void* valentina_calloc(
    valentina_context_t* ctx,
    size_t               size )
{
    return g_valentina_api_ptr->calloc( ctx, size );
}


/**********************************************************************************************/
static inline void valentina_free(
    valentina_context_t* ctx,
    void*                ptr )
{
    g_valentina_api_ptr->free( ctx, ptr );
}



/**********************************************************************************************/
#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
