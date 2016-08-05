#include "loginwidget.h"
#include "mainwidget.h"

#include <QMessageBox>

LoginWidget::LoginWidget(QWidget *parent)
    : QWidget(parent)
{
    this->setWindowTitle( "VK Music Player" );
    m_view = new QWebEngineView(this);
    m_view->load(QUrl("https://oauth.vk.com/authorize?client_id=5571496&display=page&redirect_uri=https://oauth.vk.com/blank.html&scope=audio&response_type=token&v=5.53&state=mast123"));
    m_view->show();

    connect(m_view, SIGNAL(urlChanged(QUrl)), this, SLOT(tokenPageLoaded(QUrl)));
    connect(this, SIGNAL(tokenReady(QString,QString)), SLOT(tokenReady_slot(QString,QString)));
}

LoginWidget::~LoginWidget()
{

}

void LoginWidget::tokenReady_slot(const QString &token, const QString &user_id)
{
    MainWidget *mainWidget = new MainWidget(token, user_id);
    mainWidget->show();
    this->close();
}

void LoginWidget::tokenPageLoaded(const QUrl &url)
{
    if(url.hasFragment())
    {
        QString fragment = url.fragment();
        if(fragment.contains("error"))
        {
            QMessageBox::information(0, tr("Error"), tr("Try again!"));
            m_view->load(QUrl("https://oauth.vk.com/authorize?client_id=5571496&display=page&redirect_uri=https://oauth.vk.com/blank.html&scope=audio&response_type=token&v=5.53&state=mast123"));
        } else if(fragment.contains("access_token"))
        {
            QString token, userid;
            QStringList fragments = fragment.split('&');
            for(QString str : fragments)
            {
                if(str.contains("access_token"))
                {
                    token = str.split('=')[1];
                } else if(str.contains("user_id"))
                {
                    userid = str.split('=')[1];
                }
            }
            emit tokenReady(token, userid);
        }
    }
}
