#pragma once
#include "Math.h"

template<class T>
struct Vec2
{
	Vec2() = default;

	Vec2(const T& x, const T&y);

	template<class B>
	explicit Vec2(const Vec2<B> other);


	Vec2<T>& clamp(const Vec2<T>& min, const Vec2<T>& max);

	Vec2<T> clamped(const Vec2<T>& min, const Vec2<T>& max);

	T x = 0;
	T y = 0;
};


using Vec2i = Vec2<int32_t>;


////////////////////////////////////////////////////////// definitions //////////////////////////////////////////////////////////


template<class T>
inline Vec2<T>::Vec2(const T & x, const T & y) : x(x), y(y){}

template<class T>
template<class B>
inline Vec2<T>::Vec2(const Vec2<B> other) : x(other.x), y(other.y) {}


template<class T>
inline Vec2<T>& Vec2<T>::clamp(const Vec2<T>& min, const Vec2<T>& max)
{
	::clamp(x, min.x, max.x);
	::clamp(y, min.y, max.y);
	return *this;
}

template<class T>
inline Vec2<T> Vec2<T>::clamped(const Vec2<T>& min, const Vec2<T>& max)
{
	return Vec2<T>(*this).clamp(min, max);
}

////////////////////////////////////////////////////////// operators //////////////////////////////////////////////////////////

template<class T>
inline Vec2<T> operator+(const Vec2<T> & left, const Vec2<T> & right)
{
	return Vec2<T>(left.x + right.x, left.y + right.y);
}

template<class T>
inline Vec2<T> operator-(const Vec2<T> & right)
{
	return Vec2<T>(-right.x, -right.y);
}

template<class T>
inline Vec2<T> operator-(const Vec2<T> & left, const Vec2<T> & right)
{
	return left + (-right);
}

template<class T>
inline Vec2<T> operator*(T left, const Vec2<T> & right)
{
	return Vec2<T>(left * right.x, left * right.y);
}

template<class T>
inline Vec2<T> operator*(const Vec2<T> & left, T right)
{
	return right * left;
}

template<class T>
inline Vec2<T>& operator+=(Vec2<T> & left, const Vec2<T> & right)
{
	return left = (left + right);
}

template<class T>
inline Vec2<T>& operator-=(Vec2<T> & left, const Vec2<T> & right)
{
	return left = (left - right);
}

template<class T>
inline bool operator==(const Vec2<T> & left, const Vec2<T> & right)
{
	return left.x == right.x && left.y == right.y;
}

template<class T>
inline bool operator!=(const Vec2<T> & left, const Vec2<T> & right)
{
	return !(left == right);
}