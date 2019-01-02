#pragma once

#include <windows.h>
#include <stdio.h>
#include <iostream>

#define configASSERT(x) if((x)==0) { std::cout << std::endl << "configASSERT @ " << __FUNCTION__ << ":" << __LINE__ << std::endl; std::cin.get(); }
#define ARRAY_SIZE(x)  (sizeof(x) / sizeof(x[0]))
