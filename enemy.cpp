#include "Enemy.h"
#include "BlockWindow.h"
#include "Player.h"
#include "Bullet.h"
#include <QMessageBox>
#include <QLineF>
#include <QTransform>

Enemy::Enemy(BlockWindow *window, QPointF pos, float moveLeftBound,
             float moveRightBound, QObject *parent)
    : QObject(parent), m_window(window), m_pos(pos),
    m_leftBound(moveLeftBound), m_rightBound(moveRightBound)
{
    // 加载图片
    m_imgLeft.load(":/image/201.png");
    m_imgRight.load(":/image/301.png");
    m_imgDeath.load(":/image/death.png");
    if (m_imgLeft.isNull() || m_imgDeath.isNull()||m_imgRight.isNull()) {
        QMessageBox::warning(m_window, "警告", "敌人图片加载失败！将使用默认图形。");
    }
}

void Enemy::update()
{
    if (!isAlive()) {
        die();
        return;
    }

    // 1. 往返移动
    move();

    // 2. 冷却递减
    if (m_currentCooldown > 0) {
        m_currentCooldown--;
    }

    // 3. 检测玩家并发射子弹
    checkPlayerAndFire();
}

QPixmap Enemy::currentPixmap()
{
    if (m_hp <= 0)
    {
        return m_imgDeath;
        qDebug()<<"1.4";
    }
    if (m_dir == Direction::Left) return m_imgLeft;
    if (m_dir == Direction::Right) return m_imgRight;
}


void Enemy::draw(QPainter &painter)
{
    //if (!m_isAlive) return;
    // qDebug()<<"Called!";
    // 绘制敌人（死亡显示死亡图片）
    if (m_hp <= 0)
    {
        if (!m_imgDeath.isNull())
        {
            painter.drawPixmap(m_pos.x(), m_pos.y(), m_width, m_height, m_imgDeath);
        }
        else
        {
            painter.setBrush(Qt::darkRed);
            painter.setPen(Qt::black);
            painter.drawRect(m_pos.x(), m_pos.y(), m_width, m_height);
        }
    }
    else
    {
        m_walkingStateNumber+=0.5;
        if(m_walkingStateNumber==12) m_walkingStateNumber=0;
        if(m_walkingStateNumber==0)m_imgRight.load(":/image/301.png");
        if(m_walkingStateNumber==1)m_imgRight.load(":/image/302.png");
        if(m_walkingStateNumber==2)m_imgRight.load(":/image/303.png");
        if(m_walkingStateNumber==3)m_imgRight.load(":/image/304.png");
        if(m_walkingStateNumber==4)m_imgRight.load(":/image/305.png");
        if(m_walkingStateNumber==5)m_imgRight.load(":/image/306.png");
        if(m_walkingStateNumber==6)m_imgRight.load(":/image/307.png");
        if(m_walkingStateNumber==7)m_imgRight.load(":/image/308.png");
        if(m_walkingStateNumber==8)m_imgRight.load(":/image/309.png");
        if(m_walkingStateNumber==9)m_imgRight.load(":/image/310.png");
        if(m_walkingStateNumber==10)m_imgRight.load(":/image/311.png");
        if(m_walkingStateNumber==11)m_imgRight.load(":/image/312.png");
        if(m_walkingStateNumber==0)m_imgLeft.load(":/image/201.png");
        if(m_walkingStateNumber==1)m_imgLeft.load(":/image/202.png");
        if(m_walkingStateNumber==2)m_imgLeft.load(":/image/203.png");
        if(m_walkingStateNumber==3)m_imgLeft.load(":/image/204.png");
        if(m_walkingStateNumber==4)m_imgLeft.load(":/image/205.png");
        if(m_walkingStateNumber==5)m_imgLeft.load(":/image/206.png");
        if(m_walkingStateNumber==6)m_imgLeft.load(":/image/207.png");
        if(m_walkingStateNumber==7)m_imgLeft.load(":/image/208.png");
        if(m_walkingStateNumber==8)m_imgLeft.load(":/image/209.png");
        if(m_walkingStateNumber==9)m_imgLeft.load(":/image/210.png");
        if(m_walkingStateNumber==10)m_imgLeft.load(":/image/211.png");
        if(m_walkingStateNumber==11)m_imgLeft.load(":/image/212.png");
        QPixmap img;
        if (m_dir == Direction::Left)
        {
            img = m_imgLeft;
        }
        if (m_dir == Direction::Right)
        {
            img = m_imgRight;
        }
        if (!img.isNull())
        {
            painter.drawPixmap(m_pos.x(), m_pos.y(), m_width, m_height, img);
        }
        else
        {
            painter.setBrush(Qt::red);
            painter.setPen(Qt::black);
            painter.drawRect(m_pos.x(), m_pos.y(), m_width, m_height);
            // 绘制朝向标识
            painter.drawText(m_pos.x() + m_width/2, m_pos.y() + m_height/2,
                             m_dir == Direction::Right ? "→" : "←");
        }
    }
    // 绘制生命值（调试用）
    painter.setPen(Qt::yellow);
    painter.drawText(m_pos.x(), m_pos.y() - 5, QString::number(m_hp));
}

void Enemy::takeDamage(int damage)
{
    if (!isAlive()) return;
    m_hp -= damage;
    if (m_hp < 0) m_hp = 0;
}

QRectF Enemy::rect() const
{
    return QRectF(m_pos.x(), m_pos.y(), m_width, m_height);
}

void Enemy::move()
{
    // 右移触边界则左转
    if (m_dir == Direction::Right && m_pos.x() >= m_rightBound) {
        m_dir = Direction::Left;
    }
    // 左移触边界则右转
    else if (m_dir == Direction::Left && m_pos.x() <= m_leftBound) {
        m_dir = Direction::Right;
    }

    // 移动
    if (m_dir == Direction::Right) {
        m_pos.rx() += m_moveSpeed;
    } else {
        m_pos.rx() -= m_moveSpeed;
    }

    // 边界兜底（防止出地图）
    if (m_pos.x() < 0) m_pos.setX(0);
    if (m_pos.x() + m_width > m_window->Getwidth()) m_pos.setX(m_window->Getwidth() - m_width);
}

void Enemy::checkPlayerAndFire()
{
    if (m_currentCooldown > 0 || !m_window->player()->isAlive()) return;

    Player* player = m_window->player();
    QLineF line(m_pos, player->pos());

    // 1. 检测玩家是否在同一水平线（上下误差64内，适配人物高度）
    if (qAbs(line.dy()) > 64) return;

    // 2. 检测玩家是否在敌人朝向的直线上（无障碍物）
    bool hasObstacle = false;
    float step = 5; // 射线检测步长
    for (float t = 0; t < 1; t += step / line.length()) {
        QPointF p = line.pointAt(t);
        if (m_window->checkTileCollision(p.x(), p.y(), 1, 1)) {
            hasObstacle = true;
            break;
        }
    }
    if (hasObstacle) return;

    // 3. 发射子弹
    QPointF bulletVel(0, 0);
    if (player->x() > m_pos.x()) {
        bulletVel = QPointF(BULLET_SPEED, 0);
        m_dir = Direction::Right;
    } else {
        bulletVel = QPointF(-BULLET_SPEED, 0);
        m_dir = Direction::Left;
    }
    // 子弹生成在敌人中心
    QPointF bulletPos(m_pos.x() + m_width/2 - BULLET_WIDTH/2,
                      m_pos.y() + m_height/2 - BULLET_HEIGHT/2);
    m_window->addBullet(new Bullet(m_window, bulletPos, bulletVel, BulletOwner::Enemy, m_bulletDamage));
    m_currentCooldown = m_fireCooldown;
}

void Enemy::die()
{
    m_deathFrame++;
    // 死亡动画显示30帧后消失
    if (m_deathFrame >= 30) {
        m_isAlive = false;
    }
}
