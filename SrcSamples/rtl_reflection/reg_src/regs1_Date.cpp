

#include "../reg_ids.h"
#include "../reg_decls.h"
#include "Calender.h"
#include "Event.h"
#include "Date.h"


namespace regs1::type0 {
    static void init(std::vector<rtl::Function>& fns) {

        fns.push_back(rtl::type().record<nsdate::Calender>(cxx::type::nsdate::Calender::id)
                                 .build());

        fns.push_back(rtl::type().member<nsdate::Calender>()
                                 .methodStatic()
                                 .build(&nsdate::Calender::resetMoveOpsCounter));

        fns.push_back(rtl::type().member<nsdate::Calender>()
                                 .methodStatic()
                                 .build(&nsdate::Calender::getMoveOpsCount));

        fns.push_back(rtl::type().member<nsdate::Calender>()
                                 .methodStatic()
                                 .build(&nsdate::Calender::create));

        fns.push_back(rtl::type().member<nsdate::Calender>()
                                 .method()
                                 .build(&nsdate::Calender::getSavedDate));

        fns.push_back(rtl::type().member<nsdate::Calender>()
                                 .method()
                                 .build(&nsdate::Calender::getTheDate));

        fns.push_back(rtl::type().member<nsdate::Calender>()
                                 .method()
                                 .build(&nsdate::Calender::getSavedEvent));

        fns.push_back(rtl::type().member<nsdate::Calender>()
                                 .method()
                                 .build(&nsdate::Calender::getTheEvent));

        fns.push_back(rtl::type().member<nsdate::Calender>()
                                 .methodStatic()
                                 .build(&nsdate::Calender::instanceCount));
    }
}


namespace regs1::type1 {
    static void init(std::vector<rtl::Function>& fns) {

        fns.push_back(rtl::type().record<nsdate::Date>(cxx::type::nsdate::Date::id)
                                 .build());

        fns.push_back(rtl::type().member<nsdate::Date>()
                                 .methodStatic()
                                 .build(&nsdate::Date::instanceCount));

        fns.push_back(rtl::type().member<nsdate::Date>()
                                 .methodConst()
                                 .build(&nsdate::Date::getAsString));

        fns.push_back(rtl::type().member<nsdate::Date>()
                                 .method()
                                 .build(&nsdate::Date::updateDate));

        fns.push_back(rtl::type().member<nsdate::Date>()
                                 .constructor<unsigned int, unsigned int, unsigned int>().build());

        fns.push_back(rtl::type().member<nsdate::Date>()
                                 .constructor<const std::string &>().build());
    }
}


namespace regs1::type2 {
    static void init(std::vector<rtl::Function>& fns) {

        fns.push_back(rtl::type().record<nsdate::Event>(cxx::type::nsdate::Event::id)
                                 .build());

        fns.push_back(rtl::type().member<nsdate::Event>()
                                 .methodStatic()
                                 .build(&nsdate::Event::instanceCount));

        fns.push_back(rtl::type().member<nsdate::Event>()
                                 .method()
                                 .build(&nsdate::Event::getEventDate));

        fns.push_back(rtl::type().member<nsdate::Event>()
                                 .method()
                                 .build(&nsdate::Event::reset));
    }
}

