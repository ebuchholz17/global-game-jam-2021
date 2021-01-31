#ifndef COMBAT_GAME_H
#define COMBAT_GAME_H

#define COMBAT_ARENA_WIDTH (GAME_WIDTH + 20)
#define COMBAT_ARENA_HEIGHT (GAME_HEIGHT + 10)

#define PLAYER_SPEED 10.0f
#define GOBLIN_SPEED 8.0f
#define SNAKE_SPEED 30.0f
#define SNAKE_CHARGE_SPEED 60.0f
#define DRAGON_ACCELERATION 10.0f
#define FIRE_SPEED 50.0f
#define ARROW_SPEED 50.0f
#define KNOCKBACK_SPEED 40.0f

#define MAX_HITBOXES_PER_ATTACK 15

struct dungeon_point {
    int x;
    int y;
};

#define LIST_TYPE dungeon_point
#include "list.h"

enum combat_facing {
    COMBAT_FACING_UP,
    COMBAT_FACING_DOWN,
    COMBAT_FACING_LEFT,
    COMBAT_FACING_RIGHT
};

enum combat_game_state {
    COMBAT_GAME_STATE_COMBAT,
    COMBAT_GAME_STATE_END
};

struct dungeon_position {
    dungeon_point coords;
    float subX;
    float subY;
};

struct pathfinding_node {
    dungeon_point coords;
    float value;
    pathfinding_node *prev;
};
#define LIST_TYPE pathfinding_node
#include "list.h"


unsigned int dungeon_pointTodungeon_pointGeneralHashFunction (dungeon_point a) {
    return (unsigned int)(a.x * 23535347) + (unsigned int)(a.y * 12307);
}
bool dungeon_pointTodungeon_pointGeneralHashEquals (dungeon_point a, dungeon_point b) {
    return a.x == b.x && a.y == b.y;
}
#define GENERAL_HASH_MAP_KEY_TYPE dungeon_point
#define GENERAL_HASH_MAP_TYPE dungeon_point
#include "general_hash_map.h"

struct combat_info {
    dungeon_position position;

    combat_facing facing;
    float cooldownRemaining;
    float cooldownDuration;

    float hitStunTime;
    float knockbackX;
    float knockbackY;

    int health;
    bool alive;
};

struct dungeon_monster {
    char *name;
    monster_type type;

    combat_info combatInfo; 

    aabb hurtBox; // dragon only
    dungeon_point_list segments; // snake only
    float timer;
    float moveTimer;
    float attackTimer;

    bool phase2;
    bool leftSide;
};
#define LIST_TYPE dungeon_monster
#include "list.h"

struct dungeon_player {
    combat_info combatInfo;
    weapon_type weaponType;
};

struct dungeon_attack {
    dungeon_point hitBoxes[MAX_HITBOXES_PER_ATTACK];
    int numHitBoxes;

    dungeon_point ownerPos;
    int damage;
    float t;
    float duration;
    bool ownerIsPlayer;
    bool hit;
};
#define LIST_TYPE dungeon_attack
#include "list.h"

struct dungeon_projectile {
    dungeon_position pos;
    vector2 velocity;
    dungeon_attack attack;
    char character;
    char color;

    bool pathfinding;
    dungeon_monster *target;
};
#define LIST_TYPE dungeon_projectile
#include "list.h"

struct combat_game {
    char arena[COMBAT_ARENA_WIDTH * COMBAT_ARENA_HEIGHT];

    combat_game_state state;
    bool sokobanEnabled;

    bool gameWon;

    memory_arena *memory;

    dungeon_player player;
    dungeon_monster_list monsters;

    dungeon_attack_list activeAttacks;
    dungeon_projectile_list projectiles;

    dungeon_monster *lastHitMonster;

    float endMessageTimer;

    int numTypedLetters;
    char currentText[MAX_TEXT_INPUT_LENGTH];

    bool freeze;
    float freezeTimer;

    char *soundToPlay;
};

#endif 
