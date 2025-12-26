#include <QVBoxLayout>
#include <QPalette>
#include <QPixmap>
#include <QDebug>
#include <QApplication>
#include <QKeyEvent>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QFile>
#include <QWidget>
#include <QFont>
#include <QLinearGradient>
#include <QBrush>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>

#include "startwidget.h"
#include "animationbutton.h"
#include"globalaudio.h"

extern QVector<QString> bgms;
extern QVector<QString> bgs;


StartWidget::StartWidget(QWidget *parent)
    : QWidget{parent},
    m_mainLayout(nullptr),
    m_btnLayout(nullptr),
    m_gameNameLabel(nullptr),
    m_levelBtn(nullptr),
    m_endlessBtn(nullptr),
    m_multiplayerBtn(nullptr),
    m_settingBtn(nullptr)
{
    initWidget();
    //实现按钮功能
    connect(m_levelBtn, &QPushButton::clicked, this, [this](){
        emit toLevel();
    });
    connect(m_endlessBtn, &QPushButton::clicked, this, [this](){//无尽模式
        emit toGame(17);
    });
    connect(m_multiplayerBtn, &QPushButton::clicked, this, [this](){//联机模式
        emit toMultiplayer();
    });
    connect(m_settingBtn, &QPushButton::clicked, this, [this](){
        emit toSetting();
    });
}

// 加载静态背景（复用原有图片加载逻辑）
void StartWidget::loadBg(const QString &imgBg)
{
    QPixmap pixmap(imgBg);
    if (pixmap.isNull()) {
        qDebug() << "背景图加载失败！检查资源路径或图片文件：" << imgBg;
        QPalette palette = this->palette();
        palette.setBrush(QPalette::Window, QBrush(Qt::black));
        this->setPalette(palette);
        this->setAutoFillBackground(true);
    } else {
        qDebug() << "[调试] 背景图加载成功！图片原始尺寸：" << pixmap.size();
        QPalette palette = this->palette();
        palette.setBrush(QPalette::Window,
                         QBrush(pixmap.scaled(this->size(),
                                              Qt::KeepAspectRatioByExpanding,
                                              Qt::SmoothTransformation)));
        this->setPalette(palette);
        this->setAutoFillBackground(true);
    }
}

// 重写keyPressEvent函数
void StartWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        qApp->quit();
    }
    if (event->key() == Qt::Key_A) {
        GlobalAudio::instance()->setMuted(!GlobalAudio::instance()->isMuted());
    }
    QWidget::keyPressEvent(event);
}

// 初始化炫酷游戏名称标签
void StartWidget::initGameNameLabel() {

    m_gameNameLabel = new QLabel("CONTRA", this);
    m_gameNameLabel->setFixedWidth(600);
    //字体优化
    QFont textFont;
    textFont.setFamily("Arial Black");
    textFont.setPointSize(60);
    textFont.setBold(true);
    textFont.setLetterSpacing(QFont::AbsoluteSpacing, 8);
    m_gameNameLabel->setFont(textFont);

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
    m_gameNameLabel->setStyleSheet(gradientStyle);

    m_gameNameLabel->setAlignment(Qt::AlignCenter);
    // 关键：强制标签宽度自适应内容，且水平居中
    m_gameNameLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

// 初始化界面
void StartWidget::initWidget(){
    this->setFixedSize(1024, 768);
    this->loadBg(bgs[1]);
    m_mainLayout = new QVBoxLayout(this);
    m_btnLayout = new QVBoxLayout(this);

    // 替换为炫酷的游戏名称标签初始化
    initGameNameLabel();

    // 设置按钮及定位
    m_levelBtn = new AnimationButton("关卡模式", this);
    m_levelBtn->setBtnSize(240, 80);
    m_levelBtn->setBtnFont(30, 1, Qt::white);
    // 关键2：按钮尺寸策略（固定尺寸，不拉伸）
    m_levelBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_endlessBtn = new AnimationButton("无尽模式", this);
    m_endlessBtn->setBtnSize(240, 80);
    m_endlessBtn->setBtnFont(30, 1, Qt::white);
    m_endlessBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_multiplayerBtn = new AnimationButton("联机模式", this);
    m_multiplayerBtn->setBtnSize(240, 80);
    m_multiplayerBtn->setBtnFont(30, 1, Qt::white);
    m_multiplayerBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_settingBtn = new AnimationButton("设置", this);
    m_settingBtn->setBtnSize(240, 80);
    m_settingBtn->setBtnFont(30, 1, Qt::white);
    m_settingBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // 加入按钮垂直布局
    m_btnLayout->addWidget(m_levelBtn);
    m_btnLayout->addWidget(m_endlessBtn);
    m_btnLayout->addWidget(m_multiplayerBtn);
    m_btnLayout->addWidget(m_settingBtn);

    // 核心修复：设置布局内所有控件的对齐方式（水平+垂直居中）
    m_btnLayout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    // 控件间距
    m_btnLayout->setSpacing(30);
    // 调整边距
    m_btnLayout->setContentsMargins(0, 0, 0, 0);
    m_btnLayout->insertStretch(0, 1); // 标题上方弹性空间（可选，优化垂直分布）
    m_btnLayout->addStretch(1); // 最后一个按钮下方弹性空间

    //添加到主垂直布局
    m_mainLayout->addWidget(m_gameNameLabel);
    m_mainLayout->addLayout(m_btnLayout);

    // 核心修复：设置布局内所有控件的对齐方式（水平+垂直居中）
    m_mainLayout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    // 控件间距
    m_mainLayout->setSpacing(30);
    // 调整边距
    m_mainLayout->setContentsMargins(0, 100, 0, 0);
    m_mainLayout->insertStretch(0, 1); // 标题上方弹性空间（可选，优化垂直分布）
    m_mainLayout->addStretch(1); // 最后一个按钮下方弹性空间
}
