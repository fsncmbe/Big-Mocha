#pragma once

#include <modules/ecs.hpp>
#include <modules/window.hpp>
#include <modules/resource.hpp>

#ifdef __linux__
  #define LINUX
#endif

#ifdef _WIN32
  #define WINDOWS
#endif

#define CAST_STR(o) std::any_cast<std::string>(o)