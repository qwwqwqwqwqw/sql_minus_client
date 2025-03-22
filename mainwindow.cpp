#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("sql-minus客户端");
    ui->disconnect->setEnabled(false);

    m_tcp=new QTcpSocket(this);

    connect(m_tcp,&QTcpSocket::readyRead,this,[=](){
        QByteArray data=m_tcp->readAll();
        ui->record->append("服务端:"+data);
    });

    connect(m_tcp, &QTcpSocket::disconnected, this, [=]()
            {
                m_tcp->close();
                m_tcp->deleteLater(); // delete m_tcp
                ui->record->append("服务器已经和客户端断开了连接...");
                ui->connect->setEnabled(true);
                ui->disconnect->setEnabled(false);
            });

    connect(m_tcp, &QTcpSocket::connected, this, [=](){
        ui->record->append("已经成功连接到了服务器...");
        ui->connect->setEnabled(false);
        ui->disconnect->setEnabled(true);

    });

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    QString msg=ui->msg->toPlainText();
    m_tcp->write(msg.toUtf8());
    ui->record->append("客户端:"+msg);
}


void MainWindow::on_disconnect_clicked()
{
    m_tcp->close();
}


void MainWindow::on_connect_clicked()
{
    QString ip=ui->ip->text();
    unsigned short port=ui->port->text().toUShort();
    m_tcp->connectToHost(QHostAddress(ip),port);
}

