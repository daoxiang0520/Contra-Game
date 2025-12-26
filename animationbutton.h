#ifndef ANIMATIONBUTTON_H
#define ANIMATIONBUTTON_H

#include <QPushButton>
#include <QColor>

class AnimationButton : public QPushButton
{
    Q_OBJECT
public:
    // 构造函数
    explicit AnimationButton(const QString &text, QWidget *parent = nullptr);
    ~AnimationButton() override;
    // 设置按钮大小
    void setBtnSize(int width, int height);
    // 设置按钮样式（背景/边框/边框宽度/圆角）
    void setBtnStyle(QColor bgColor, QColor borderColor, int borderWidth, int borderRadius);
    // 设置文本边距（左上右下）
    void setBtnPadding(int pl, int pt, int pr, int pb);
    // 设置文本样式（字号/加粗/颜色）
    void setBtnFont(int fs, bool ib, QColor tc);

private:
    // 更新QSS样式
    void updateStyleSheet();

    // 样式配置参数
    int m_width = 120; // 默认宽度
    int m_height = 50; // 默认高度
    QColor m_bgColor = QColor(255, 153, 0); // 常态背景色（橙色）
    QColor m_borderColor = QColor(255, 204, 0); // 常态边框色
    int m_borderWidth = 2; // 默认边框宽度
    int m_borderRadius = 8; // 默认圆角
    int m_paddingLeft = 0; // 文本左内边距
    int m_paddingTop = 2; // 文本上内边距
    int m_paddingRight = 0; // 文本右内边距
    int m_paddingBottom = 2; // 文本下内边距
    int m_fontSize = 14; // 默认字号
    bool m_isBold = false; // 默认不加粗
    QColor m_textColor = QColor(255, 255, 255); // 默认文本白色
};

#endif // ANIMATIONBUTTON_H
