#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("sql-minus客户端");
    ui->disconnect->setEnabled(false);
    ui->file->setEditTriggers(QAbstractItemView::NoEditTriggers);
    model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels({"数据库"}); // 设置列标题
    ui->file->setModel(model); // 关联模型到视图

    m_tcp=new QTcpSocket(this);

    connect(ui->file, &QTreeView::doubleClicked, this, &MainWindow::onItemDoubleClicked);


    connect(m_tcp,&QTcpSocket::readyRead,this,[=](){
        receivedData=m_tcp->readAll();
        ui->record->append("服务端:"+receivedData);

        if (receivedData.startsWith("数据库使用成功")) {
            handleCurrentDatabase();
            receivedData.clear();
            return;
        }

        if (receivedData.endsWith("\n\n")) {
            handleDatabaseList();
            ui->pushButton_3->setEnabled(true); // 恢复按钮
        }

        if (receivedData.startsWith("DESCRIBE_RESPONSE")) {
            handleDescribeResponse(receivedData);
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
    model->setHorizontalHeaderLabels({"数据库", "表名"}); // 设置列标题为 "数据库" 和 "表名"

    QStandardItem *currentDbItem = nullptr;

    for (const QString &line : lines) {
        // 如果是数据库名（没有缩进）
        if (!line.startsWith("\t")) {
            // 新建数据库节点
            currentDbItem = new QStandardItem(line);
            model->appendRow(currentDbItem);
        }
        // 如果是表名（以制表符开始）
        else if (line.startsWith("\t") && currentDbItem) {
            QString tableName = line.mid(1);  // 去掉前面的制表符
            QStandardItem *tableItem = new QStandardItem(tableName);
            currentDbItem->appendRow(tableItem);  // 将表加入当前数据库下
        }
    }

    ui->record->append(QString("成功加载 %1 个数据库").arg(lines.size()));
}



void MainWindow::handleCurrentDatabase() {
    QString text = QString::fromUtf8(receivedData);
    const QString prefix = QStringLiteral("数据库使用成功");

    if (text.startsWith(prefix)) {
        // 去掉前缀
        QString remainder = text.mid(prefix.length()).trimmed();
        if (remainder.startsWith(':'))
            remainder = remainder.mid(1).trimmed();

        currentDatabase = remainder;
        ui->record->append(QStringLiteral("当前使用的数据库：%1").arg(currentDatabase));
    }
}

void MainWindow::onItemDoubleClicked(const QModelIndex &index) {
    if (index.isValid()) {
        QString data = index.data(Qt::DisplayRole).toString();
        qDebug() << "双击的节点:" << data;
        // 在此添加自定义处理逻辑

        if (!currentDatabase.isEmpty() && !data.isEmpty()) {
            QString request = QString("DESCRIBE %1;").arg(data);
            m_tcp->write(request.toUtf8());  // 发送请求给服务器查询表结构
        }
    }
}

void MainWindow::handleDescribeResponse(const QByteArray &data) {
    // 假设服务器返回的数据格式如下：
    // +---------------------+------------+----------+----------------+
    // | Field Name          | Type       | Length   | Constraints    |
    // +---------------------+------------+----------+----------------+
    // | id                  | INTEGER    | 4        | PK             |
    // | name                | VARCHAR    | 255      | NN             |
    // +---------------------+------------+----------+----------------+

    QString text = QString::fromUtf8(data);
    QStringList lines = text.split('\n', Qt::SkipEmptyParts);

    // 清除旧的表数据
    ui->table->clearSpans();
    QStandardItemModel *model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels({"字段", "类型", "长度", "约束条件"});  // 表头

    // 解析表结构数据
    for (const QString &line : lines) {
        QStringList fieldInfo = line.split('|', Qt::SkipEmptyParts);
        if (fieldInfo.size() == 4) {
            // 创建表格行
            QList<QStandardItem *> row;
            for (const QString &item : fieldInfo) {
                row.append(new QStandardItem(item.trimmed()));
            }
            model->appendRow(row);
        }
    }

    // 将表格模型设置到 QTableView 中显示
    ui->table->setModel(model);
}
