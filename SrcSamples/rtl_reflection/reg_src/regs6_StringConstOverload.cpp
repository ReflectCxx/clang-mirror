

#include "../reg_ids.h"
#include "../reg_decls.h"
#include "StringConstOverload.h"


namespace regs6::type0 {
    static void init(std::vector<rtl::Function>& fns) {

        fns.push_back(rtl::type().record<StrConstOverload>(cxx::type::StrConstOverload::id)
                                 .build());

        fns.push_back(rtl::type().member<StrConstOverload>()
                                 .method<void>()
                                 .build(&StrConstOverload::reverseString));

        fns.push_back(rtl::type().member<StrConstOverload>()
                                 .methodConst<void>()
                                 .build(&StrConstOverload::reverseString));
    }
}

