#pragma once
#include <string>
#include <memory>
#include <SFML/Graphics/Font.hpp>

struct Store
{
    std::string working_directory;
    std::map<std::string, std::unique_ptr<sf::Font>> fonts;

    void loadFont(const std::string& filepath, const std::string& name)
    {
        fonts[name] = std::make_unique<sf::Font>();
        fonts[name]->loadFromFile(working_directory + filepath);
    }

    [[nodiscard]]
    const sf::Font* getFont(const std::string& name) const
    {
        return &(*fonts.at(name));
    }
};

template<int N=0>
struct GlobalStore
{
    static Store instance;

    static void initialize(const std::string& wd)
    {
        instance.working_directory = wd.substr(0, wd.rfind('/') + 1);
    }

    static void loadFont(const std::string& filepath, const std::string& name)
    {
        instance.loadFont(filepath, name);
    }

    static const sf::Font* getFont(const std::string& name)
    {
        return instance.getFont(name);
    }
};

template<int N>
Store GlobalStore<N>::instance;

using ResourceStore = GlobalStore<>;

