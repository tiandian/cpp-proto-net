#pragma once

#include "LogManager.h"
#include "Configuration.h"

#define logger (*g_pLogger)

extern CLogManager* g_pLogger;
extern CConfiguration config;
