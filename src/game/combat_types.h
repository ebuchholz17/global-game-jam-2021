#ifndef COMBAT_TYPES_H
#define COMBAT_TYPES_H

enum monster_type {
    MONSTER_TYPE_GOBLIN,
    MONSTER_TYPE_GIANT_SNAKE,
    MONSTER_TYPE_DRAGON
};

enum weapon_type {
    WEAPON_TYPE_UNARMED,
    WEAPON_TYPE_DAGGER,
    WEAPON_TYPE_SWORD,
    WEAPON_TYPE_BOW,
    WEAPON_TYPE_SPELLBOOK
};

struct combat_parameters {
    int numMonsters;
    monster_type monsterType;
    weapon_type weaponType;
    int spawnPositionsID;
    bool sokobanEnabled;
};

#endif
