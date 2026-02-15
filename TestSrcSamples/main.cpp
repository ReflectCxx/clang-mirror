
#include <iostream>
#include <cxx_mirror.h>

#include "Date.h"
#include "Person.h"

static std::string getDOBstr(unsigned day, unsigned month, unsigned year) {
    
    auto fnGetDateStr = cxx::mirror().getFunction(cxx::fn::nsdate::getDateAsString::id);
    auto getDateAsString = fnGetDateStr->argsT<unsigned, unsigned, unsigned>()
                                       .returnT<std::string>();
    if (!getDateAsString) {
        std::cout << "\n functor init error : " << rtl::to_string(getDateAsString.get_init_error());
        std::abort();
    }
    return getDateAsString(day, month, year);
}


int main() {

    nsdate::Date dateObj;
    
    auto fnGetDateStr = cxx::mirror().getFunction(cxx::fn::nsdate::getDateAsString::id);

    auto getDateAsString = fnGetDateStr->argsT<nsdate::Date>()
                                       .returnT<std::string>();
    if (!getDateAsString) {
        std::cout << "\n functor init error : " << rtl::to_string(getDateAsString.get_init_error());
        std::abort();
    }

    std::cout << "\n default date : " << getDateAsString(dateObj);

    dateObj.set(getDOBstr(20, 9, 2023));
    std::string dobStr = getDateAsString(dateObj);

    std::cout << "\n set date of birth : " << dobStr;

    auto clsPerson = cxx::mirror().getRecord(cxx::type::Person::id);
    auto fnGetAccessCard = clsPerson->getMethod(cxx::type::Person::fn::getAccessCard::id);

    auto getAccessCard = fnGetAccessCard->argsT<std::string_view, const Person&>()
                                        .returnT<std::string>();
    if (!getAccessCard) {
        std::cout << "\n functor init error : " << rtl::to_string(getAccessCard.get_init_error());
        std::abort();
    }

    Person personObj("Leo", dobStr);

    std::cout << "\n created access-card :\n" << getAccessCard("", personObj) << std::endl;

    auto fnSetLastName = clsPerson->getMethod(cxx::type::Person::fn::setLastName::id);
    auto setLastName = fnSetLastName->targetT<Person>()
                                    .argsT<std::string_view>().returnT();
    if (!setLastName) {
        std::cout << "\n functor init error : " << rtl::to_string(setLastName.get_init_error());
        std::abort();
    }

    setLastName(personObj)("Winston");

    constexpr auto purposeStr = "Safety & Security audit.";
    std::cout << "\n updated access-card :\n" << getAccessCard(purposeStr, personObj) << std::endl;

	return 0;
}