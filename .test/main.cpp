//
// Created by clem on 25.07.21.
//

#include <stdio.h>
#include <sol/sol.hpp>
#include <vector.hpp>
#include <SFML/Graphics/Texture.hpp>

int main()
{
    auto lua = sol::state();
    auto tex = sf::Texture();

    auto vec = cip::Vector4f(1, 2, 3, 4);

    int i = -5
    std::cout << std::endl;

    for (size_t i = 0; i < 4; ++i)
        std::cout << vec.at(i) << " ";

    std::cout << std::endl;



    std::cout << "hello world" << std::endl;
}

