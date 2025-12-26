#ifndef RESULTWINDOW_H
#define RESULTWINDOW_H

// Qt核心头文件（按功能分类，方便理解）
#include <QWidget>       // 所有Qt窗口控件的基类，必须包含
#include <QVBoxLayout>   // 垂直布局管理器，用于排列按钮/文字
#include <QLabel>        // 显示文本标签（通关/阵亡提示）
#include <QPushButton>   // 按钮控件（返回关卡选择）
#include <QKeyEvent>     // 按键事件处理（支持R键返回）
#include <QFont>         // 字体设置（调整文字大小/样式）
#include <QObject>       // Q_OBJECT宏依赖，支持信号槽机制
#include <QRect>         // 矩形区域计算（窗口居中）
#include <QApplication>  // 应用程序核心类，获取屏幕信息
#include <QScreen>       // Qt5.11+ 替代废弃的QDesktopWidget，实现窗口居中

#include "animationbutton.h"

// 枚举类型：定义游戏结果类型（通关/阵亡）
// 用枚举可以避免魔法值，代码更易读
enum class ResultType {
    LevelCleared,  // 成功通关
    GameOver       // 玩家阵亡
};

class ResultWindow : public QWidget
{
    Q_OBJECT  // 必须添加，否则信号槽机制失效

public:
    // 构造函数：参数1=结果类型，参数2=父窗口（默认空）
    explicit ResultWindow(ResultType type, QWidget *parent = nullptr);

signals:
    // 自定义信号：点击返回按钮/按R键时触发，通知主窗口返回关卡选择
    void backToLevel();

protected:
    // 重写按键事件：处理R键返回逻辑
    void keyPressEvent(QKeyEvent *event) override;

private:
    // 初始化界面的私有函数（把UI初始化逻辑抽离，代码更整洁）
    void initUI();
    void initGameOverLabel();
    // 成员变量（仅保留必要的，减少内存占用）
    ResultType m_resultType;   // 存储当前结果类型（通关/阵亡）
    QLabel *m_titleLabel;      // 显示"成功通关"/"您已阵亡"的标签
    AnimationButton *m_backBtn;    // 返回关卡选择的按钮
};

#endif // RESULTWINDOW_H
