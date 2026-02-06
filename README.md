# clang-mirror

`clang-mirror` is a Clang-based code generation tool that produces AST-driven IDs and metadata registration code to enable portable runtime reflection for C++ without compiler extensions.

It enables user-defined types, functions, and member functions to be discovered and invoked at runtime using compile-time validated IDs – without manual registration.

After the generated `id` headers and registration sources are built into your project, reflected entities can be discovered and invoked at runtime. Registration is initialized lazily on the first call to `cxx::mirror()`, so no runtime cost is incurred if reflection is never used.

## Example
Given a function :

```c++
std::string complexToStr(float real, float img);
```

You can call it at runtime by ID :

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
					 
// resolved successfully?
if(method) {
    std::string name = method(personObj)();  // invokes Person::getName()
}

```

## How it works?

`clang-mirror` automatically generates the boilerplate registration code required by the [Reflection Template Library (RTL)](https://github.com/ReflectCxx/ReflectionTemplateLibrary-CPP) to enable a runtime reflection system for C++ projects.


Given one or more source files, it uses the `clang` frontend to analyze the AST and extract developer-defined symbols – including classes, structs, free functions, and member functions. These symbols are then organized under the `cxx` namespace and emitted as `constexpr` string identifiers.

The generated sources are compiled and linked statically alongside **RTL** as part of your build.

Integration is straightforward: include the generated header (`cxx_mirror.h`) and link against the **RTL** library. Your project then becomes runtime-reflection ready. You can introspect registered types and invoke functions through **RTL**’s APIs, with full runtime resolution handled automatically.

## Key Features

* ✅ **Standard C++** – Generates clean, portable C++20 code.

* ✅ **Type-Safe** – Compile-time validation of reflection queries.

* ✅ **Non-Intrusive** – No modifications to your source code required.


## Current Status

**⚠️ Early Development – Bootstrapping Phase**

clang-mirror is actively being built and is not yet ready for general use. In the meantime, you can check out [Reflection Template Library (RTL)](https://github.com/ReflectCxx/ReflectionTemplateLibrary-CPP) to explore the underlying runtime reflection system that `clang-mirror` is designed to support.
