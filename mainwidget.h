#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QVector>
#include <QQueue>
#include <QMediaPlayer>

#include "playlistmember.h"
#include "playlistwidget.h"
#include "imagebutton.h"

class QLabel;
class QPushButton;
class QNetworkReply;
class QMediaPlaylist;
class QSlider;
class QCheckBox;
class QMediaContent;
class QLineEdit;
class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(const QString &token, const QString &user_id, QWidget *parent = 0);
    ~MainWidget();

private:
    QString m_user_id;
    QString m_token;

    QVector<PlaylistMember*> m_playlist;
    PlaylistWidget *m_playlistWidget;

    QLabel *m_currentLabel;
    ImageButton *m_playPauseButton;

    QMediaPlayer *m_player;
    QMediaPlaylist *m_mediaWebPlaylist;

    QSlider *m_seekslider;
    QSlider *m_volumeSlider;

    QLabel *m_timeLabel;
    QLabel *m_volumeLabel;

    QQueue<QUrl> m_downloadQueue;

    QCheckBox *m_cacheCurrent;

    QLineEdit *m_searchEdit;

    bool m_removeCurrent;
    bool m_playState;

    void loadUserPlaylist(const QString &user_id = QString(""));

signals:
    void playlistReady();
    void playStateChanged(bool newState);

private slots:
    void playlistReady_slot(QNetworkReply *reply);
    void fillPlaylistWidget_slot();
    void playStateChanged_slot(bool newState);
    void playOrPause();
    void currentChanged_slot(PlaylistMember *newCurrent);
    void mediaStatusChanged_slot(QMediaPlayer::MediaStatus status);
    void updateSlider(qint64 time);
    void cacheCurrentCheckboxChecked(bool checked);
    void tryToStartDownload();
    void downloadFinished(QNetworkReply *reply);
};

#endif // MAINWIDGET_H
