extern "C" {
#include "sentry_boot.h"

#include "sentry_symbolizer.h"
}

#include <dlfcn.h>
#include <string.h>
#include <stdlib.h>
#include <cxxabi.h>

bool
sentry__symbolize(
    void *addr, void (*func)(const sentry_frame_info_t *, void *), void *data)
{
    Dl_info info;

    if (dladdr(addr, &info) == 0) {
        return false;
    }

    sentry_frame_info_t frame_info;
    memset(&frame_info, 0, sizeof(sentry_frame_info_t));
    frame_info.load_addr = info.dli_fbase;
    frame_info.symbol_addr = info.dli_saddr;
    frame_info.instruction_addr = addr;
    char *demangled = NULL;
    if (info.dli_sname) {
        int status;
        demangled = abi::__cxa_demangle(info.dli_sname, NULL, NULL, &status);
    }
    frame_info.symbol = demangled ? demangled : info.dli_sname;
    frame_info.object_name = info.dli_fname;
    func(&frame_info, data);
    free(demangled);

    return true;
}
