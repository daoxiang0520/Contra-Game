#include <QStackedWidget>
#include <QMainWindow>
#include <QWidget>
#include <QKeyEvent>
#include <QScreen>
#include <QApplication>
#include <QDir>

#include "mainwindow.h"
#include "startwidget.h"
#include "levelwidget.h"
#include "settingwidget.h"
#include "blockwindow.h"
#include "GameDefs.h"

int currentLevel = 0; // 默认当前关卡不存在，初始化为0；
int unlockedLevel = 15; // 默认已解锁15关
int currentDifficulty = EASY; // 默认难度简单

// 全局资源定义
QString bg1 = ":/imgs/bg1.jpg";
QString bg2 = ":/imgs/bg2.jpg";
QString bg3 = ":/imgs/bg3.jpg";
QString bg4 = ":/imgs/bg4.jpg";
QVector<QString> bgs = {bg1, bg2, bg3, bg4};

QString bgm1 = QDir::currentPath()+"/bgms/cn.mp3";
QString bgm2 = QDir::currentPath()+"/bgms/zhyy.mp3";
QString bgm3 = QDir::currentPath()+"/bgms/qsx.mp3";
QString bgm4 = QDir::currentPath()+"/bgms/tgz.mp3";
QVector<QString> bgms = {bgm1, bgm2, bgm3, bgm4};

QString bgmName1 = "春泥";
QString bgmName2 = "最后一页";
QString bgmName3 = "牵丝戏";
QString bgmName4 = "探故知";
QVector<QString> bgmNames = {bgmName1, bgmName2, bgmName3, bgmName4};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 初始化栈容器
    stackWidget = new QStackedWidget(this);

    // 初始化主窗口基础属性
    this->setFixedSize(1024,768);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->centerWindow();
    this->setCentralWidget(stackWidget);

    // 只创建「非游戏界面」并加入栈
    startWidget = new StartWidget(stackWidget);
    levelWidget = new LevelWidget(stackWidget);
    settingWidget = new SettingWidget(stackWidget);
    multiplayerWidget = new MultiplayerWidget(stackWidget);
    connectdia=new ConnectDialog(this);

    stackWidget->addWidget(startWidget);    // 索引 0：开始界面
    stackWidget->addWidget(levelWidget);   // 索引 1：关卡选择
    stackWidget->addWidget(settingWidget); // 索引 2：设置界面
    stackWidget->addWidget(multiplayerWidget); // 索引 3：多人界面

    stackWidget->setCurrentIndex(0);
    // 开始界面信号与槽
    connect(startWidget, &StartWidget::toGame, this, [=](int levelNum) {
        createGameWidget(levelNum);
        stackWidget->setCurrentIndex(4);
    });
    connect(startWidget, &StartWidget::toLevel, this, [=]() {
        stackWidget->setCurrentIndex(1);
    });
    connect(startWidget, &StartWidget::toSetting, this, [=]() {
        stackWidget->setCurrentIndex(2);
    });
    connect(startWidget, &StartWidget::toMultiplayer, this, [=]() {
        network=new NetworkManager(this);
        network->connectToServer("47.100.161.93",10086);
        connectdia->recover();
        connect(connectdia->button,&QPushButton::clicked,this,[=](){stackWidget->setCurrentIndex(0);});
        connect(network,&NetworkManager::tcpConnected,connectdia,&ConnectDialog::setLabelText);
        connect(network,&NetworkManager::tcpConnected,connectdia,&ConnectDialog::close);
        connect(network,&NetworkManager::tcpConnected,this,[=](){stackWidget->setCurrentIndex(3);});
        connectdia->exec();
    });
    // 关卡界面信号与槽
    connect(levelWidget, &LevelWidget::toGame, this, [=](int levelNum) {
        createGameWidget(levelNum);
        stackWidget->setCurrentIndex(4);
    });
    connect(levelWidget, &LevelWidget::toStart, this, [=]() {
        stackWidget->setCurrentIndex(0);
    });

    // 设置界面信号与槽
    connect(settingWidget, &SettingWidget::toStart, this, [=]() {
        stackWidget->setCurrentIndex(0);
    });
    connect(onlineWidget, &OnlineGameWidget::toStart, this, [=]() {
        stackWidget->setCurrentIndex(0);
    });

    // 多人界面信号与槽
    connect(multiplayerWidget, &MultiplayerWidget::toGame, this, [=](bool isCreator) {
        if(gameWidget!=nullptr){
            stackWidget->removeWidget(gameWidget);
            delete gameWidget;
        }
        if(onlineWidget!=nullptr){
            stackWidget->removeWidget(onlineWidget);
            delete onlineWidget;
            onlineWidget=nullptr;
        }
        if(isCreator){
            network->createRoom(1);
        }else{
            bool bOk = false;
            int R = QInputDialog::getInt(this, "加入房间", "请输入房间号", 0, 0, 999999, 1, &bOk);
            if(bOk&&R>=0&&R<=999999){
                network->joinRoom(R);
            }
        }/*connect(network,&NetworkManager::roomError,this,[=](QString ms){
            connectdia->setLabel(ms);
            connectdia->setButton("确定");
            qDebug()<<"ROOMERR"<<ms;
            connectdia->exec();
        });*/qDebug()<<"JOINED";
        onlineWidget=new OnlineGameWidget(network,1,stackWidget);
        stackWidget->addWidget(onlineWidget);
        stackWidget->setCurrentIndex(4);
        connect(network, &NetworkManager::mainRoom, this, [=](QString ms) {
            connectdia->setLabel(ms);
            connectdia->setButton("确定");
            qDebug()<<"ROOMERR"<<ms;
            connectdia->exec();
            stackWidget->setCurrentIndex(0);
        });
    });
    connect(multiplayerWidget, &MultiplayerWidget::toStart, this, [=]() {
        stackWidget->setCurrentIndex(0);
    });


}
MainWindow::~MainWindow()
{
    // 删除游戏界面实例
    if (gameWidget != nullptr) {
        delete gameWidget;
        gameWidget = nullptr;
    }if(network!=nullptr){
        delete network;
        network=nullptr;
    }if(onlineWidget!=nullptr){
        delete onlineWidget;
        onlineWidget=nullptr;
    }
}

void MainWindow::createGameWidget(int levelNum)
{

    // 继续游玩
    if (gameWidget != nullptr && currentLevel == levelNum) {
        gameWidget->gameContinue();
        return; // 立即终止
    }

    currentLevel = levelNum;

    // 如果已有旧的游戏界面，先从栈中移除并删除（避免重复添加）
    if (gameWidget != nullptr){
        stackWidget->removeWidget(gameWidget); // 从栈中移除
        delete gameWidget; // 释放内存
        gameWidget = nullptr; // 置空指针
    }
    if(onlineWidget!=nullptr){
        stackWidget->removeWidget(onlineWidget);
        delete onlineWidget;
        onlineWidget=nullptr;
    }

    // 动态创建 BlockWindow，构建时传入levelNum
    gameWidget = new BlockWindow(levelNum, stackWidget);

    // 游戏界面信号与槽
    connect(gameWidget, &BlockWindow::toStart, this, [=]() {
        stackWidget->setCurrentIndex(0);
        gameWidget->gamePause(); // 避免后台继续游玩
    });
    connect(gameWidget, &BlockWindow::toLevel, this, [=]() {
        stackWidget->setCurrentIndex(1);
        gameWidget->gamePause(); // 避免后台继续游玩
    });
    connect(gameWidget, &BlockWindow::unlock, levelWidget, [=](){
        levelWidget->setUnlock(); // 解锁关卡
    });
    stackWidget->addWidget(gameWidget);
}

// 窗口居中方法（保持不变）
void MainWindow::centerWindow()
{
    QRect screenGeometry = QApplication::primaryScreen()->geometry();
    int x = (screenGeometry.width() - this->width()) / 2;
    int y = (screenGeometry.height() - this->height()) / 2;
    this->move(x, y);
}
