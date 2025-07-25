#pragma once

#include <common/basics.hpp>
#include <modules/ecs.hpp>
#include <modules/window.hpp>
#include <modules/resource.hpp>

#ifdef __linux__
  #define LINUX
#endif

#ifdef _WIN32
  #define WINDOWS
#endif

#define CAST_TEXTFILE(o) std::any_cast<mocha::Textfile>(o)