#include <QVBoxLayout>
#include <QPalette>
#include <QPixmap>
#include <QDebug>
#include <QApplication>
#include <QKeyEvent>
#include <QVector>
#include "levelwidget.h"
#include "animationbutton.h"

extern int unlockedLevel;

extern QVector<QString> bgs;

LevelWidget::LevelWidget(QWidget *parent)
    : QWidget{parent}
{
    //设置游戏开始界面窗口
    this->setFixedSize(1024, 768); // 固定窗口大小
    //设置背景
    this->loadBg(bgs[2]);

    // 初始化布局
    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->setSpacing(50); // 按钮间距
    mainLayout->setContentsMargins(50, 250, 50, 50); // 布局边距

    //设置关卡选择按钮

    levels.resize(row);
    for(int i = 0; i < row; i++) levels[i].resize(col);

    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; j++) {
            int levelNum = i*col + j + 1;
            levels[i][j] = new AnimationButton(QString::number(levelNum), this);
            levels[i][j]->setBtnSize(100, 100);
            levels[i][j]->setBtnFont(30, 1, Qt::white);
            // 未解锁的关卡禁用按钮
            if (levelNum > unlockedLevel) {
                levels[i][j]->setEnabled(false); // 核心禁用函数
            } else {
                levels[i][j]->setEnabled(true);  // 已解锁的启用按钮
            }
            // 加入网格布局
            mainLayout->addWidget(levels[i][j], i, j);
            connect(levels[i][j], &QPushButton::clicked, this, [this, levelNum](){
                emit toGame(levelNum);
            });
        }
    }
}

// 重写 keyPressEvent 函数
void LevelWidget::keyPressEvent(QKeyEvent *event) {
    // 判断是否按下 Esc 键（Qt::Key_Escape 对应 Esc 键）
    if (event->key() == Qt::Key_Escape) {
        // 退出游戏（关闭整个应用程序）
        emit toStart();
    }

    // 必须调用父类函数，否则子界面的键盘事件会失效（比如游戏中的移动、射击）
    QWidget::keyPressEvent(event);
}

//绘制背景
void LevelWidget::loadBg(const QString &imgBg)
{
    //设置背景图
    QPixmap pixmap(imgBg); // 使用Qpixmap对象加载背景图
    if (pixmap.isNull()) { // 检测路径是否正确
        qDebug() << "背景图加载失败！检查资源路径或图片文件：" << imgBg;
        QPalette palette = this->palette();
        palette.setBrush(QPalette::Window, QBrush(Qt::black)); // 设置黑色画刷
        this->setPalette(palette);
        this->setAutoFillBackground(true); // 开启自动填充
    } else {
        qDebug() << "[调试] 背景图加载成功！图片原始尺寸：" << pixmap.size();
        // QPalette强制缩放背景图
        QPalette palette = this->palette(); // 复制一份默认方案进行修改
        //tips:QPalette管理控件：颜色、背景、文本等样式属性
        // Window角色设置背景
        //setBrush函数
        palette.setBrush(QPalette::Window, // 参数1：代表控件的背景
                         //参数2：创建画刷
                         QBrush(pixmap.scaled(this->size(), // 参数2.1 图片需要缩放到的大小：窗口大小
                                              Qt::KeepAspectRatioByExpanding, // 参数2.2 缩放规则：扩展/切割
                                              Qt::SmoothTransformation))); // 参数2.3 缩放算法：平滑插值
        this->setPalette(palette); // 应用修改好的palette
        this->setAutoFillBackground(true); // 强制启用：确保背景图填充整个窗口
    }


}

void LevelWidget::setUnlock(){
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; j++) {
            int levelNum = i*col + j + 1;
            // 未解锁的关卡禁用按钮
            if (levelNum > unlockedLevel) {
                levels[i][j]->setEnabled(false); // 核心禁用函数
            } else {
                levels[i][j]->setEnabled(true);  // 已解锁的启用按钮
            }
        }
    }
}
