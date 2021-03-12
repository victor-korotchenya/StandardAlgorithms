#pragma once

// todo: del it.

#define DISABLE_ALL_CONSTRUCTORS(Class) \
  Class() = delete; \
  Class(Class&) = delete; \
  Class(Class&&) = delete; \
  Class &operator =(Class&) = delete; \
  Class &operator =(Class&&) = delete;