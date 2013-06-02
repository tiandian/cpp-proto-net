#pragma once

#include <exception>

class CUnexpectedSideException : public std::exception
{
public:
	virtual const char* what() const throw()
	{
		return "unexpected leg side";
	}
};

class CUnexpectedPositionDirectionException : public std::exception
{
public:
	virtual const char* what() const throw()
	{
		return "unexpected position direction";
	}
};