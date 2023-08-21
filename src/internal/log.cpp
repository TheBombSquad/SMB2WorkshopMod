#include "log.h"

extern "C" void __cxa_pure_virtual() {
    mkb::OSPanic(__FILE__, __LINE__, "Pure virtual function call");
    mkb::OSReport("[wsmod] Tried calling a pure virtual function\n");
    while (true)
        ;
}
