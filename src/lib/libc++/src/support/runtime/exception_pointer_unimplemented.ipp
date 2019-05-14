// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <stdio.h>
#include <stdlib.h>

#include "../../debug.h"

namespace std {

exception_ptr::~exception_ptr() _NOEXCEPT
{
	PANIC("exception_ptr not yet implemented\n");
  ::abort();
}

exception_ptr::exception_ptr(const exception_ptr& other) _NOEXCEPT
    : __ptr_(other.__ptr_)
{
	PANIC("exception_ptr not yet implemented\n");
  ::abort();
}

exception_ptr& exception_ptr::operator=(const exception_ptr& other) _NOEXCEPT
{
	(void)other;
	PANIC("exception_ptr not yet implemented\n");
  ::abort();
}

nested_exception::nested_exception() _NOEXCEPT
    : __ptr_(current_exception())
{
}

#if !defined(__GLIBCXX__)

nested_exception::~nested_exception() _NOEXCEPT
{
}

#endif

_LIBCPP_NORETURN
void
nested_exception::rethrow_nested() const
{
	PANIC("exception_ptr not yet implemented\n");
  ::abort();
#if 0
  if (__ptr_ == nullptr)
      terminate();
  rethrow_exception(__ptr_);
#endif // FIXME
}

exception_ptr current_exception() _NOEXCEPT
{
  PANIC("exception_ptr not yet implemented\n");
  ::abort();
}

_LIBCPP_NORETURN
void rethrow_exception(exception_ptr p)
{
	(void)p;
	PANIC("exception_ptr not yet implemented\n");
  ::abort();
}

} // namespace std
