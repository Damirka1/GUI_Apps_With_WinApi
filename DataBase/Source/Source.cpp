#include "../Headers/DatabaseApp.h"

int main(int argc, char** argv)
{
	DatabaseApp ap;
	Database* base = ap.CreateDatabase("Test.db");

	base->AddField("Index", Database::FieldType::Int32);
	base->AddField("User", Database::FieldType::String);
	base->AddField("Password", Database::FieldType::String);

	int index = 0;

	Database::Row r = base->CreateRow();
	r.AddDataAtRow("Index", &index);
	r.AddDataAtRow("User", "Damirka");
	r.AddDataAtRow("Password", "Hello, world!");

	index++;

	Database::Row r2 = base->CreateRow();
	r2.AddDataAtRow("Index", &index);
	r2.AddDataAtRow("User", "Damir");
	r2.AddDataAtRow("Password", "Test123");

	ap.PrintDatabase("Test.db");

	return 0;
}