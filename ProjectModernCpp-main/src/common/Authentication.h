#pragma once

#ifdef COMMON_EXPORTS
	#define COMMON_API __declspec(dllexport)
#else
	#define COMMON_API __declspec(dllimport)
#endif

namespace Authentication
{
	COMMON_API bool checkPassword(const char*) noexcept;
	COMMON_API bool checkEmail(const char*) noexcept;
}
