#include "setup.h"

#include <string>
#include <iostream>
#include <vector>
#include <signal.h>
#include <sys/types.h>
#include <malloc.h>
#include <new>

using namespace std;

void select(string champ,string var);

void insert(string fname,string lname,string id,string section,string birthdate_string);

void _find_to_delete(string champs, string valeur);

void del(string champ,string var);

void _find_to_update(string filtre, string valeur, string champ_modifie, string valeur_modifiee);

void update(string filtre,string valeur,string champ,string val_modif);

void transaction(void);
