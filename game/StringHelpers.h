#pragma once

using namespace std;

template <typename T>
string toString(const T& value)
{
	stringstream stream;
	stream << value;
	return stream.str();
}