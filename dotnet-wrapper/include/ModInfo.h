#ifndef BASE_MOD_MODINFO_H
#define BASE_MOD_MODINFO_H

#include <string>

#include "util/Reflection.h"

#include "ClrHost.h"

static ClrHost *clrHost = nullptr;

#define GLOBAL extern "C" __declspec(dllexport)

GLOBAL std::string ModName;
GLOBAL std::string ModVersion;
GLOBAL std::string ModDescription;
GLOBAL std::string ModAuthors;

#endif //BASE_MOD_MODINFO_H
