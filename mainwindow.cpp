#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QTime>
#include <QDir>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_toExcleBtn_clicked()
{
    if(getFileData(true))
    {
        assert(cvsData2json());
        analysisJson();
        QString data = GetCurrentDateTime();
        QString savePath = data+".csv";
        write_csv(savePath);
    }
}


void MainWindow::on_toJsonBtn_clicked()
{
    if(getFileData(false))
    {
        assert(cvsData2json());
        QString data = GetCurrentDateTime();
        QString savePath = data+".json";
        write_json(savePath);
    }
}


void MainWindow::write_json(QString path)
{
    assert(!jsonData.isNull());
    createFile("./JSON/",path);
    QFile file("./JSON/"+path);

    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonData.toObject());

    if(!file.open(QIODevice::ReadWrite)) {
        qDebug() << "File open error";
    } else {
        qDebug() <<"File open!";
    }
    file.write(jsonDoc.toJson());
    file.close();
     QMessageBox::information(NULL,"","Convert excle success! \n \n "+m_pSavePath+"/JSON/"+path);
}

void MainWindow::write_csv(QString path)
{
    assert(!jsonData.isNull());
    createFile("./CSV/",path);

    QFile outFile("./CSV/"+path);
    assert(outFile.open(QIODevice::WriteOnly));
    QStringList lines;

    QJsonObject jsonLang = jsonData.toObject();
    QStringList keys = jsonLang.keys();
    for(int i=0;i<keys.length();i++)
    {
       QString item = keys[i]+","+jsonLang[keys[i]].toString()+"\n";
       outFile.write(item.toStdString().c_str());
    }
    outFile.close();

    QMessageBox::information(NULL,"","Convert excle success! \n \n "+m_pSavePath+"/CSV/"+path);
}






QString MainWindow::GetCurrentDateTime()
{
    QDate date(QDate::currentDate());
    QTime time = QTime::currentTime();
    QString  dateString = QString::number(date.year())+QString::number(date.month())+QString::number(date.day())+QString::number(time.hour())+QString::number(time.minute())+
                          QString::number(time.second());
    return dateString;
}


bool MainWindow::getFileData(bool isJson)
{
    //定义文件对话框类
    QFileDialog *fileDialog = new QFileDialog(this);
    //定义文件对话框标题
    fileDialog->setWindowTitle(QStringLiteral("选中文件"));
    //设置默认文件路径
    fileDialog->setDirectory(".");
    //设置文件过滤器
    if(isJson)
        fileDialog->setNameFilter(tr("File(*.json)"));
    else
        fileDialog->setNameFilter(tr("File(*.csv)"));
    //设置可以选择多个文件,默认为只能选择一个文件QFileDialog::ExistingFiles
    fileDialog->setFileMode(QFileDialog::ExistingFiles);
    //设置视图模式
    fileDialog->setViewMode(QFileDialog::Detail);
    //打印所有选择的文件的路径
    QStringList fileNames;
    if (fileDialog->exec()) {
        fileNames = fileDialog->selectedFiles();
    }

    if(fileNames.length() <1)
        return false;

    //采用普通方式打开文件，并且存入allDada中，注意这是一种QByteArray格式
    QFile loadFile(fileNames[0]);
    if(!loadFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "could't open projects json";
        QMessageBox::warning(this,"Warning","Cannot open file: "+fileNames[0],QMessageBox::Abort);
        return false;
    }
    allData = loadFile.readAll();
    loadFile.close();


    return true;

}



bool MainWindow::cvsData2json( )
{

    QJsonObject jsonLang;

    QStringList lines;/*行数据*/

    QTextStream stream_text(allData);
    while (!stream_text.atEnd())
    {
        lines.push_back(stream_text.readLine());
    }
    for (int j = 0; j < lines.size(); j++)
    {
        QString line = lines.at(j);
        QStringList split = line.split(",");/*列数据*/
        QString value;
        for (int col = 1; col < split.size(); col++)
            value = value+split.at(col)+(col+1==split.size()?"":",");
        jsonLang.insert(split[0],value);
    }

    if(jsonLang.isEmpty())
        return false;
    else
        jsonData = QJsonValue(jsonLang);

    return true;


}

bool MainWindow::analysisJson()
{
    QString jsonString = (QString)allData;
    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toStdString().c_str(),&error);
    if(jsonDoc.isNull())
        jsonDoc = QJsonDocument::fromJson(jsonString.toLocal8Bit().data());

     if(jsonDoc.isNull())
         return false;
     else if(jsonDoc.isObject())
         jsonData = QJsonValue(jsonDoc.object());
     else if(jsonDoc.isArray())
     {
        QJsonArray arrayJson = jsonDoc.array();
        jsonData = QJsonValue(arrayJson);
     }
    qDebug()<<jsonData.toObject().size();
    return true;
}


void MainWindow::createFile(QString filePath,QString fileName)
{
    QDir tempDir;
    //临时保存程序当前路径
    QString currentDir = tempDir.currentPath();
    //如果filePath路径不存在，创建它
    if(!tempDir.exists(filePath))
    {
        qDebug()<<QObject::tr("不存在该路径")<<endl;
        tempDir.mkpath(filePath);
    }
    QFile *tempFile = new QFile;
    //将程序的执行路径设置到filePath下
    tempDir.setCurrent(filePath);
    qDebug()<<tempDir.currentPath();
    //检查filePath路径下是否存在文件fileName,如果停止操作。
    if(tempFile->exists(fileName))
    {
        qDebug()<<QObject::tr("文件存在");
        return ;
    }
    //此时，路径下没有fileName文件，使用下面代码在当前路径下创建文件
    tempFile->setFileName(fileName);
    if(!tempFile->open(QIODevice::WriteOnly|QIODevice::Text))
    {
        qDebug()<<QObject::tr("打开失败");
    }
    tempFile->close();
    //将程序当前路径设置为原来的路径
    tempDir.setCurrent(currentDir);
    m_pSavePath = tempDir.currentPath();
}
