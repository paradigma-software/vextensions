/**********************************************************************************************/
/* generate_series.c   	                                                   					  */
/*                                                                       					  */
/* Copyright Paradigma Software, 1998-2026                                                    */
/* All Rights Reserved                                                   					  */
/**********************************************************************************************/
/**
    @brief Valentina Extension for `Generate Series` Procedure
            Demonstrates how to create a procedure that generates a series of numbers
            and returns them as a random-access cursor with a single column and N rows.

    Procedure to generate a series of numbers
        is based on the provided start, end, and step values.

    Declaration of the procedure can looks like this:
        ```sql
        CALL generate_series( start, end [, step ] );
        ```

    Arguments:
        - `start`: The starting value of the series (can be an integer or a double).
        - `end`: The ending value of the series (can be an integer or a double).
        - `step`: The step value for the series (optional, defaults to 1 for integers and 1.0 for doubles).

    @note This extension is written in C and uses the Valentina Extension API.


    @example 1 usage in Valentina SQL as direct call to the procedure:
    {
        "CALL generate_series( 1, 10, 2 );"

            result:     +------------------+
                        | generate_series  |
                        +------------------+
                        | 1                |
                        | 3                |
                        | 5                |
                        | 7                |
                        | 9                |
                        +------------------+

        "CALL generate_series( 1.0, 10.0, 0.5 );"

            result:     +------------------+
                        | generate_series  |
                        +------------------+
                        | 1.0              |
                        | 1.5              |
                        | 2.0              |
                        | 2.5              |
                        | ...              |
                        | 9.5              |
                        +------------------+
    }


    @example 2 usage in Valentina SQL as Table Function, i.e. in FROM clause:
    {
        "SELECT * FROM generate_series( 1, 10, 2 );"
    }
*/
/**********************************************************************************************/


// STD:
#include <stdlib.h>

// VALENTINA DB EXTENSION:
//
// The only file you need include to create a Valentina extension in C/C++
//
#include <FBL_Extension.h>

/**********************************************************************************************/
const char*         COLUMN_NAMES[]        = { "generate_series" };
ValentinaValueType  COLUMN_TYPES_DOUBLE[] = { VALENTINA_TYPE_DOUBLE };
ValentinaValueType  COLUMN_TYPES_INT64[]  = { VALENTINA_TYPE_INT64 };


/**********************************************************************************************/
typedef struct
{
    double start;
    double end;
    double step;
    double pos;
}
cursor_double_t;


/**********************************************************************************************/
typedef struct
{
    int64_t start;
    int64_t end;
    int64_t step;
    int64_t pos;
}
cursor_int64_t;


/**********************************************************************************************/
void cursor_close( valentina_context_t* ctx, void* cursor )
{
    free( cursor );
}


/**********************************************************************************************/
valentina_value_t cursor_column_value_double( valentina_context_t* ctx, void* cursor, int32_t index )
{
    cursor_double_t* c = cursor;

    return valentina_create_double( ctx, c->start + c->step * c->pos );
}


/**********************************************************************************************/
valentina_value_t cursor_column_value_int64( valentina_context_t* ctx, void* cursor, int32_t index )
{
    cursor_int64_t* c = cursor;

    return valentina_create_int64( ctx, c->start + c->step * c->pos );
}


/**********************************************************************************************/
int64_t cursor_row_count_double( valentina_context_t* ctx, void* cursor )
{
    cursor_double_t* c = cursor;

    return ( c->end - c->start ) / c->step   + 1;
}


/**********************************************************************************************/
int64_t cursor_row_count_int64( valentina_context_t* ctx, void* cursor )
{
    cursor_int64_t* c = cursor;

    return ( c->end - c->start ) / c->step   + 1;
}


/**********************************************************************************************/
ValentinaResultCode cursor_move_double( valentina_context_t* ctx, void* cursor, int64_t row )
{
    cursor_double_t* c = cursor;

    if( row >= 0 && row < cursor_row_count_double( ctx, cursor ) )
    {
        c->pos = row;
        return VALENTINA_OK;
    }

    return VALENTINA_DONE;
}


/**********************************************************************************************/
ValentinaResultCode cursor_move_int64( valentina_context_t* ctx, void* cursor, int64_t row )
{
    cursor_int64_t* c = cursor;

    if( row >= 0 && row < cursor_row_count_int64( ctx, cursor ) )
    {
        c->pos = row;
        return VALENTINA_OK;
    }

    return VALENTINA_DONE;
}


/**********************************************************************************************/
valentina_cursor_t* generate_series( valentina_context_t* ctx, valentina_value_t* args )
{
    valentina_external_cursor_t series_cursor =
    {
        .columnCount = 1,
        .columnNames = COLUMN_NAMES,

        .xClose      = cursor_close,
    };

    switch( valentina_type_of( args[ 0 ] ) )
    {
        case VALENTINA_TYPE_DOUBLE  :
        {
            double step = 1.;
            if( valentina_arg_count( ctx ) == 3 )
                step = valentina_get_double( args[ 2 ] );

            if( !step )
            {
                valentina_throw_error( ctx, "The step can't be zero.", -1 );
                return NULL;
            }

            series_cursor.xColumnValue = cursor_column_value_double;
            series_cursor.xMove        = cursor_move_double;
            series_cursor.xRowCount    = cursor_row_count_double;
            series_cursor.columnTypes  = COLUMN_TYPES_DOUBLE;

            cursor_double_t* cursor_double = calloc( sizeof(cursor_double_t), 1 );
            series_cursor.cursor = cursor_double;

            *cursor_double = (cursor_double_t)
            {
                .start = valentina_get_double( args[ 0 ] ),
                .end   = valentina_get_double( args[ 1 ] ),
                .step  = step
            };
        }
        break;

        case VALENTINA_TYPE_INT64   :
        case VALENTINA_TYPE_UINT64  :
        {
            int64_t step = 1;
            if( valentina_arg_count( ctx ) == 3 )
                step = valentina_get_int64( args[ 2 ] );

            if( !step )
            {
                valentina_throw_error( ctx, "The step can't be zero.", -1 );
                return NULL;
            }

            series_cursor.xColumnValue = cursor_column_value_int64;
            series_cursor.xMove        = cursor_move_int64;
            series_cursor.xRowCount    = cursor_row_count_int64;
            series_cursor.columnTypes  = COLUMN_TYPES_INT64;

            cursor_int64_t* cursor_int64 = calloc( sizeof(cursor_int64_t), 1 );
            series_cursor.cursor = cursor_int64;

            *cursor_int64 = (cursor_int64_t)
            {
                .start = valentina_get_int64( args[ 0 ] ),
                .end   = valentina_get_int64( args[ 1 ] ),
                .step  = step
            };
        }
        break;

        default:
            valentina_throw_error( ctx, "The start value must be an integer or a double.", -1 );
            return NULL;
    }

    valentina_cursor_t* cursor;
    if( valentina_create_cursor_from_external_cursor( ctx, &series_cursor, &cursor ) != VALENTINA_OK )
    {
        valentina_throw_error( ctx, valentina_error_message( ctx ), -1 );
        return NULL;
    }

    return cursor;
}


/**********************************************************************************************/
/**
    @brief Registers the generate_series() procedure with the Valentina DB engine,
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
    // Register the generate_series() procedure:
    valentina_create_procedure( ctx, (valentina_procedure_t) {
        .xProc   = generate_series,     // pointer to the function that implements the procedure
        .name    = "generate_series",
        .argDesc = "start, stop [,step]",
        .minArg  = 2,
        .maxArg  = 3,
        .desc    = "Generates a series of numbers within a given interval."
    });
}
