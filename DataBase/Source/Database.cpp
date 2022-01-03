#include "../Headers/Database.h"

Database::Database(std::string* Name)
	:
	Name(*Name)
{
}

bool Database::AddField(std::string Name, FieldType Type, unsigned int FieldLength)
{
	if (Name.length())
	{
		unsigned int Size = 0;
		switch (Type)
		{
		case Database::FieldType::Int8:
			Size = 1;
			break;
		case Database::FieldType::Int16:
			Size = 2;
			break;
		case Database::FieldType::Float32:
		case Database::FieldType::Int32:
			Size = 4;
			break;
		case Database::FieldType::Float64:
		case Database::FieldType::Int64:
			Size = 8;
			break;
		case Database::FieldType::String:
			Size = FieldLength;
			break;
		case Database::FieldType::Undefined:
		default:
			Size = 0;
			break;
		}
		Layouts.emplace_back(static_cast<unsigned int>(Layouts.size()), Name, Type, Size);
		return true;
	}
	return false;
}

bool Database::RemoveField(std::string Name)
{
	for (auto b = Layouts.begin(), e = Layouts.end(); b != e; ++b)
	{
		if (b->Name == Name)
		{
			Layouts.erase(b);
			return true;
		}
	}
	return false;
}

bool Database::RemoveField(unsigned int Index)
{
	for (auto b = Layouts.begin(), e = Layouts.end(); b != e; ++b)
	{
		if (b->Index == Index)
		{
			Layouts.erase(b);
			return true;
		}
	}
	return false;
}

Database::Row Database::CreateRow()
{
	if (FinalFieldsCount == 0)
		FinalFieldsCount = static_cast<unsigned int>(Layouts.size());

	if (FinalFieldsCount > 0)
	{
		void* pRow = malloc(sizeof(void*) * FinalFieldsCount);
		memset(pRow, 0, sizeof(void*) * FinalFieldsCount);
		Rows.push_back(pRow);

		return { this, static_cast<unsigned int>(Rows.size() - 1), pRow };
	}
	return {};
}

bool Database::RemoveRow(int Index)
{
	if (Index >= Rows.size())
		return false;

	void* pRow = Rows[Index];

	for (unsigned int i = 0; i < FinalFieldsCount; i++)
	{
		void* pField = static_cast<char*>(pRow) + i * sizeof(void*);
		if (pField)
		{
			free(pField);
			pField = NULL;
		}
	}
	free(pRow);
	Rows.erase(Rows.begin() + Index);
	return true;
}

bool Database::AddDataToRow(unsigned int RowIndex, unsigned int FieldIndex, const void* pData)
{
	if (FieldIndex < FinalFieldsCount && pData)
	{
		void* pRow = Rows[RowIndex];
		void* pField = static_cast<char*>(pRow) + FieldIndex * sizeof(void*);

		FieldLayout* lay = &Layouts[FieldIndex];

		void* pTmp = malloc(lay->Size);
		memcpy(pField, &pTmp, sizeof(void*));

		if (lay->Type == FieldType::String)
		{
			memcpy(pTmp, pData, static_cast<size_t>(strlen(static_cast<const char*>(pData)) + 1));
			return true;
		}
		memcpy(pTmp, pData, lay->Size);
		return true;
	}
	return false;
}

bool Database::AddDataToRow(unsigned int RowIndex, std::string FieldName, const void* pData)
{
	if (pData)
	{
		void* pRow = Rows[RowIndex];

		for (auto b = Layouts.begin(), e = Layouts.end(); b != e; ++b)
		{
			if (b->Name == FieldName)
			{
				void* pField = static_cast<char*>(pRow) + b->Index * sizeof(void*);
				void* pTmp = malloc(b->Size);
				memcpy(pField, &pTmp, sizeof(void*));

				if (b->Type == FieldType::String)
				{
					memcpy(pTmp, pData, static_cast<size_t>(strlen(static_cast<const char*>(pData)) + 1));
					return true;
				}
				memcpy(pTmp, pData, b->Size);
				return true;
			}
		}
	}
	return false;
}

bool Database::RemoveDataFromRow(unsigned int RowIndex, unsigned int FieldIndex)
{
	if (FieldIndex < FinalFieldsCount)
	{
		void* pRow = Rows[RowIndex];
		void* pField = static_cast<char*>(pRow) + RowIndex * sizeof(void*);

		if (pField)
		{
			free(pField);
			pField = NULL;
			return true;
		}
	}
	return false;
}

Database::Row::Row(Database* pDatabase, unsigned int Index, void* pRow)
	:
	pDatabase(pDatabase),
	Index(Index),
	pRow(pRow)
{}

bool Database::Row::AddDataAtRowByIndex(unsigned int FieldIndex, const void* pData)
{
	return pDatabase->AddDataToRow(Index, FieldIndex, pData);
}

bool Database::Row::AddDataAtRow(std::string FieldName, const void* pData)
{
	return pDatabase->AddDataToRow(Index, FieldName, pData);
}

Database::FieldLayout::FieldLayout(unsigned int Index, std::string Name, FieldType Type, unsigned int Size)
	:
	Index(Index),
	Name(Name),
	Type(Type),
	Size(Size)
{

}
