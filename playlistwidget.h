#ifndef PLAYLISTWIDGET_H
#define PLAYLISTWIDGET_H

#include <QScrollArea>
#include <QVector>

#include "playlistmember.h"
#include "clickablelabel.h"

class QLabel;
class QScrollArea;
class QVBoxLayout;
class QMediaPlayer;
class QCheckBox;
class PlaylistWidget : public QScrollArea
{
    Q_OBJECT

public:
    explicit PlaylistWidget(QWidget *parent = 0);
    ~PlaylistWidget();

    void addPlaylistMember(PlaylistMember *member);
    void removeMember(int id);
    void clear();

    PlaylistMember* getCurrent() const;
    PlaylistMember* getPrevious() const;
    int getCurrentId() const;
    int getPreviousId() const;

    void setCurrent(int id);

private:
    QVector<PlaylistMember*> m_members;
    QVector<ClickableLabel*> m_labels;  //labels for all members

    QVBoxLayout *mainLayout;

    QWidget *m_container;

    int m_currentId;
    int m_prevId;

signals:
    void currentChanged(PlaylistMember* current);
    void cacheCheckboxStateChanged(bool checked);

public slots:
    void addNewMember(PlaylistMember *newMember);
    void searchMembersByName(const QString& name);

private slots:
    void updateHighlight(int new_id);
};

#endif // PLAYLISTWIDGET_H
