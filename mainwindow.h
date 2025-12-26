#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include<QInputDialog>
#include<QDialog>

#include"startwidget.h"
#include"settingwidget.h"
#include"levelwidget.h"
#include"blockwindow.h"
#include"multiplayerwidget.h"
#include"onlinegamewidget.h"
#include"networkmanager.h"
#include"connectdialog.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    QStackedWidget *stackWidget;  // 栈容器指针
    BlockWindow *gameWidget = nullptr; // 游戏界面指针
    StartWidget *startWidget;
    LevelWidget *levelWidget;
    SettingWidget *settingWidget;
    MultiplayerWidget *multiplayerWidget;
    OnlineGameWidget *onlineWidget=nullptr;
    NetworkManager *network;
    ConnectDialog *connectdia;
    bool connected;
    // 方法声明
    void centerWindow(); // 窗口居中
    void createGameWidget(int levelNum); // 动态创建游戏界面
};

#endif // MAINWINDOW_H
