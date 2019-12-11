#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QDebug>


namespace Ui {
class MainWindow;
}




class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_toExcleBtn_clicked();

    void on_toJsonBtn_clicked();

private:

    Ui::MainWindow *ui;
    QByteArray  allData;
    QJsonValue jsonData;

    QString    m_pSavePath;


    bool analysisJson();
    bool cvsData2json();
    bool getFileData(bool isJson);

    QString GetCurrentDateTime();

    void write_csv(QString path);
    void write_json(QString path);

    void createFile(QString filePath,QString fileName);





};

#endif // MAINWINDOW_H
