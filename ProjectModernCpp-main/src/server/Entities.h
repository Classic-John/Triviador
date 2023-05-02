#pragma once

#include <string>
#include <stdint.h>

struct UserEntity
{
	uint32_t userId;
	std::string userName;
	std::string password;
	std::string email;
};