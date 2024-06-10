#include "log.h"

extern "C" void __cxa_pure_virtual() {
    mkb::OSPanic(__FILE__, __LINE__, "Pure virtual function call");
    LOG("Tried calling a pure virtual function");
    while (true)
        ;
}

extern "C" void __assert_func(const char* file, int line, const char* func_name, const char* assert_code) {
    mkb::OSPanic(__FILE__, __LINE__, "Assertion %s failed in function %s", func_name, assert_code);
    LOG("Called __assert_func");
    while (true)
        ;
}
