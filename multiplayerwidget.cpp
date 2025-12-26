#include <QKeyEvent>

#include "multiplayerwidget.h"



extern QVector<QString> bgs;

MultiplayerWidget::MultiplayerWidget(QWidget *parent)
    : QWidget{parent},
    m_mainLayout(nullptr),
    m_joinBtn(nullptr),
    m_createBtn(nullptr)
{
    initWidget();
    //实现按钮功能
    connect(m_createBtn, &QPushButton::clicked, this, [this](){
        emit toGame(true);
    });
    connect(m_joinBtn, &QPushButton::clicked, this, [this](){//无尽模式
        emit toGame(false);
    });
}

// 加载静态背景
void MultiplayerWidget::loadBg(const QString &imgBg)
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
void MultiplayerWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        emit toStart();
    }
    QWidget::keyPressEvent(event);
}

// 初始化界面
void MultiplayerWidget::initWidget(){
    this->setFixedSize(1024, 768);
    this->loadBg(bgs[3]);

    m_mainLayout = new QVBoxLayout(this);

    // 设置按钮及定位
    m_createBtn = new AnimationButton("创建房间", this);
    m_createBtn->setBtnSize(240, 80);
    m_createBtn->setBtnFont(30, 1, Qt::white);
    m_createBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_joinBtn = new AnimationButton("加入游戏", this);
    m_joinBtn->setBtnSize(240, 80);
    m_joinBtn->setBtnFont(30, 1, Qt::white);
    m_joinBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);


    // 加入按钮垂直布局
    m_mainLayout->addWidget(m_createBtn);
    m_mainLayout->addWidget(m_joinBtn);


    // 核心修复：设置布局内所有控件的对齐方式（水平+垂直居中）
    m_mainLayout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    // 控件间距
    m_mainLayout->setSpacing(30);
    // 调整边距
    m_mainLayout->setContentsMargins(0, 200, 0, 0);
    m_mainLayout->insertStretch(0, 1); // 标题上方弹性空间（可选，优化垂直分布）
    m_mainLayout->addStretch(1); // 最后一个按钮下方弹性空间
}
