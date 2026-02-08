

#include "../reg_ids.h"
#include "../reg_decls.h"
#include "StringConst.h"


namespace regs11::type0 {
    static void init(std::vector<rtl::Function>& fns) {

        fns.push_back(rtl::type().record<StrConst>(cxx::type::StrConst::id)
                                 .build());

        fns.push_back(rtl::type().member<StrConst>()
                                 .methodConst<std::string_view>()
                                 .build(&StrConst::revStrOverloadValCRef));

        fns.push_back(rtl::type().member<StrConst>()
                                 .methodConst<const std::string_view &>()
                                 .build(&StrConst::revStrOverloadValCRef));

        fns.push_back(rtl::type().member<StrConst>()
                                 .methodConst<void>()
                                 .build(&StrConst::reverseString));

        fns.push_back(rtl::type().member<StrConst>()
                                 .methodConst<const char *>()
                                 .build(&StrConst::reverseString));

        fns.push_back(rtl::type().member<StrConst>()
                                 .methodConst<std::string>()
                                 .build(&StrConst::reverseString));

        fns.push_back(rtl::type().member<StrConst>()
                                 .methodConst<std::string &>()
                                 .build(&StrConst::reverseString));

        fns.push_back(rtl::type().member<StrConst>()
                                 .methodConst<std::string &&>()
                                 .build(&StrConst::reverseString));

        fns.push_back(rtl::type().member<StrConst>()
                                 .methodConst<const std::string &>()
                                 .build(&StrConst::reverseString));

        fns.push_back(rtl::type().member<StrConst>()
                                 .methodConst<std::string *>()
                                 .build(&StrConst::reverseString));

        fns.push_back(rtl::type().member<StrConst>()
                                 .methodConst<const std::string *>()
                                 .build(&StrConst::reverseString));

        fns.push_back(rtl::type().member<StrConst>()
                                 .methodConst()
                                 .build(&StrConst::revStrConstRefArg));

        fns.push_back(rtl::type().member<StrConst>()
                                 .methodConst()
                                 .build(&StrConst::revStrRValueRefArg));

        fns.push_back(rtl::type().member<StrConst>()
                                 .methodConst()
                                 .build(&StrConst::revStrNonConstRefArg));

        fns.push_back(rtl::type().member<StrConst>()
                                 .methodConst<std::string_view &>()
                                 .build(&StrConst::revStrOverloadRefAndCRef));

        fns.push_back(rtl::type().member<StrConst>()
                                 .methodConst<const std::string_view &>()
                                 .build(&StrConst::revStrOverloadRefAndCRef));

        fns.push_back(rtl::type().member<StrConst>()
                                 .methodConst<std::string_view>()
                                 .build(&StrConst::revStrOverloadValRef));

        fns.push_back(rtl::type().member<StrConst>()
                                 .methodConst<std::string_view &>()
                                 .build(&StrConst::revStrOverloadValRef));
    }
}

