#include "data_recorder.h"


void DataRecorder::init(const std::string& filename)
{
	if (filename.empty())
	{
		return;
	}
	_file = fopen(filename.c_str(), "wt");
	fprintf(_file, "t");
}


void DataRecorder::reg(const std::string& name, double& value)
{
	if (!_file)
	{
		return;
	}
	RecorderItem item;
	item._name = name;
	item._type = 'd';
	item._value_pointer = &value;
	_items.push_back(item);

	fprintf(_file, ",%s", name.c_str());
}


void DataRecorder::reg(const std::string& name, int& value)
{
	if (!_file)
	{
		return;
	}
	RecorderItem item;
	item._name = name;
	item._type = 'i';
	item._value_pointer = &value;
	_items.push_back(item);

	fprintf(_file, ",%s", name.c_str());
}


void DataRecorder::update(double t)
{
	if (!_file)
	{
		return;
	}
	fprintf(_file, "\n%g", t);
	for (const auto& item : _items)
	{
		if (item._type == 'd')
		{
			fprintf(_file, ",%g", *(double*)item._value_pointer);
		}
		else if (item._type == 'i')
		{
			fprintf(_file, ",%d", *(int*)item._value_pointer);
		}
	}
	fflush(_file);
}
