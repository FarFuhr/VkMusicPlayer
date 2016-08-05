#include "playlistwidget.h"

#include <QVBoxLayout>
#include <QScrollArea>

#include <QDebug>

#include <QDir>

#include <QCheckBox>

PlaylistWidget::PlaylistWidget(QWidget *parent)
    : QScrollArea(parent), m_currentId(0), m_prevId(0)
{
    m_container = new QWidget;

    mainLayout = new QVBoxLayout;

    this->setWidgetResizable(true);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    m_container->setLayout(mainLayout);
    this->setWidget(m_container);
}

PlaylistWidget::~PlaylistWidget()
{
    for(PlaylistMember *member : m_members)
        delete member;
    for(QLabel *label : m_labels)
        label->deleteLater();
}

void PlaylistWidget::addNewMember(PlaylistMember *newMember)
{
    ClickableLabel *newLabel = new ClickableLabel(m_labels.size(), newMember->artist+" - "+newMember->title);
    newLabel->setAlignment(Qt::AlignCenter);
    newLabel->setMouseTracking(true);
    m_labels.push_back(newLabel);

    bool cached = false;
    QFileInfo info(newMember->localfile);
    if(info.exists() && info.isFile())
    {
        cached = true;
    }
    newMember->cached = cached;
    m_members.push_back(newMember);

    connect(newLabel, SIGNAL(doubleClicked(int)), this, SLOT(updateHighlight(int)));

    mainLayout->insertWidget(mainLayout->count(), newLabel);
}

void PlaylistWidget::removeMember(int id)
{
    if(m_currentId != id)
    {
        mainLayout->removeWidget(m_labels[id]);

        delete m_members[id];
        m_members.remove(id);
        m_labels[id]->deleteLater();
        m_labels.remove(id);
    } else
    {
        qDebug() << "Can't remove: Current";
    }
}

PlaylistMember* PlaylistWidget::getCurrent() const
{
    return m_members[m_currentId];
}

int PlaylistWidget::getCurrentId() const
{
    return m_currentId;
}

void PlaylistWidget::setCurrent(int id)
{
    if(id < m_members.size() && id >= 0)
    {
        updateHighlight(id);
    } else
    {
        //do something?
        qDebug() << "Error: id out of range (m_members.size() <= id)";
    }
}

PlaylistMember* PlaylistWidget::getPrevious() const
{
    return m_members[m_prevId];
}

int PlaylistWidget::getPreviousId() const
{
    return m_prevId;
}

void PlaylistWidget::updateHighlight(int new_id)
{
    m_prevId = m_currentId;
    m_currentId = new_id;

    m_labels[m_prevId]->unselect();
    m_labels[m_currentId]->select();

    QFile file(m_members[new_id]->localfile);
    m_members[new_id]->cached = file.exists();

    emit currentChanged(m_members[new_id]);
}

void PlaylistWidget::clear()
{
    for(auto i : m_labels)
        delete i;
    for(auto i : m_members)
        delete i;

    m_labels.clear();
    m_members.clear();
}

void PlaylistWidget::searchMembersByName(const QString &name)
{
    for(int i = 0; i < m_labels.size(); ++i)
        m_labels[i]->hide();

    if(name.isEmpty())
        for(int i = 0; i < m_labels.size(); ++i)
            m_labels[i]->show();
    else
    {
        for(int i = 0; i < m_members.size(); ++i)
            if(QString(m_members[i]->artist+" - "+m_members[i]->title).contains(name, Qt::CaseInsensitive))
                m_labels[i]->show();
    }
}
