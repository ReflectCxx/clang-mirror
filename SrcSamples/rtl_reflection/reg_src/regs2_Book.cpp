

#include "../reg_ids.h"
#include "../reg_decls.h"
#include "Book.h"


namespace regs2::type0 {
    static void init(std::vector<rtl::Function>& fns) {

        fns.push_back(rtl::type().record<Book>(cxx::type::Book::id)
                                 .build());

        fns.push_back(rtl::type().member<Book>()
                                 .constructor<double, std::string>().build());

        fns.push_back(rtl::type().member<Book>()
                                 .method()
                                 .build(&Book::setAuthor));

        fns.push_back(rtl::type().member<Book>()
                                 .methodConst()
                                 .build(&Book::getTitle));

        fns.push_back(rtl::type().member<Book>()
                                 .method()
                                 .build(&Book::addCopyrightTag));

        fns.push_back(rtl::type().member<Book>()
                                 .method()
                                 .build(&Book::setDescription));

        fns.push_back(rtl::type().member<Book>()
                                 .method()
                                 .build(&Book::getPublishedOn));

        fns.push_back(rtl::type().member<Book>()
                                 .methodStatic()
                                 .build(&Book::getInstanceCount));

        fns.push_back(rtl::type().member<Book>()
                                 .method<void>()
                                 .build(&Book::updateBookInfo));

        fns.push_back(rtl::type().member<Book>()
                                 .method<const char *, double, std::string>()
                                 .build(&Book::updateBookInfo));

        fns.push_back(rtl::type().member<Book>()
                                 .method<std::string, double, const char *>()
                                 .build(&Book::updateBookInfo));

        fns.push_back(rtl::type().member<Book>()
                                 .method()
                                 .build(&Book::addPreface));
    }
}

