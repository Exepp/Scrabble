#pragma once
#include <stdint.h>
#include <math.h>

template<class T>
constexpr inline T maxVal(const T & val1, const T & val2)
{
	return val1 > val2 ? val1 : val2;
}

template<class T>
constexpr inline T minVal(const T & val1, const T & val2)
{
	return val1 < val2 ? val1 : val2;
}

template<class T>
inline T clamped(const T & value, const T& min, const T& max)
{
	return minVal(maxVal(value, min), max);
}

template<class T>
inline T& clamp(T& value, const T& min, const T& max)
{
	return value = clamped(value, min, max);
}

template<class T>
inline void swap(T& value1, T& value2)
{
	T temp = value1;
	value1 = value2;
	value2 = temp;
}