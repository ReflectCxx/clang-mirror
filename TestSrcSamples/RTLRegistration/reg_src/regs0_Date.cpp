

#include "../reg_ids.h"
#include "../reg_decls.h"
#include "Date.h"


namespace regs0::fn {
    void init(std::vector<rtl::Function>& fns) {

        fns.push_back(rtl::type().function<nsdate::Date>(cxx::fn::nsdate::getDateAsString::id)
                                 .build(&nsdate::getDateAsString));

        fns.push_back(rtl::type().function<unsigned int, unsigned int, unsigned int>(cxx::fn::nsdate::getDateAsString::id)
                                 .build(&nsdate::getDateAsString));
    }
}


namespace regs0::type0 {
    void init(std::vector<rtl::Function>& fns) {

        fns.push_back(rtl::type().record<nsdate::Date>(cxx::type::nsdate::Date::id)
                                 .build());

        fns.push_back(rtl::type().member<nsdate::Date>()
                                 .method(cxx::type::nsdate::Date::fn::set::id)
                                 .build(&nsdate::Date::set));
    }
}

