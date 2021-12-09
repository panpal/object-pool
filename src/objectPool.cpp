#include <iostream>
#include <stdlib.h>

#include "objectPool.h"

int main(int argc, char **argv)
{

    auto constructor = []()
    {
        int val = rand();
        std::cout << "hi I am constructed " << val << std::endl;
        return new int(val);
    };

    auto destructor = [](const Object<int> &)
    { std::cerr << "bye I am destructed" << std::endl; };

    ObjectPool<int, decltype(constructor), decltype(destructor)> pool(
        (size_t)1, std::optional<size_t>{(size_t)32}, constructor, destructor);

    {
        auto temp = pool.getObject();
    }
}