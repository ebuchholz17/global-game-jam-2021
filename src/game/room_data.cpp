void buildItems (explore_game *exploreGame, memory_arena *memory) {
    exploreGame->allItems = dungeon_itemListInit(memory, 10);
    {
        dungeon_item item = {};
        item.id = 0;
        item.name = "Dagger";
        item.description = "A short steel dagger coming to a sharp point.";
        item.roomDescription = "You see a lone, discarded dagger on the floor.";
        item.equippable = true;
        item.weaponType = WEAPON_TYPE_DAGGER;

        item.alternateNames = charPtrListInit(memory, 10);
        listPush(&item.alternateNames, "DAGGER");
        listPush(&item.alternateNames, "KNIFE");

        listPush(&exploreGame->allItems, item);
    }
    {
        dungeon_item item = {};
        item.id = 1;
        item.name = "Longsword";
        item.description = "This long steel sword must be almost four feet long.";
        item.roomDescription = "You see the glint of a steel longsword hidden among the rose bushes.";
        item.equippable = true;
        item.weaponType = WEAPON_TYPE_SWORD;

        item.alternateNames = charPtrListInit(memory, 10);
        listPush(&item.alternateNames, "LONGSWORD");
        listPush(&item.alternateNames, "LONG SWORD");
        listPush(&item.alternateNames, "SWORD");

        listPush(&exploreGame->allItems, item);
    }
    {
        dungeon_item item = {};
        item.id = 2;
        item.name = "Bow";
        item.description = "A wooden bow accompanied by a quiver of arrows.";
        item.roomDescription = "Among the rubble is a wooden bow and arrows, hidden away long ago." ;
        item.equippable = true;
        item.weaponType = WEAPON_TYPE_BOW;

        item.alternateNames = charPtrListInit(memory, 10);
        listPush(&item.alternateNames, "BOW");
        listPush(&item.alternateNames, "ARROW");
        listPush(&item.alternateNames, "BOW AND ARROW");
        listPush(&item.alternateNames, "BOW AND ARROWS");

        listPush(&exploreGame->allItems, item);
    }
    {
        dungeon_item item = {};
        item.id = 3;
        item.name = "Magic Bracelet";
        item.description = "A gold bangle emanting a magical energy engraved with the name: \"Sou-ko Ba'an\""; 
        item.roomDescription = "Secreted away in the corner is a magic bracelet, upon a pedestal....\nYou could reach it if you could navigate through the tables.";
        item.equippable = false;

        item.alternateNames = charPtrListInit(memory, 10);
        listPush(&item.alternateNames, "MAGIC BRACELET");
        listPush(&item.alternateNames, "BRACELET");
        listPush(&item.alternateNames, "BANGLE");
        listPush(&item.alternateNames, "GOLD BANGLE");

        listPush(&exploreGame->allItems, item);
    }
    {
        dungeon_item item = {};
        item.id = 4;
        item.name = "Rune-engraved Key";
        item.description = "A metal key glowing with runic inscriptions."; 
        item.roomDescription = "At your feet, dangerously close to tumbling off the edge, is a small\nkey.";
        item.equippable = false;

        item.alternateNames = charPtrListInit(memory, 10);
        listPush(&item.alternateNames, "KEY");
        listPush(&item.alternateNames, "RUNE KEY");
        listPush(&item.alternateNames, "RUNE-ENGRAVED KEY");
        listPush(&item.alternateNames, "RUNE ENGRAVED KEY");

        listPush(&exploreGame->allItems, item);
    }
    {
        dungeon_item item = {};
        item.id = 5;
        item.name = "Spell Book";
        item.description = "A book of magic spells, instructions not included."; 
        item.roomDescription = "";
        item.equippable = false;
        item.weaponType = WEAPON_TYPE_SPELLBOOK;

        item.alternateNames = charPtrListInit(memory, 10);
        listPush(&item.alternateNames, "BOOK");
        listPush(&item.alternateNames, "SPELL BOOK");
        listPush(&item.alternateNames, "SPELLBOOK");

        listPush(&exploreGame->allItems, item);
    }
}

void buildRooms (explore_game *exploreGame, memory_arena *memory) {
    exploreGame->allRooms = dungeon_roomListInit(memory, 20);

    {
        dungeon_room room = {};
        room.id = 0;
        room.title = "Sunlit Back Room";
        room.description = R"room(The ceiling of this room has collapsed, allowing the sun to shine 
through. Rubble covers the floor. There is an open doorway to the
north.

(Type HELP for instructions).)room";
        room.itemID = -1;

        room.exits = room_exitListInit(memory, 6);
        {
            room_exit exit = {};
            exit.connectedRoomID = 1;
            exit.direction = ROOM_EXIT_DIRECTION_NORTH;
            listPush(&room.exits, exit);
        }

        listPush(&exploreGame->allRooms, room);
    }
//------------------------------------------------------------------------------------------------
    {
        dungeon_room room = {};
        room.id = 1;
        room.title = "Dim Hallway";
        room.description = R"room(This is dimly lit hallway leading east to west. You can see the sun 
shining through a doorway to the south.)room";
        room.itemID = -1;

        room.exits = room_exitListInit(memory, 6);
        {
            room_exit exit = {};
            exit.connectedRoomID = 0;
            exit.direction = ROOM_EXIT_DIRECTION_SOUTH;
            listPush(&room.exits, exit);
        }
        {
            room_exit exit = {};
            exit.connectedRoomID = 2;
            exit.direction = ROOM_EXIT_DIRECTION_WEST;
            listPush(&room.exits, exit);
        }
        {
            room_exit exit = {};
            exit.connectedRoomID = 8;
            exit.direction = ROOM_EXIT_DIRECTION_EAST;
            listPush(&room.exits, exit);
        }

        listPush(&exploreGame->allRooms, room);
    }
//------------------------------------------------------------------------------------------------
    {
        dungeon_room room = {};
        room.id = 2;
        room.title = "Armory";
        room.description = R"room(From all the barrels and weapon racks, you figure this must have
been an armory. Unfortunately the room has been almost entirely
looted.

A doorway leads to a hallway to the east. A ladder leads downward
through an open grate in the floor. There is a spiral staircase
leading upward.)room";

        room.itemID = 0;

        room.exits = room_exitListInit(memory, 6);
        {
            room_exit exit = {};
            exit.connectedRoomID = 1;
            exit.direction = ROOM_EXIT_DIRECTION_EAST;
            listPush(&room.exits, exit);
        }
        {
            room_exit exit = {};
            exit.connectedRoomID = 3;
            exit.direction = ROOM_EXIT_DIRECTION_DOWN;
            listPush(&room.exits, exit);
        }
        {
            room_exit exit = {};
            exit.connectedRoomID = 12;
            exit.direction = ROOM_EXIT_DIRECTION_UP;
            listPush(&room.exits, exit);
        }

        listPush(&exploreGame->allRooms, room);
    }
//------------------------------------------------------------------------------------------------
    {
        dungeon_room room = {};
        room.id = 3;
        room.title = "Basement";
        room.description = R"room(This is a small dark room with a ladder leading upward. A rusted, 
metal door has fallen off its hinges, revealing an even darker room 
to the east.

                  You can't help but feel                        
                                           c
                  q                        r
                  u                        a
                  i                        m
                  t                        p
                  e                        e
                                           d
                  in this room)room";

        room.itemID = -1;
        room.numMonsters = 1;
        room.monsterType = MONSTER_TYPE_GOBLIN;
        room.spawnPositionsID = 0;

        room.exits = room_exitListInit(memory, 6);
        {
            room_exit exit = {};
            exit.connectedRoomID = 2;
            exit.direction = ROOM_EXIT_DIRECTION_UP;
            listPush(&room.exits, exit);
        }
        {
            room_exit exit = {};
            exit.connectedRoomID = 4;
            exit.direction = ROOM_EXIT_DIRECTION_EAST;
            listPush(&room.exits, exit);
        }

        listPush(&exploreGame->allRooms, room);
    }
//------------------------------------------------------------------------------------------------
    {
        dungeon_room room = {};
        room.id = 4;
        room.title = "Dungeon";
        room.description = R"room(Wrapping around the room are rows and rows of prison cells. Most of
them are empty, and the rest are full of bones. It's so dark that
you can barely see your hand in front of your face.

To the west is an opening where a door has rusted and fallen down.
To the east is a open gate, that is barely standing.

In the floor in the center of the room is a large, circular hole,
bloodstained around the edges. A chain is fixed to a nearby post
and dangles down into the hole.)room";

        room.itemID = -1;

        room.exits = room_exitListInit(memory, 6);
        {
            room_exit exit = {};
            exit.connectedRoomID = 5;
            exit.direction = ROOM_EXIT_DIRECTION_DOWN;
            listPush(&room.exits, exit);
        }
        {
            room_exit exit = {};
            exit.connectedRoomID = 3;
            exit.direction = ROOM_EXIT_DIRECTION_WEST;
            listPush(&room.exits, exit);
        }
        {
            room_exit exit = {};
            exit.connectedRoomID = 6;
            exit.direction = ROOM_EXIT_DIRECTION_EAST;
            listPush(&room.exits, exit);
        }

        listPush(&exploreGame->allRooms, room);
    }
    {
        dungeon_room room = {};
        room.id = 5;
        room.title = "Snake Pit";
        room.description = R"room(You step onto a floor covered in what feels like human bones, but 
it's so dark you can't be sure. You wonder if anyone has ever made 
it out of this room alive.

There's a small hole in the wall to the east, which you probably can
climb through. There's a chain dangling from the ceiling that you
can climb up.
                                                          
     But              you         get        the        f 
                                                        e 
                                                        e       that
there                                                   l 
             might                                      i 
                       be                               n 
                                                        g  something
         around
                                                here...
    ...s l i t h e r i ng)room";
        room.itemID = -1;

        room.numMonsters = 1;
        room.monsterType = MONSTER_TYPE_GIANT_SNAKE;
        room.spawnPositionsID = 1;

        room.exits = room_exitListInit(memory, 6);
        {
            room_exit exit = {};
            exit.connectedRoomID = 4;
            exit.direction = ROOM_EXIT_DIRECTION_UP;
            listPush(&room.exits, exit);
        }
        {
            room_exit exit = {};
            exit.connectedRoomID = 7;
            exit.direction = ROOM_EXIT_DIRECTION_EAST;
            listPush(&room.exits, exit);
        }

        listPush(&exploreGame->allRooms, room);
    }
    {
        dungeon_room room = {};
        room.id = 6;
        room.title = "Collapsed Spire";
        room.description = R"room(The wreckage of the castle spire hangs above you, and there is 
rubble all around you, although the center has been cleared out.
Some of the wreckage has punctured a hole in the floor, which you 
could probably drop down through, though it's unlikely you could 
climb back up. There is also an open gate to the west.

With the amount of                          this is probably a place
debris scattered                                   that has become a
around the                    r                            veritable
room,                          u                                 den
                                b
                                 b
                                  l                           Better
of                                 e                         keep an
creepy                                                   eye out for
crawlies.                                             monsters here.)room";
        room.itemID = -1;

        room.numMonsters = 4;
        room.monsterType = MONSTER_TYPE_GOBLIN;
        room.spawnPositionsID = 2;

        room.exits = room_exitListInit(memory, 6);
        {
            room_exit exit = {};
            exit.connectedRoomID = 4;
            exit.direction = ROOM_EXIT_DIRECTION_WEST;
            listPush(&room.exits, exit);
        }
        {
            room_exit exit = {};
            exit.connectedRoomID = 7;
            exit.direction = ROOM_EXIT_DIRECTION_DOWN;
            listPush(&room.exits, exit);
        }

        listPush(&exploreGame->allRooms, room);
    }
    {
        dungeon_room room = {};
        room.id = 7;
        room.title = "Caved-in Cubby Hole";
        room.description = R"room(This is a small, cramped crawlspace, littered with rubble and 
debris. You don't see any connecting doors, although a hole to the 
west is small enough to shimmy through. Light pokes through from 
above, but there is no way to climb up.

There is a tattered note here with some writing scrawled on it. It's
pinned under a rock that won't budge, along with a singed skeletal 
arm.)room";
        room.itemID = 2;

        room.textToRead = R"room("To whoever finds this: I have found the treasure that is the key to
escaping this place, but I do not dare attempt to take it. A 
ferocious monster guards it, and there is no way to best the flames
it wields. It seems it is my fate to waste away here in this spire,
lest I be reduced to ashes. May fortune find you, and grant you
better luck than I.")room";

        room.exits = room_exitListInit(memory, 6);
        {
            room_exit exit = {};
            exit.connectedRoomID = 5;
            exit.direction = ROOM_EXIT_DIRECTION_WEST;
            listPush(&room.exits, exit);
        }

        listPush(&exploreGame->allRooms, room);
    }
    {
        dungeon_room room = {};
        room.id = 8;
        room.title = "Library: Lower Level";
        room.description = R"room(You've entered a vast hall that was once a grand library. Two story
high bookcases line the walls and divide the room into sections.
While there are missing books here and there, most of them seem
intact.

A door to the west leads out into the hallway, and there is a ladder
you can go up to reach the second floor.

On a wide table in front of you, there is an open journal with some
notes scrawled in it. Although the handwriting is sloppy, you can 
probably read it with a little effort.)room";
        room.itemID = -1;

        room.textToRead = R"room("The following is a record of the spells that I have deciphered of
the spellbook that resides in this very castle. I've locked the 
book away for safe-keeping, but if anything should happen to me, let
these notes serve as a guide for whomever may come across it.

"FIREBALL - Chant this word to emit a flaming ball of fire that 
seeks your foes.

"FREEZE - Use this to halt your enemies in their tracks.

"There is one more spell I have uncovered, but its meaning eludes 
me. The letters seem to be encrypted: LRPOTETE.")room";

        room.exits = room_exitListInit(memory, 6);
        {
            room_exit exit = {};
            exit.connectedRoomID = 1;
            exit.direction = ROOM_EXIT_DIRECTION_WEST;
            listPush(&room.exits, exit);
        }
        {
            room_exit exit = {};
            exit.connectedRoomID = 9;
            exit.direction = ROOM_EXIT_DIRECTION_UP;
            listPush(&room.exits, exit);
        }

        listPush(&exploreGame->allRooms, room);
    }
    {
        dungeon_room room = {};
        room.id = 9;
        room.title = "Library: Upper Level";
        room.description = R"room(This is the top floor of a grand library. The bookcases seem to
stretch out into the horizon.

Light pours into the room through shattered windows to the west,
where there is a doorway. A ladder used for reaching books can be
used to go down to the first floor.)room";
        room.itemID = -1;
        room.hasChest = true;

        room.exits = room_exitListInit(memory, 6);
        {
            room_exit exit = {};
            exit.connectedRoomID = 8;
            exit.direction = ROOM_EXIT_DIRECTION_DOWN;
            listPush(&room.exits, exit);
        }
        {
            room_exit exit = {};
            exit.connectedRoomID = 10;
            exit.direction = ROOM_EXIT_DIRECTION_WEST;
            listPush(&room.exits, exit);
        }

        listPush(&exploreGame->allRooms, room);
    }
    {
        dungeon_room room = {};
        room.id = 10;
        room.title = "Courtyard Garden";
        room.description = R"room(This courtyard must have once housed a beautiful, well-maintained
garden. It has since become overgrown and is infested with vines and
thorny rose bushes.

To the east is a kind of grand entryway. There are discarded plates
and silverware at the foot of a door to the west.

To the north is a shattered window that you can climb through, but
it seems like a steep drop, so you may not be able to climb back
through.)room";
        room.itemID = 1;

        room.exits = room_exitListInit(memory, 6);
        {
            room_exit exit = {};
            exit.connectedRoomID = 9;
            exit.direction = ROOM_EXIT_DIRECTION_EAST;
            listPush(&room.exits, exit);
        }
        {
            room_exit exit = {};
            exit.connectedRoomID = 11;
            exit.direction = ROOM_EXIT_DIRECTION_WEST;
            listPush(&room.exits, exit);
        }
        {
            room_exit exit = {};
            exit.connectedRoomID = 13;
            exit.direction = ROOM_EXIT_DIRECTION_NORTH;
            listPush(&room.exits, exit);
        }

        listPush(&exploreGame->allRooms, room);
    }
    {
        dungeon_room room = {};
        room.id = 11;
        room.title = "Kitchen";
        room.description = R"room(This is the castle's kitchen, judging by the arrangement of tables,
cabinets, and ovens. An entryway on the east side leads to the 
castle courtyard. You also see a door to the north.

The room is in a disorderly state, pots and pans strewn all over...

a           h              a        t            j           m
            e                       h            u           e
p           r              p        e            s           s
o           e              a        r            t           s
t           ,              n        e                        .
                                    ,            a

It's wonder anything ever got done in this kitchen, nevermind 
cooking.)room";
        room.itemID = -1;

        room.numMonsters = 3;
        room.monsterType = MONSTER_TYPE_GOBLIN;
        room.spawnPositionsID = 3;

        room.exits = room_exitListInit(memory, 6);
        {
            room_exit exit = {};
            exit.connectedRoomID = 10;
            exit.direction = ROOM_EXIT_DIRECTION_EAST;
            listPush(&room.exits, exit);
        }
        {
            room_exit exit = {};
            exit.connectedRoomID = 12;
            exit.direction = ROOM_EXIT_DIRECTION_NORTH;
            listPush(&room.exits, exit);
        }

        listPush(&exploreGame->allRooms, room);
    }
    {
        dungeon_room room = {};
        room.id = 12;
        room.title = "West Spire";
        room.description = R"room(This is the western spire of the castle. Through the window you can
see nothing but open air where the eastern spire should be. You do
see flashes of bright orange now and again through the shattered
glass windows of a large room that must be a few rooms ahead of 
this one.

A door to the east looks like it leads into a hallway, but the edges
seem to be darkened somehow. A staircase leads downward, and there
is an open door to the south.)room";
        room.itemID = -1;

        room.exits = room_exitListInit(memory, 6);
        {
            room_exit exit = {};
            exit.connectedRoomID = 13;
            exit.direction = ROOM_EXIT_DIRECTION_EAST;
            listPush(&room.exits, exit);
        }
        {
            room_exit exit = {};
            exit.connectedRoomID = 11;
            exit.direction = ROOM_EXIT_DIRECTION_SOUTH;
            listPush(&room.exits, exit);
        }
        {
            room_exit exit = {};
            exit.connectedRoomID = 2;
            exit.direction = ROOM_EXIT_DIRECTION_DOWN;
            listPush(&room.exits, exit);
        }

        listPush(&exploreGame->allRooms, room);
    }
    {
        dungeon_room room = {};
        room.id = 13;
        room.title = "Charred Hallway";
        room.description = R"room(This is a majestic hallway lined with tapetries and paintings. Well,
maybe it once was, but now the hallway is completely blackened and
covered in ash. The hallway runs from west to east, and there is a
large entryway to the north.

There is a shattered window to the south, but it is too high to 
reach.)room";
        room.itemID = -1;

        room.exits = room_exitListInit(memory, 6);
        {
            room_exit exit = {};
            exit.connectedRoomID = 12;
            exit.direction = ROOM_EXIT_DIRECTION_WEST;
            listPush(&room.exits, exit);
        }
        {
            room_exit exit = {};
            exit.connectedRoomID = 14;
            exit.direction = ROOM_EXIT_DIRECTION_EAST;
            listPush(&room.exits, exit);
        }
        {
            room_exit exit = {};
            exit.connectedRoomID = 15;
            exit.direction = ROOM_EXIT_DIRECTION_NORTH;
            listPush(&room.exits, exit);
        }

        listPush(&exploreGame->allRooms, room);
    }
    {
        dungeon_room room = {};
        room.id = 14;
        room.title = "Shambling Spire";
        room.description = R"room(You walk through the door to open air. If you had taken one more 
step, you would have taken a long tumble down to the depths of the 
castle.

The spire that once stood here has completely collapsed. What 
remains of the walls appears to be swaying in the wind. The rest of
it is a collection of loose bricks and snapped beams.

Actually, with some very careful footing, you could probably climb
down, but it would be impossible to climb back up.)room";
        room.itemID = 4;

        room.exits = room_exitListInit(memory, 6);
        {
            room_exit exit = {};
            exit.connectedRoomID = 13;
            exit.direction = ROOM_EXIT_DIRECTION_WEST;
            listPush(&room.exits, exit);
        }
        {
            room_exit exit = {};
            exit.connectedRoomID = 6;
            exit.direction = ROOM_EXIT_DIRECTION_DOWN;
            listPush(&room.exits, exit);
        }

        listPush(&exploreGame->allRooms, room);
    }
    {
        dungeon_room room = {};
        room.id = 15;
        room.title = "Great Hall";
        room.description = R"room(This would be a majestic dining hall if not for the blackened walls 
and tattered tapetries. The stained glass windows are shattered, a
charcoal-black blanket of ash covers the room. South is an entryway.
                                             over
        F                            turned         t
        o                                           o
        r                t
              reason,    h    t                serve
                         e    a                        a
         s                    b                        s
         o                    l  a
         m                    e   r
         e                    s    e
                                               barricades.)room";
        room.itemID = 3;

        room.numMonsters = 1;
        room.monsterType = MONSTER_TYPE_DRAGON;
        room.spawnPositionsID = 4;

        room.exits = room_exitListInit(memory, 6);
        {
            room_exit exit = {};
            exit.connectedRoomID = 13;
            exit.direction = ROOM_EXIT_DIRECTION_SOUTH;
            listPush(&room.exits, exit);
        }

        listPush(&exploreGame->allRooms, room);
    }
}
