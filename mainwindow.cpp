#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("sql-minus客户端");
    ui->disconnect->setEnabled(false);
    ui->jiegou->setEnabled(false);
    ui->file->setEditTriggers(QAbstractItemView::NoEditTriggers);
    model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels({"数据库"}); // 设置列标题
    ui->file->setModel(model);    // 关联模型到视图


    m_tcp=new QTcpSocket(this);

    ui->msg->setTextInteractionFlags(Qt::TextEditorInteraction);

    // 2) 连接选区变化信号
    connect(ui->msg, &QTextEdit::selectionChanged,
            this, &MainWindow::onMsgSelectionChanged);


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

        if(receivedData.startsWith("SELECT_RESPONSE")){
            handleSelectResponse(receivedData);
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

void MainWindow::onMsgSelectionChanged() {
    // 读取选中文本（不含富文本标签）
    QString sel = ui->msg->textCursor().selectedText().trimmed();
    if (!sel.isEmpty()) {
        msg = sel;
        qDebug() << "选中文本：" << sel;
    }
    else msg=ui->msg->toPlainText();
}



void MainWindow::on_pushButton_clicked()
{
    onMsgSelectionChanged();
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
        data1 = index.data(Qt::DisplayRole).toString();
        qDebug() << "双击的节点:" << data1;
        // 在此添加自定义处理逻辑

        if (!currentDatabase.isEmpty() && !data1.isEmpty()) {
            if(!ui->jiegou->isEnabled()){
            QString request = QString("DESCRIBE %1;").arg(data1);
            m_tcp->write(request.toUtf8());  // 发送请求给服务器查询表结构
            }
            else{
            QString request = QString("SELECT * FROM %1;").arg(data1);
            m_tcp->write(request.toUtf8());
            }
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


void MainWindow::handleSelectResponse(const QByteArray &data) {
    // 清理旧模型
    QAbstractItemModel* oldModel = ui->table->model();
    if (oldModel) oldModel->deleteLater();

    QStandardItemModel *model = new QStandardItemModel(this);
    ui->table->setModel(model);

    QString text = QString::fromUtf8(data).trimmed();
    QStringList lines = text.split('\n', Qt::SkipEmptyParts);

    QStringList headers;
    bool isHeaderSet = false;
    bool isInDataSection = false; // 新增状态标记

    for (const QString &line : lines) {
        QString trimmedLine = line.trimmed();

        // 1. 识别分隔线（允许更灵活的分隔线格式）
        if (trimmedLine.startsWith('+') && trimmedLine.endsWith('+')) {
            if (!isHeaderSet) {
                // 第一个分隔线后的内容是表头
                isInDataSection = true;
            } else {
                // 第二个分隔线后的内容是数据
                isInDataSection = true;
            }
            continue;
        }

        // 2. 仅处理分隔线之间的内容
        if (!isInDataSection) continue;

        // 3. 分割字段并清理空白
        QStringList fields = trimmedLine.split('|', Qt::SkipEmptyParts);
        fields.erase(std::remove_if(fields.begin(), fields.end(),
                                    [](const QString& s){ return s.trimmed().isEmpty(); }),
                     fields.end());

        // 4. 处理表头或数据
        if (!isHeaderSet) {
            headers = fields;
            model->setHorizontalHeaderLabels(headers);
            isHeaderSet = true;
            isInDataSection = false; // 表头处理完毕，等待数据分隔线
            qDebug() << "[Header]" << headers;
        } else {
            if (fields.size() == headers.size()) {
                QList<QStandardItem*> rowItems;
                foreach (const QString& field, fields) {
                    QString cleanField = field.trimmed();
                    rowItems << new QStandardItem(cleanField);
                }
                model->appendRow(rowItems);
                qDebug() << "[Data Row]" << fields;
            } else {
                qWarning() << "列数不匹配，期望" << headers.size()
                        << "实际" << fields.size() << "内容:" << fields;
            }
        }
    }

    // 显示优化
    ui->table->resizeColumnsToContents();
    ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->table->update();
}


void MainWindow::on_jiegou_clicked()
{
    ui->jiegou->setEnabled(false);
    ui->shuju->setEnabled(true);
    if (!currentDatabase.isEmpty() && !data1.isEmpty()) {
            QString request = QString("DESCRIBE %1;").arg(data1);
            m_tcp->write(request.toUtf8());  // 发送请求给服务器查询表结构
    }

}


void MainWindow::on_shuju_clicked()
{
    ui->jiegou->setEnabled(true);
    ui->shuju->setEnabled(false);
    if (!currentDatabase.isEmpty() && !data1.isEmpty()) {
        QString request = QString("SELECT * FROM %1;").arg(data1);
        m_tcp->write(request.toUtf8());
    }

}


void MainWindow::on_pushButton_2_clicked()
{
    ui->msg->clear();
}


void MainWindow::on_choose_clicked()
{
    // 1. 弹出文件对话框，仅显示 .sql 文件
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("选择 SQL 脚本文件"),
        QDir::currentPath(),
        tr("SQL 文件 (*.sql);;所有文件 (*.*)")
        );  // :contentReference[oaicite:0]{index=0} :contentReference[oaicite:1]{index=1}

    // 2. 用户取消则直接返回
    if (fileName.isEmpty()) {
        return;
    }

    // 3. 打开文件，只读文本模式
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("打开失败"), file.errorString());
        return;
    }  // :contentReference[oaicite:2]{index=2} :contentReference[oaicite:3]{index=3}

    // 4. 读取全部内容到 QString
    QTextStream in(&file);
    QString sqlContent = in.readAll();    // :contentReference[oaicite:4]{index=4} :contentReference[oaicite:5]{index=5}

    ui->jiaoben->append(sqlContent);
    file.close();

}


void MainWindow::on_pushButton_4_clicked()
{
    m_tcp->write(ui->jiaoben->toPlainText().toUtf8());
    ui->record->append("客户端:"+ui->jiaoben->toPlainText());
}

