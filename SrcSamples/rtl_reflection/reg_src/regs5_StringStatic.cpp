

#include "../reg_ids.h"
#include "../reg_decls.h"
#include "StringStatic.h"


namespace regs5::type0 {
    static void init(std::vector<rtl::Function>& fns) {

        fns.push_back(rtl::type().record<StrStatic>(cxx::type::StrStatic::id)
                                 .build());

        fns.push_back(rtl::type().member<StrStatic>()
                                 .methodStatic<std::string_view>()
                                 .build(&StrStatic::revStrOverloadValCRef));

        fns.push_back(rtl::type().member<StrStatic>()
                                 .methodStatic<const std::string_view &>()
                                 .build(&StrStatic::revStrOverloadValCRef));

        fns.push_back(rtl::type().member<StrStatic>()
                                 .methodStatic<void>()
                                 .build(&StrStatic::reverseString));

        fns.push_back(rtl::type().member<StrStatic>()
                                 .methodStatic<const char *>()
                                 .build(&StrStatic::reverseString));

        fns.push_back(rtl::type().member<StrStatic>()
                                 .methodStatic<std::string>()
                                 .build(&StrStatic::reverseString));

        fns.push_back(rtl::type().member<StrStatic>()
                                 .methodStatic<std::string &>()
                                 .build(&StrStatic::reverseString));

        fns.push_back(rtl::type().member<StrStatic>()
                                 .methodStatic<std::string &&>()
                                 .build(&StrStatic::reverseString));

        fns.push_back(rtl::type().member<StrStatic>()
                                 .methodStatic<const std::string &>()
                                 .build(&StrStatic::reverseString));

        fns.push_back(rtl::type().member<StrStatic>()
                                 .methodStatic<std::string *>()
                                 .build(&StrStatic::reverseString));

        fns.push_back(rtl::type().member<StrStatic>()
                                 .methodStatic<const std::string *>()
                                 .build(&StrStatic::reverseString));

        fns.push_back(rtl::type().member<StrStatic>()
                                 .methodStatic()
                                 .build(&StrStatic::revStrConstRefArg));

        fns.push_back(rtl::type().member<StrStatic>()
                                 .methodStatic()
                                 .build(&StrStatic::revStrRValueRefArg));

        fns.push_back(rtl::type().member<StrStatic>()
                                 .methodStatic()
                                 .build(&StrStatic::revStrNonConstRefArg));

        fns.push_back(rtl::type().member<StrStatic>()
                                 .methodStatic<std::string_view &>()
                                 .build(&StrStatic::revStrOverloadRefAndCRef));

        fns.push_back(rtl::type().member<StrStatic>()
                                 .methodStatic<const std::string_view &>()
                                 .build(&StrStatic::revStrOverloadRefAndCRef));

        fns.push_back(rtl::type().member<StrStatic>()
                                 .methodStatic<std::string_view>()
                                 .build(&StrStatic::revStrOverloadValRef));

        fns.push_back(rtl::type().member<StrStatic>()
                                 .methodStatic<std::string_view &>()
                                 .build(&StrStatic::revStrOverloadValRef));
    }
}

