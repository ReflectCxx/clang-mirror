
#pragma once
#include <array>
#include <string_view>

namespace cxx {

namespace fn {
namespace nsdate {
namespace getDateAsString {
    inline constexpr std::string_view id = "nsdate::getDateAsString";
    inline constexpr std::array<std::string_view, 2> signatures = {
        "std::string(nsdate::Date)",
        "std::string(unsigned int, unsigned int, unsigned int)"
    };
}}}

namespace type {
namespace Person {
    inline constexpr std::string_view id = "Person";
}}

namespace type {
namespace Person {
namespace fn {
namespace setLastName {
    inline constexpr std::string_view id = "setLastName";
    inline constexpr std::array<std::string_view, 1> signatures = {
        "void(const std::string_view)"
    };
}}}}

namespace type {
namespace Person {
namespace fn {
namespace getFirstName {
    inline constexpr std::string_view id = "getFirstName";
    inline constexpr std::array<std::string_view, 1> signatures = {
        "std::string(void)"
    };
}}}}

namespace type {
namespace Person {
namespace fn {
namespace setAddress {
    inline constexpr std::string_view id = "setAddress";
    inline constexpr std::array<std::string_view, 1> signatures = {
        "void(const std::string_view)"
    };
}}}}

namespace type {
namespace Person {
namespace fn {
namespace getAccessCard {
    inline constexpr std::string_view id = "getAccessCard";
    inline constexpr std::array<std::string_view, 1> signatures = {
        "std::string(const std::string_view, const Person &)"
    };
}}}}


namespace type {
namespace nsdate {
namespace Date {
    inline constexpr std::string_view id = "nsdate::Date";
}}}

namespace type {
namespace nsdate {
namespace Date {
namespace fn {
namespace set {
    inline constexpr std::string_view id = "set";
    inline constexpr std::array<std::string_view, 1> signatures = {
        "bool(const std::string_view)"
    };
}}}}}


}