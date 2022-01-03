#pragma once
#include <vector>
#include <string>

class Database
{
	friend class DatabaseApp;
	friend class Row;

public:
	enum class FieldType
	{
		Int8,
		Int16,
		Int32,
		Int64,
		Float32,
		Float64,
		String,
		Undefined
	};

	class Row
	{
		Database* pDatabase;
		unsigned int Index;
		void* pRow;
	public:
		Row(Database* pDatabase, unsigned int Index, void* pRow);
		Row() = default;

		bool AddDataAtRow(std::string FieldName, const void* pData);
		bool AddDataAtRowByIndex(unsigned int FieldIndex, const void* pData);
	};

private:
	struct FieldLayout
	{
		unsigned int Index;
		std::string Name;
		FieldType Type;
		unsigned int Size;

	public:
		FieldLayout(unsigned int Index, std::string Name, FieldType Type, unsigned int Size);
	};

	std::string Name;

	std::vector<FieldLayout> Layouts;

	std::vector<void*> Rows;
	unsigned int FinalFieldsCount;


private:
	Database(std::string* Name);
	bool AddDataToRow(unsigned int RowIndex, unsigned int FieldIndex, const void* pData);
	bool AddDataToRow(unsigned int RowIndex, std::string FieldName, const void* pData);
	bool RemoveDataFromRow(unsigned int RowIndex, unsigned int FieldIndex);

public:
	bool AddField(std::string Name, FieldType Type, unsigned int FieldLength = 255);
	bool RemoveField(std::string Name);
	bool RemoveField(unsigned int Index);
	Row CreateRow();
	bool RemoveRow(int Index);

};