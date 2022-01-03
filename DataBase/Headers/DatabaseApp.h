#pragma once
#include "Database.h"
#include <stdio.h>

class DatabaseApp
{
	std::vector<Database*> Bases;

public:
	Database* CreateDatabase(std::string Name);

	void PrintDatabase(std::string Name);

	void PrintDatabase(Database* Database);
	
};