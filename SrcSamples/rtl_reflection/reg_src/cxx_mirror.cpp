

#include <vector>

#include "../reg_decls.h"

namespace cxx { 

    const rtl::CxxMirror& mirror()
    {
        static auto mirror = rtl::CxxMirror([]() {

            std::vector<rtl::Function> fns;

            regs7::fn::init(fns);
            regs10::fn::init(fns);
            regs1::type0::init(fns);
            regs1::type1::init(fns);
            regs1::type2::init(fns);
            regs0::type0::init(fns);
            regs3::type0::init(fns);
            regs2::type0::init(fns);
            regs4::type0::init(fns);
            regs5::type0::init(fns);
            regs6::type0::init(fns);
            regs8::type0::init(fns);
            regs8::type1::init(fns);
            regs8::type2::init(fns);
            regs8::type3::init(fns);
            regs8::type4::init(fns);
            regs9::type0::init(fns);
            regs11::type0::init(fns);

            return fns;

        }());
        return mirror;
    }
}