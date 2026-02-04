#pragma once

#include <tuple>
#include <string>
#include <vector>
#include <chrono>
#include <string_view>
#include <unordered_map>

namespace clmr {

	inline constexpr std::string_view RESET = "\033[0m";
	inline constexpr std::string_view DARK_RED = "\033[31m";
	inline constexpr std::string_view RED = "\033[38;2;231;72;86m";
	inline constexpr std::string_view GREEN = "\033[32m";
	inline constexpr std::string_view YELLOW = "\033[33m";
	inline constexpr std::string_view BLUE = "\033[34m";
	inline constexpr std::string_view MAGENTA = "\033[35m";
	inline constexpr std::string_view CYAN = "\033[38;2;97;214;214m";
	inline constexpr std::string_view WHITE = "\033[37m";
	inline constexpr std::string_view TEAL = "\033[38;2;0;128;128m";
	inline constexpr std::string_view GREY = "\033[38;2;118;118;118m";

	inline constexpr std::string_view CONST = "const";
	inline constexpr std::string_view ENUM = "enum";
	inline constexpr std::string_view CLASS = "class";
	inline constexpr std::string_view STRUCT = "struct";
	


	inline constexpr std::string_view NS_CXX = "cxx";
	inline constexpr std::string_view NS_RTL = "rtl";
	inline constexpr std::string_view NS_TYPE = "type";
	inline constexpr std::string_view NS_FUNCTION = "fn";
	inline constexpr std::string_view NS_REGISTER = "regs";
	
	inline constexpr std::string_view REGIS_INIT_DECL = "static void init(std::vector<rtl::Function>&)";
	inline constexpr std::string_view REGIS_INIT_DEFN = "static void init(std::vector<rtl::Function>& fns)";
}


namespace clmr 
{
	enum class MetaKind 
	{
		None,
		Ctor,
		NonMemberFn,
		MemberFnConst,
		MemberFnStatic,
		MemberFnNonConst
	};

	struct File 
	{
		static constexpr std::string_view dirRtl = "rtl";
		static constexpr std::string_view dirClmr = "clmr";
		static constexpr std::string_view prefixRegs = "reg_defs";
		static constexpr std::string_view nameIDsHeader = "reg_ids.h";
		static constexpr std::string_view nameRegHeader = "reg_decls.h";
		static constexpr std::string_view nameCxxHeader = "cxx_mirror.h";
		static constexpr std::string_view nameCxxSource = "cxx_mirror.cpp";
		static constexpr std::string_view incRtlAccess = "rtl/rtl_access.h";
		static constexpr std::string_view incRtlBuilder = "rtl/rtl_builder.h";
	};

	struct ASTRecordMeta;
	struct ASTCodeMeta;

	using CxxRecordsMap = std::unordered_map<std::string, ASTRecordMeta>;
	using CxxFunctionsMap = std::unordered_map<std::string, ASTCodeMeta>;

	using Clock = std::chrono::high_resolution_clock;
	using Second = std::chrono::duration<double, std::ratio<1> >;
	using ErrorTuple = std::tuple<std::string, std::string, std::string>;
}


#define GETTER_CREF(_varType, _name, _var)       \
inline constexpr const _varType& get##_name() const {  \
    return _var;                             \
}

#define GETTER_BOOL(_name, _var)              \
inline constexpr const bool is##_name() const {     \
    return _var;                          \
}
