
#include <iostream>

#include <cxx_mirror.h>

int main(int argc, const char** argv)
{
	{
		auto classId = cxx::type::nsdate::Date::id;

		auto class_ = cxx::mirror().getRecord(classId);
		if (!class_) {
			std::cout << "\nType "<< classId << " not found." << std::endl;
			return 0;
		}

		auto [err, dateObj] = class_->ctorT()(rtl::alloc::Stack);
		if (err != rtl::error::None) {
			std::cout << "\nObject construction failed." << std::endl;
			return 0;
		}

		auto methodId = cxx::type::nsdate::Date::fn::getAsString::id;

		auto methodMeta = class_->getMethod(methodId);
		if (!methodMeta) {
			std::cout << "\nMethod " << methodId << " not found." << std::endl;
			return 0;
		}

		auto getAsString = methodMeta->targetT().argsT().returnT<std::string>();

		if (!getAsString) {
			std::cout << "\nMethod didn't resolve. error : " << rtl::to_string(getAsString.get_init_error()) << std::endl;
			return 0;
		}

		auto [err0, str] = getAsString(std::cref(dateObj))();
		std::cout << "\nReturned value : " << *str << std::endl;
	}
	return 0;
}