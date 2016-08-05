#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>
#include <QWebEngineView>

class LoginWidget : public QWidget
{
    Q_OBJECT

public:
    LoginWidget(QWidget *parent = 0);
    ~LoginWidget();

private:
    QWebEngineView *m_view;

signals:
    void tokenReady(const QString &token, const QString &user_id);

private slots:
    void tokenPageLoaded(const QUrl &url);
    void tokenReady_slot(const QString &token, const QString &user_id);
};

#endif // LOGINWIDGET_H
