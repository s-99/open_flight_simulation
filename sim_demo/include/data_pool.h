#pragma once
#include <string>
#include <vector>

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

	void reg_data(const std::string& name, double& v, const std::string& publisher)
	{
		DataItem item;
		item._name = name;
		item._type = 'd';
		item._pointer = &v;
		item._length = sizeof(double);
		item._publisher = publisher;
		_data_items.push_back(item);
	}

	void reg_data(const std::string& name, int& v, const std::string& publisher)
	{
		DataItem item;
		item._name = name;
		item._type = 'i';
		item._pointer = &v;
		item._length = sizeof(int);
		item._publisher = publisher;
		_data_items.push_back(item);
	}

	void reg_data(const std::string& name, bool& v, const std::string& publisher)
	{
		DataItem item;
		item._name = name;
		item._type = '?';
		item._pointer = &v;
		item._length = sizeof(bool);
		item._publisher = publisher;
		_data_items.push_back(item);
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
		printf("bind %s failed\n", #v); \
		return false; \
	}