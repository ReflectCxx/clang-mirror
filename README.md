# clang-mirror

`clang-mirror` generates compile-time AST-driven portable C++ code to enable runtime reflection.

Example – given a function:

```c++
std::string complexToStr(float real, float img);
```

You can call it at runtime by ID:

```c++
#include "cxx_mirror.h"  // The generated header.
// ...

{
    // Find the function using its AST-generated, compile-time-checked, constexpr ID.
    auto cToStr = cxx::mirror().getFunction(cxx::fn::complexToStr::id)
                               ->argsT<float, float>()
                               .returnT<std::string>();

    // `cToStr` is a functor that encapsulates the underlying function pointer.
    if (cToStr) {  // resolved successfully?
        std::string result = cToStr(61, 35);  // Works!
    }
}
```

Reflect any `class` or `struct`:

```c++
class Person {
public:
    std::string getName();
// ...
};
```

Call a member function by ID:

```c++
// Navigate via Intellisense to locate the IDs.
auto clsId = cxx::type::Person::id;
auto fnId  = cxx::type::Person::fn::getName::id;

// Lookup the class by ID.
auto classPerson = cxx::mirror().getRecord(clsId);
auto getName = classPerson->getMethod(fnId);  // Query method metadata.

// Get functor from metadata.
auto method = getName->targetT<Person>()
                     .argsT()
                     .returnT<std::string>();

std::string name = method(personObj)();  // invokes Person::getName()
```


## How it works?

`clang-mirror` generates the boilerplate registration code to be used with "Reflection Template Library" to enable runtime reflection. 
Along with that it also generates constexpr string IDs which are basically the names of the class class/struct, function or any member function as written by the developer,
It also organises the IDs enclosed in the namespaces in which they are decleared in your project, can be navigated via Intellisense, giving you a sense of compile time introspection of entities in your project.

You just have to include the generated header `cxx_mirror.h` and statically-link the source files with the project and it becomes Runtime-Reflecection ready.


## Key Features

* ✅ **Zero Boilerplate** – Analyzes your codebase, generates everything.

* ✅ **Type-Safe** – Compile-time validation of reflection queries.

* ✅ **Non-Intrusive** – No modifications to your source code required.

* ✅ **Standard C++** – Generates clean, portable C++20 code.

---

## Current Status

**⚠️ Early Development – Bootstrapping Phase**

clang-mirror is actively being built and is not yet ready for general use. In the meantime, you can check out [Reflection Template Library (RTL)](https://github.com/ReflectCxx/ReflectionTemplateLibrary-CPP) to explore the underlying runtime reflection system that `clang-mirror` is designed to support.
