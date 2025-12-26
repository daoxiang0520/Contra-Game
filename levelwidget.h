#ifndef LEVELWIDGET_H
#define LEVELWIDGET_H

#include <QWidget>

#include "animationbutton.h"


class LevelWidget : public QWidget
{
    Q_OBJECT
protected:
    void keyPressEvent(QKeyEvent *event) override;
public:
    explicit LevelWidget(QWidget *parent = nullptr);
    void setUnlock();
signals:
    void toStart();
    void toGame(int levelNum);
private:
    void loadBg(const QString &imgBg);
    int row = 4;
    int col = 4;
    QVector<QVector<AnimationButton*>> levels;
};

#endif // LEVELWIDGET_H
