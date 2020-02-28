#include "mainwidget.h"
#include "ui_mainwidget.h"

#include <QNetworkAccessManager>
#include <QRegularExpression>   // 正则表达式
#include <QStandardItemModel>
#include <QNetworkRequest>
#include <QMessageBox>
#include <QEventLoop>
#include <QTreeView>
#include <QFile>
#include <QDir>

#if _MSC_VER >= 1600
    #pragma execution_character_set("utf-8")
#endif

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::getAllUrls()
{
    QRegularExpression re("(https?|ftp|file)://[-A-Za-z0-9+&@#/%?=~_|!:,.;]+[-A-Za-z0-9+&@#/%=~_|]");
    QRegularExpressionMatchIterator i=re.globalMatch(HtmlResponse);
    while (i.hasNext())
        allUrls.push_back(i.next().captured(0));
}

void MainWidget::getImageUrls()
{
    QRegularExpression re("<img.*?src=\"(?<url>(.*?))\"");
    QRegularExpressionMatchIterator i=re.globalMatch(HtmlResponse);
    while (i.hasNext())
        imageUrls.push_back(i.next().captured("url"));
}

void MainWidget::getMyDiliUserInfo(const QUrl url)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request(url);

    //设置Headers的信息
    request.setRawHeader("User-Agent", "MyOwnBrowser 1.0");

    connect(manager,SIGNAL(finished(QNetworkReply *)),this,
            SLOT(replyFinished(QNetworkReply *)));

    manager->get(request);  // 发送请求
}

void MainWidget::iniUrlsTreeView()
{
    /* 创建模型 */
    QStandardItemModel *model = new QStandardItemModel(ui->UrlsTreeView);
    model->setHorizontalHeaderLabels(QStringList()<<QString("Url"));
    model->setParent(this);

    /* 创建 Image Urls 条目对象 */
    QStandardItem *ImageUrlsItem = new QStandardItem(tr("Image Urls"));
    model->setItem(0,ImageUrlsItem);
    for(QUrl &url:imageUrls)
        model->item(0)->appendRow(new QStandardItem(url.toString()));

    /* 创建 All Urls 条目对象 */
    QStandardItem *AllUrlsItem = new QStandardItem(tr("All Urls"));
    model->setItem(1,AllUrlsItem);
    for(QUrl &url:allUrls)
        model->item(1)->appendRow(new QStandardItem(url.toString()));

    ui->UrlsTreeView->setModel(model);
}

void MainWidget::downlodaImage()
{
    int i=1;
    /* 判断路径是否存在 */
    QDir imageDir("./image");
    if(!imageDir.exists())
    {
        if(!imageDir.mkpath("./"))
        {
            QMessageBox::critical(this,"错误","文件夹创建失败");
            return;
        }
    }
    /* 遍历 URL 下载图片 */
    for(QUrl &imageUrl:imageUrls)
    {
        QNetworkAccessManager manager;
        QNetworkRequest request(imageUrl);

        QNetworkReply *reply = manager.get(request);    // 发送请求
        //开启一个局部的事件循环,等待响应结束，退出
        QEventLoop loop;
        connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        loop.exec();
        //判断是否出错,出错则结束
        if (reply->error() != QNetworkReply::NoError)
        {
            QMessageBox::critical(this,imageUrl.toString(),
                QString("下载失败:%1").arg(reply->errorString()));
            continue;
        }
        //保存文件
        QFile file(QString("./image/image_%1.%2").arg(i)
                   .arg(imageUrl.toString().split('.').last()));
        if(!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::critical(this,file.fileName(),
                QString("图片保存失败:%1").arg(file.errorString()));
            continue;
        }
        file.write(reply->readAll());
        file.close();
        reply->deleteLater();
        i++;
    }

    ui->DownloadBtn->setText("下载图片");
    ui->DownloadBtn->setEnabled(true);

    ui->StartBtn->setEnabled(true);

    QMessageBox::information(this,"提示","图片下载完成");
}

void MainWidget::on_StartBtn_clicked()
{
    if(ui->UrlLineEdit->text().isEmpty())
    {
        QMessageBox::warning(this,"错误","请输入URL地址");
        return;
    }
    this->getMyDiliUserInfo(ui->UrlLineEdit->text());
    ui->StartBtn->setText("正在抓取...");
    ui->StartBtn->setEnabled(false);
}

void MainWidget::replyFinished(QNetworkReply *reply)
{
    HtmlResponse.clear();
    while (!reply->atEnd())
        HtmlResponse+=reply->readAll();
    reply->deleteLater();

    ui->StartBtn->setText("开始抓取");
    ui->StartBtn->setEnabled(true);
    ui->DownloadBtn->setEnabled(true);

    this->getAllUrls();
    this->getImageUrls();
    this->iniUrlsTreeView();

    QMessageBox::information(this, "Info", "获取完毕");
}

void MainWidget::on_DownloadBtn_clicked()
{
    ui->DownloadBtn->setText("下载中...");
    ui->DownloadBtn->setEnabled(false);

    ui->StartBtn->setEnabled(false);

    this->downlodaImage();
}
