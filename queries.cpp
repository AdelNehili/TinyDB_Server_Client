#include "queries.hpp"

// execute_* ///////////////////////////////////////////////////////////////////

void execute_select(database_t* const db, const char* const field,
                    const char* const value, char* buffer_to_change) {
  std::function<bool(const student_t&)> predicate = get_filter(field, value);
  int i = 0;
  if (!predicate) {
    query_fail_bad_filter(buffer_to_change, field, value);
    return;
  }
  for (const student_t& s : db->data) {
    if (predicate(s)) {
		student_to_str(buffer_to_change,&s,4096);
		i=i+1;
    }
  }
  student_nbr_to_buffer(buffer_to_change,4096,i);
}

void execute_update(database_t* const db, const char* const ffield, const char* const fvalue, const char* const efield, const char* const evalue, char* buffer_to_change) {
  std::function<bool(const student_t&)> predicate = get_filter(ffield, fvalue);
  int i = 0;
  if (!predicate) {
    query_fail_bad_filter(buffer_to_change, ffield, fvalue);
    return;
  }
  std::function<void(student_t&)> updater = get_updater(efield, evalue);
  if (!updater) {
    query_fail_bad_update(buffer_to_change, efield, evalue);
    return;
  }
  for (student_t& s : db->data) {
    if (predicate(s)) {
      updater(s);
      i++;
    }
  }
  student_nbr_to_buffer(buffer_to_change,4096,i);
}

void execute_insert(database_t* const db, const char* const fname,
                    const char* const lname, const unsigned id, const char* const section,
                    const tm birthdate, char* buffer_to_change) {
	db->data.emplace_back();
	student_t *s = &db->data.back();
	s->id = id;
	snprintf(s->fname, sizeof(s->fname), "%s", fname);
	snprintf(s->lname, sizeof(s->lname), "%s", lname);
	snprintf(s->section, sizeof(s->section), "%s", section);
	s->birthdate = birthdate;
	student_to_str(buffer_to_change,s,4096);
}

void execute_delete(database_t* const db, const char* const field,
                    const char* const value, char* buffer_to_change) {
  std::function<bool(const student_t&)> predicate = get_filter(field, value);
  int i = 0;
  if (!predicate) {
    query_fail_bad_filter(buffer_to_change, field, value);
    return;
  }
  for (const student_t& s : db->data) {
    if (predicate(s)) {
		i=i+1;
    }
  }
  auto new_end = remove_if(db->data.begin(), db->data.end(), predicate);
  db->data.erase(new_end, db->data.end());
  student_nbr_to_buffer(buffer_to_change,4096,i);
}

// parse_and_execute_* ////////////////////////////////////////////////////////

void parse_and_execute_select(database_t* db, const char* const query,char* buffer_to_change) {
  char ffield[32], fvalue[64];  // filter data
  int  counter;
  if (sscanf(query, "select %31[^=]=%63s%n", ffield, fvalue, &counter) != 2) {
    query_fail_bad_format(buffer_to_change, "select");
  } else
    execute_select( db, ffield, fvalue,buffer_to_change);
	}

void parse_and_execute_update(database_t* db, const char* const query, char* buffer_to_change) {
  char ffield[32], fvalue[64];  // filter data
  char efield[32], evalue[64];  // edit data
  int counter;
  if (sscanf(query, "update %31[^=]=%63s set %31[^=]=%63s%n", ffield, fvalue, efield, evalue,
             &counter) != 4) {
    query_fail_bad_format(buffer_to_change, "update");
  } else {
    execute_update(db, ffield, fvalue, efield, evalue,buffer_to_change);
  }
}

void parse_and_execute_insert(database_t* db, const char* const query, char* buffer_to_change) {
  char      fname[64], lname[64], section[64], date[11];
  unsigned  id;
  tm        birthdate;
  int       counter;
  if (sscanf(query, "insert %63s %63s %u %63s %10s%n", fname, lname, &id, section, date, &counter) != 5 || strptime(date, "%d/%m/%Y", &birthdate) == NULL) {
    query_fail_bad_format(buffer_to_change, "insert");
  } else {
    execute_insert(db, fname, lname, id, section, birthdate,buffer_to_change);
  }
}

void parse_and_execute_delete(database_t* db, const char* const query, char* buffer_to_change) {
  char ffield[32], fvalue[64]; // filter data
  int counter;
  if (sscanf(query, "delete %31[^=]=%63s%n", ffield, fvalue, &counter) != 2) {
    query_fail_bad_format(buffer_to_change, "delete");
  } else {
    execute_delete(db, ffield, fvalue,buffer_to_change);
  }
}


// query_fail_* ///////////////////////////////////////////////////////////////

void query_fail_bad_query_type(char* buffer_to_change) {
	snprintf(buffer_to_change,4096,"Error : Unknown query type \n");
}

void query_fail_bad_format(char* buffer_to_change, const char * const query_type) {
	snprintf(buffer_to_change,4096,"Error : Syntax error in %s \n",query_type);
}

void query_fail_bad_filter(char* buffer_to_change, const char* const field, const char* const filter) {
	snprintf(buffer_to_change,4096,"Error : %s = %s is not a valid filter \n",field,filter);
}

void query_fail_bad_update(char* buffer_to_change, const char* const field, const char* const filter) {
	snprintf(buffer_to_change,4096,"Error : You can not apply %s = %s \n", field,filter);
}

