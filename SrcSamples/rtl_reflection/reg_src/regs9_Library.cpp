

#include "../reg_ids.h"
#include "../reg_decls.h"
#include "Library.h"


namespace regs9::type0 {
    static void init(std::vector<rtl::Function>& fns) {

        fns.push_back(rtl::type().record<Library>(cxx::type::Library::id)
                                 .build());

        fns.push_back(rtl::type().member<Library>()
                                 .methodStatic()
                                 .build(&Library::getBookByTitle));

        fns.push_back(rtl::type().member<Library>()
                                 .methodStatic()
                                 .build(&Library::getInstanceCount));

        fns.push_back(rtl::type().member<Library>()
                                 .methodStatic()
                                 .build(&Library::getBooksCount));

        fns.push_back(rtl::type().member<Library>()
                                 .methodStatic()
                                 .build(&Library::addBook));
    }
}

