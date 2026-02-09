
#include <iostream>

#include <cxx_mirror.h>

int main(int argc, const char** argv)
{
	{
		auto revStr = cxx::mirror().getFunction(cxx::fn::reverseString::id).value();

		auto functor = revStr.argsT().returnT<std::string>();

		auto str = functor();
	} {
		auto class_ = cxx::mirror().getRecord(cxx::type::nsdate::Date::id);

		auto [err, dateObj] = class_->ctorT()(rtl::alloc::Stack);

		auto methodMeta = class_->getMethod(cxx::type::nsdate::Date::fn::getAsString::id);

		auto getAsString = methodMeta->targetT().argsT().returnT<std::string>();

		auto [err0, str] = getAsString(std::cref(dateObj))();
		std::cout << "\nReturned value : " << *str << std::endl;
	}
	return 0;
}