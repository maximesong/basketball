#ifndef _SOUND_H_
#define _SOUND_H_

#include <QMap>

#ifdef __linux__
#include <phonon/mediaobject.h>
#else
#include <QSound>
#endif

class Sound {
public:
	Sound();
	~Sound();
	void updateSource(const QString &name, const QString &filename);
	void play(const QString &name);
private:
	QMap<QString, QString> m_sources;
#ifdef __linux__
	Phonon::MediaObject *m_player;
#endif
};

#endif /* _SOUND_H_ */

