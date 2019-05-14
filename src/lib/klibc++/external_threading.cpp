
#include "include/__external_threading"

#include "../../debug.h"

bool __libcpp_mutex_trylock(__libcpp_mutex_t* __m)
{
	(void)__m;

	PANIC("__libcpp_mutex_trylock not implemented");

	return false;
}

bool __libcpp_mutex_lock(__libcpp_mutex_t* __m)
{
	(void)__m;

	PANIC("__libcpp_mutex_lock not implemented");

	return false;
}

bool __libcpp_mutex_unlock(__libcpp_mutex_t* __m)
{
	(void)__m;

	PANIC("__libcpp_mutex_unlock not implemented");

	return false;
}

void __libcpp_thread_yield(void)
{
}