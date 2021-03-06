#include "../myenum.hpp"
#include <iostream>

/*
 * This example shows the enum reflection API.
 * All query methods are C++11 constexpr
 */

int main()
{
    using MyEnumStaticRefl = cpp::static_reflection::Enum<MyEnum>;

    // Number of enum constants declared:
    static_assert(MyEnumStaticRefl::count() == 3, "MyEnum::count() != 3");

    // has the enumeration a constant with this value?:
    static_assert(MyEnumStaticRefl::has(42), "No value 42 in MyEnum");
    static_assert(MyEnumStaticRefl::has(43), "No value 43 in MyEnum");
    static_assert(MyEnumStaticRefl::has(44), "No value 44 in MyEnum");
    static_assert(!MyEnumStaticRefl::has(45), "Value 45 in MyEnum");

    // has the enumeration a constant with this name?:
    static_assert(MyEnumStaticRefl::has("FOO"),  "No 'FOO' value in MyEnum");
    static_assert(MyEnumStaticRefl::has("BAR"),  "No 'BAR' value in MyEnum");
    static_assert(MyEnumStaticRefl::has("QUUX"), "No 'QUUX' value in MyEnum");
    static_assert(!MyEnumStaticRefl::has("FOOBARQUUX"), "'FOOBARQUUX' value in MyEnum");

    // value of the constant with the given name (Undefined behavior if there's no such constant, see has()):
    static_assert(MyEnumStaticRefl::fromString("FOO") == MyEnum::FOO,   "Parsed 'FOO' value not equal to MyEnum::FOO value");
    static_assert(MyEnumStaticRefl::fromString("BAR") == MyEnum::BAR,   "Parsed 'BAR' value not equal to MyEnum::BAR value");
    static_assert(MyEnumStaticRefl::fromString("QUUX") == MyEnum::QUUX, "Parsed 'QUUX' value not equal to MyEnum::QUUX value");

    // Name of the constant with the given value (Undefined behavior if there's no constant with that value, see has()):
    static_assert(MyEnumStaticRefl::value(0) == MyEnum::FOO,  "MyEnum[0] value not equal to MyEnum::FOO value");
    static_assert(MyEnumStaticRefl::value(1) == MyEnum::BAR,  "MyEnum[1] value not equal to MyEnum::BAR value");
    static_assert(MyEnumStaticRefl::value(2) == MyEnum::QUUX, "MyEnum[2] value not equal to MyEnum::QUUX value");

    // Iteration over enumeration values (With range for):
    for(MyEnum value : MyEnumStaticRefl::values())
    {
        std::cout << "MyEnum::" << MyEnumStaticRefl::toString(value) << ": "
                  << static_cast<MyEnumStaticRefl::UnderlyingType>(value)
                  << std::endl;
    }

    // Iteration over enumeration values (With raw indexed loop):
    for(std::size_t i = 0; i < MyEnumStaticRefl::count(); ++i)
    {
        std::cout << "MyEnum[" << i << "]: MyEnum::" << MyEnumStaticRefl::name(i)
                  << " (" << static_cast<MyEnumStaticRefl::UnderlyingType>(MyEnumStaticRefl::value(i)) << ")"
                  << std::endl;
    }
}
