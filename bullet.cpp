#include "bullet.h"
#include "BlockWindow.h"
#include "Player.h"
#include "Enemy.h"
#include <QRectF>

Bullet::Bullet(BlockWindow *window, QPointF pos, QPointF velocity,
               BulletOwner owner, int damage, QObject *parent)
    : QObject(parent), m_window(window), m_pos(pos), m_velocity(velocity), m_owner(owner), m_damage(damage)
{}
//parent利用对象树，保证子类的析构（似乎没调用这个参数，待实现？？）

void Bullet::update()
{
    if (!m_isAlive) return;

    //计算运动
    m_pos += m_velocity;

    //出地图则消失
    QRectF mapRect(0, 0, m_window->Getwidth(), m_window->Getheight());
    if (!mapRect.contains(rect())) {
        m_isAlive = false;
        return;
    }

    //障碍物碰撞检测
    if (m_window->checkTileCollision(m_pos.x(), m_pos.y(), BULLET_WIDTH, BULLET_HEIGHT)) {
        m_isAlive = false;
        return;
    }

    //活体碰撞检测
    onCollision();
}

void Bullet::draw(QPainter &painter)
{
    if (!m_isAlive) return;
    // 绘制子弹（玩家子弹红色，敌人子弹绿色）
    // painter.setBrush(m_owner == BulletOwner::Player ? Qt::red : Qt::green);
    // painter.setPen(Qt::black);
    // painter.drawRect(m_pos.x(), m_pos.y(), BULLET_WIDTH, BULLET_HEIGHT);
}

QRectF Bullet::rect() const
{
    return QRectF(m_pos.x(), m_pos.y(), BULLET_WIDTH, BULLET_HEIGHT);
}

void Bullet::onCollision()
{
    // 玩家子弹碰撞敌人
    if (m_owner == BulletOwner::Player) {
        for (Enemy* enemy : m_window->enemies()) {
            if (enemy->isAlive() && enemy->rect().intersects(rect())) {
                enemy->takeDamage(m_damage);
                m_isAlive = false;
                return;
            }
        }
    }
    // 敌人子弹碰撞玩家
    else if (m_owner == BulletOwner::Enemy) {
        Player* player = m_window->player();
        if (player->isAlive() && player->rect().intersects(rect())) {
            player->takeDamage(m_damage);
            m_isAlive = false;
            return;
        }
    }
}
