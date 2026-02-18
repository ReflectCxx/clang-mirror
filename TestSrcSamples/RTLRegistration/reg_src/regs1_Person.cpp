

#include "../reg_ids.h"
#include "../reg_decls.h"
#include "Person.h"


namespace regs1::type0 {
    void init(std::vector<rtl::Function>& fns) {

        fns.push_back(rtl::type().record<Person>(cxx::type::Person::id)
                                 .build());

        fns.push_back(rtl::type().member<Person>()
                                 .methodStatic(cxx::type::Person::fn::getAccessCard::id)
                                 .build(&Person::getAccessCard));

        fns.push_back(rtl::type().member<Person>()
                                 .method(cxx::type::Person::fn::setLastName::id)
                                 .build(&Person::setLastName));

        fns.push_back(rtl::type().member<Person>()
                                 .method(cxx::type::Person::fn::setAddress::id)
                                 .build(&Person::setAddress));

        fns.push_back(rtl::type().member<Person>()
                                 .methodConst(cxx::type::Person::fn::getFirstName::id)
                                 .build(&Person::getFirstName));

        fns.push_back(rtl::type().member<Person>()
                                 .constructor<const std::string_view, const std::string_view>().build());
    }
}

