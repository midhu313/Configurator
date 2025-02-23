#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QSettings>
#include <QDir>
#include <QFileInfoList>
#include <QTextStream>
#include <QMap>
#include <QDebug>
#include "redisaccess.h"

typedef struct {
    char config_file_name[80];
    char param_name[40];
    char value[40];
}Config_Update_Params_S;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void HandleConnectioButtonClickedEvent();
    void HandleRefreshButtonClickEvent();
    void HandleAppConfigSelectorButton();
    void HandleRedisConnectionStateChange(bool state);
    void HandleRedisErrorMessage(QString msg);
private:

    RedisAccess *redis;
    bool isConnected;

    Ui::MainWindow *ui;

    QVBoxLayout *leftLayout;
    QVBoxLayout *rightLayout;

    void saveSettings();
    void loadSettings();
    void loadAvailableConfigSelectionButtons();
    QMap<QString,QString> readConfigurationFile(const QString &filePath);
    void loadApplicationConfigManagementUI(QString cfgFileName);


};
#endif // MAINWINDOW_H
