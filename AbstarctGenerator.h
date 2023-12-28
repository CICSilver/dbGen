#pragma once
#include <string>
class AbstractGenerator
{
public:
	virtual std::string generat() = 0;
	virtual ~AbstractGenerator() = default;

private:
	bool isUsingQt;
};
