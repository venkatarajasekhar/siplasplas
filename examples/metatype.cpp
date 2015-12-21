
#include "metatype/metatype.hpp"
#include <iostream>

struct BaseClass
{
    virtual ~BaseClass() = default;
    virtual void talk() const = 0;
};

struct Duck : public BaseClass
{
    Duck() = default;

    void talk() const override
    {
        std::cout << "Quack!" << std::endl;        
    }
};

struct Cat : public BaseClass
{
    Cat() = default;

    void talk() const override
    {
        std::cout << "Mew!" << std::endl;        
    }
};

static_assert(ctti::type_id<int>().hash() == ctti::unnamed_type_id<int>().hash(), "???");

int main()
{
    cpp::MetaTypeSystem::registerMetatype<Cat>();
    cpp::MetaTypeSystem::registerMetatype<Duck>();

    auto cat = cpp::MetaTypeSystem::create<BaseClass>("Cat");
    auto duck = cpp::MetaTypeSystem::create<BaseClass>("Duck");

    cat->talk();
    duck->talk();

    cpp::MetaTypeSystem::destroy(cat);
    cpp::MetaTypeSystem::destroy(duck);
}