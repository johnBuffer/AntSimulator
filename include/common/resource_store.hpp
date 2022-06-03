#pragma once
#include <string>
#include <memory>
#include <SFML/Graphics/Font.hpp>

struct Store
{
    std::string working_directory;
    std::map<std::string, std::unique_ptr<sf::Font>>    fonts;
    std::map<std::string, std::unique_ptr<sf::Texture>> textures;

    void loadFont(const std::string& filepath, const std::string& name)
    {
        fonts[name] = std::make_unique<sf::Font>();
        fonts[name]->loadFromFile(getFullPath(filepath));
    }

    void loadTexture(const std::string& filepath, const std::string& name)
    {
        textures[name] = std::make_unique<sf::Texture>();
        textures[name]->loadFromFile(getFullPath(filepath));
        textures[name]->setSmooth(true);
    }

    [[nodiscard]]
    const sf::Font* getFont(const std::string& name) const
    {
        return &(*fonts.at(name));
    }

    [[nodiscard]]
    const sf::Texture* getTexture(const std::string& name) const
    {
        return &(*textures.at(name));
    }

    [[nodiscard]]
    std::string getFullPath(const std::string& filepath) const
    {
        return working_directory + filepath;
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

    static void loadTexture(const std::string& filepath, const std::string& name)
    {
        instance.loadTexture(filepath, name);
    }

    static const sf::Font* getFont(const std::string& name)
    {
        return instance.getFont(name);
    }

    static const sf::Texture* getTexture(const std::string& name)
    {
        return instance.getTexture(name);
    }
};

template<int N>
Store GlobalStore<N>::instance;

using ResourceStore = GlobalStore<>;

