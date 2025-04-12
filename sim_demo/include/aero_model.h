#pragma once

#include <string>
#include <vector>

#include "fparser.hh"
#include "json.hpp"
#include "interpolate.h"

using json = nlohmann::json;
using std::string;
using std::vector;
using std::map;


class Table
{
public:
	string _name;
	int _dim;
	string _x0_name;
	string _x1_name;
	vector<double> _x0;
	vector<double> _x1;
	vector<vector<double>> _value;

	Table() = default;
	bool parse(const string& name, const json::object_t& content);
	string dump(int n = 5) const;
};


class AeroModel;


class Cell
{
public:
	string _name;
	string _content;
	AeroModel* _model = nullptr;
	vector<Cell*> _refs;
	double _value = 0.0;

	virtual ~Cell() = default;
	virtual bool parse() = 0;
	virtual string dump() const = 0;
	virtual void eval() = 0;
};


class CellTable : public Cell
{
public:
	Table* _table = nullptr;
	Interpolator* _interpolator = nullptr;
	CellTable(AeroModel* model, const std::string& name, const std::string& content)
	{
		_model = model;
		_name = name;
		_content = content.substr(1);
	}
	bool parse() override;
	string dump() const override;
	void eval() override;
};


class CellFormula : public Cell
{
public:
	FunctionParser* _parser = nullptr;

	CellFormula(AeroModel* model, const std::string& name, const std::string& content)
	{
		_model = model;
		_name = name;
		_content = content;
	}
	bool parse() override;
	string dump() const override;
	void eval() override;
};


class CellInput : public Cell
{
public:
	CellInput(AeroModel* model, const std::string& name, const double value)
	{
		_name = name;
		_value = value;
		_content = std::to_string(value);
	}
	bool parse() override;
	string dump() const override;
	void eval() override
	{
		// do nothing
	}
};


class AeroModel
{
public:
	vector<Table*> _tables;
	vector<Cell*> _cells;
	vector<Cell*> _inputs;

	~AeroModel()
	{
		for (const auto* table : _tables) delete table;
		for (const auto* cell : _cells) delete cell;
		for (const auto* cell : _inputs) delete cell;
	}

	Table* find_table(const string& name) const
	{
		for (auto* table : _tables)
			if (table->_name == name) 
				return table; 
		return nullptr;
	}

	Cell* find_cell(const string& name) const
	{
		for (auto* cell : _inputs) 
			if (cell->_name == name) 
				return cell;
		for (auto* cell : _cells)
			if (cell->_name == name)
				return cell; 
		return nullptr;
	}

	bool parse(const string& filename);
	bool parse(const json& data);
	string dump() const;
	string dump_value() const;
	void eval();

	bool set_input(const string& name, double value)
	{
		if (auto* cell = find_cell(name))
		{
			cell->_value = value;
		}
		else
		{
			return false;
		}
		return true;
	}

	double get_variable(const string& name) const
	{
		if (const auto* cell = find_cell(name))
		{
			return cell->_value;
		}
		else
		{
			return 0.0;
		}
	}
};

