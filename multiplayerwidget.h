#ifndef MULTIPLAYERWIDGET_H
#define MULTIPLAYERWIDGET_H

#include <QWidget>
#include <QVBoxLayout>

#include"animationbutton.h"

class MultiplayerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MultiplayerWidget(QWidget *parent = nullptr);
protected:
    void keyPressEvent(QKeyEvent *event) override;
signals:
    void toStart();
    void toGame(bool isCreator);

private:
    void loadBg(const QString &imgBg);
    void initWidget();

    QVBoxLayout *m_mainLayout;

    AnimationButton *m_createBtn;
    AnimationButton *m_joinBtn;
};

#endif // MULTIPLAYERWIDGET_H
