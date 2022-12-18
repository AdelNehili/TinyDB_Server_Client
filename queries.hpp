#ifndef _QUERIES_HPP
#define _QUERIES_HPP

#include <cstdio>

#include "db.hpp"

// execute_* //////////////////////////////////////////////////////////////////

void execute_select( database_t* const db, const char* const field,
                    const char* const value, char* buffer_to_change);

void execute_update( database_t* const db, const char* const ffield,
                    const char* const fvalue, const char* const efield, const char* const evalue, char* buffer_to_change);

void execute_insert( database_t* const db, const char* const fname,
                    const char* const lname, const unsigned id, const char* const section,
                    const tm birthdate, char* buffer_to_change);

void execute_dump( database_t* const db);

// parse_and_execute_* ////////////////////////////////////////////////////////

void parse_and_execute_select( database_t* db, const char* const query, char* buffer_to_change);

void parse_and_execute_update( database_t* db, const char* const query, char* buffer_to_change);

void parse_and_execute_insert( database_t* db, const char* const query, char* buffer_to_change);

void parse_and_execute_delete( database_t* db, const char* const query, char* buffer_to_change);

	//void parse_and_execute( database_t* db, const char* const query, char* buffer_to_change);

// query_fail_* ///////////////////////////////////////////////////////////////

/** Those methods write a descriptive error message on fout */

void query_fail_bad_query_type(char* buffer_to_change);

void query_fail_bad_format(char* buffer_to_change, const char* const query_type);

void query_fail_bad_filter(char* buffer_to_change, const char* const field, const char* const filter);

void query_fail_bad_update(char* buffer_to_change, const char* const field, const char* const filter);

#endif
