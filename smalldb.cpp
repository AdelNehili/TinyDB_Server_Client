#include "db.hpp"
#include "queries.hpp"
#include <stdio.h>

int main(int argc, char const *argv[]) {
	
	database_t db;
	const char *database_file;
	if (argc > 1) {
		database_file = argv[1];
	} else {
		fprintf(stderr, "Mandatory parameter (db file) missing.\n");
		return 1;
	}
   
	db_load(&db, database_file);
	
	char line[200];
	
	while (1) {
		
		printf("> ");
		fgets(line,200,stdin);
		FILE * file = fopen("error.txt", "rb");
		parse_and_execute(file , &db ,line);
		fclose(file);
	}
	
	db_save(&db);
	printf("Done.\n");
	
	return 0;
}
