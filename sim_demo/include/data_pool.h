#pragma once
#include <string>
#include <vector>

#include "fmtlog.h"

class DataItem
{
public:
	std::string _name;
	char _type;
	void* _pointer;
	unsigned char _length;
	std::string _publisher;
};


class DataPool
{
public:
	std::vector<DataItem> _data_items;

	bool reg_data(const std::string& name, double& v, const std::string& publisher)
	{
		if (find_data(name))
		{
			loge("DataPool::reg_data: duplicate data name: {}, publisher: {}\n", name, publisher);
			return false;
		}
		DataItem item;
		item._name = name;
		item._type = 'd';
		item._pointer = &v;
		item._length = sizeof(double);
		item._publisher = publisher;
		_data_items.push_back(item);
		return true;
	}

	bool reg_data(const std::string& name, int& v, const std::string& publisher)
	{
		if (find_data(name))
		{
			loge("DataPool::reg_data: duplicate data name: {}, publisher: {}\n", name, publisher);
			return false;
		}
		DataItem item;
		item._name = name;
		item._type = 'i';
		item._pointer = &v;
		item._length = sizeof(int);
		item._publisher = publisher;
		_data_items.push_back(item);
		return true;
	}

	bool reg_data(const std::string& name, bool& v, const std::string& publisher)
	{
		if (find_data(name))
		{
			loge("DataPool::reg_data: duplicate data name: {}, publisher: {}\n", name, publisher);
			return false;
		}
		DataItem item;
		item._name = name;
		item._type = '?';
		item._pointer = &v;
		item._length = sizeof(bool);
		item._publisher = publisher;
		_data_items.push_back(item);
		return true;
	}

	DataItem* find_data(const std::string& name)
	{
		for (auto& item : _data_items)
		{
			if (item._name == name)
			{
				return &item;
			}
		}
		return nullptr;
	}

	void dump()
	{
		std::string buffer;
		for (auto& item : _data_items)
		{
			std::format_to(std::back_inserter(buffer), "{}:{}[{}]\n", item._publisher, item._name, item._type);
		}
		logi("DataPool::dump\n{}", buffer);
	}
};


class BinderItem
{
public:
	DataItem* _data_item;
	void* _target_pointer;
};


class DataBinder
{
public:
	std::vector<BinderItem> _binder_items;

	bool bind(DataPool& data_pool, const std::string& name, double& v)
	{
		DataItem* item = data_pool.find_data(name);
		if (!item)
		{
			return false;
		}

		if (item->_type != 'd')
		{
			return false;
		}

		BinderItem binder_item;
		binder_item._data_item = item;
		binder_item._target_pointer = &v;
		_binder_items.push_back(binder_item);

		return true;
	}

	bool bind(DataPool& data_pool, const std::string& name, int& v)
	{
		DataItem* item = data_pool.find_data(name);
		if (!item)
		{
			return false;
		}

		if (item->_type != 'i')
		{
			return false;
		}

		BinderItem binder_item;
		binder_item._data_item = item;
		binder_item._target_pointer = &v;
		_binder_items.push_back(binder_item);

		return true;
	}

	bool bind(DataPool& data_pool, const std::string& name, bool& v)
	{
		DataItem* item = data_pool.find_data(name);
		if (!item)
		{
			return false;
		}

		if (item->_type != '?')
		{
			return false;
		}

		BinderItem binder_item;
		binder_item._data_item = item;
		binder_item._target_pointer = &v;
		_binder_items.push_back(binder_item);

		return true;
	}

	void update()
	{
		for (auto& item : _binder_items)
		{
			memcpy(item._target_pointer, item._data_item->_pointer, item._data_item->_length);
		}
	}
};


#define BIND_DATA(v) \
	if (!_binder.bind(_vehicle->_data_pool, #v, _##v)) \
	{ \
		failed_input += #v ","; \
	}