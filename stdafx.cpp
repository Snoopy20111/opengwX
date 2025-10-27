// stdafx.cpp : source file that includes just the standard includes
// OpenGW.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

// Seems like a good enough place to put in solution-wide hacks, right?

// stub for CxxFrameHandler3 errors, because neither RXDK nor NXDK support exceptions
extern "C" void cdecl __CxxFrameHandler3() {}

// Broken features in RXDK:
// std::atomic
// std::mutex
// std::shared_ptr