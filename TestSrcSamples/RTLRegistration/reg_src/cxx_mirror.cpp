

#include <vector>

#include "../reg_decls.h"

namespace cxx { 

    const rtl::CxxMirror& mirror()
    {
        static auto mirror = rtl::CxxMirror([]() {

            std::vector<rtl::Function> fns;

            regs1::fn::init(fns);
            regs0::type0::init(fns);
            regs1::type0::init(fns);
            return fns;
        }());
        return mirror;
    }
}