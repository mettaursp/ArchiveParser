#pragma once

#include <string>

std::string lower(const std::string& text);

template <typename T>
const T* streamOf(const void* stream)
{
	return reinterpret_cast<const T*>(stream);
}

template <typename T>
T* streamOf(void* stream)
{
	return reinterpret_cast<T*>(stream);
}