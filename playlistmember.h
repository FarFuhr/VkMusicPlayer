#ifndef PLAYLISTMEMBER_H
#define PLAYLISTMEMBER_H

#include <QString>

struct PlaylistMember
{
    PlaylistMember(){}
    PlaylistMember(QString _artist, QString _title, QString _url, long _duration, bool _cached) : artist(_artist), title(_title), url(_url), duration(_duration), cached(_cached) {
        localfile = QString("./cache/")+url.split('/').at(url.split('/').size()-1);
    }

    QString artist;
    QString title;
    QString url;
    QString localfile;
    long duration;
    bool cached;
};

#endif // PLAYLISTMEMBER_H
