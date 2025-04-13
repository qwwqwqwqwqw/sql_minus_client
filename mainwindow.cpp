#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("sql-minus客户端");
    ui->disconnect->setEnabled(false);
    model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels({"数据库"}); // 设置列标题
    ui->file->setModel(model); // 关联模型到视图

    m_tcp=new QTcpSocket(this);


    connect(m_tcp,&QTcpSocket::readyRead,this,[=](){
        receivedData=m_tcp->readAll();
        ui->record->append("服务端:"+receivedData);

        if (receivedData.endsWith("\n\n")) {
            handleDatabaseList();
            ui->pushButton_3->setEnabled(true); // 恢复按钮
        }
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

    connect(m_tcp, &QTcpSocket::errorOccurred, this, [=](QAbstractSocket::SocketError error) {
        Q_UNUSED(error);
        ui->record->append("错误: " + m_tcp->errorString());
        ui->pushButton_3->setEnabled(true); // 错误时恢复按钮
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


void MainWindow::on_pushButton_3_clicked() {
    QString msg_1 = "1";
    m_tcp->write(msg_1.toUtf8());
    ui->pushButton_3->setEnabled(false);
    receivedData.clear(); // 准备接收新数据
}

void MainWindow::handleDatabaseList() {
    // 假设数据以 \n 作为结束符（与服务器协议一致）
    if (receivedData.contains("\n")) {
        // 提取完整数据（去除结束符）
        int endPos = receivedData.lastIndexOf("\n");
        QByteArray completeData = receivedData.left(endPos);
        receivedData = receivedData.mid(endPos + 1); // 保留未处理数据

        processData(completeData);
        ui->pushButton_3->setEnabled(true); // 恢复按钮
    }
}


void MainWindow::processData(const QByteArray &data) {
    QString text = QString::fromUtf8(data).trimmed();
    QStringList lines = text.split('\n', Qt::SkipEmptyParts);

    model->clear();
    model->setHorizontalHeaderLabels({"数据库"}); // 重新设置表头

    for (const QString &line : lines) {
        QStandardItem *item = new QStandardItem(line);
        model->appendRow(item);
    }

    ui->record->append(QString("成功加载 %1 个数据库").arg(lines.size()));
}

