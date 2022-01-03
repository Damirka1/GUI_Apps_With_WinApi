#include "../Headers/DatabaseApp.h"

Database* DatabaseApp::CreateDatabase(std::string Name)
{
	Database* base = new Database(&Name);
	Bases.push_back(base);
	return base;
}

void DatabaseApp::PrintDatabase(std::string Name)
{
	for (auto b = Bases.begin(), e = Bases.end(); b != e; ++b)
	{
		Database base(&Name);
		if ((*b)->Name == Name)
			return PrintDatabase(*b);
	}
}

void DatabaseApp::PrintDatabase(Database* Database)
{
	for (auto b = Database->Layouts.begin(), e = Database->Layouts.end(); b != e; ++b)
		printf("%s ", b->Name.c_str());

	putchar('\n');

	for (unsigned int i = 0; i < Database->Rows.size(); i++)
	{
		void* pRow = Database->Rows[i];
		for (unsigned int j = 0; j < Database->FinalFieldsCount; j++)
		{
			void* pField = static_cast<char*>(pRow) + j * sizeof(void*);
			void* pData = NULL;
			memcpy(&pData, pField, sizeof(void*));

			switch (Database->Layouts[j].Type)
			{
			case Database::FieldType::Int8:
				printf("%hhd ", *static_cast<__int8*>(pData));
				break;
			case Database::FieldType::Int16:
				printf("%hd ", *static_cast<__int16*>(pData));
				break;
			case Database::FieldType::Float32:
				printf("%f ", *static_cast<float*>(pData));
				break;
			case Database::FieldType::Int32:
				printf("%i ", *static_cast<int*>(pData));
				break;
			case Database::FieldType::Float64:
				printf("%lf ", *static_cast<double*>(pData));
				break;
			case Database::FieldType::Int64:
				printf("%lli ", *static_cast<long long*>(pData));
				break;
			case Database::FieldType::String:
				printf("%s ", static_cast<const char*>(pData));
				break;
			default:
				break;
			}

		}
		putchar('\n');
	}
}
