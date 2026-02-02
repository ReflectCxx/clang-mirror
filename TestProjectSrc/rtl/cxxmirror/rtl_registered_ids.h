
#pragma once
#include <string_view>

namespace cxx {

namespace fn {
namespace complex {
namespace getMagnitude {
    inline constexpr std::string_view id = "complex::getMagnitude";
    inline constexpr std::string_view sign0 = "void";
}}}

namespace fn {
namespace complex {
namespace setReal {
    inline constexpr std::string_view id = "complex::setReal";
    inline constexpr std::string_view sign0 = "double";
}}}

namespace fn {
namespace complex {
namespace setImaginary {
    inline constexpr std::string_view id = "complex::setImaginary";
    inline constexpr std::string_view sign0 = "double";
}}}

namespace fn {
namespace getComplexNumAsString {
    inline constexpr std::string_view id = "getComplexNumAsString";
    inline constexpr std::string_view sign0 = "void";
}}

namespace fn {
namespace revStrOverloadValCRef {
    inline constexpr std::string_view id = "revStrOverloadValCRef";
    inline constexpr std::string_view sign0 = "std::string_view";
    inline constexpr std::string_view sign1 = "const std::string_view &";
}}

namespace fn {
namespace reverseString {
    inline constexpr std::string_view id = "reverseString";
    inline constexpr std::string_view sign0 = "void";
    inline constexpr std::string_view sign1 = "const char *";
    inline constexpr std::string_view sign2 = "std::string";
    inline constexpr std::string_view sign3 = "std::string &";
    inline constexpr std::string_view sign4 = "std::basic_string<char> &&";
    inline constexpr std::string_view sign5 = "const std::string &";
    inline constexpr std::string_view sign6 = "std::string *";
    inline constexpr std::string_view sign7 = "const std::string *";
}}

namespace fn {
namespace revStrConstRefArg {
    inline constexpr std::string_view id = "revStrConstRefArg";
    inline constexpr std::string_view sign0 = "const std::string_view &";
}}

namespace fn {
namespace revStrRValueRefArg {
    inline constexpr std::string_view id = "revStrRValueRefArg";
    inline constexpr std::string_view sign0 = "std::basic_string_view<char> &&";
}}

namespace fn {
namespace revStrNonConstRefArg {
    inline constexpr std::string_view id = "revStrNonConstRefArg";
    inline constexpr std::string_view sign0 = "std::string_view &";
}}

namespace fn {
namespace revStrOverloadRefAndCRef {
    inline constexpr std::string_view id = "revStrOverloadRefAndCRef";
    inline constexpr std::string_view sign0 = "std::string_view &";
    inline constexpr std::string_view sign1 = "const std::string_view &";
}}

namespace fn {
namespace revStrOverloadValRef {
    inline constexpr std::string_view id = "revStrOverloadValRef";
    inline constexpr std::string_view sign0 = "std::string_view";
    inline constexpr std::string_view sign1 = "std::string_view &";
}}

namespace type {
namespace StrMute {
    inline constexpr std::string_view id = "StrMute";
}}

namespace type {
namespace StrMute {
namespace fn {
namespace revStrOverloadValCRef {
    inline constexpr std::string_view id = "revStrOverloadValCRef";
    inline constexpr std::string_view sign0 = "std::string_view";
    inline constexpr std::string_view sign1 = "const std::string_view &";
}}}}

namespace type {
namespace StrMute {
namespace fn {
namespace reverseString {
    inline constexpr std::string_view id = "reverseString";
    inline constexpr std::string_view sign0 = "void";
    inline constexpr std::string_view sign1 = "const char *";
    inline constexpr std::string_view sign2 = "std::string";
    inline constexpr std::string_view sign3 = "std::string &";
    inline constexpr std::string_view sign4 = "std::basic_string<char> &&";
    inline constexpr std::string_view sign5 = "const std::string &";
    inline constexpr std::string_view sign6 = "std::string *";
    inline constexpr std::string_view sign7 = "const std::string *";
}}}}

namespace type {
namespace StrMute {
namespace fn {
namespace revStrConstRefArg {
    inline constexpr std::string_view id = "revStrConstRefArg";
    inline constexpr std::string_view sign0 = "const std::string_view &";
}}}}

namespace type {
namespace StrMute {
namespace fn {
namespace revStrRValueRefArg {
    inline constexpr std::string_view id = "revStrRValueRefArg";
    inline constexpr std::string_view sign0 = "std::basic_string_view<char> &&";
}}}}

namespace type {
namespace StrMute {
namespace fn {
namespace revStrNonConstRefArg {
    inline constexpr std::string_view id = "revStrNonConstRefArg";
    inline constexpr std::string_view sign0 = "std::string_view &";
}}}}

namespace type {
namespace StrMute {
namespace fn {
namespace revStrOverloadRefAndCRef {
    inline constexpr std::string_view id = "revStrOverloadRefAndCRef";
    inline constexpr std::string_view sign0 = "std::string_view &";
    inline constexpr std::string_view sign1 = "const std::string_view &";
}}}}

namespace type {
namespace StrMute {
namespace fn {
namespace revStrOverloadValRef {
    inline constexpr std::string_view id = "revStrOverloadValRef";
    inline constexpr std::string_view sign0 = "std::string_view";
    inline constexpr std::string_view sign1 = "std::string_view &";
}}}}


namespace type {
namespace nsdate {
namespace Calender {
    inline constexpr std::string_view id = "nsdate::Calender";
}}}

namespace type {
namespace nsdate {
namespace Calender {
namespace fn {
namespace getMoveOpsCount {
    inline constexpr std::string_view id = "getMoveOpsCount";
    inline constexpr std::string_view sign0 = "void";
}}}}}

namespace type {
namespace nsdate {
namespace Calender {
namespace fn {
namespace create {
    inline constexpr std::string_view id = "create";
    inline constexpr std::string_view sign0 = "void";
}}}}}

namespace type {
namespace nsdate {
namespace Calender {
namespace fn {
namespace getSavedDate {
    inline constexpr std::string_view id = "getSavedDate";
    inline constexpr std::string_view sign0 = "void";
}}}}}

namespace type {
namespace nsdate {
namespace Calender {
namespace fn {
namespace getTheDate {
    inline constexpr std::string_view id = "getTheDate";
    inline constexpr std::string_view sign0 = "void";
}}}}}

namespace type {
namespace nsdate {
namespace Calender {
namespace fn {
namespace getSavedEvent {
    inline constexpr std::string_view id = "getSavedEvent";
    inline constexpr std::string_view sign0 = "void";
}}}}}

namespace type {
namespace nsdate {
namespace Calender {
namespace fn {
namespace getTheEvent {
    inline constexpr std::string_view id = "getTheEvent";
    inline constexpr std::string_view sign0 = "void";
}}}}}

namespace type {
namespace nsdate {
namespace Calender {
namespace fn {
namespace instanceCount {
    inline constexpr std::string_view id = "instanceCount";
    inline constexpr std::string_view sign0 = "void";
}}}}}

namespace type {
namespace nsdate {
namespace Calender {
namespace fn {
namespace resetMoveOpsCounter {
    inline constexpr std::string_view id = "resetMoveOpsCounter";
    inline constexpr std::string_view sign0 = "void";
}}}}}


namespace type {
namespace nsdate {
namespace Date {
    inline constexpr std::string_view id = "nsdate::Date";
}}}

namespace type {
namespace nsdate {
namespace Date {
namespace fn {
namespace instanceCount {
    inline constexpr std::string_view id = "instanceCount";
    inline constexpr std::string_view sign0 = "void";
}}}}}

namespace type {
namespace nsdate {
namespace Date {
namespace fn {
namespace getAsString {
    inline constexpr std::string_view id = "getAsString";
    inline constexpr std::string_view sign0 = "void";
}}}}}

namespace type {
namespace nsdate {
namespace Date {
namespace fn {
namespace updateDate {
    inline constexpr std::string_view id = "updateDate";
    inline constexpr std::string_view sign0 = "std::string";
}}}}}


namespace type {
namespace nsdate {
namespace Event {
    inline constexpr std::string_view id = "nsdate::Event";
}}}

namespace type {
namespace nsdate {
namespace Event {
namespace fn {
namespace instanceCount {
    inline constexpr std::string_view id = "instanceCount";
    inline constexpr std::string_view sign0 = "void";
}}}}}

namespace type {
namespace nsdate {
namespace Event {
namespace fn {
namespace getEventDate {
    inline constexpr std::string_view id = "getEventDate";
    inline constexpr std::string_view sign0 = "void";
}}}}}

namespace type {
namespace nsdate {
namespace Event {
namespace fn {
namespace reset {
    inline constexpr std::string_view id = "reset";
    inline constexpr std::string_view sign0 = "void";
}}}}}


namespace type {
namespace StrStatic {
    inline constexpr std::string_view id = "StrStatic";
}}

namespace type {
namespace StrStatic {
namespace fn {
namespace revStrOverloadValCRef {
    inline constexpr std::string_view id = "revStrOverloadValCRef";
    inline constexpr std::string_view sign0 = "std::string_view";
    inline constexpr std::string_view sign1 = "const std::string_view &";
}}}}

namespace type {
namespace StrStatic {
namespace fn {
namespace reverseString {
    inline constexpr std::string_view id = "reverseString";
    inline constexpr std::string_view sign0 = "void";
    inline constexpr std::string_view sign1 = "const char *";
    inline constexpr std::string_view sign2 = "std::string";
    inline constexpr std::string_view sign3 = "std::string &";
    inline constexpr std::string_view sign4 = "std::basic_string<char> &&";
    inline constexpr std::string_view sign5 = "const std::string &";
    inline constexpr std::string_view sign6 = "std::string *";
    inline constexpr std::string_view sign7 = "const std::string *";
}}}}

namespace type {
namespace StrStatic {
namespace fn {
namespace revStrConstRefArg {
    inline constexpr std::string_view id = "revStrConstRefArg";
    inline constexpr std::string_view sign0 = "const std::string_view &";
}}}}

namespace type {
namespace StrStatic {
namespace fn {
namespace revStrRValueRefArg {
    inline constexpr std::string_view id = "revStrRValueRefArg";
    inline constexpr std::string_view sign0 = "std::basic_string_view<char> &&";
}}}}

namespace type {
namespace StrStatic {
namespace fn {
namespace revStrNonConstRefArg {
    inline constexpr std::string_view id = "revStrNonConstRefArg";
    inline constexpr std::string_view sign0 = "std::string_view &";
}}}}

namespace type {
namespace StrStatic {
namespace fn {
namespace revStrOverloadRefAndCRef {
    inline constexpr std::string_view id = "revStrOverloadRefAndCRef";
    inline constexpr std::string_view sign0 = "std::string_view &";
    inline constexpr std::string_view sign1 = "const std::string_view &";
}}}}

namespace type {
namespace StrStatic {
namespace fn {
namespace revStrOverloadValRef {
    inline constexpr std::string_view id = "revStrOverloadValRef";
    inline constexpr std::string_view sign0 = "std::string_view";
    inline constexpr std::string_view sign1 = "std::string_view &";
}}}}


namespace type {
namespace Book {
    inline constexpr std::string_view id = "Book";
}}

namespace type {
namespace Book {
namespace fn {
namespace setAuthor {
    inline constexpr std::string_view id = "setAuthor";
    inline constexpr std::string_view sign0 = "std::string";
}}}}

namespace type {
namespace Book {
namespace fn {
namespace getTitle {
    inline constexpr std::string_view id = "getTitle";
    inline constexpr std::string_view sign0 = "void";
}}}}

namespace type {
namespace Book {
namespace fn {
namespace addCopyrightTag {
    inline constexpr std::string_view id = "addCopyrightTag";
    inline constexpr std::string_view sign0 = "const std::string";
}}}}

namespace type {
namespace Book {
namespace fn {
namespace setDescription {
    inline constexpr std::string_view id = "setDescription";
    inline constexpr std::string_view sign0 = "std::string";
}}}}

namespace type {
namespace Book {
namespace fn {
namespace getPublishedOn {
    inline constexpr std::string_view id = "getPublishedOn";
    inline constexpr std::string_view sign0 = "void";
}}}}

namespace type {
namespace Book {
namespace fn {
namespace getInstanceCount {
    inline constexpr std::string_view id = "getInstanceCount";
    inline constexpr std::string_view sign0 = "void";
}}}}

namespace type {
namespace Book {
namespace fn {
namespace updateBookInfo {
    inline constexpr std::string_view id = "updateBookInfo";
    inline constexpr std::string_view sign0 = "void";
    inline constexpr std::string_view sign1 = "const char *, double, std::string";
    inline constexpr std::string_view sign2 = "std::string, double, const char *";
}}}}

namespace type {
namespace Book {
namespace fn {
namespace addPreface {
    inline constexpr std::string_view id = "addPreface";
    inline constexpr std::string_view sign0 = "const std::string, const std::string &";
}}}}


namespace type {
namespace Person {
    inline constexpr std::string_view id = "Person";
}}

namespace type {
namespace Person {
namespace fn {
namespace getProfile {
    inline constexpr std::string_view id = "getProfile";
    inline constexpr std::string_view sign0 = "void";
    inline constexpr std::string_view sign1 = "std::string, unsigned long long";
    inline constexpr std::string_view sign2 = "_Bool";
}}}}

namespace type {
namespace Person {
namespace fn {
namespace getFirstName {
    inline constexpr std::string_view id = "getFirstName";
    inline constexpr std::string_view sign0 = "void";
}}}}

namespace type {
namespace Person {
namespace fn {
namespace getInstanceCount {
    inline constexpr std::string_view id = "getInstanceCount";
    inline constexpr std::string_view sign0 = "void";
}}}}

namespace type {
namespace Person {
namespace fn {
namespace createConst {
    inline constexpr std::string_view id = "createConst";
    inline constexpr std::string_view sign0 = "void";
}}}}

namespace type {
namespace Person {
namespace fn {
namespace updateAddress {
    inline constexpr std::string_view id = "updateAddress";
    inline constexpr std::string_view sign0 = "void";
    inline constexpr std::string_view sign1 = "void";
    inline constexpr std::string_view sign2 = "std::string";
    inline constexpr std::string_view sign3 = "std::string";
}}}}

namespace type {
namespace Person {
namespace fn {
namespace createPtr {
    inline constexpr std::string_view id = "createPtr";
    inline constexpr std::string_view sign0 = "void";
}}}}

namespace type {
namespace Person {
namespace fn {
namespace getDefaults {
    inline constexpr std::string_view id = "getDefaults";
    inline constexpr std::string_view sign0 = "void";
}}}}

namespace type {
namespace Person {
namespace fn {
namespace deletePtr {
    inline constexpr std::string_view id = "deletePtr";
    inline constexpr std::string_view sign0 = "const Person *";
}}}}

namespace type {
namespace Person {
namespace fn {
namespace updateLastName {
    inline constexpr std::string_view id = "updateLastName";
    inline constexpr std::string_view sign0 = "std::string";
}}}}


namespace type {
namespace Animal {
    inline constexpr std::string_view id = "Animal";
}}

namespace type {
namespace Animal {
namespace fn {
namespace getFamilyName {
    inline constexpr std::string_view id = "getFamilyName";
    inline constexpr std::string_view sign0 = "void";
}}}}

namespace type {
namespace Animal {
namespace fn {
namespace setAnimalName {
    inline constexpr std::string_view id = "setAnimalName";
    inline constexpr std::string_view sign0 = "std::string &";
    inline constexpr std::string_view sign1 = "std::basic_string<char> &&";
    inline constexpr std::string_view sign2 = "const std::string &";
    inline constexpr std::string_view sign3 = "const std::string &";
}}}}

namespace type {
namespace Animal {
namespace fn {
namespace setFamilyName {
    inline constexpr std::string_view id = "setFamilyName";
    inline constexpr std::string_view sign0 = "const std::string";
}}}}

namespace type {
namespace Animal {
namespace fn {
namespace getInstanceCount {
    inline constexpr std::string_view id = "getInstanceCount";
    inline constexpr std::string_view sign0 = "void";
}}}}

namespace type {
namespace Animal {
namespace fn {
namespace updateZooKeeper {
    inline constexpr std::string_view id = "updateZooKeeper";
    inline constexpr std::string_view sign0 = "std::string &";
    inline constexpr std::string_view sign1 = "std::basic_string<char> &&";
    inline constexpr std::string_view sign2 = "const std::string &";
}}}}


namespace type {
namespace StrWrap {
    inline constexpr std::string_view id = "StrWrap";
}}

namespace type {
namespace StrWrap {
namespace fn {
namespace sstr {
    inline constexpr std::string_view id = "sstr";
    inline constexpr std::string_view sign0 = "void";
}}}}


namespace type {
namespace StrWrapA {
    inline constexpr std::string_view id = "StrWrapA";
}}


namespace type {
namespace StrWrapB {
    inline constexpr std::string_view id = "StrWrapB";
}}


namespace type {
namespace StrWrapC {
    inline constexpr std::string_view id = "StrWrapC";
}}


namespace type {
namespace StrWrapD {
    inline constexpr std::string_view id = "StrWrapD";
}}


namespace type {
namespace StrConstOverload {
    inline constexpr std::string_view id = "StrConstOverload";
}}

namespace type {
namespace StrConstOverload {
namespace fn {
namespace reverseString {
    inline constexpr std::string_view id = "reverseString";
    inline constexpr std::string_view sign0 = "void";
    inline constexpr std::string_view sign1 = "void";
}}}}


namespace type {
namespace StrConst {
    inline constexpr std::string_view id = "StrConst";
}}

namespace type {
namespace StrConst {
namespace fn {
namespace revStrOverloadValCRef {
    inline constexpr std::string_view id = "revStrOverloadValCRef";
    inline constexpr std::string_view sign0 = "std::string_view";
    inline constexpr std::string_view sign1 = "const std::string_view &";
}}}}

namespace type {
namespace StrConst {
namespace fn {
namespace reverseString {
    inline constexpr std::string_view id = "reverseString";
    inline constexpr std::string_view sign0 = "void";
    inline constexpr std::string_view sign1 = "const char *";
    inline constexpr std::string_view sign2 = "std::string";
    inline constexpr std::string_view sign3 = "std::string &";
    inline constexpr std::string_view sign4 = "std::basic_string<char> &&";
    inline constexpr std::string_view sign5 = "const std::string &";
    inline constexpr std::string_view sign6 = "std::string *";
    inline constexpr std::string_view sign7 = "const std::string *";
}}}}

namespace type {
namespace StrConst {
namespace fn {
namespace revStrConstRefArg {
    inline constexpr std::string_view id = "revStrConstRefArg";
    inline constexpr std::string_view sign0 = "const std::string_view &";
}}}}

namespace type {
namespace StrConst {
namespace fn {
namespace revStrRValueRefArg {
    inline constexpr std::string_view id = "revStrRValueRefArg";
    inline constexpr std::string_view sign0 = "std::basic_string_view<char> &&";
}}}}

namespace type {
namespace StrConst {
namespace fn {
namespace revStrNonConstRefArg {
    inline constexpr std::string_view id = "revStrNonConstRefArg";
    inline constexpr std::string_view sign0 = "std::string_view &";
}}}}

namespace type {
namespace StrConst {
namespace fn {
namespace revStrOverloadRefAndCRef {
    inline constexpr std::string_view id = "revStrOverloadRefAndCRef";
    inline constexpr std::string_view sign0 = "std::string_view &";
    inline constexpr std::string_view sign1 = "const std::string_view &";
}}}}

namespace type {
namespace StrConst {
namespace fn {
namespace revStrOverloadValRef {
    inline constexpr std::string_view id = "revStrOverloadValRef";
    inline constexpr std::string_view sign0 = "std::string_view";
    inline constexpr std::string_view sign1 = "std::string_view &";
}}}}


namespace type {
namespace Library {
    inline constexpr std::string_view id = "Library";
}}

namespace type {
namespace Library {
namespace fn {
namespace getBookByTitle {
    inline constexpr std::string_view id = "getBookByTitle";
    inline constexpr std::string_view sign0 = "const std::string &";
}}}}

namespace type {
namespace Library {
namespace fn {
namespace getInstanceCount {
    inline constexpr std::string_view id = "getInstanceCount";
    inline constexpr std::string_view sign0 = "void";
}}}}

namespace type {
namespace Library {
namespace fn {
namespace getBooksCount {
    inline constexpr std::string_view id = "getBooksCount";
    inline constexpr std::string_view sign0 = "void";
}}}}

namespace type {
namespace Library {
namespace fn {
namespace addBook {
    inline constexpr std::string_view id = "addBook";
    inline constexpr std::string_view sign0 = "const Book &";
}}}}


}