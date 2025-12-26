#ifndef GAMEDEFS_H
#define GAMEDEFS_H

// 全局难度枚举
enum Difficulty{
    EASY,
    NORMAL,
    DIFFICULT,
    HELL
};

// 朝向枚举
enum class Direction {
    Left,
    Right
};

// 子弹所属者（区分玩家/敌人子弹）
enum class BulletOwner {
    Player,
    Enemy,
    None
};

// 核心尺寸常量（32×32方块，32×64人物，32×32地图）
const int TILE_SIZE = 32;                // 地图方块尺寸
const int MAP_WIDTH_TILES = 32;          // 地图宽度（方块数）
const int MAP_HEIGHT_TILES = 24;         // 地图高度（方块数）
const int WINDOW_WIDTH = TILE_SIZE * MAP_WIDTH_TILES;  // 窗口宽度 1024
const int WINDOW_HEIGHT = TILE_SIZE * MAP_HEIGHT_TILES;// 窗口高度 1024

// 玩家常量
const int PLAYER_WIDTH = 32;             // 玩家宽度
const int PLAYER_HEIGHT = 64;            // 玩家高度
const int PLAYER_MOVE_SPEED = 4;         // 玩家移动速度
const int PLAYER_JUMP_FORCE = -20;       // 玩家跳跃力度
const int PLAYER_GRAVITY = 1;            // 玩家重力
const int PLAYER_MAX_FALL_SPEED = 12;    // 玩家最大下落速度
const int PLAYER_JUMP_BUFFER_DISTANCE = 64;
const int PLAYER_JUMP_BUFFER_NUMBER = 12;
const int PLAYER_CURED_FRAME = 3;
const int PLAYER_INJURED_FRAME = 3;

//其他常量
const int SPRING_VELOCITY = -40; //跳跃方块速度

// 敌人常量
const int ENEMY_WIDTH = 32;              // 敌人宽度
const int ENEMY_HEIGHT = 64;             // 敌人高度
const int ENEMY_SPEED = 2;               // 敌人移动速度
const int ENEMY_MAX_HP = 5;              // 敌人初始生命值
const int ENEMY_DYING_FRAME = 30;       //敌人死亡动画帧数

// 子弹常量
const int BULLET_WIDTH = 8;              // 子弹宽度
const int BULLET_HEIGHT = 8;             // 子弹高度
const int BULLET_SPEED = 10;             // 子弹速度
const int PLAYER_BULLET_DAMAGE = 1;             // 子弹伤害
const int ENEMY_BULLET_DAMAGE = 1;             // 子弹伤害
const int BULLET_DEVIATION_DISTANCE = 12;   //子弹偏移距离

// 通用常量
const int FIRE_COOLDOWN = 40;            // 发射冷却（帧数）
const int SPIKE_COOLDOWN = 40;            // 地磁伤害冷却（帧数）
const int HEALING_COOLDOWN = 40;            // 治疗冷却（帧数）

#endif // GAMEDEFS_H
