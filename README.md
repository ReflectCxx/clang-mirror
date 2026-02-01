# clang-mirror

**Automatic generation of metadata to enable runtime reflection for C++ projects.**

clang-mirror is a Clang-based tool that analyzes your C++ codebase and auto-generates reflection metadata for the [Reflection Template Library (RTL)](https://github.com/ReflectCxx/ReflectionTemplateLibrary-CPP).

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

clang-mirror generates:

1. **Type-safe reflection identifiers** with IntelliSense support
2. **Complete RTL registration code** ready to compile
3. **Namespace-organized metadata** for all your types and functions

**Result:** Just `#include` the generated file and access your entire codebase reflectively.

```cpp
#include "cxx_mirror.h"

// Type-safe lookup with autocomplete,
auto classPerson = cxx::mirror().getRecord(cxx::type::Person::id);
// Query method, get metadata.
auto getName = classPerson->getMethod(cxx::type::Person::method::getName);

// Runtime invocation.
auto ctorPerson = classPerson->ctor();  // Get constructor.
auto [err, personObj] = ctorPerson();   // Create instance.
auto method = getName->targetT().argsT().returnT(); // Get functor from metadata.
std::string name = method(personObj)();
```

## Why clang-mirror?

### Without clang-mirror (Manual Registration):
```cpp
// Have to write this for EVERY class, EVERY method:
rtl::type().record("Person").build();
rtl::type().member().constructor().build();
rtl::type().member().method("getName").build(&Person::getName);
rtl::type().member().method("updateAddress").build(&Person::updateAddress);
// ...repeat for 100 classes...
```

**Painful. Error-prone. Gets out of sync with code.**

### With clang-mirror (Automated):
```bash
clang-mirror --input src/ --out-dir=build/
```

**Done.** All types, methods, and functions automatically registered.

## Key Features

✅ **Zero Boilerplate** – Analyzes your codebase, generates everything  
✅ **IntelliSense Integration** – Generated IDs work with autocomplete  
✅ **Type-Safe** – Compile-time validation of reflection queries  
✅ **Non-Intrusive** – No modifications to your source code required  
✅ **Standard C++** – Generates clean, portable C++20 code  

---

## Current Status

**⚠️ Early Development – Bootstrapping Phase**

clang-mirror is actively being built and is not yet ready for general use. In the meantime, you can check out [Reflection Template Library (RTL)](https://github.com/ReflectCxx/ReflectionTemplateLibrary-CPP) to explore the underlying runtime reflection system that clang-mirror is designed to support.
