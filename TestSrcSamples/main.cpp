
#include <iostream>
#include <cxx_mirror.h>

int main() {

	auto oGetDateAsStr = cxx::mirror().getFunction(cxx::fn::nsdate::getDateAsString::id);

	if (!oGetDateAsStr) {
		std::cout << "\n function " << cxx::fn::nsdate::getDateAsString::id << " not registered.";
		return 0;
	}

	auto getDateAsString = oGetDateAsStr->argsT<unsigned, unsigned, unsigned>().returnT<std::string>();
	if (!getDateAsString) {
		std::cout << "\n Refleceted functor " << cxx::fn::nsdate::getDateAsString::id << " not initialized.";
		std::cout << "\n Error - " << rtl::to_string(getDateAsString.get_init_error());
		return 0;
	}

	std::cout << "\n Reflected Date : " << getDateAsString(15, 2, 2026);

	return 0;
}