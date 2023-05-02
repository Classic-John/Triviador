#include "Authentication.h"
#include <regex>


bool Authentication::checkPassword(const char* password) noexcept
{
	//this regular expression checks whether the password is at least 6 characters long, contains at least one 
	//lowercase letter, one uppercase letter, one number and one special character
	std::regex correctPassword("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[^\\da-zA-Z]).{6,}$");
	return std::regex_match(password, correctPassword);
}

bool Authentication::checkEmail(const char* email) noexcept
{
	std::regex correctEmail("^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\\.[a-zA-Z0-9-.]+$");
	return std::regex_match(email, correctEmail);
}