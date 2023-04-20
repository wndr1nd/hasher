#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QDebug>
#include <QFileSystemModel>
#include <QDir>
#include <QWidget>
#include <QtWidgets/QListView>
#include <iostream>
#include <string>
#include <QDataStream>
#include "md5.h"
#include "sha256.h"
#include "crc32.h"
#include "sha1.h"
#include <QThread>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    void confirm();
    void check_param(QString check = "");
    void set_log(std::string log);


    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    std::string param;
    std::string param2;
    int threads = 1;

signals:
    void message(std::string str);

public slots:
    void on_list_doubleClicked(const QModelIndex &index);

    void on_pushButton_clicked();

private slots:
    void on_check_hash_clicked();

    void on_check_files_clicked();

    void on_delete_2_clicked();

private:
    Ui::MainWindow *ui;
    QFileSystemModel *model;
    QListView *listview;
    QFileInfoList listfile;
    QThread m_thread;
};
#endif // MAINWINDOW_H
