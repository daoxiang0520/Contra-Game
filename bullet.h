#ifndef BULLET_H
#define BULLET_H

#include <QObject>
#include <QPainter>
#include <QPointF>
#include "GameDefs.h"

// 前置声明
class BlockWindow;
class Player;
class Enemy;

class Bullet : public QObject
{
    Q_OBJECT
public:
    explicit Bullet(BlockWindow* window, QPointF pos, QPointF velocity,
                    BulletOwner owner, int damage, QObject *parent = nullptr);

    // 核心方法
    bool isDirectionRight() const{return m_velocity.x() > 0.0f;};
    void update();
    void draw(QPainter& painter);
    bool isAlive() const { return m_isAlive; }
    BulletOwner owner() const { return m_owner; }
    QRectF rect() const; // 碰撞盒

    // 碰撞处理
    void onCollision();  // 碰撞后逻辑（消失+扣血）

private:
    BlockWindow* m_window;          // 游戏窗口
    QPointF m_pos;                  // 位置
    QPointF m_velocity;                  // 速度
    BulletOwner m_owner;            // 所属者
    bool m_isAlive = true;          // 是否存活
    int m_damage;   // 伤害值
};

#endif // BULLET_H
