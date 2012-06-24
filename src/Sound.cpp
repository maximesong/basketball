#include "Sound.h"

Sound::Sound()
{
#ifdef __linux__
	m_player = Phonon::createPlayer(Phonon::MusicCategory, QString(""));
#endif	
}

Sound::~Sound()
{
#ifdef __linux__
	delete m_player;
#endif
}
void Sound::updateSource(const QString &name, const QString &filename)
{
	m_sources[name] = filename;
}

void Sound::play(const QString &name)
{
#ifdef __linux__
	m_player->setCurrentSource(m_sources[name]);
	m_player->play();
#else
	QSound::play(m_sources[name]);
#endif
}
