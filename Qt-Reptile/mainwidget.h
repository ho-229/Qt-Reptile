#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QList>
#include <QWidget>
#include <QNetworkReply>

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

private:
    Ui::MainWidget *ui;

    QString HtmlResponse;
    QList<QUrl>imageUrls;
    QList<QUrl>allUrls;

    inline void getAllUrls();           // 获取全部URL
    inline void getImageUrls();         // 获取图片URL
    void getMyDiliUserInfo(const QUrl url);   // 获取网页源代码
    void iniUrlsTreeView();             // 初始化 QTreeView
    void downlodaImage();               // 下载图片

private slots:

    void on_StartBtn_clicked();

    void on_DownloadBtn_clicked();

    /* 自定义信号槽 */
    void replyFinished(QNetworkReply *);

};

#endif // MAINWIDGET_H
