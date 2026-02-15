
#include <iostream>
#include <cxx_mirror.h>

#include "Date.h"

int main() {

	nsdate::Date dateObj;

	auto fnGetDateStr = cxx::mirror().getFunction(cxx::fn::nsdate::getDateAsString::id);

	auto getDateAsString = fnGetDateStr->argsT<nsdate::Date>()
		                               .returnT<std::string>();
	if (!getDateAsString) {
		std::cout << "\n Functor initialization error : " << rtl::to_string(getDateAsString.get_init_error());
		return 1;
	}

	std::string dateStr = getDateAsString(dateObj);
	std::cout << "\n Default date : " << dateStr;

	return 0;
}