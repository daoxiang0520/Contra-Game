#include <QKeyEvent>
#include <QWidget>
#include <QFont>
#include <QColor>
#include <QHBoxLayout>
#include <QPushButton>
#include <QIcon>
#include <QDebug>
#include <QStringList> // 新增：处理字符串
#include <QStyle>     // 新增：样式辅助
#include <QPainter>

#include "settingwidget.h"
#include "globalaudio.h"
#include "globalsound.h"
#include "GameDefs.h"

extern QVector<QString> bgms;
extern QVector<QString> bgmNames;
extern QVector<QString> bgs;

extern int currentDifficulty;


SettingWidget::SettingWidget(QWidget *parent)
    : QWidget{parent},
    m_currentBgmIndex(0), // 初始化索引

    m_difficultyControlTextLabel(nullptr),
    m_difficultyControlLayout(nullptr),
    m_prevDifficultyBtn(nullptr),
    m_nextDifficultyBtn(nullptr),
    m_currentDifficultyLabel(nullptr),

    m_volumeTextLabel(nullptr),
    m_volumeControlLayout(nullptr),
    m_muteBtn(nullptr),
    m_volumeSlider(nullptr),
    m_volumeLabel(nullptr),

    m_soundTextLabel(nullptr),
    m_soundControlLayout(nullptr),
    m_soundMuteBtn(nullptr),
    m_soundSlider(nullptr),
    m_soundLabel(nullptr),

    m_bgmControlTextLabel(nullptr),
    m_bgmControlLayout(nullptr),
    m_prevBgmBtn(nullptr),
    m_nextBgmBtn(nullptr),
    m_bgmTitleLabel(nullptr),
    m_bgmPlayer(nullptr),

    m_mainLayout(nullptr)

{
    //初始化界面显示
    initWidget();

    // 信号槽连接实现核心逻辑

    // 全局音量信号槽
    connect(m_volumeSlider, &QSlider::valueChanged, this, &SettingWidget::onVolumeSliderValueChanged); // 滑块修改全局音量
    connect(GlobalAudio::instance(), &GlobalAudio::volumeChanged, this, &SettingWidget::syncVolumeSlider); // 全局音量同步滑块
    connect(m_muteBtn, &QPushButton::clicked, this, [this]() {
        bool currentMuted = GlobalAudio::instance()->isMuted();
        GlobalAudio::instance()->setMuted(!currentMuted);
    }); // 静音按钮点击->切换全局静音状态
    connect(GlobalAudio::instance(), &GlobalAudio::mutedChanged, this, [this](bool isMuted) {
        updateMuteButtonIcon(isMuted); // 更新图标
    }); // 全局静音状态变化->更新按钮图标和滑块

    // 全局音效信号槽
    connect(m_soundSlider, &QSlider::valueChanged, this, &SettingWidget::onSoundSliderValueChanged); // 滑块修改全局音效
    connect(GlobalSound::instance(), &GlobalSound::volumeChanged, this, &SettingWidget::syncSoundSlider); // 全局音效同步滑块
    connect(m_soundMuteBtn, &QPushButton::clicked, this, [this]() {
        bool currentMuted = GlobalSound::instance()->isMuted();
        GlobalSound::instance()->setMuted(!currentMuted);
    }); // 静音按钮点击->切换全局音效静音状态
    connect(GlobalSound::instance(), &GlobalSound::mutedChanged, this, [this](bool isMuted) {
        updateSoundMuteButtonIcon(isMuted);
    }); // 全局音效静音状态变化->更新按钮图标和滑块

    // BGM 切换信号槽
    connect(m_prevBgmBtn, &QPushButton::clicked, this, &SettingWidget::switchToPrevBgm);
    connect(m_nextBgmBtn, &QPushButton::clicked, this, &SettingWidget::switchToNextBgm);

    // 难度切换信号槽
    connect(m_prevDifficultyBtn, &QPushButton::clicked, this, &SettingWidget::switchToPrevDifficulty);
    connect(m_nextDifficultyBtn, &QPushButton::clicked, this, &SettingWidget::switchToNextDifficulty);

}

// 初始化界面
void SettingWidget::initWidget(){
    //设置固定大小
    this->setFixedSize(1024, 768);
    this->loadBg(bgs[0]);

    // 初始化 BGM 列表和播放器
    m_bgmList = bgms; // 关联全局 4 首 BGM
    m_bgmPlayer = new BgmPlayer(this);
    // 初始播放第 1 首
    if (!m_bgmList.isEmpty()) {
        m_bgmPlayer->playBgm(m_bgmList[m_currentBgmIndex]);
    }

    // 初始化设置题头
    initSettingLabel();

    // 设置项目1：音量设置
    initVolumeControlWidgets();

    // 设置项目2：音效设置
    initSoundControlWidgets();

    // 设置项目3：BGM切换
    initBgmControlWidgets();

    // 设置项目4：难度切换
    initDifficultyControlWidgets();

    // 全体布局管理，垂直布局，控件居中排列
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(100, 0, 100, 0); // 清空默认边距
    m_mainLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter); // 顶部居中
    m_mainLayout->addSpacing(30); // 顶部间距
    m_mainLayout->addWidget(m_settingLabel); // 设置文本
    m_mainLayout->addSpacing(10); // 与音量控制区拉开间距
    m_mainLayout->addWidget(m_difficultyControlTextLabel);  // 难度控制标题
    m_mainLayout->addSpacing(5);
    m_mainLayout->addLayout(m_difficultyControlLayout); // 难度切换按钮布局
    m_mainLayout->addSpacing(10);
    m_mainLayout->addWidget(m_volumeTextLabel); // 音量文本
    m_mainLayout->addSpacing(5); // 间距
    m_mainLayout->addLayout(m_volumeControlLayout); // 加入音量控制行
    m_mainLayout->addSpacing(10);
    m_mainLayout->addWidget(m_soundTextLabel);  // 音效音量标题
    m_mainLayout->addSpacing(5);
    m_mainLayout->addLayout(m_soundControlLayout); // 音效音量控制
    m_mainLayout->addSpacing(10); // 与音量控制区拉开间距
    m_mainLayout->addWidget(m_bgmControlTextLabel);  // bgm控制标题
    m_mainLayout->addSpacing(5);
    m_mainLayout->addLayout(m_bgmControlLayout); // BGM 切换按钮布局

    // 初始更新曲目显示
    updateBgmDisplay();
    // 初始更新难度显示
    updateDifficultyDisplay();
}



// 重写 keyPressEvent 函数
void SettingWidget::keyPressEvent(QKeyEvent *event) {
    // 判断是否按下 Esc 键（Qt::Key_Escape 对应Esc键）
    if (event->key() == Qt::Key_Escape) {
        emit toStart();
        event->accept(); // 标记事件已处理，避免透传
        return;
    }
    // 必须调用父类函数，否则子界面的键盘事件会失效（比如游戏中的移动、射击）
    QWidget::keyPressEvent(event);
}

//绘制背景
void SettingWidget::loadBg(const QString &imgBg)
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
        palette.setBrush(QPalette::Window,
                         QBrush(pixmap.scaled(this->size(),
                                              Qt::KeepAspectRatioByExpanding,
                                              Qt::SmoothTransformation)));
        this->setPalette(palette);
        this->setAutoFillBackground(true);
    }
}

// 初始化设置界面名称标签
void SettingWidget::initSettingLabel() {

    m_settingLabel = new QLabel("设置", this);
    // 字体优化
    QFont textFont;
    textFont.setFamily("Arial Black");
    textFont.setPointSize(60);
    textFont.setBold(true);
    textFont.setLetterSpacing(QFont::AbsoluteSpacing, 8);
    m_settingLabel->setFont(textFont);

    // 渐变文字效果（改用样式表实现）
    QString gradientStyle = QString(R"(
        QLabel {
            background-color: rgba(0,0,0,0.3);
            border: 2px solid #FFD700;
            padding: 10px 20px;
            border-radius: 8px;
            color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 red, stop:0.5 yellow, stop:1 red);
        }
    )");
    m_settingLabel->setStyleSheet(gradientStyle);

    m_settingLabel->setAlignment(Qt::AlignCenter);
}

// 初始化音量控制控件
void SettingWidget::initVolumeControlWidgets(){
    // 音量控制标签
    m_volumeTextLabel = new QLabel("音量控制", this);
    QFont textFont = m_volumeTextLabel->font();
    textFont.setPointSize(28);  // 字体大小
    textFont.setBold(true);     // 加粗
    m_volumeTextLabel->setFont(textFont);
    m_volumeTextLabel->setStyleSheet("color: white;"); // 白色文本（适配背景）
    m_volumeTextLabel->setAlignment(Qt::AlignCenter);  // 居中对齐

    // 静音按钮
    initMuteButton();
    // 音量滑块
    initVolumeSlider();
    // 音量显示标签 显示当前音量百分比
    m_volumeLabel = new QLabel("100%");
    QFont numFont = m_volumeLabel->font();
    numFont.setPointSize(24);
    numFont.setBold(true);
    m_volumeLabel->setFont(numFont);
    m_volumeLabel->setStyleSheet("color: white;");
    m_volumeLabel->setAlignment(Qt::AlignCenter);
    m_volumeLabel->setFixedWidth(100); // 固定宽度
    m_volumeLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred); // 禁止拉伸
    m_volumeLabel->setMinimumWidth(100); // 确保宽度不被压缩

    //初始化同步滑块初始位置,与全局音频当前音量同步
    int initVolume = GlobalAudio::instance()->volume();
    m_volumeSlider->setValue(initVolume);
    m_volumeLabel->setText(QString("%1%").arg(initVolume));

    //装入水平布局
    m_volumeControlLayout = new QHBoxLayout();
    m_volumeControlLayout->setSpacing(30);  // 按钮和滑块之间的间距
    m_volumeControlLayout->setContentsMargins(50, 0, 50, 0); // 布局左右留边，防止贴边
    m_volumeControlLayout->setAlignment(Qt::AlignCenter); // 水平布局内容居中
    m_volumeControlLayout->addWidget(m_muteBtn); // 添加静音按钮
    m_volumeControlLayout->addWidget(m_volumeSlider); // 添加音量滑块
    m_volumeControlLayout->addWidget(m_volumeLabel); // 添加音量显示标签
    m_volumeControlLayout->addStretch(1); // 右侧弹性空间
    m_volumeControlLayout->insertStretch(0, 1); // 左侧弹性空间
}

// 初始化静音按钮
void SettingWidget::initMuteButton()
{
    m_muteBtn = new QPushButton(this);

    // 设置按钮大小，与滑块统一
    m_muteBtn->setFixedSize(100, 60);

    // 设置按钮样式（去除默认边框、背景，仅保留图标）
    m_muteBtn->setStyleSheet(R"(
        QPushButton {
            border: none;
            background: transparent;
            icon-size: 40px 40px; /* 图标大小 */
            color: white;          /* 基础色 */
            icon-color: white;     /* Qt6.5+ 强制图标色 */
        }
        QPushButton:hover {
            background-color: rgba(255, 255, 255, 0.1); /* 悬浮半透明背景 */
            border-radius: 4px;
        }
        QPushButton:pressed {
            background-color: rgba(255, 255, 255, 0.2); /* 按下加深背景 */
        }
    )");

    // 初始化图标
    bool initMuted = GlobalAudio::instance()->isMuted();
    updateMuteButtonIcon(initMuted);
}

// 初始化音量滑块
void SettingWidget::initVolumeSlider()
{
    m_volumeSlider = new QSlider(Qt::Horizontal, this);
    m_volumeSlider->setRange(0, 100);  // 音量范围：0~100
    m_volumeSlider->setSingleStep(1);  // 步长：1
    m_volumeSlider->setPageStep(10);   // 翻页步长：10
    m_volumeSlider->setMinimumHeight(30); // 滑块高度（便于点击）
    m_volumeSlider->setFixedWidth(300); // 滑块最大宽度

    // 滑块样式
    m_volumeSlider->setStyleSheet(R"(
    /* 滑块轨道样式 */
    QSlider::groove:horizontal {
        height: 8px;
        background: rgba(255, 255, 255, 0.2);
        border-radius: 4px;
    }

    /* 已选中部分轨道样式 */
    QSlider::sub-page:horizontal {
        background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                  stop:0 #4a90e2, stop:1 #5cb85c);
        border-radius: 4px;
    }

    /* 滑块柄样式 - 正常状态 */
    QSlider::handle:horizontal {
        width: 20px;
        height: 20px;
        margin: -6px 0; /* 扩展点击区域 */
        background: white;
        border-radius: 10px;
        border: 2px solid rgba(255, 255, 255, 0.5);
    }

    /* 滑块柄样式 - 悬停状态 */
    QSlider::handle:horizontal:hover {
        background: #f0f0f0;
        border-color: white;
        box-shadow: 0 0 5px rgba(255, 255, 255, 0.8);
    }

    /* 滑块柄样式 - 按下状态 */
    QSlider::handle:horizontal:pressed {
        background: #e0e0e0;
        border-color: #dddddd;
        box-shadow: 0 0 3px rgba(0, 0, 0, 0.3) inset;
    }
)");
}

// 根据静音状态更新按钮图标
void SettingWidget::updateMuteButtonIcon(bool isMuted)
{
    QStyle *style = this->style();
    QIcon originalIcon;
    if (isMuted) {
        originalIcon = style->standardIcon(QStyle::SP_MediaVolumeMuted);
    } else {
        originalIcon = style->standardIcon(QStyle::SP_MediaVolume);
    }
    // 关键：将原始黑色图标转为白色
    m_muteBtn->setIcon(tintIcon(originalIcon, Qt::white, 40));
}

// 滑块值变化槽函数：更新全局音量和显示标签
void SettingWidget::onVolumeSliderValueChanged(int value)
{
    GlobalAudio::instance()->setVolume(value);  // 同步到全局音频
    m_volumeLabel->setText(QString("%1%").arg(value)); // 更新音量显示
}

// 同步滑块位置槽函数：全局音量变化时更新滑块
void SettingWidget::syncVolumeSlider(int value)
{
    m_volumeSlider->blockSignals(true);  // 阻止信号循环（避免触发自身valueChanged）
    m_volumeSlider->setValue(value);     // 更新滑块位置
    m_volumeLabel->setText(QString("%1%").arg(value)); // 更新显示
    m_volumeSlider->blockSignals(false); // 恢复信号
}

void SettingWidget::initSoundControlWidgets(){
    // 音效标题
    m_soundTextLabel = new QLabel("音效控制", this);
    QFont textFont = m_soundTextLabel->font();
    textFont.setPointSize(28);
    textFont.setBold(true);
    m_soundTextLabel->setFont(textFont);
    m_soundTextLabel->setStyleSheet("color: white;");
    m_soundTextLabel->setAlignment(Qt::AlignCenter);

    // 音效静音按钮
    initSoundMuteButton();
    // 音效滑块
    initSoundSlider();
    // 音效音量显示
    m_soundLabel = new QLabel("80%");  // 音效默认音量80%
    QFont numFont = m_soundLabel->font();
    numFont.setPointSize(24);
    numFont.setBold(true);
    m_soundLabel->setFont(numFont);
    m_soundLabel->setStyleSheet("color: white;");
    m_soundLabel->setAlignment(Qt::AlignCenter);
    m_soundLabel->setFixedWidth(100);
    m_soundLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    // 初始化同步滑块
    int initSoundVolume = GlobalSound::instance()->volume();
    m_soundSlider->setValue(initSoundVolume);
    m_soundLabel->setText(QString("%1%").arg(initSoundVolume));

    // 布局
    m_soundControlLayout = new QHBoxLayout();
    m_soundControlLayout->setSpacing(30);
    m_soundControlLayout->setContentsMargins(50, 0, 50, 0);
    m_soundControlLayout->setAlignment(Qt::AlignCenter);
    m_soundControlLayout->addWidget(m_soundMuteBtn);
    m_soundControlLayout->addWidget(m_soundSlider);
    m_soundControlLayout->addWidget(m_soundLabel);
    m_soundControlLayout->addStretch(1);
    m_soundControlLayout->insertStretch(0, 1);
}

void SettingWidget::initSoundMuteButton()
{
    m_soundMuteBtn = new QPushButton(this);
    m_soundMuteBtn->setFixedSize(100, 60);
    m_soundMuteBtn->setStyleSheet(R"(
        QPushButton {
            border: none;
            background: transparent;
            icon-size: 40px 40px;
            color: white;          /* 基础色 */
            icon-color: white;     /* Qt6.5+ 强制图标色 */
        }
        QPushButton:hover {
            background-color: rgba(255, 255, 255, 0.1);
            border-radius: 4px;
        }
        QPushButton:pressed {
            background-color: rgba(255, 255, 255, 0.2);
        }
    )");
    bool initMuted = GlobalSound::instance()->isMuted();
    updateSoundMuteButtonIcon(initMuted);
}

void SettingWidget::initSoundSlider()
{
    m_soundSlider = new QSlider(Qt::Horizontal, this);
    m_soundSlider->setRange(0, 100);
    m_soundSlider->setSingleStep(1);
    m_soundSlider->setPageStep(10);
    m_soundSlider->setMinimumHeight(30);
    m_soundSlider->setFixedWidth(300);
    m_soundSlider->setStyleSheet(R"(
    QSlider::groove:horizontal {
        height: 8px;
        background: rgba(255, 255, 255, 0.2);
        border-radius: 4px;
    }
    QSlider::sub-page:horizontal {
        background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                                  stop:0 #e2904a, stop:1 #e2c54a);
        border-radius: 4px;
    }
    QSlider::handle:horizontal {
        width: 20px;
        height: 20px;
        margin: -6px 0;
        background: white;
        border-radius: 10px;
        border: 2px solid rgba(255, 255, 255, 0.5);
    }
    QSlider::handle:horizontal:hover {
        background: #f0f0f0;
        border-color: white;
        box-shadow: 0 0 5px rgba(255, 255, 255, 0.8);
    }
    QSlider::handle:horizontal:pressed {
        background: #e0e0e0;
        border-color: #dddddd;
        box-shadow: 0 0 3px rgba(0, 0, 0, 0.3) inset;
    }
)");
}

void SettingWidget::updateSoundMuteButtonIcon(bool isMuted)
{
    QStyle *style = this->style();
    QIcon originalIcon;
    if (isMuted) {
        originalIcon = style->standardIcon(QStyle::SP_MediaVolumeMuted);
    } else {
        originalIcon = style->standardIcon(QStyle::SP_MediaVolume);
    }
    // 关键：转为白色图标
    m_soundMuteBtn->setIcon(tintIcon(originalIcon, Qt::white, 40));
}

void SettingWidget::onSoundSliderValueChanged(int value)
{
    GlobalSound::instance()->setVolume(value);
    m_soundLabel->setText(QString("%1%").arg(value));
}

void SettingWidget::syncSoundSlider(int value)
{
    m_soundSlider->blockSignals(true);
    m_soundSlider->setValue(value);
    m_soundLabel->setText(QString("%1%").arg(value));
    m_soundSlider->blockSignals(false);
}


// 初始化 BGM 控制控件
void SettingWidget::initBgmControlWidgets()
{
    m_bgmControlTextLabel = new QLabel("BGM切换", this);
    QFont textFont = m_bgmControlTextLabel->font();
    textFont.setPointSize(28);  // 字体大小
    textFont.setBold(true);     // 加粗
    m_bgmControlTextLabel->setFont(textFont);
    m_bgmControlTextLabel->setStyleSheet("color: white;"); // 白色文本（适配背景）
    m_bgmControlTextLabel->setAlignment(Qt::AlignCenter);  // 居中对齐
    // 曲目显示标签
    m_bgmTitleLabel = new QLabel(this);
    QFont bgmFont = m_bgmTitleLabel->font();
    bgmFont.setPointSize(24);
    bgmFont.setBold(true);
    m_bgmTitleLabel->setFont(bgmFont);
    m_bgmTitleLabel->setStyleSheet(R"(
        QLabel {
            color: white;
            background: rgba(0, 0, 0, 0.2); /* 可选：加半透明背景增强可读性 */
            border-radius: 4px;
            padding: 5px 15px;
        }
    )");
    m_bgmTitleLabel->setAlignment(Qt::AlignCenter);
    m_bgmTitleLabel->setFixedWidth(300); // 固定宽度（根据需求调整）
    m_bgmTitleLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred); // 禁止拉伸
    m_bgmTitleLabel->setMinimumWidth(300); // 确保宽度不被压缩

    // 上一首/下一首按钮
    m_prevBgmBtn = new QPushButton(this);
    m_nextBgmBtn = new QPushButton(this);

    // 按钮固定大小
    m_prevBgmBtn->setFixedSize(60, 60);  // 固定宽高
    m_nextBgmBtn->setFixedSize(60, 60);
    m_prevBgmBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed); // 完全禁止拉伸
    m_nextBgmBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // 按钮样式设置（与静音按钮风格统一）
    QString btnStyle = R"(
        QPushButton {
            border: none;
            background: transparent;
            icon-size: 40px 40px;
            color: white;          /* 基础色 */
            icon-color: white;     /* Qt6.5+ 强制图标色 */
        }
        QPushButton:hover {
            background-color: rgba(255, 255, 255, 0.1);
            border-radius: 4px;
        }
        QPushButton:pressed {
            background-color: rgba(255, 255, 255, 0.2);
        }
    )";
    m_prevBgmBtn->setStyleSheet(btnStyle);
    m_nextBgmBtn->setStyleSheet(btnStyle);

    // 设置按钮图标
    // 在 initBgmControlWidgets 函数中，替换原来的图标设置代码：
    QStyle *style = this->style();
    // 上一首图标：先获取原始图标，再转为白色
    QIcon prevIcon = style->standardIcon(QStyle::SP_MediaSkipBackward);
    m_prevBgmBtn->setIcon(tintIcon(prevIcon, Qt::white, 40));
    // 下一首图标：同理
    QIcon nextIcon = style->standardIcon(QStyle::SP_MediaSkipForward);
    m_nextBgmBtn->setIcon(tintIcon(nextIcon, Qt::white, 40));

    // BGM 控制按钮布局 : 布局固定间距 + 禁止控件拉伸
    m_bgmControlLayout = new QHBoxLayout();
    m_bgmControlLayout->setSpacing(30); // 固定按钮与中间标签的间距（根据需求调整）
    m_bgmControlLayout->setContentsMargins(100, 0, 100, 0); // 左右留边（可选）
    m_bgmControlLayout->setAlignment(Qt::AlignCenter);

    // 添加控件并设置拉伸权重（核心：0=不拉伸，1=拉伸）
    m_bgmControlLayout->addWidget(m_prevBgmBtn, 0); // 权重0：不拉伸
    m_bgmControlLayout->addWidget(m_bgmTitleLabel, 0); // 权重0：不拉伸
    m_bgmControlLayout->addWidget(m_nextBgmBtn, 0); // 权重0：不拉伸

    // 添加弹性空间使整体居中，不影响控件大小
    m_bgmControlLayout->insertStretch(0, 1); // 左侧弹性空间
    m_bgmControlLayout->addStretch(1); // 右侧弹性空间
}

// 切换上一首 BGM
void SettingWidget::switchToPrevBgm()
{
    if (m_bgmList.isEmpty()) return;
    // 循环索引：0 → 3 → 2 → 1 → 0
    m_currentBgmIndex = (m_currentBgmIndex - 1 + m_bgmList.size()) % m_bgmList.size();
    // 停止当前播放，启动新曲目
    m_bgmPlayer->toggleBgm(); // 先暂停当前
    m_bgmPlayer->playBgm(m_bgmList[m_currentBgmIndex]);
    // 更新显示
    updateBgmDisplay();
}

// 切换下一首 BGM
void SettingWidget::switchToNextBgm()
{
    if (m_bgmList.isEmpty()) return;
    // 循环索引：0 → 1 → 2 → 3 → 0
    m_currentBgmIndex = (m_currentBgmIndex + 1) % m_bgmList.size();
    // 停止当前播放，启动新曲目
    m_bgmPlayer->toggleBgm(); // 先暂停当前
    m_bgmPlayer->playBgm(m_bgmList[m_currentBgmIndex]);
    // 更新显示
    updateBgmDisplay();
}

// 更新曲目显示标签
void SettingWidget::updateBgmDisplay()
{
    if (m_bgmList.isEmpty()) {
        m_bgmTitleLabel->setText("无可用BGM");
        return;
    }
    // 显示格式：当前曲目 (序号/总数)
    QString displayText = QString("%1 (%2/%3)").arg(bgmNames[m_currentBgmIndex])
                              .arg(m_currentBgmIndex + 1)
                              .arg(m_bgmList.size());
    m_bgmTitleLabel->setText(displayText);
}


// 初始化难度控制控件
void SettingWidget::initDifficultyControlWidgets()
{
    m_difficultyControlTextLabel = new QLabel("难度切换", this);
    QFont textFont = m_difficultyControlTextLabel->font();
    textFont.setPointSize(28);  // 字体大小
    textFont.setBold(true);     // 加粗
    m_difficultyControlTextLabel->setFont(textFont);
    m_difficultyControlTextLabel->setStyleSheet("color: white;"); // 白色文本（适配背景）
    m_difficultyControlTextLabel->setAlignment(Qt::AlignCenter);  // 居中对齐

    // 难度显示标签
    m_currentDifficultyLabel = new QLabel(this);
    QFont difficultyFont = m_currentDifficultyLabel->font();
    difficultyFont.setPointSize(24);
    difficultyFont.setBold(true);
    m_currentDifficultyLabel->setFont(difficultyFont);
    m_currentDifficultyLabel->setStyleSheet(R"(
        QLabel {
            color: white;
            background: rgba(0, 0, 0, 0.2); /* 可选：加半透明背景增强可读性 */
            border-radius: 4px;
            padding: 5px 15px;
        }
    )");
    m_currentDifficultyLabel->setAlignment(Qt::AlignCenter);
    m_currentDifficultyLabel->setFixedWidth(300); // 固定宽度（根据需求调整）
    m_currentDifficultyLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred); // 禁止拉伸
    m_currentDifficultyLabel->setMinimumWidth(300); // 确保宽度不被压缩

    // 上一首/下一首按钮
    m_prevDifficultyBtn = new QPushButton(this);
    m_nextDifficultyBtn = new QPushButton(this);

    // 按钮固定大小
    m_prevDifficultyBtn->setFixedSize(60, 60);  // 固定宽高
    m_nextDifficultyBtn->setFixedSize(60, 60);
    m_prevDifficultyBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed); // 完全禁止拉伸
    m_nextDifficultyBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // 按钮样式设置（与静音按钮风格统一）
    QString btnStyle = R"(
        QPushButton {
            border: none;
            background: transparent;
            icon-size: 40px 40px;
            color: white;          /* 基础色 */
            icon-color: white;     /* Qt6.5+ 强制图标色 */
        }
        QPushButton:hover {
            background-color: rgba(255, 255, 255, 0.1);
            border-radius: 4px;
        }
        QPushButton:pressed {
            background-color: rgba(255, 255, 255, 0.2);
        }
    )";
    m_prevDifficultyBtn->setStyleSheet(btnStyle);
    m_nextDifficultyBtn->setStyleSheet(btnStyle);

    // 设置按钮图标
    // 在 initDifficultyControlWidgets 函数中，替换原来的图标设置代码：
    QStyle *style = this->style();
    // 上一首图标：先获取原始图标，再转为白色
    QIcon prevIcon = style->standardIcon(QStyle::SP_MediaSkipBackward);
    m_prevDifficultyBtn->setIcon(tintIcon(prevIcon, Qt::white, 40));
    // 下一首图标：同理
    QIcon nextIcon = style->standardIcon(QStyle::SP_MediaSkipForward);
    m_nextDifficultyBtn->setIcon(tintIcon(nextIcon, Qt::white, 40));

    // 难度控制按钮布局 : 布局固定间距 + 禁止控件拉伸
    m_difficultyControlLayout = new QHBoxLayout();
    m_difficultyControlLayout->setSpacing(30); // 固定按钮与中间标签的间距（根据需求调整）
    m_difficultyControlLayout->setContentsMargins(100, 0, 100, 0); // 左右留边（可选）
    m_difficultyControlLayout->setAlignment(Qt::AlignCenter);

    // 添加控件并设置拉伸权重（核心：0=不拉伸，1=拉伸）
    m_difficultyControlLayout->addWidget(m_prevDifficultyBtn, 0); // 权重0：不拉伸
    m_difficultyControlLayout->addWidget(m_currentDifficultyLabel, 0); // 权重0：不拉伸
    m_difficultyControlLayout->addWidget(m_nextDifficultyBtn, 0); // 权重0：不拉伸

    // 添加弹性空间使整体居中，不影响控件大小
    m_difficultyControlLayout->insertStretch(0, 1); // 左侧弹性空间
    m_difficultyControlLayout->addStretch(1); // 右侧弹性空间
}

// 切换上一难度
void SettingWidget::switchToPrevDifficulty()
{
    currentDifficulty = ((currentDifficulty - 1) + 4) % 4;
    // 更新显示
    updateDifficultyDisplay();
}

// 切换下一难度
void SettingWidget::switchToNextDifficulty()
{
    currentDifficulty = ((currentDifficulty + 1)) % 4;
    // 更新显示
    updateDifficultyDisplay();
}

// 更新难度显示标签
void SettingWidget::updateDifficultyDisplay()
{
    QString displayText;
    switch(currentDifficulty){
    case EASY:
        displayText = "EASY";
        break;
    case NORMAL:
        displayText = "NORMAL";
        break;
    case DIFFICULT:
        displayText = "DIFFICULT";
        break;
    case HELL:
        displayText = "HELL";
        break;
    }

    m_currentDifficultyLabel->setText(displayText);
}

// 图标上色核心函数：将原始图标渲染为指定颜色（默认白色）
QIcon SettingWidget::tintIcon(const QIcon& icon, const QColor& color, int size) {
    // 创建透明画布
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    // 1. 绘制原始图标（黑色）
    icon.paint(&painter, pixmap.rect());
    // 2. 设置混合模式：只保留图标区域，填充目标颜色
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(pixmap.rect(), color);
    painter.end();

    return QIcon(pixmap);
}
