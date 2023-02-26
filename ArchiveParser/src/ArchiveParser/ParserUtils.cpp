#include "ParserUtils.h"

std::string lower(const std::string& text)
{
	std::string lower = text;

	for (size_t i = 0; i < lower.size(); ++i)
		if (lower[i] >= 'A' && lower[i] <= 'Z')
			lower[i] += 'a' - 'A';

	return lower;
}