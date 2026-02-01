# clang-mirror

**Automatic generation of metadata to enable runtime reflection for C++ projects.**

`clang-mirror` is a Clang-based tool that analyzes your C++ code and auto-generates the registration code needed to statically link the [Reflection Template Library (RTL)](https://github.com/ReflectCxx/ReflectionTemplateLibrary-CPP) and enable runtime reflection.

**No manual registration. No macros. No boilerplate.**

## What It Does

Given your C++ code:

```cpp
class Person {
public:
    std::string getName() const;
    void updateAddress(std::string addr);
};
```

`clang-mirror` generates:

1. **Complete RTL registration code** – ready to compile.
2. **Type-safe reflection identifiers** – that always resolve.
3. **Namespace-organized metadata** – for all your types and functions.

**Result:** Just `#include ` the generated file and access your entire codebase reflectively.

```cpp
#include "cxx_mirror.h"
// ...

auto classId = cxx::type::Person::id;   // Compile-checked, generated via AST.
auto classPerson = cxx::mirror().getRecord(classId);   // Type-safe lookup.
auto fnId = cxx::type::Person::fn::getName::id;    // Introspect via IntelliSense.
auto getName = classPerson->getMethod(fnId);   // Query method, get metadata.

// Runtime invocations. Get functor from metadata,
auto method = getName->targetT<Person>().argsT().returnT<std::string>();
std::string name = method(personObj)(); // Person::getName() called.
```

## Why clang-mirror?

Without `clang-mirror` (Manual Registration required):
```cpp
rtl::type().record<Person>("Person").build();
rtl::type().member<Person>().method("getName").build(&Person::getName);
rtl::type().member<Person>().method("updateAddress").build(&Person::updateAddress);
// ...
```

With `clang-mirror` (Automated):
```bash
clang-mirror --input src/ --out-dir=build/
```

**Done.** All types, methods, and functions automatically registered.

## Key Features

* ✅ **Zero Boilerplate** – Analyzes your codebase, generates everything.

* ✅ **Type-Safe** – Compile-time validation of reflection queries.

* ✅ **Non-Intrusive** – No modifications to your source code required.

* ✅ **Standard C++** – Generates clean, portable C++20 code.

---

## Current Status

**⚠️ Early Development – Bootstrapping Phase**

clang-mirror is actively being built and is not yet ready for general use. In the meantime, you can check out [Reflection Template Library (RTL)](https://github.com/ReflectCxx/ReflectionTemplateLibrary-CPP) to explore the underlying runtime reflection system that clang-mirror is designed to support.
