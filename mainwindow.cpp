#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    leftLayout = nullptr;
    rightLayout = nullptr;
    connect(ui->buttonConnection,&QPushButton::clicked,this,&MainWindow::HandleConnectioButtonClickedEvent);
    connect(ui->buttonRefresh,&QPushButton::clicked,this,&MainWindow::HandleRefreshButtonClickEvent);
    redis = new RedisAccess(this);
    connect(redis,&RedisAccess::connectionStatusChange,this,&MainWindow::HandleRedisConnectionStateChange);
    connect(redis,&RedisAccess::redisErrorMessage,this,&MainWindow::HandleRedisErrorMessage);
    isConnected = false;
    loadSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::HandleConnectioButtonClickedEvent(){
    qInfo()<<"Connect button clicked";
    if(!isConnected)
        redis->connect(ui->lineEditTargetIP->text());
    else
        redis->disconnect();
    saveSettings();
}

void MainWindow::HandleRefreshButtonClickEvent(){
    qInfo()<<"Refresh button clicked";
    saveSettings();
    loadAvailableConfigSelectionButtons();
}

void MainWindow::saveSettings(){
    QSettings settings("MPJ","Configurator");
    settings.setValue("filelocation",ui->lineEditFileLocation->text());
    settings.setValue("targetip",ui->lineEditTargetIP->text());
}

void MainWindow::loadSettings(){
    QSettings settings("MPJ","Configurator");
    QString retrivedSettings = settings.value("filelocation","/home/midhulaj/config").toString();
    ui->lineEditFileLocation->setText(retrivedSettings);
    retrivedSettings = settings.value("targetip","127.0.0.1").toString();
    ui->lineEditTargetIP->setText(retrivedSettings);
}

void MainWindow::loadAvailableConfigSelectionButtons(){
    if(leftLayout!=nullptr){
        delete leftLayout;
    }
    leftLayout = new QVBoxLayout(ui->leftFrame);
    ui->leftFrame->setLayout(leftLayout);
    //Create vertical buttons based on available configurations
    QDir directory(ui->lineEditFileLocation->text());
    if(!directory.exists()){
        QMessageBox::critical(nullptr,"Error","The given file path is not correct.");
        return;
    }
    QStringList filters;
    filters<<"*.cfg";
    directory.setNameFilters(filters);

    QFileInfoList fileList = directory.entryInfoList();
    for(const QFileInfo &fileInfo : fileList){
        QString filename = fileInfo.completeBaseName();
        QPushButton *btn = new QPushButton(filename,ui->leftFrame);
        leftLayout->addWidget(btn);
        connect(btn,&QPushButton::clicked,this,&MainWindow::HandleAppConfigSelectorButton);
    }
    leftLayout->addStretch();//Push buttons to the top
}

void MainWindow::HandleAppConfigSelectorButton(){
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if(btn){
        QString btn_name = btn->text();
        qDebug()<<"Clicked button:"<<btn_name;
        loadApplicationConfigManagementUI(btn_name);
    }
}

QMap<QString,QString> MainWindow::readConfigurationFile(const QString &filePath){
    QMap<QString,QString> keyValueMap;
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        QMessageBox::critical(nullptr,"Error",QString("Couldn't open the file:%1").arg(filePath));
        return keyValueMap;
    }
    QTextStream in(&file);
    while(!in.atEnd()){
        QString line = in.readLine().trimmed();
        if(line.isEmpty() || line.startsWith('#'))
            continue;
        QStringList parts = line.split('=');
        if(parts.size()==2){
            keyValueMap[parts[0].trimmed()]=parts[1].trimmed();
        }
    }
    file.close();
    return keyValueMap;
}

void MainWindow::loadApplicationConfigManagementUI(QString cfgFileName){
    if(rightLayout!=nullptr){
        delete rightLayout;
    }
    rightLayout = new QVBoxLayout(ui->rightFrame);
    ui->rightFrame->setLayout(rightLayout);
    //Create a Scroll Area
    QScrollArea *scrollArea = new QScrollArea(ui->rightFrame);
    scrollArea->setWidgetResizable(true);
    //Create a container widget for the scroll area
    QWidget *containerWidget = new QWidget();
    QVBoxLayout *containerLayout = new QVBoxLayout(containerWidget);
    QString filePath = QString("%1/%2.cfg").arg(ui->lineEditFileLocation->text()).arg(cfgFileName);
    QMap<QString,QString> target_configs = readConfigurationFile(filePath);
    for(auto it = target_configs.begin(); it != target_configs.end(); ++it){
        QWidget *rowWidget = new QWidget(containerWidget);
        QHBoxLayout *rowLayout = new QHBoxLayout(rowWidget);

        QLineEdit *edit1 = new QLineEdit(rowWidget);
        edit1->setText(it.key());
        edit1->setReadOnly(true);
        QLineEdit *edit2 = new QLineEdit(rowWidget);
        edit2->setText(it.value());
        QPushButton *updateButton = new QPushButton("Update",rowWidget);

        rowLayout->addWidget(edit1);
        rowLayout->addWidget(edit2);
        rowLayout->addWidget(updateButton);
        connect(updateButton,&QPushButton::clicked,[=](){
           qDebug()<<QString("Row Button Clicked. '%1'.'%2'").arg(edit1->text()).arg(edit2->text());
           redis->publish("testchannel",QString("%1.%2").arg(edit1->text()).arg(edit2->text()));
        });
        containerLayout->addWidget(rowWidget);
    }

    containerLayout->addStretch();//Keep the rows aligned at the top
    scrollArea->setWidget(containerWidget);
    rightLayout->addWidget(scrollArea);
}

void MainWindow::HandleRedisConnectionStateChange(bool state){
    isConnected = state;
    ui->buttonConnection->setText(state?"Disconnect":"Connect");
}

void MainWindow::HandleRedisErrorMessage(QString msg){
    QMessageBox::critical(nullptr,"Error",QString("%1").arg(msg));
}
