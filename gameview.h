#ifndef GAMEVIEW_H
#define GAMEVIEW_H
#include<QMainWindow>
#include<QGraphicsView>
#include <QEvent>
#include <QKeyEvent>
#include<set>
#include<QtMath>
#include<QTimer>

class GameView:public QGraphicsView
{
    Q_OBJECT
public:
    GameView(QWidget *parent = nullptr);
protected:
    void keyPressEvent(QKeyEvent *event) override ;

    void keyReleaseEvent(QKeyEvent *event) override ;
    void MoveViewPoint();
private:
    std::set<int> Key;
    QTimer* timer;
    int speed=16;
signals:
    void gameKeyPressed(int key);
    void gameKeyReleased(int key);
};

#endif // GAMEVIEW_H
