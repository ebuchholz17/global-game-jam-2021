void buildItems (explore_game *exploreGame, memory_arena *memory) {
    exploreGame->allItems = dungeon_itemListInit(memory, 10);
    {
        dungeon_item item = {};
        item.id = 0;
        item.name = "Dagger";
        item.description = "A short steel dagger coming to a sharp point.";
        item.roomDescription = "You see a lone, discarded dagger on the floor.";
        item.equippable = true;

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
        item.description = "A gold bangle emanting a magical energy engraved with the name: \"Sou'koban\""; 
        item.roomDescription = "In the far corner of the room is a magic bracelet, sitting on a pedestal.";
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
        item.roomDescription = "At your feet, dangerously close to tumbling off the edge, is a small key.";
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
north.)room";
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
        room.description = R"room()room";

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
        room.description = R"room()room";
        room.itemID = -1;

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
        room.description = R"room()room";
        room.itemID = -1;

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
        room.description = R"room()room";
        room.itemID = 2;

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
        room.description = R"room()room";
        room.itemID = -1;

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
        room.description = R"room()room";
        room.itemID = -1;

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
        room.description = R"room()room";
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
        room.description = R"room()room";
        room.itemID = -1;

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
        room.description = R"room()room";
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
        room.description = R"room()room";
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
        room.description = R"room()room";
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
        room.description = R"room()room";
        room.itemID = 3;

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
