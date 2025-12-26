#ifndef STARTWIDGET_H
#define STARTWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>

#include "animationbutton.h"

class StartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StartWidget(QWidget *parent = nullptr);

signals:
    void toLevel();
    void toSetting();
    void toMultiplayer();
    void toGame(int levelNum);
protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void loadBg(const QString &imgBg);

    void initWidget();
    void initGameNameLabel();

    QVBoxLayout *m_mainLayout;
    QLabel *m_gameNameLabel;
    QVBoxLayout *m_btnLayout;

    AnimationButton *m_levelBtn;
    AnimationButton *m_endlessBtn;
    AnimationButton *m_multiplayerBtn;
    AnimationButton *m_settingBtn;
};

#endif // STARTWIDGET_H
