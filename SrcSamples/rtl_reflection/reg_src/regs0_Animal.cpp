

#include "../reg_ids.h"
#include "../reg_decls.h"
#include "Animal.h"


namespace regs0::type0 {
    static void init(std::vector<rtl::Function>& fns) {

        fns.push_back(rtl::type().record<Animal>(cxx::type::Animal::id)
                                 .build());

        fns.push_back(rtl::type().member<Animal>()
                                 .constructor<const std::string &>().build());

        fns.push_back(rtl::type().member<Animal>()
                                 .methodConst()
                                 .build(&Animal::getFamilyName));

        fns.push_back(rtl::type().member<Animal>()
                                 .method<std::string &>()
                                 .build(&Animal::setAnimalName));

        fns.push_back(rtl::type().member<Animal>()
                                 .method<std::string &&>()
                                 .build(&Animal::setAnimalName));

        fns.push_back(rtl::type().member<Animal>()
                                 .method<const std::string &>()
                                 .build(&Animal::setAnimalName));

        fns.push_back(rtl::type().member<Animal>()
                                 .methodConst<const std::string &>()
                                 .build(&Animal::setAnimalName));

        fns.push_back(rtl::type().member<Animal>()
                                 .method()
                                 .build(&Animal::setFamilyName));

        fns.push_back(rtl::type().member<Animal>()
                                 .methodStatic()
                                 .build(&Animal::getInstanceCount));

        fns.push_back(rtl::type().member<Animal>()
                                 .methodStatic<std::string &>()
                                 .build(&Animal::updateZooKeeper));

        fns.push_back(rtl::type().member<Animal>()
                                 .methodStatic<std::string &&>()
                                 .build(&Animal::updateZooKeeper));

        fns.push_back(rtl::type().member<Animal>()
                                 .methodStatic<const std::string &>()
                                 .build(&Animal::updateZooKeeper));
    }
}

