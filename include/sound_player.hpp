#ifndef SOUNDPLAYER_HPP_INCLUDED
#define SOUNDPLAYER_HPP_INCLUDED

#include <SFML/Audio.hpp>
#include <string>
#include <list>

struct SoundHandler
{
    size_t maxLivingSounds;
    sf::SoundBuffer soundBuffer;
    std::list<sf::Sound> livingSounds;

    void update();
};

class SoundPlayer
{
public:
	static void      playInstanceOf(size_t soundID, float volume = 20.0f);
	static void      setPitch(float pitch);
    static size_t    registerSound(const std::string& filename, size_t maxSounds = 10);
    static sf::Sound getInstanceOf(size_t soundID);

private:
    static std::vector<SoundHandler> _buffers;
	static float s_pitch;
};

#endif // SOUNDPLAYER_HPP_INCLUDED
