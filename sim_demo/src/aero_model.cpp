#include "aero_model.h"
#include "util.h"
#include "extrasrc/fpaux.hh"

#include <iostream>
#include <fstream>
#include <format>


bool Table::parse(const string& name, const json::object_t& content)
{
	_name = name;
	_dim = static_cast<int>(content.size()) - 1;
	vector<double> v;
	for (auto& [key, value] : content)
	{
		if (starts_with(key, "x0:"))
		{
			_x0_name = key.substr(3);
			try
			{
				_x0 = value.get<vector<double>>();
			}
			catch (const std::exception& e)
			{
				std::cout << "Table \"" << _name << "\" x0 parse failed: " << e.what() << std::endl;
				return false;
			}
		}
		else if (starts_with(key, "x1:"))
		{
			_x1_name = key.substr(3);
			try
			{
				_x1 = value.get<vector<double>>();
			}
			catch (const std::exception& e)
			{
				std::cout << "Table \"" << _name << "\" x1 parse failed: " << e.what() << std::endl;
				return false;
			}
		}
		else if (key == "data")
		{
			try
			{
				v = value.get<vector<double>>();
			}
			catch (const std::exception& e)
			{
				std::cout << "Table \"" << _name << "\" data parse failed: " << e.what() << std::endl;
				return false;
			}
		}
	}
	if (_dim == 2)
	{
		if (v.size() != _x0.size() * _x1.size())
		{
			std::cout << "Table \"" << _name << "\" data size mismatch: " << v.size() << " != " << _x0.size() * _x1.size() << std::endl;
			return false;
		}
		_value = reshape(v, _x1.size(), _x0.size());
	}
	else
	{
		if (v.size() != _x0.size())
		{
			std::cout << "Table \"" << _name << "\" data size mismatch: " << v.size() << " != " << _x0.size() << std::endl;
			return false;
		}
		_value = reshape(v, 1, _x0.size());
	}

	return true;
}


string Table::dump(int n) const
{
	string result = std::format("{} [dim={:d}]:\n", _name, _dim);
	result += std::format("{:8s} [{:2d}]: {}\n", _x0_name, _x0.size(), dump_vector(_x0, n));
	if (_dim == 2)
	{
		result += std::format("{:8s} [{:2d}]: {}\n", _x1_name, _x1.size(), dump_vector(_x1, n));
		result += std::format("data     [{:2d}][{:2d}]:\n", _value.size(), _value[0].size());
		for (auto& row : _value)
		{
			result += dump_vector(row, n) + "\n";
		}
	}
	else
	{
		result += std::format("data     [{:2d}]: ", _value[0].size());
		result += dump_vector(_value[0], n) + "\n";
	}
	return result;
}


bool CellTable::parse()
{
	if (auto* table = _model->find_table(_content); !table)
	{
		std::cout << "Table not found: " << _content << std::endl;
		return false;
	}
	else
	{
		_table = table;
	}

	
	if (auto* c = _model->find_cell(_table->_x0_name); !c)
	{
		std::cout << "parse \"" << _name << "\" failed, x0 not found: " << _table->_x0_name << std::endl;
		return false;
	}
	else
	{
		_refs.push_back(c);
	}

	if (_table->_dim == 2)
	{
		_interpolator = new Interpolator2DLinear(_table->_x0, _table->_x1, _table->_value);
		if (auto* c = _model->find_cell(_table->_x1_name); !c)
		{
			std::cout << "parse \"" << _name << "\" failed, x1 not found: " << _table->_x1_name << std::endl;
			return false;
		}
		else
		{
			_refs.push_back(c);
		}
	}
	else
	{
		_interpolator = new InterpolatorLinear(_table->_x0, _table->_value[0]);
	}
	
	return true;
}


string CellTable::dump() const
{
	string result;
	result += "CellTable: " + _name + ":" + _content;
	return result;
}


void CellTable::eval()
{
	if (_table->_dim == 2)
	{
		auto& x0 = _refs[0]->_value;
		auto& x1 = _refs[1]->_value;
		_value = dynamic_cast<Interpolator2DLinear*>(_interpolator)->eval(x0, x1);
	}
	else
	{
		auto& x0 = _refs[0]->_value;
		_value = dynamic_cast<InterpolatorLinear*>(_interpolator)->eval(x0);
	}
}


double d2r(const double* p)
{
	return p[0] * 3.1415926535897932384 / 180.0;
}


bool CellFormula::parse()
{
	_parser = new FunctionParser();
	_parser->AddFunction("d2r", d2r, 1);

	vector<string> variables;
	// "0.5 * rho * velocity * velocity"
	int r = _parser->ParseAndDeduceVariables(_content, variables);
	if (r != -1)
	{
		std::cout << "Parse error: content=" << _content << std::endl;
		return false;
	}

	// variables: "rho", "velocity"
	for (auto& v : variables)
	{
		if (auto* c = _model->find_cell(v); !c)
		{
			std::cout << "parse \"" << _name << "\" failed, variable not found: " << v << std::endl;
			return false;
		}
		else
		{
			_refs.push_back(c);
		}
	}

	return true;	
}


string CellFormula::dump() const
{
	string result;
	result += "CellFormula: " + _name + ":" + _content;
	return result;
}


void CellFormula::eval()
{
	vector<double> values;
	for (const auto* c : _refs)
	{
		values.push_back(c->_value);
	}
	_value = _parser->Eval(values.data());
}


bool CellInput::parse()
{
	_value = std::stod(_content);
	return true;
}


string CellInput::dump() const
{
	string result;
	result += "CellInput: " + _name + ":" + _content;
	return result;
}


bool AeroModel::parse(const string& filename)
{
	std::ifstream f(filename);
	json data;
	try
	{
		data = json::parse(f);
	}
	catch (json::parse_error& e)
	{
		std::cout << "Parse error: " << e.what() << std::endl;
		return false;
	}
	auto table = data["table"];
	for (auto& [key, value] : table.items())
	{
		auto* t = _tables.emplace_back(new Table);
		if (auto r = t->parse(key, value); !r)
			return false;
	}

	auto model = data["model"];
	for (auto it = model.begin(); it != model.end(); ++it)
	{
		if (starts_with(it.value(), "#"))
		{
			_cells.emplace_back(new CellTable(this, it.key(), it.value()));
		}
		else
		{
			_cells.emplace_back(new CellFormula(this, it.key(), it.value()));
		}
	}

	auto input = data["input"];
	for (auto& [key, value] : input.items())
	{
		_inputs.emplace_back(new CellInput(this, key, value));
	}

	for (auto& cell : _cells)
	{
		if (!cell->parse())
			return false;
	}

	return true;
}


string AeroModel::dump() const
{
	string result;
	for (const auto* table : _tables)
	{
		result += table->dump(3) + "\n";
	}
	for (const auto* cell : _cells)
	{
		result += cell->dump() + "\n";
	}
	return result;
}


string AeroModel::dump_value() const
{
	string result;
	for (const auto* cell : _cells)
	{
		result += cell->_name + "=" + std::to_string(cell->_value) + "\n";
	}
	return result;
}


void AeroModel::eval()
{
	for (auto* cell : _cells)
	{
		cell->eval();
	}
}