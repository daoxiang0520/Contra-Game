#include "ResultWindow.h"

#include "animationbutton.h"

extern int currentLevel;

ResultWindow::ResultWindow(ResultType type, QWidget *parent)
    : QWidget(parent),
    m_resultType(type),
    m_titleLabel(nullptr),
    m_backBtn(nullptr)
{

    setWindowModality(Qt::ApplicationModal); // 模态窗口：阻塞其他窗口操作
    setFixedSize(1024, 768);
    setWindowTitle("游戏结果");
    // setWindowFlags(Qt::Window);

    // 获取系统主屏幕对象
    QScreen *mainScreen = QApplication::primaryScreen();
    if (mainScreen) { // 判空：防止极端情况下屏幕对象为空导致崩溃
        QRect screenRect = mainScreen->geometry(); // 获取屏幕的几何区域（宽高/坐标）
        // 计算居中坐标：屏幕中心 - 窗口自身中心
        QPoint centerPos = screenRect.center() - this->rect().center();
        centerPos.setX(0);
        centerPos.setY(0);
        this->move(centerPos); // 移动窗口到屏幕正中央
    }

    // 初始化界面控件
    initUI();
}

// 界面初始化函数：集中处理所有UI控件的创建和样式
void ResultWindow::initUI()
{
    //创建垂直布局管理器，把控件垂直排列
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(40); // 控件之间的间距（40像素）
    mainLayout->setContentsMargins(50, 250, 50, 50); // 布局的内边距

    initGameOverLabel();

    //返回按钮
    m_backBtn = new AnimationButton("返回", this);
    QFont btnFont("微软雅黑", 14); // 按钮字体：微软雅黑，大小14
    m_backBtn->setBtnSize(240, 80);
    m_backBtn->setBtnFont(30, 1, Qt::white);

    //连接按钮点击信号到自定义信号（点击按钮触发返回）
    connect(m_backBtn, &QPushButton::clicked, this, &ResultWindow::backToLevel);

    //将控件添加到布局中
    mainLayout->addWidget(m_titleLabel, 0, Qt::AlignCenter);
    QVBoxLayout *btnLayout = new QVBoxLayout(this);    // 添加标题标签
    btnLayout->addWidget(m_backBtn, 0, Qt::AlignCenter); // 添加按钮，并水平居中
    mainLayout->addLayout(btnLayout, 0);

    // // 步骤1：添加顶部弹性空间（拉伸因子1）→ 把标题“顶”到垂直中心
    // mainLayout->addStretch(1);
    // // 步骤2：添加标题标签（水平居中）
    // mainLayout->addWidget(m_titleLabel, 0, Qt::AlignCenter);
    // // 步骤3：添加底部弹性空间（拉伸因子1）→ 标题上下弹性空间相等，实现垂直居中
    // mainLayout->addStretch(1);
    // // 步骤4：添加按钮（水平居中）+ 底部固定间距（避免贴边）
    // mainLayout->addWidget(m_backBtn, 0, Qt::AlignCenter);
    // mainLayout->addSpacing(50); // 按钮底部留50px空白，不贴边


    setLayout(mainLayout);
}

// 重写按键事件：支持按R键返回关卡选择
void ResultWindow::keyPressEvent(QKeyEvent *event)
{
    // 判断是否按下R键（忽略大小写，Qt::Key_R包含大小写）
    if (event->key() == Qt::Key_R) {
        emit backToLevel(); // 触发返回信号
    }
    // 调用父类的按键事件：保证其他按键逻辑不丢失
    QWidget::keyPressEvent(event);
}

// 初始化炫酷游戏名称标签
void ResultWindow::initGameOverLabel() {

    // 标题标签
    m_titleLabel = new QLabel(this);
    m_titleLabel->setFixedWidth(600);
    //字体优化
    QFont textFont;
    textFont.setFamily("Arial Black");
    textFont.setPointSize(60);
    textFont.setBold(true);
    textFont.setLetterSpacing(QFont::AbsoluteSpacing, 8);
    m_titleLabel->setFont(textFont);
    // 2. 渐变文字效果（改用样式表实现）
    QString gradientStyle = QString(R"(
        QLabel {
            background-color: rgba(0,0,0,0.3);
            border: 2px solid #FFD700;
            padding: 10px 20px;
            border-radius: 8px;
            color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 red, stop:0.5 yellow, stop:1 red);
        }
    )");
    m_titleLabel->setStyleSheet(gradientStyle);

    m_titleLabel->setAlignment(Qt::AlignCenter);
    // 关键：强制标签宽度自适应内容，且水平居中
    m_titleLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    // 根据结果类型设置不同的文字和颜色
    if (m_resultType == ResultType::LevelCleared)
    {
        m_titleLabel->setText("成功通关");
        //m_titleLabel->setStyleSheet("color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 green, stop:0.5 yellow, stop:1 green);"); // 绿色（通关）
    }
    else
    {
        m_titleLabel->setText("您已阵亡");
        //m_titleLabel->setStyleSheet("color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 red, stop:0.5 yellow, stop:1 red);"); // 红色（阵亡）
    }
}
