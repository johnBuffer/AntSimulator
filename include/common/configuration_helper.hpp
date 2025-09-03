#pragma once
#include "./configuration_loader.hpp"

template<typename TVecType>
void loadIntoVec(cload::ConfigurationLoader const& loader, std::string const& key, TVecType* attribute)
{
    if (loader.tryReadSequenceIntoArray<2>(key, &attribute->x)) {
        std::cout << '"' << key << "\" loaded with value: [" << attribute->x << ", " << attribute->y << "]" << std::endl;
    } else {
        std::cout << "[WARNING] \"" << key << "\" could not be loaded, using default value of [" << attribute->x << ", " << attribute->y << "]" << std::endl;
    }
}

template<typename TType>
void loadInto(cload::ConfigurationLoader const& loader, std::string const& key, TType* attribute)
{
    if (loader.tryReadValueInto(key, attribute)) {
        std::cout << '"' << key << "\" loaded with value: " << *attribute << std::endl;
    } else {
        std::cout << "[WARNING] \"" << key << "\" could not be loaded, using default value of " << *attribute << std::endl;
    }
}

template<>
inline void loadInto<sf::Vector2i>(cload::ConfigurationLoader const& loader, std::string const& key, sf::Vector2i* attribute)
{
    loadIntoVec<sf::Vector2i>(loader, key, attribute);
}

template<>
inline void loadInto<sf::Vector2u>(cload::ConfigurationLoader const& loader, std::string const& key, sf::Vector2u* attribute)
{
    loadIntoVec<sf::Vector2u>(loader, key, attribute);
}

template<>
inline void loadInto<sf::Vector2f>(cload::ConfigurationLoader const& loader, std::string const& key, sf::Vector2f* attribute)
{
    loadIntoVec<sf::Vector2f>(loader, key, attribute);
}