

#include "../reg_ids.h"
#include "../reg_decls.h"
#include "StringWrap.h"


namespace regs8::type0 {
    static void init(std::vector<rtl::Function>& fns) {

        fns.push_back(rtl::type().record<StrWrap>(cxx::type::StrWrap::id)
                                 .build());

        fns.push_back(rtl::type().member<StrWrap>()
                                 .constructor<const std::string &>().build());

        fns.push_back(rtl::type().member<StrWrap>()
                                 .methodConst()
                                 .build(&StrWrap::sstr));
    }
}


namespace regs8::type1 {
    static void init(std::vector<rtl::Function>& fns) {

        fns.push_back(rtl::type().record<StrWrapA>(cxx::type::StrWrapA::id)
                                 .build());

        fns.push_back(rtl::type().member<StrWrapA>()
                                 .constructor<std::string_view>().build());

        fns.push_back(rtl::type().member<StrWrapA>()
                                 .constructor<std::string &>().build());

        fns.push_back(rtl::type().member<StrWrapA>()
                                 .constructor<const std::string &>().build());

        fns.push_back(rtl::type().member<StrWrapA>()
                                 .constructor<std::string &&>().build());

        fns.push_back(rtl::type().member<StrWrapA>()
                                 .constructor<const char *>().build());
    }
}


namespace regs8::type2 {
    static void init(std::vector<rtl::Function>& fns) {

        fns.push_back(rtl::type().record<StrWrapB>(cxx::type::StrWrapB::id)
                                 .build());

        fns.push_back(rtl::type().member<StrWrapB>()
                                 .constructor<const std::string &>().build());

        fns.push_back(rtl::type().member<StrWrapB>()
                                 .constructor<std::string &>().build());
    }
}


namespace regs8::type3 {
    static void init(std::vector<rtl::Function>& fns) {

        fns.push_back(rtl::type().record<StrWrapC>(cxx::type::StrWrapC::id)
                                 .build());

        fns.push_back(rtl::type().member<StrWrapC>()
                                 .constructor<std::string &>().build());
    }
}


namespace regs8::type4 {
    static void init(std::vector<rtl::Function>& fns) {

        fns.push_back(rtl::type().record<StrWrapD>(cxx::type::StrWrapD::id)
                                 .build());

        fns.push_back(rtl::type().member<StrWrapD>()
                                 .constructor<const std::string &>().build());
    }
}

