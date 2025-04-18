#pragma once

#include <string>
#include <vector>
#include <cstdio>

class RecorderItem
{
public:
	std::string _name;
	char _type;
	void* _value_pointer;
};

class DataRecorder
{
public:
	DataRecorder() = default;
	~DataRecorder()
	{
		if (_file)
		{
			fclose(_file);
		}
	}
	void init(const std::string& filename);
	void reg(const std::string& name, double& value);
	void reg(const std::string& name, int& value);
	void update(double t);

protected:
	std::vector<RecorderItem> _items;
	FILE* _file = nullptr;
};