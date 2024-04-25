#pragma once

#include <string>
#include <vector>
#include <format>

using std::string;
using std::vector;


inline
bool starts_with(const string& str, const string& prefix)
{
	return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}


inline
vector<vector<double>> reshape(const vector<double>& v, size_t n, size_t m)
{
	vector<vector<double>> result(n, vector<double>(m));
	for (size_t i = 0; i < n; ++i)
	{
		for (size_t j = 0; j < m; j++)
		{
			result[i][j] = v[i * m + j];
		}
	}
	return result;
}


inline
string dump_vector(const vector<double>& v, int n)
{
	int i = 0;
	string result;
	for (auto& value : v)
	{
		result += std::format("{:8.3f} ", value);
		i++;
		if (i >= n)
		{
			if (v.size() > i) result += " ...";
			break;
		}
	}
	return result;
}

