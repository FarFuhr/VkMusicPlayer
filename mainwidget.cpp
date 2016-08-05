#include "mainwidget.h"

#include <QGridLayout>
#include <QVBoxLayout>

#include <QUrl>
#include <QUrlQuery>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>

#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QCheckBox>
#include <QLineEdit>

#include <QMediaPlayer>
#include <QMediaPlaylist>

#include <QDir>

MainWidget::MainWidget(const QString &token, const QString &user_id, QWidget *parent)
    : QWidget(parent), m_token(token), m_user_id(user_id), m_playState(false)
{
    m_removeCurrent = false;
    m_player = new QMediaPlayer;
    m_player->setVolume(100);

    m_mediaWebPlaylist = new QMediaPlaylist(m_player);
    m_mediaWebPlaylist->setObjectName("web");
    m_player->setPlaylist(m_mediaWebPlaylist);

    loadUserPlaylist();
    this->resize(800, 600);
    this->setWindowTitle("VK Music Player");


    QDir dir("./cache/");
    if(!dir.exists())
        dir.mkpath(".");

    QGridLayout* mainLayout = new QGridLayout;

    m_currentLabel = new QLabel;
    m_currentLabel->setFont(QFont("Arial", 16));
    m_currentLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    mainLayout->addWidget(m_currentLabel, 0, 0, 1, 9, Qt::AlignCenter);

    ImageButton *prevButton = new ImageButton("prevbutton.png");
    mainLayout->addWidget(prevButton, 2, 3, 1, 1, Qt::AlignTop | Qt::AlignRight);

    m_playPauseButton = new ImageButton("playbutton.png");
    mainLayout->addWidget(m_playPauseButton, 2, 4, 1, 1, Qt::AlignTop);

    ImageButton *nextButton = new ImageButton("nextbutton.png");
    mainLayout->addWidget(nextButton, 2, 5, 1, 1, Qt::AlignTop | Qt::AlignLeft);

    m_playlistWidget = new PlaylistWidget;
    mainLayout->addWidget(m_playlistWidget, 3, 0, 3, 9);

    m_seekslider = new QSlider;
    m_seekslider->setOrientation(Qt::Horizontal);
    mainLayout->addWidget(m_seekslider, 1, 0, 1, 7);

    m_volumeSlider = new QSlider;
    m_volumeSlider->setOrientation(Qt::Horizontal);
    m_volumeSlider->setMaximum(100);
    m_volumeSlider->setValue(100);
    m_volumeSlider->setSingleStep(1);
    mainLayout->addWidget(m_volumeSlider, 1, 7, 1, 2);

    m_timeLabel = new QLabel;
    m_timeLabel->setText("0:00");
    mainLayout->addWidget(m_timeLabel, 2, 0, 1, 2, Qt::AlignTop);

    m_volumeLabel = new QLabel;
    m_volumeLabel->setText(QString::number(m_player->volume())+"%");
    mainLayout->addWidget(m_volumeLabel, 2, 7, 1, 2, Qt::AlignTop | Qt::AlignHCenter);

    m_cacheCurrent = new QCheckBox(tr("&Save"));
    mainLayout->addWidget(m_cacheCurrent, 2, 7, 1, 2, Qt::AlignBottom | Qt::AlignRight);

    m_searchEdit = new QLineEdit;
    m_searchEdit->setPlaceholderText(tr("Search"));
    mainLayout->addWidget(m_searchEdit, 2, 0, 1, 8, Qt::AlignBottom);

    this->setLayout(mainLayout);

    connect(this, SIGNAL(playlistReady()),
            SLOT(fillPlaylistWidget_slot()));
    connect(this, SIGNAL(playStateChanged(bool)),
            SLOT(playStateChanged_slot(bool)));

    connect(m_playPauseButton, SIGNAL(pressed()), this, SLOT(playOrPause()));

    connect(nextButton, &ImageButton::pressed,
            [=](){
                m_playlistWidget->setCurrent(m_playlistWidget->getCurrentId()+1);
                m_player->play();
                emit playStateChanged(true);
            });

    connect(prevButton, &ImageButton::pressed,
            [=](){
                m_playlistWidget->setCurrent(m_playlistWidget->getCurrentId()-1);
                m_player->play();
                emit playStateChanged(true);
            });


    connect(m_player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
            this, SLOT(mediaStatusChanged_slot(QMediaPlayer::MediaStatus)));

    connect(m_playlistWidget, SIGNAL(currentChanged(PlaylistMember*)),
            this, SLOT(currentChanged_slot(PlaylistMember*)));

    connect(m_player, SIGNAL(positionChanged(qint64)),
            this, SLOT(updateSlider(qint64)));

    connect(m_player, &QMediaPlayer::volumeChanged,
            [=](int v){ m_volumeLabel->setText(QString::number(v)+"%"); });

    connect(m_seekslider, &QSlider::sliderReleased,
            [=](){ m_player->setPosition(m_seekslider->value()); } );
    connect(m_seekslider, &QSlider::sliderMoved,
            [=](int time){
                m_timeLabel->setText(QString("%1:%2").arg((int)time/1000/60).arg(time/1000-time/1000/60*60 < 10 ?
                                                                                     QString("0")+QString::number(time/1000-time/1000/60*60) : QString::number(time/1000-time/1000/60*60)));
            });

    connect(m_volumeSlider,
            &QSlider::valueChanged,
            [=](int v){ m_player->setVolume(v); });

    connect(m_cacheCurrent, SIGNAL(toggled(bool)),
            this, SLOT(cacheCurrentCheckboxChecked(bool)));

    connect(m_searchEdit, SIGNAL(textChanged(QString)),
            m_playlistWidget, SLOT(searchMembersByName(QString)));
}

MainWidget::~MainWidget()
{
    m_player->deleteLater();
}

void MainWidget::loadUserPlaylist(const QString &user_id)
{
    QString loadUserID = (user_id.isEmpty() ? m_user_id : user_id);

    QUrlQuery requestQuery;
    requestQuery.addQueryItem("owner_id", loadUserID);
    requestQuery.addQueryItem("count", "6000");
    requestQuery.addQueryItem("access_token", m_token);
    requestQuery.addQueryItem("v", "5.53");

    QUrl requestUrl = "https://api.vk.com/method/audio.get";
    requestUrl.setQuery(requestQuery);

    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    mgr->get(QNetworkRequest(requestUrl));
    connect(mgr, SIGNAL(finished(QNetworkReply*)), this, SLOT(playlistReady_slot(QNetworkReply*)));
}

void MainWidget::playlistReady_slot(QNetworkReply *reply)
{
    QByteArray data = reply->readAll();

    if(!data.isEmpty())
    {
        QJsonObject jsonObj = QJsonDocument::fromJson(data).object();
        QJsonArray responseArray = jsonObj["response"].toObject()["items"].toArray();
        m_playlistWidget->clear();

        if(!responseArray.isEmpty())
        {
            for(long i = 0; i < responseArray.size(); ++i)
            {
                QJsonObject currentVal = responseArray[i].toObject();

                m_playlist.push_back(new PlaylistMember(currentVal["artist"].toString(), currentVal["title"].toString(), currentVal["url"].toString().split('?')[0], currentVal["duration"].toInt(), false));
                m_playlistWidget->addNewMember(m_playlist[m_playlist.size()-1]);
                m_mediaWebPlaylist->addMedia(QUrl(m_playlist.at(i)->url));
            }
        }
        m_playlistWidget->setCurrent(0);
        playStateChanged_slot(false);
        emit playlistReady();
    }
    reply->deleteLater();
}

void MainWidget::fillPlaylistWidget_slot()
{

}

void MainWidget::playStateChanged_slot(bool newState)
{
    m_playState = newState;
    m_playPauseButton->loadPixmap(newState ? "pausebutton.png" : "playbutton.png");
}

void MainWidget::playOrPause()
{
    if(m_playState)
        m_player->pause();
    else
        m_player->play();

    emit playStateChanged(!m_playState);
}

void MainWidget::currentChanged_slot(PlaylistMember *newCurrent)
{
    if(m_removeCurrent)
    {
        QFile file(m_playlistWidget->getPrevious()->url.split('/').at(m_playlistWidget->getPrevious()->url.split('/').size()-1));
        file.remove();
        m_removeCurrent = false;
    }
    m_cacheCurrent->setChecked(newCurrent->cached);

    if(newCurrent->cached)
        m_player->setMedia(QUrl::fromLocalFile(newCurrent->localfile));
    else
        m_player->setPlaylist(m_mediaWebPlaylist);

    m_currentLabel->setText(newCurrent->artist+" - "+newCurrent->title);
    m_mediaWebPlaylist->setCurrentIndex(m_playlistWidget->getCurrentId());
    m_seekslider->setMaximum(newCurrent->duration*1000);
    m_seekslider->setValue(0);

    if(m_playState)
    {
        emit playStateChanged(true);
        m_player->play();
    }
    else
        emit playStateChanged(false);
}

void MainWidget::mediaStatusChanged_slot(QMediaPlayer::MediaStatus status)
{
    if(status == QMediaPlayer::EndOfMedia)
        m_playlistWidget->setCurrent(m_playlistWidget->getCurrentId()+1);
}

void MainWidget::updateSlider(qint64 time)
{
    if(!m_seekslider->isSliderDown())
    {
        m_timeLabel->setText(QString("%1:%2").arg((int)time/1000/60).arg(time/1000-time/1000/60*60 < 10 ? QString("0")+QString::number(time/1000-time/1000/60*60) : QString::number(time/1000-time/1000/60*60)));
        m_seekslider->setValue(time);
    }
}

void MainWidget::cacheCurrentCheckboxChecked(bool checked)
{
    if(checked)
    {
        if(!m_playlistWidget->getCurrent()->cached)
            m_downloadQueue.enqueue(m_playlistWidget->getCurrent()->url);
        m_removeCurrent = false;
        m_playlistWidget->getCurrent()->cached = true;
        tryToStartDownload();
    }
    else
    {
        m_removeCurrent = true;
    }
}

void MainWidget::tryToStartDownload()
{
    if(!m_downloadQueue.isEmpty())
    {
        QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
        connect(mgr, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(downloadFinished(QNetworkReply*)));
        mgr->get(QNetworkRequest(QUrl(m_downloadQueue.dequeue())));
    }
}

void MainWidget::downloadFinished(QNetworkReply *reply)
{
    QFile file("./cache/"+reply->url().toString().split('/').at(reply->url().toString().split('/').size()-1));
    if(file.open(QFile::WriteOnly))
    {
        file.write(reply->readAll());
        file.flush();
        file.close();
    }
    reply->deleteLater();
}
