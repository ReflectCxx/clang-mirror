

#include <cxx_mirror.h>

#include "Date.h"

int main(int argc, const char** argv)
{
	{
		auto revStr = cxx::mirror().getFunction(cxx::fn::reverseString::id).value();

		auto functor = revStr.argsT().returnT<std::string>();

		auto str = functor();
	} {
		auto clsDate = cxx::mirror().getRecord(cxx::type::nsdate::Date::id).value();

		auto dateObj = clsDate.ctorT()(rtl::alloc::Stack).robject;

		auto getAsString = clsDate.getMethod(std::string(cxx::type::nsdate::Date::fn::getAsString::id));

		auto functor = getAsString->targetT<const nsdate::Date>().argsT().returnT<std::string>();

		auto str = functor(nsdate::Date())();
		str;
	}
	return 0;
}