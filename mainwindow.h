#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QThread>
#include <QHeaderView>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_pushButton_clicked();

    void on_disconnect_clicked();

    void on_connect_clicked();

    void on_pushButton_3_clicked();

    void handleDatabaseList();
    void processData(const QByteArray &data);
    void handleCurrentDatabase();
    void onItemDoubleClicked(const QModelIndex &index);

    void handleDescribeResponse(const QByteArray &data);

    void handleSelectResponse(const QByteArray &data);

    void on_jiegou_clicked();

    void on_shuju_clicked();

private:
    Ui::MainWindow *ui;
    QTcpSocket* m_tcp;
    QString data1;
    QStandardItemModel *model;
    QByteArray receivedData;
    QString currentDatabase;
};
#endif // MAINWINDOW_H
