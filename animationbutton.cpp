#include "animationbutton.h"
#include <QFont>
#include <QString>

AnimationButton::AnimationButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent)
{
    // 启用toggle模式
    setCheckable(true);

    // 设置最小尺寸
    setMinimumSize(m_width, m_height);

    // 初始化字体
    QFont font = this->font();
    font.setPointSize(m_fontSize);
    font.setBold(m_isBold);
    setFont(font);

    // 初始化样式
    updateStyleSheet();
}

// 析构函数
AnimationButton::~AnimationButton() = default;

// 设置按钮大小
void AnimationButton::setBtnSize(int width, int height)
{
    m_width = width;
    m_height = height;
    setFixedSize(width, height);
    updateStyleSheet();
}

// 设置按钮样式
void AnimationButton::setBtnStyle(QColor bgColor, QColor borderColor, int borderWidth, int borderRadius)
{
    m_bgColor = bgColor;
    m_borderColor = borderColor;
    m_borderWidth = borderWidth;
    m_borderRadius = borderRadius;
    updateStyleSheet();
}

// 设置文本边距
void AnimationButton::setBtnPadding(int pl, int pt, int pr, int pb)
{
    m_paddingLeft = pl;
    m_paddingTop = pt;
    m_paddingRight = pr;
    m_paddingBottom = pb;
    updateStyleSheet();
}

// 设置文本样式
void AnimationButton::setBtnFont(int fs, bool ib, QColor tc)
{
    m_fontSize = fs;
    m_isBold = ib;
    m_textColor = tc;
    // 更新字体
    QFont font = this->font();
    font.setPointSize(m_fontSize);
    font.setBold(m_isBold);
    setFont(font);
    // 更新样式
    updateStyleSheet();
}

// QSS样式
void AnimationButton::updateStyleSheet()
{
    // 拼接QSS字符串，映射所有状态样式（与原paintEvent逻辑完全对齐）
    QString qss = QString(R"(
        QPushButton {
            /* 常态样式 */
            background-color: %1;
            border: %2px solid %3;
            border-radius: %4px;
            color: %5;
            font-size: %6px;
            font-weight: %7;
            padding-left: %8px;
            padding-top: %9px;
            padding-right: %10px;
            padding-bottom: %11px;
            min-width: %12px;
            min-height: %13px;
            /* 平滑过渡：所有状态变化带200ms动画 */
            transition: all 200ms ease-in-out;
        }

        /* 禁用状态 */
        QPushButton:disabled {
            background-color: #666666;
            border-color: #444444;
            color: #aaaaaa;
            border-width: %2px;
        }

        /* 选中状态（toggle） */
        QPushButton:checked {
            background-color: #FFCC00;
            border-color: #FFFF66;
            color: #333333;
            border-width: 3px;
        }

        /* 按下状态 */
        QPushButton:pressed {
            background-color: #EE8800;
            border-color: #CC6600;
            padding-top: %14px;
            padding-bottom: %15px;
            /* 按下偏移：模拟凹陷 */
            margin-top: 1px;
            margin-left: 1px;
        }

        /* 悬浮状态（非按下/非选中/非禁用） */
        QPushButton:hover:!pressed:!checked:!disabled {
            background-color: #FFAA00;
            border-color: #FFFF66;
            border-width: 3px;
        }
    )").arg(
                          m_bgColor.name(), // %1 常态背景色
                          QString::number(m_borderWidth), // %2 边框宽度
                          m_borderColor.name(), // %3 常态边框色
                          QString::number(m_borderRadius),// %4 圆角
                          m_textColor.name(), // %5 文本色
                          QString::number(m_fontSize), // %6 字号
                          m_isBold ? "bold" : "normal", // %7 字体加粗
                          QString::number(m_paddingLeft), // %8 左内边距
                          QString::number(m_paddingTop), // %9 上内边距
                          QString::number(m_paddingRight), // %10 右内边距
                          QString::number(m_paddingBottom), // %11 下内边距
                          QString::number(m_width), // %12 宽度
                          QString::number(m_height), // %13 高度
                          QString::number(m_paddingTop + 2), // %14 按下上内边距（+2）
                          QString::number(m_paddingBottom - 2) // %15 按下下内边距（-2）
                          );

    // 应用QSS
    setStyleSheet(qss);
}
