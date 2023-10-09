#include "log.h"

extern "C" void __cxa_pure_virtual() {
    mkb::OSPanic(__FILE__, __LINE__, "Pure virtual function call");
    LOG("Tried calling a pure virtual function");
    while (true)
        ;
}
