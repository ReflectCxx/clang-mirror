# clang-mirror

`clang-mirror` generates compile-time AST-driven portable C++ code to enable runtime reflection in your project.

For example, you have a these declared somewhere in your project:
```c++
std::string complexToStr(float real, float img);
```
```c++
class Person {
public:
    std::string getName();
//...
};
```
At runtime you can call the function by ID:
```c++
#include "cxx_mirror.h"  // The generated header.
//...

{
    // Find the function using its AST-generated, compile-time-checked ID.
    auto cToStr = cxx::mirror().getFunction(cxx::fn::complexToStr::id)
                               ->argsT<float, float>()
                               .returnT<std::string>();

    // `cToStr` is a functor that encapsulates the underlying function pointer.
    if(cToStr) {   // resolved successfully?
        std::string result = cToStr(61, 35);  // Works!
    }
}
```

## What It Does?

Given your C++ code:

```cpp
class Person {
public:
    std::string getName() const;
    void updateAddress(std::string addr);
};
```

`clang-mirror` generates:

* *Complete RTL registration code* – ready to compile.
* *Type-safe reflection identifiers* – that always resolve.
* *Namespace-organized metadata* – for all your types and functions.

#### Result 👉

Just `#include ` the generated file and access your entire codebase reflectively.

```cpp
#include "cxx_mirror.h"  // The generated header.
// ...

auto typId = cxx::type::Person::id;  // Compile-checked, generated via AST.
auto classPerson = cxx::mirror().getRecord(typId);  // Type-safe lookup.
// ...

{
    // If the ID is present, the method is registered (guaranteed!).
    auto fnId = cxx::type::Person::fn::getName::id;
    auto getName = classPerson->getMethod(fnId);  // Query method, get metadata.

    // Runtime invocations. Get functor from metadata,
    auto method = getName->targetT<Person>().argsT().returnT<std::string>();
    std::string name = method(personObj)();  // Person::getName() called.
}
//...

{
    auto fnId = cxx::type::Person::fn::updateAddress::id;  // Introspect via IntelliSense.
    auto updateAddress = classPerson->getMethod(fnId);  // Query method, get metadata.

    // Invoke updateAddress(). Get functor from metadata,
    auto method = updateAddress->targetT<Person>().argsT<std::string>().returnT();
    method(personObj)("new address");  // Person::updateAddress("new address") called.
}
```

## Why clang-mirror?

Without `clang-mirror`, Manual Registration required:
```cpp
rtl::type().record<Person>("Person").build();
rtl::type().member<Person>().method("getName").build(&Person::getName);
rtl::type().member<Person>().method("updateAddress").build(&Person::updateAddress);
// ...
```

With `clang-mirror`, Automated:
```bash
clang-mirror --input src/ --out-dir=build/
```

**Done.** All types, methods, and functions automatically registered and compile checked.

## Key Features

* ✅ **Zero Boilerplate** – Analyzes your codebase, generates everything.

* ✅ **Type-Safe** – Compile-time validation of reflection queries.

* ✅ **Non-Intrusive** – No modifications to your source code required.

* ✅ **Standard C++** – Generates clean, portable C++20 code.

---

## Current Status

**⚠️ Early Development – Bootstrapping Phase**

clang-mirror is actively being built and is not yet ready for general use. In the meantime, you can check out [Reflection Template Library (RTL)](https://github.com/ReflectCxx/ReflectionTemplateLibrary-CPP) to explore the underlying runtime reflection system that `clang-mirror` is designed to support.
