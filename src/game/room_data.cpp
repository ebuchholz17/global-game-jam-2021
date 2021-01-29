void buildRooms (explore_game *exploreGame, memory_arena *memory) {
    exploreGame->allRooms = dungeon_roomListInit(memory, 100);

    int currentRoomID = 0;
    {
        dungeon_room room = {};
        room.id = currentRoomID++;
        room.title = "Test Room";
        room.description = R"room(This is an example of the description text for a room.

Here we are testing that the room text will display correctly.

test
    test
        test
            test

test.)room";
        listPush(&exploreGame->allRooms, room);

        room.exits = room_exitListInit(memory, 6);
        {
            room_exit exit = {};
            exit.connectedRoomID = 1;
            exit.direction = ROOM_EXIT_DIRECTION_NORTH;
            listPush(&room.exits, exit);
        }
    }
//------------------------------------------------------------------------------------------------
    {
        dungeon_room room = {};
        room.id = currentRoomID++;
        room.title = "Test Room";
        room.description = R"room(This is an example of the description text for a room.

Here we are testing that the room text will display correctly.

test
    test
        test
            test

test.)room";
        listPush(&exploreGame->allRooms, room);

        room.exits = room_exitListInit(memory, 6);
        {
            room_exit exit = {};
            exit.connectedRoomID = 1;
            exit.direction = ROOM_EXIT_DIRECTION_NORTH;
            listPush(&room.exits, exit);
        }
    }
}
