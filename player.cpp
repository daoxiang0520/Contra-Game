#include "player.h"
#include "BlockWindow.h"
#include "bullet.h"
#include <QKeyEvent>
#include <QMessageBox>
#include <QDebug>
extern int currentLevel;
QPixmap Player::currentPixmap()
{
    //qDebug()<<"!!!";
    if(m_isOnHealing)
    {
        if(m_curedFrame > 0)
        {
            m_curedFrame--;
            m_walkingStateNumber = 0;

            return (
                (m_dir == Direction::Right)
                ? m_cureRight
                : m_cureLeft
                );
        }
        else if(m_currentHealingCooldown == m_healingCooldown)
        {
            if( m_hp == m_maxHP )
            {
                return (
                    (m_dir == Direction::Right)
                        ? m_imgRight
                        : m_imgLeft
                    );
            }
            m_curedFrame = PLAYER_CURED_FRAME;
            m_curedFrame--;
            m_walkingStateNumber = 0;
            SoundPlayer *sp = new SoundPlayer(this);
            QString cureSound = QDir::currentPath()+"/../../bgms/cure.mp3";
            sp->playSound(cureSound);

            return (
                (m_dir == Direction::Right)
                    ? m_cureRight
                    : m_cureLeft
                );
        }
        else return (
                (m_dir == Direction::Right)
                    ? m_imgRight
                    : m_imgLeft);
    }
    else if(m_isOnSpike)
    {
        if(m_injuredFrame > 0)
        {
            m_injuredFrame --;
            m_walkingStateNumber = 0;

            return (
                (m_dir == Direction::Right)
                    ? m_injureRight
                    : m_injureLeft
                );

        }
        else if(m_currentSpikeCooldown == m_spikeCooldown)
        {
            m_injuredFrame = PLAYER_INJURED_FRAME;
            m_injuredFrame --;
            m_walkingStateNumber = 0;
            SoundPlayer *sp = new SoundPlayer(this);
            QString injureSound = QDir::currentPath()+"/../../bgms/injure.ogg";
            sp->playSound(injureSound);

            return (
                (m_dir == Direction::Right)
                    ? m_injureRight
                    : m_injureLeft
                );
        }
        else return (
                (m_dir == Direction::Right)
                    ? m_imgRight
                    : m_imgLeft);
    }
    else return (
            (m_dir == Direction::Right)
                ? m_imgRight
                : m_imgLeft);
}
Player::Player(BlockWindow *window, QObject *parent)
    : QObject(parent), m_window(window), m_x(m_window->PLAYER_START_X), m_y(m_window->PLAYER_START_Y), m_vx(0), m_vy(0)
{
    if(currentDifficulty==0) PLAYER_MAX_HP=20;
    else if(currentDifficulty==1) PLAYER_MAX_HP=15;
    else if(currentDifficulty==2) PLAYER_MAX_HP=10;
    else if(currentDifficulty==3) PLAYER_MAX_HP=5;
    m_hp=PLAYER_MAX_HP;
    m_maxHP = PLAYER_MAX_HP;
    m_gravity = (currentLevel == 16) ?0.01:1.0;
    QStringList rightPaths = {
        ":/image/001.png",
        ":/image/002.png",
        ":/image/003.png",
        ":/image/004.png",
        ":/image/005.png",
        ":/image/006.png",
        ":/image/007.png",
        ":/image/008.png",
        ":/image/009.png",
        ":/image/010.png",
        ":/image/011.png",
        ":/image/012.png"
    };
    QStringList leftPaths = {
        ":/image/101.png",
        ":/image/102.png",
        ":/image/103.png",
        ":/image/104.png",
        ":/image/105.png",
        ":/image/106.png",
        ":/image/107.png",
        ":/image/108.png",
        ":/image/109.png",
        ":/image/110.png",
        ":/image/111.png",
        ":/image/112.png"
    };

    cureLeftPath = ":/image/curedleft.png";
    m_cureLeft.load(cureLeftPath);
    cureRightPath = ":/image/curedright.png";
    m_cureRight.load(cureRightPath);

    injureLeftPath = ":/image/injuredleft.png";
    m_injureLeft.load(injureLeftPath);
    injureRightPath = ":/image/injuredright.png";
    m_injureRight.load(injureRightPath);

    for (const QString &p : rightPaths) { QPixmap px; px.load(p); m_walkRight.append(px); }
    for (const QString &p : leftPaths)  { QPixmap px; px.load(p); m_walkLeft.append(px); }

    if (!m_walkRight.isEmpty()) m_imgRight = m_walkRight[0];
    if (!m_walkLeft.isEmpty())  m_imgLeft  = m_walkLeft[0];
    if (!m_walkRight.isEmpty()) m_imgRight = m_walkRight[0];
    if (!m_walkLeft.isEmpty())  m_imgLeft  = m_walkLeft[0];
    // 加载朝向图片
    m_imgRight.load(":/image/001.png");
    m_imgLeft.load(":/image/101.png");
    if (m_imgRight.isNull() || m_imgLeft.isNull()) {
        QMessageBox::warning(m_window, "警告", "玩家图片加载失败！将使用默认图形。");
    }
}
QJsonObject Player::toJsonObject() const
{
    QJsonObject obj;
    obj.insert("playerSpeedX", m_vx);
    obj.insert("playerPositionX", m_x);
    obj.insert("playerHP", m_hp);


    return obj;
}

void Player::handleKeyPress(QKeyEvent *event)
{
    // qDebug() << "Called: Player::handleKeyPress";
    if (event->isAutoRepeat()) return;
    if (!isAlive()) return;
    // qDebug() << "bbb";
    switch (event->key()) {
    case Qt::Key_Left:
        m_isLeftPressed = true;
        m_dir = Direction::Left; // 更新朝向
        // qDebug() << "aaa";
        break;
    case Qt::Key_Right:
        m_isRightPressed = true;
        m_dir = Direction::Right;// 更新朝向
        break;
    case Qt::Key_Up:
        m_isJumpPressed = true;
        // if ((m_isOnGround || m_jumpBufferFrames > 0) && !m_isJumping)
        if ((m_isOnGround || m_jumpBufferFrames > 0) )
        {
            //跳跃缓冲帧
            m_vy = PLAYER_JUMP_FORCE;
            m_isJumping = true;
            m_isOnGround = false;
            m_jumpBufferFrames = 0;

        }
        break;
    case Qt::Key_Space: // 按空格发射子弹
        m_isFirePressed = true;
        fireBullet();
        break;
    case Qt::Key_C: // 按C改变发射模式
        m_isFireModeChangedTriggered = true;
        break;
    default:
        break;
    }
}

void Player::handleKeyRelease(QKeyEvent *event)
{
    // qDebug() << "Called: Player::handleKeyRelease";
    if (event->isAutoRepeat()) return;

    switch (event->key()) {
    case Qt::Key_Left:
        m_isLeftPressed = false;
        if(m_isRightPressed == true)m_dir = Direction::Right;
        break;
    case Qt::Key_Right:
        m_isRightPressed = false;
        if(m_isLeftPressed == true)m_dir = Direction::Left;
        break;
    case Qt::Key_Up:
        m_isJumpPressed = false;
        break;
    case Qt::Key_Space:
        m_isFirePressed = false;
        break;
    case Qt::Key_C: // 按C改变发射模式
        m_isFireModeChangedTriggered = true;
        changeFireMode();
    default:
        break;
    }
}

void Player::update(float deltaTime)
{
    // qDebug() << "playerUpdateCalledSTARTED" << numDebug++;
    // if(m_willSoonOnGround||m_jumpBufferFrames)
    // {
    //     qDebug()<<"Update() called!";
    //     qDebug()<<"BufferFrame"<<m_jumpBufferFrames;
    //     qDebug()<<"OnGround"<<(m_isOnGround?"true":"false");
    //     qDebug()<<"WillOnGround"<<(m_willSoonOnGround?"true":"false");
    // }
    // qDebug() << "playerUpdateCalled" << numDebug++;
    // qDebug() << (m_isLeftPressed?"Pressed":"NoResponse");
    m_walkingStateNumber++;
    if(m_walkingStateNumber==12) m_walkingStateNumber=0;
    if (!isAlive()) return;

    // 发射冷却递减
    if (m_currentFireCooldown > 0) {
        m_currentFireCooldown--;
    }

    //受到地刺伤害的冷却（保护期，防止一直受伤）
    if (m_currentSpikeCooldown > 0) {
        m_currentSpikeCooldown--;
    }

    //受到地刺伤害的冷却（保护期，防止一直受伤）
    if (m_currentHealingCooldown > 0) {
        m_currentHealingCooldown--;
    }

    //水平速度
    m_vx = 0;
    if (m_isLeftPressed) m_vx += -PLAYER_MOVE_SPEED;
    if (m_isRightPressed) m_vx += PLAYER_MOVE_SPEED;
    if(m_vx == 0) m_walkingStateNumber = 0;//如果速度为0，保持站立姿势

    // if(m_vx==0)
    // {
    //     qDebug()<<"no speed！";
    //         m_vx=1;
    // }
    //计算移动并且检测碰撞
    m_x += m_vx;
    // if(m_vx==0)
    // {
    //     qDebug()<<"no speed again！";
    //     m_vx=1;
    // }
    // else qDebug()<<"yes speed again！";
    resolveHorizontalCollision();

    //检测当前有没有在弹跳方块
    detectSpring();
    // qDebug() << "Called!!";

    //竖直速度
    if (!m_isOnGround||m_isOnSpring)
    {
        m_vy += m_gravity;
        if (m_vy > PLAYER_MAX_FALL_SPEED) m_vy = PLAYER_MAX_FALL_SPEED;
        //防止速度过高
        //if(m_vy>0) qDebug()<<"falling";
        m_y += m_vy;
    }
    if(m_vy != 0) m_walkingStateNumber = 0;//如果速度为0，保持站立姿势

    //垂直方向碰撞检测
    resolveVerticalCollision();

    //检测当前有没有在地刺上
    detectSpike();

    //检测当前有没有在治疗方块
    detectHealing();


    //处理缓冲帧
    if (m_jumpBufferFrames > 0) {
        m_jumpBufferFrames--;
        // qDebug()<<m_jumpBufferFrames<<"!!";
    }

    updateAnimation();
    // qDebug() << "playerUpdateCalledFINISHED" << numDebug++;
}

void Player::draw(QPainter &painter)
{
    if (!isAlive()) {
        painter.setPen(Qt::red);
        painter.setFont(QFont("Arial", 20));
        painter.drawText(WINDOW_WIDTH/2 - 80, WINDOW_HEIGHT/2, "玩家死亡！");
        return;
    }

    // 根据朝向绘制图片
    QPixmap img = (m_dir == Direction::Right) ? m_imgRight : m_imgLeft;
    // if (!img.isNull()) {
    //     painter.drawPixmap(m_x, m_y, m_width, m_height, img);
    // } else {
    //     painter.setBrush(Qt::blue);
    //     painter.setPen(Qt::black);
    //     painter.drawRect(m_x, m_y, m_width, m_height);
    //     // 绘制朝向标识
    //     painter.drawText(m_x + m_width/2, m_y + m_height/2,
    //                      m_dir == Direction::Right ? "→" : "←");
    // }

    // 绘制生命值
    painter.setPen(Qt::green);
    painter.setFont(QFont("Arial", 12));
    // painter.drawText(m_x, m_y - 5, QString("HP: %1").arg(m_hp));
}
void Player::setPosition(int x, int y)
{
    m_x = x;
    m_y = y;
    m_vx = 0;
    m_vy = 0;
    m_isOnGround = false;
    m_isJumping = false;
    m_walkingStateNumber = 0;
    // 如需同步 scene item 请由外部 (BlockWindow) 更新 m_playerItem 的位置
}
void Player::takeDamage(int damage)
{
    if (!isAlive()) return;
    m_hp -= damage;
    if (m_hp < 0) m_hp = 0;
}

QRectF Player::rect() const
{
    return QRectF(m_x, m_y, m_width, m_height);
}

bool Player::checkCollision(int x, int y)
{
    if (!m_window) return false;
    return m_window->checkPlayerCollision(x, y, m_width, m_height);
}

void Player::resolveHorizontalCollision()
{
    // qDebug()<<"Called:resolveHorizontalCollision()";
    // 左边界限制
    if (m_x < 0) {
        m_x = 0;
        m_vx = 0;
        return;
    }
    // qDebug()<<"step1";

     //右边界限制
    if (m_x + m_width > m_window->Getwidth()) {
        m_x = m_window->Getwidth() - m_width;
        m_vx = 0;
        return;
    }
    // qDebug()<<"step2";
    //发生碰撞后精细回退（如果现在撞墙了，就一像素一像素地退回到之前没撞的状态）
    int step = (m_vx > 0) ? 1 : -1;
    while (step != 0 && checkCollision(m_x, m_y)) {
        m_x -= step;
        // qDebug()<<"step3";
        // 边界兜底
        if (m_x < 0) {
            m_x = 0;
            break;
        }
        if (m_x + m_width > m_window->Getwidth()) {
            m_x = m_window->Getwidth() - m_width;
            break;
        }
    }

    // 贴墙后清零速度
    if (checkCollision(m_x + step, m_y)) {
        m_vx = 0;
    }
}

void Player::resolveVerticalCollision()
{
    if (!m_window) return;

    //防止跳出上面
    if (m_y < 0) {
        m_y = 0;
        m_vy = 0;
        return;
    }

    //处理掉下低层
    if (m_y + m_height > m_window->Getheight()) {
        // m_x = PLAYER_START_X;
        // m_y = PLAYER_START_Y;
        // m_vy = 0;
        // m_isOnGround = false;
        // takeDamage(1); //扣血
        // return;
    }

    //发生碰撞后精细回退
    int step = (m_vy > 0) ? 1 : -1;
    while (step != 0 && checkCollision(m_x, m_y)) {
        m_y -= step;
        if (m_y < 0) {
            m_y = 0;
            break;
        }
    }

    //落地判断及处理
    m_isOnGround = m_window->isPlayerOnGround(m_x, m_y, m_width, m_height);
    if (m_vy > 0 && m_isOnGround) {
        // 对齐瓦片顶部，防止脚卡人方块里
        int footY = m_y + m_height;
        int tileSize = m_window->tileSize();
        m_y = (footY / tileSize) * tileSize - m_height;
        m_vy = 0;
        m_isJumping = false;

    } else if (m_vy < 0 && checkCollision(m_x, m_y + step)) {
        // 撞顶处理
        m_vy = 0;
        m_isOnGround = false;
    }

    //即将落地检测（跳跃缓冲优化）

    m_willSoonOnGround = m_window->isPlayerOnGround(m_x, m_y + PLAYER_JUMP_BUFFER_DISTANCE, m_width, m_height);
    if(m_vy < 0 || m_isOnGround) m_willSoonOnGround = false; //向上运动不可能
    if(m_willSoonOnGround && m_jumpBufferFrames == 0)
    {
        m_jumpBufferFrames = PLAYER_JUMP_BUFFER_NUMBER; // 缓冲帧
    }

}

void Player::detectSpike()
{
    if(!m_isOnGround)
    {
        m_isOnSpike = false;
        m_currentSpikeCooldown = 0;
        return;
    }

    //检测站的格子的类型是不是地刺
    m_isOnSpike = (m_window->playerOnGroundState(m_x, m_y, m_width, m_height) == 2)? true : false;


    if(m_currentSpikeCooldown == 0 && m_isOnSpike)
    {
        takeDamage(1);
        m_currentSpikeCooldown = m_spikeCooldown;
    }
    return;
}

void Player::detectHealing()
{
    if(!m_isOnGround)
    {
        m_isOnHealing = false;
        m_currentHealingCooldown = 0;
        return;
    }

    //检测站的格子的类型是不是治疗方块
    m_isOnHealing = (m_window->playerOnGroundState(m_x, m_y, m_width, m_height) == 3)? true : false;

    if(m_currentHealingCooldown == 0 && m_isOnHealing)
    {
        if(m_hp < m_maxHP) takeDamage(-1);
        m_currentHealingCooldown = m_healingCooldown;
    }
    return;
}
void Player::detectSpring()
{
    if(!m_isOnGround)
    {
        m_isOnSpring = false;
        return;
    }

    //检测站的格子的类型是不是弹簧
    m_isOnSpring = (m_window->playerOnGroundState(m_x, m_y, m_width, m_height) == 4)? true : false;
    //qDebug() << m_window->playerOnGroundState(m_x, m_y, m_width, m_height) <<"??";
    //qDebug() << (m_isOnSpring?"true":"false") <<"??";

    if(m_isOnSpring)
    {
        m_vy = SPRING_VELOCITY;
    }

    //qDebug() << (m_isOnSpring?"true":"false") <<"??";
    return;
}

void Player::updateAnimation()
{
    if (m_walkRight.isEmpty() || m_walkLeft.isEmpty()) return; // safe guard
    int idx = m_walkingStateNumber % m_walkRight.size();
    if (m_isOnGround) {
        m_imgRight = m_walkRight[idx];
        m_imgLeft  = m_walkLeft [idx];
    } else {
        m_imgRight = m_walkRight[qBound(0, 4, m_walkRight.size()-1)];
        m_imgLeft  = m_walkLeft [qBound(0, 4, m_walkLeft.size()-1)];
    }
}

void Player::fireBullet()
{
    if (!isAlive() || m_currentFireCooldown > 0) return;

    // 子弹生成在玩家朝向的一端
    QPointF bulletPos;
    QPointF bulletVel;


    switch(m_fireBulletMode)
    {
        case 0:
            if (m_dir == Direction::Right)
            {
                bulletPos = QPointF(m_x + m_width - BULLET_WIDTH, m_y + m_height/2 - BULLET_HEIGHT/2);
                bulletVel = QPointF(BULLET_SPEED, 0);
            }
            else
            {
                bulletPos = QPointF(m_x, m_y + m_height/2 - BULLET_HEIGHT/2);
                bulletVel = QPointF(-BULLET_SPEED, 0);
            }
            m_window->addBullet(new Bullet(m_window, bulletPos, bulletVel, BulletOwner::Player,m_bulletDamage));
            break;
        case 1:
            if (m_dir == Direction::Right)
            {
                bulletPos = QPointF(m_x + m_width - BULLET_WIDTH, m_y + m_height/2 - BULLET_HEIGHT/2 - BULLET_DEVIATION_DISTANCE);
                bulletVel = QPointF(BULLET_SPEED, 0);
            }
            else
            {
                bulletPos = QPointF(m_x, m_y + m_height/2 - BULLET_HEIGHT/2 - BULLET_DEVIATION_DISTANCE);
                bulletVel = QPointF(-BULLET_SPEED, 0);
            }
            m_window->addBullet(new Bullet(m_window, bulletPos, bulletVel, BulletOwner::Player,m_bulletDamage));//向上偏移
            if (m_dir == Direction::Right)
            {
                bulletPos = QPointF(m_x + m_width - BULLET_WIDTH, m_y + m_height/2 - BULLET_HEIGHT/2 + BULLET_DEVIATION_DISTANCE);
                bulletVel = QPointF(BULLET_SPEED, 0);
            }
            else
            {
                bulletPos = QPointF(m_x, m_y + m_height/2 - BULLET_HEIGHT/2 + BULLET_DEVIATION_DISTANCE);
                bulletVel = QPointF(-BULLET_SPEED, 0);
            }
            m_window->addBullet(new Bullet(m_window, bulletPos, bulletVel, BulletOwner::Player,m_bulletDamage));//向下偏移
            if (m_dir == Direction::Right)
            {
                bulletPos = QPointF(m_x + m_width - BULLET_WIDTH, m_y + m_height/2 - BULLET_HEIGHT/2);
                bulletVel = QPointF(BULLET_SPEED, 0);
            }
            else
            {
                bulletPos = QPointF(m_x, m_y + m_height/2 - BULLET_HEIGHT/2);
                bulletVel = QPointF(-BULLET_SPEED, 0);
            }
            m_window->addBullet(new Bullet(m_window, bulletPos, bulletVel, BulletOwner::Player,m_bulletDamage));//无偏移

            break;
        case 2:
            if (m_dir == Direction::Right)
            {
                bulletPos = QPointF(m_x + m_width - BULLET_WIDTH, m_y + m_height/2 - BULLET_HEIGHT/2);
                bulletVel = QPointF(BULLET_SPEED, 0);
            }
            else
            {
                bulletPos = QPointF(m_x, m_y + m_height/2 - BULLET_HEIGHT/2);
                bulletVel = QPointF(-BULLET_SPEED, 0);
            }
            m_window->addBullet(new Bullet(m_window, bulletPos, bulletVel, BulletOwner::Player,m_bulletDamage));//无偏移
            if (m_dir == Direction::Right)
            {
                bulletPos = QPointF(m_x + m_width - BULLET_WIDTH, m_y + m_height/2 - BULLET_HEIGHT/2);
                bulletVel = QPointF(BULLET_SPEED, 0.5);
            }
            else
            {
                bulletPos = QPointF(m_x, m_y + m_height/2 - BULLET_HEIGHT/2);
                bulletVel = QPointF(-BULLET_SPEED, 0.5);
            }
            m_window->addBullet(new Bullet(m_window, bulletPos, bulletVel, BulletOwner::Player,m_bulletDamage));//斜下偏移2
            if (m_dir == Direction::Right)
            {
                bulletPos = QPointF(m_x + m_width - BULLET_WIDTH, m_y + m_height/2 - BULLET_HEIGHT/2);
                bulletVel = QPointF(BULLET_SPEED, 1);
            }
            else
            {
                bulletPos = QPointF(m_x, m_y + m_height/2 - BULLET_HEIGHT/2);
                bulletVel = QPointF(-BULLET_SPEED, 1);
            }
            m_window->addBullet(new Bullet(m_window, bulletPos, bulletVel, BulletOwner::Player,m_bulletDamage));//斜下偏移1
            if (m_dir == Direction::Right)
            {
                bulletPos = QPointF(m_x + m_width - BULLET_WIDTH, m_y + m_height/2 - BULLET_HEIGHT/2);
                bulletVel = QPointF(BULLET_SPEED, -0.5);
            }
            else
            {
                bulletPos = QPointF(m_x, m_y + m_height/2 - BULLET_HEIGHT/2);
                bulletVel = QPointF(-BULLET_SPEED, -0.5);
            }
            m_window->addBullet(new Bullet(m_window, bulletPos, bulletVel, BulletOwner::Player,m_bulletDamage));//斜上偏移2
            if (m_dir == Direction::Right)
            {
                bulletPos = QPointF(m_x + m_width - BULLET_WIDTH, m_y + m_height/2 - BULLET_HEIGHT/2);
                bulletVel = QPointF(BULLET_SPEED, -1);
            }
            else
            {
                bulletPos = QPointF(m_x, m_y + m_height/2 - BULLET_HEIGHT/2);
                bulletVel = QPointF(-BULLET_SPEED, -1);
            }
            m_window->addBullet(new Bullet(m_window, bulletPos, bulletVel, BulletOwner::Player,m_bulletDamage));//斜上偏移1

            break;
        default:

            break;
    }


    m_currentFireCooldown = m_fireCooldown; // 重置冷却
}

void Player::changeFireMode()
{
    m_fireBulletMode ++;
    if(m_fireBulletMode == 3) m_fireBulletMode = 0;
    return;
}
