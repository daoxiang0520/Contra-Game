#include "gameview.h"
inline int max(int _X,int _Y){
    return _X>_Y?_X:_Y;
}inline int min(int _X,int _Y){
    return _X<_Y?_X:_Y;
}
GameView::GameView(QWidget *parent) {
    setFocusPolicy(Qt::StrongFocus);
    timer=new QTimer(this);
    connect(timer,&QTimer::timeout,this,&GameView::MoveViewPoint);
}

void GameView::keyPressEvent(QKeyEvent *event){
    int k = event->key();
    if(k==Qt::Key_Up||k==Qt::Key_Down||k==Qt::Key_W||k==Qt::Key_S)return;
    if (k == Qt::Key_Left || k == Qt::Key_Right||k == Qt::Key_S || k == Qt::Key_D) {
        // 自己处理或转发到游戏逻辑（发信号或调用回调）
        //emit gameKeyPressed(k);
        // 不调用 QGraphicsView::keyPressEvent，防止视图默认滚动
        Key.insert(k);
        timer->start(16);
        return;
    }
    QGraphicsView::keyPressEvent(event); // 其它键交给基类
}
void GameView::keyReleaseEvent(QKeyEvent *event){
    int k = event->key();
    if(k==Qt::Key_Up||k==Qt::Key_Down||k==Qt::Key_W||k==Qt::Key_S)return;
    if (k == Qt::Key_Left || k == Qt::Key_Right||k == Qt::Key_S || k == Qt::Key_D){
        //emit gameKeyReleased(k);
        Key.erase(k);
        timer->stop();
        return;
    }
    QGraphicsView::keyReleaseEvent(event);
}
void GameView::MoveViewPoint(){
    if(!scene()||Key.empty())return;
    QPointF viewCenter=mapToScene(viewport()->rect().center());
    QRectF SceneRect=scene()->sceneRect();
    if(Key.count(Qt::Key_Right)||Key.count(Qt::Key_D))viewCenter.rx()+=16;
    if(Key.count(Qt::Key_Left)||Key.count(Qt::Key_A))viewCenter.rx()-=16;
    viewCenter.rx()=max(viewCenter.x(),512);
    viewCenter.rx()=min(viewCenter.x(),SceneRect.width()-512);
    qDebug()<<SceneRect.width()<<viewCenter.x()<<viewCenter.y();
    centerOn(viewCenter);
}
