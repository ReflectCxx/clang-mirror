

#include "../reg_ids.h"
#include "../reg_decls.h"
#include "Person.h"


namespace regs3::type0 {
    static void init(std::vector<rtl::Function>& fns) {

        fns.push_back(rtl::type().record<Person>(cxx::type::Person::id)
                                 .build());

        fns.push_back(rtl::type().member<Person>()
                                 .constructor<const std::string &>().build());

        fns.push_back(rtl::type().member<Person>()
                                 .methodStatic()
                                 .build(&Person::getInstanceCount));

        fns.push_back(rtl::type().member<Person>()
                                 .methodStatic()
                                 .build(&Person::createConst));

        fns.push_back(rtl::type().member<Person>()
                                 .method<void>()
                                 .build(&Person::updateAddress));

        fns.push_back(rtl::type().member<Person>()
                                 .methodConst<void>()
                                 .build(&Person::updateAddress));

        fns.push_back(rtl::type().member<Person>()
                                 .method<std::string>()
                                 .build(&Person::updateAddress));

        fns.push_back(rtl::type().member<Person>()
                                 .methodConst<std::string>()
                                 .build(&Person::updateAddress));

        fns.push_back(rtl::type().member<Person>()
                                 .methodStatic<void>()
                                 .build(&Person::getProfile));

        fns.push_back(rtl::type().member<Person>()
                                 .methodStatic<std::string, unsigned long long>()
                                 .build(&Person::getProfile));

        fns.push_back(rtl::type().member<Person>()
                                 .methodStatic<bool>()
                                 .build(&Person::getProfile));

        fns.push_back(rtl::type().member<Person>()
                                 .method()
                                 .build(&Person::getFirstName));

        fns.push_back(rtl::type().member<Person>()
                                 .methodStatic()
                                 .build(&Person::createPtr));

        fns.push_back(rtl::type().member<Person>()
                                 .methodStatic()
                                 .build(&Person::getDefaults));

        fns.push_back(rtl::type().member<Person>()
                                 .methodStatic()
                                 .build(&Person::deletePtr));

        fns.push_back(rtl::type().member<Person>()
                                 .methodConst()
                                 .build(&Person::updateLastName));
    }
}

