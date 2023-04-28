#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <chrono>
#include <QMessageBox>
#include "no_gui.h"
#include <QThread>
#include <QTimer>
#include <QDialogButtonBox>
#include <QFormLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(800, 440);

    this->setWindowTitle("Hasher");
    model = new QFileSystemModel(this);
    model->setFilter(QDir::AllEntries);
    model->setRootPath("");
    ui->list->setModel(model); //установка модели файловой сиситемы в виджет list
    ui->list2->setReadOnly(true);

}

MainWindow::~MainWindow()
{
    delete ui;
}


std::vector<Ihash *> alg_vec; //алгоритмы выбранные пользователем


void MainWindow::check_param(QString check) //проверка выбранных параметров
{
    if (ui->md5->isChecked() && check != "")
    {
        alg_vec.push_back(new MD5);
    }
    if (ui->sha256->isChecked() && check != "")
    {
        alg_vec.push_back(new SHA256);
    }
    if (ui->sha1->isChecked() && check != "")
    {
        alg_vec.push_back(new SHA1);
    }
    if (ui->crc32->isChecked() && check != "")
    {
        alg_vec.push_back(new CRC32);
    }
    if (ui->recursive->isChecked())
    {
        param = "rec";
    }
    else
    {
        param = "";
    }
    if (ui->separate->isChecked())
    {
        param2 = "sep";
    }
    else
    {
        param2 = "";
    }
    if (!ui->threads->text().isEmpty())
    {
        threads = ui->threads->text().toInt();
    }
    else
    {
        threads = 1;
    }
}

void MainWindow::set_log(std::string log) //вывод информации
{
    ui->list2->appendPlainText(QString::fromStdString(log));
    QTextCursor cur = ui->list2->textCursor();
    cur.movePosition(QTextCursor::End);
    ui->list2->setTextCursor(cur); //установка курсора в конец для удобства прокрутки
}


void MainWindow::on_list_doubleClicked(const QModelIndex &index)
{
    listview = (QListView *)sender(); //указатель на объект от которого поступил сигнал(т.е представление файловой системы)
    QFileInfo fileinfo = model->fileInfo(index);

    if (fileinfo.fileName() == "..")
    {
        QDir dir = fileinfo.dir();
        dir.cd("..");
        listview->setRootIndex(model->index(dir.absolutePath())); //через индекс модели перемещаемся по директориям
    }
    else if (fileinfo.fileName() == ".")
    {
        listview->setRootIndex(model->index(""));
    }
    else if (fileinfo.isDir())
    {
        listview->setRootIndex(index);
    }

}


void MainWindow::on_pushButton_clicked()
{
    ui->list2->clear();

 QDir dir = QDir(model->filePath(listview->currentIndex())); //получение пути к текущей директории
 check_param("write");

 start_hash(dir.absolutePath().toStdString(), alg_vec, param, param2, threads);
 alg_vec.clear();

}


void MainWindow::on_check_hash_clicked()
{
    ui->list2->clear();

    QDir dir = QDir(model->filePath(listview->currentIndex()));
    check_param();
    check_hash(param, param2, dir.absolutePath().toStdString(), threads);

}


void MainWindow::on_check_files_clicked()
{
    ui->list2->clear();

    QDir dir = QDir(model->filePath(listview->currentIndex()));
    check_param();
    check_file(param, param2, dir.absolutePath().toStdString());

}


void MainWindow::on_delete_2_clicked()
{
    ui->list2->clear();

    QDialog confirmed;
    confirmed.setModal(true);
    QLabel *line = new QLabel(&confirmed); //текст
    QDialogButtonBox *btn_box = new QDialogButtonBox(&confirmed);
    btn_box->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(btn_box, &QDialogButtonBox::accepted, &confirmed, &QDialog::accept);
    connect(btn_box, &QDialogButtonBox::rejected, &confirmed, &QDialog::reject);
    //создание и соединение с сигналами. connect(объект, сигнал, объект2, сигнал2)

    QFormLayout *layout = new QFormLayout(); //для более корректного отображения
    layout->addRow(tr("Confirm?"), line);   //tr translate
    layout->addWidget(btn_box);

    confirmed.setLayout(layout);

    if(confirmed.exec() == QDialog::Accepted)
    {
        QDir dir = QDir(model->filePath(listview->currentIndex()));
        delete_hash(dir.absolutePath().toStdString());
        ui->list2->setPlainText("clear");
    }
    //что интересно, при выходе из функции QDialog confirmed уничтожится, а за ним и QLabel *line
    //и QDialogButtonBox *btn_box так они находятся в layout а он тем временем установлен в confirmed
    //в итоге вроде бы утечек быть не должно)
}

