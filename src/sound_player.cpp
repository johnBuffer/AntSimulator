#include "sound_player.hpp"


std::vector<SoundHandler> SoundPlayer::_buffers;
float SoundPlayer::s_pitch = 1.0f;

void SoundHandler::update()
{
    livingSounds.remove_if( [](sf::Sound& s){return s.getStatus() == sf::Sound::Stopped;} );
}

size_t SoundPlayer::registerSound(const std::string& filename, size_t maxSounds)
{
    _buffers.push_back(SoundHandler());
    SoundHandler& handler = _buffers.back();
    handler.soundBuffer.loadFromFile(filename);
    handler.maxLivingSounds = maxSounds;

    return _buffers.size()-1;
}

void SoundPlayer::playInstanceOf(size_t soundID, float volume)
{
    SoundHandler& handler = _buffers[soundID];
    std::list<sf::Sound>& soundList(handler.livingSounds);

    soundList.push_back(sf::Sound());
    sf::Sound& newSound = soundList.back();
    newSound.setBuffer(_buffers[soundID].soundBuffer);
	newSound.setVolume(volume);
	newSound.setPitch(s_pitch);
    newSound.play();

    if (soundList.size() > handler.maxLivingSounds)
    {
        soundList.front().stop();
        soundList.pop_front();
    }

    handler.update();
}

void SoundPlayer::setPitch(float pitch)
{
	s_pitch = pitch;
	for (SoundHandler& sh : _buffers) {
		for (sf::Sound& snd : sh.livingSounds) {
			snd.setPitch(pitch);
		}
	}
}

sf::Sound SoundPlayer::getInstanceOf(size_t soundID)
{
    sf::Sound sound(_buffers[soundID].soundBuffer);
    return sound;
}
