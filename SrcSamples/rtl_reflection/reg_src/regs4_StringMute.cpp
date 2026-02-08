

#include "../reg_ids.h"
#include "../reg_decls.h"
#include "StringMute.h"


namespace regs4::type0 {
    static void init(std::vector<rtl::Function>& fns) {

        fns.push_back(rtl::type().record<StrMute>(cxx::type::StrMute::id)
                                 .build());

        fns.push_back(rtl::type().member<StrMute>()
                                 .method<std::string_view>()
                                 .build(&StrMute::revStrOverloadValCRef));

        fns.push_back(rtl::type().member<StrMute>()
                                 .method<const std::string_view &>()
                                 .build(&StrMute::revStrOverloadValCRef));

        fns.push_back(rtl::type().member<StrMute>()
                                 .method<void>()
                                 .build(&StrMute::reverseString));

        fns.push_back(rtl::type().member<StrMute>()
                                 .method<const char *>()
                                 .build(&StrMute::reverseString));

        fns.push_back(rtl::type().member<StrMute>()
                                 .method<std::string>()
                                 .build(&StrMute::reverseString));

        fns.push_back(rtl::type().member<StrMute>()
                                 .method<std::string &>()
                                 .build(&StrMute::reverseString));

        fns.push_back(rtl::type().member<StrMute>()
                                 .method<std::string &&>()
                                 .build(&StrMute::reverseString));

        fns.push_back(rtl::type().member<StrMute>()
                                 .method<const std::string &>()
                                 .build(&StrMute::reverseString));

        fns.push_back(rtl::type().member<StrMute>()
                                 .method<std::string *>()
                                 .build(&StrMute::reverseString));

        fns.push_back(rtl::type().member<StrMute>()
                                 .method<const std::string *>()
                                 .build(&StrMute::reverseString));

        fns.push_back(rtl::type().member<StrMute>()
                                 .method()
                                 .build(&StrMute::revStrConstRefArg));

        fns.push_back(rtl::type().member<StrMute>()
                                 .method()
                                 .build(&StrMute::revStrRValueRefArg));

        fns.push_back(rtl::type().member<StrMute>()
                                 .method()
                                 .build(&StrMute::revStrNonConstRefArg));

        fns.push_back(rtl::type().member<StrMute>()
                                 .method<std::string_view &>()
                                 .build(&StrMute::revStrOverloadRefAndCRef));

        fns.push_back(rtl::type().member<StrMute>()
                                 .method<const std::string_view &>()
                                 .build(&StrMute::revStrOverloadRefAndCRef));

        fns.push_back(rtl::type().member<StrMute>()
                                 .method<std::string_view>()
                                 .build(&StrMute::revStrOverloadValRef));

        fns.push_back(rtl::type().member<StrMute>()
                                 .method<std::string_view &>()
                                 .build(&StrMute::revStrOverloadValRef));
    }
}

