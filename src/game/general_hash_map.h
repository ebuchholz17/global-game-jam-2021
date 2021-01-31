#ifndef GENERAL_HASH_MAP_H 
#define GENERAL_HASH_MAP_H

#define GENERAL_HASH_MAP_TYPE_STRUCT(x, y) GENERAL_HASH_MAP_TYPE_STRUCT_(x, y)
#define GENERAL_HASH_MAP_TYPE_STRUCT_(x, y) x##_to_##y##_g_hash_map
#define GENERAL_HASH_MAP_ENTRY_STRUCT(x, y) GENERAL_HASH_MAP_ENTRY_STRUCT_(x, y)
#define GENERAL_HASH_MAP_ENTRY_STRUCT_(x, y) x##_to_##y##_g_hash_map_value
#define GENERAL_HASH_MAP_INIT(x, y) GENERAL_HASH_MAP_INIT_(x, y)
#define GENERAL_HASH_MAP_INIT_(x, y) x##To##y##GeneralHashMapInit
#define GENERAL_HASH_MAP_HASH_FUNCTION(x, y) GENERAL_HASH_MAP_HASH_FUNCTION_(x, y)
#define GENERAL_HASH_MAP_HASH_FUNCTION_(x, y) x##To##y##GeneralHashFunction
#define GENERAL_HASH_MAP_EQUALITY_FUNCTION(x, y) GENERAL_HASH_MAP_EQUALITY_FUNCTION_(x, y)
#define GENERAL_HASH_MAP_EQUALITY_FUNCTION_(x, y) x##To##y##GeneralHashEquals

#endif

// TODO(ebuchholz): generate names from type+key type, not just type
struct GENERAL_HASH_MAP_ENTRY_STRUCT(GENERAL_HASH_MAP_KEY_TYPE, GENERAL_HASH_MAP_TYPE) {
    GENERAL_HASH_MAP_KEY_TYPE key;
    GENERAL_HASH_MAP_TYPE value;
    bool enteredIntoHashmap;
};

struct GENERAL_HASH_MAP_TYPE_STRUCT(GENERAL_HASH_MAP_KEY_TYPE, GENERAL_HASH_MAP_TYPE) {
    GENERAL_HASH_MAP_ENTRY_STRUCT(GENERAL_HASH_MAP_KEY_TYPE, GENERAL_HASH_MAP_TYPE) *entries;
    unsigned int capacity;
};

void storeHashMapValue (GENERAL_HASH_MAP_TYPE_STRUCT(GENERAL_HASH_MAP_KEY_TYPE, GENERAL_HASH_MAP_TYPE) *hashMap, GENERAL_HASH_MAP_TYPE value, GENERAL_HASH_MAP_KEY_TYPE key) {
    unsigned int hash = GENERAL_HASH_MAP_HASH_FUNCTION(GENERAL_HASH_MAP_KEY_TYPE, GENERAL_HASH_MAP_TYPE)(key);
    unsigned int mapIndex = hash % hashMap->capacity;
    unsigned int originalMapIndex = mapIndex; // check that we actually find it after looping all the way around
    GENERAL_HASH_MAP_ENTRY_STRUCT(GENERAL_HASH_MAP_KEY_TYPE, GENERAL_HASH_MAP_TYPE) *existingEntry = &hashMap->entries[mapIndex];

    bool replacedValue = false;
    while (existingEntry->enteredIntoHashmap) {
        if (GENERAL_HASH_MAP_EQUALITY_FUNCTION(GENERAL_HASH_MAP_KEY_TYPE, GENERAL_HASH_MAP_TYPE)(key, existingEntry->key)) {
            existingEntry->value = value;
            replacedValue = true;
            break;
        }
        else {
            mapIndex = (mapIndex + 1) % hashMap->capacity;
            existingEntry = &hashMap->entries[mapIndex];
            assert(mapIndex != originalMapIndex);
        }
    }
    if (!replacedValue) {
        GENERAL_HASH_MAP_ENTRY_STRUCT(GENERAL_HASH_MAP_KEY_TYPE, GENERAL_HASH_MAP_TYPE) entry = {};
        entry.key = key;
        entry.value = value;
        entry.enteredIntoHashmap = true;
        hashMap->entries[mapIndex] = entry;
    }
}

GENERAL_HASH_MAP_TYPE *getHashMapValuePointer (GENERAL_HASH_MAP_TYPE_STRUCT(GENERAL_HASH_MAP_KEY_TYPE, GENERAL_HASH_MAP_TYPE) *hashMap, GENERAL_HASH_MAP_KEY_TYPE key) {
    GENERAL_HASH_MAP_TYPE *result;

    unsigned int hash = GENERAL_HASH_MAP_HASH_FUNCTION(GENERAL_HASH_MAP_KEY_TYPE, GENERAL_HASH_MAP_TYPE)(key);
    unsigned int mapIndex = hash % hashMap->capacity;
    unsigned int originalMapIndex = mapIndex; // check that we actually find it after looping all the way around

    while (true) {
        GENERAL_HASH_MAP_ENTRY_STRUCT(GENERAL_HASH_MAP_KEY_TYPE, GENERAL_HASH_MAP_TYPE) *entry = &hashMap->entries[mapIndex];
        if (entry->enteredIntoHashmap) {
            if (GENERAL_HASH_MAP_EQUALITY_FUNCTION(GENERAL_HASH_MAP_KEY_TYPE, GENERAL_HASH_MAP_TYPE)(key, entry->key)) {
                result = &entry->value;
                break;
            }
            else {
                mapIndex = (mapIndex + 1) % hashMap->capacity;
                if (mapIndex == originalMapIndex) {
                    return 0;
                }
            }
        }
        else {
            return 0;
        }
    }

    return result;
}

GENERAL_HASH_MAP_TYPE getHashMapValue (GENERAL_HASH_MAP_TYPE_STRUCT(GENERAL_HASH_MAP_KEY_TYPE, GENERAL_HASH_MAP_TYPE) *hashMap, GENERAL_HASH_MAP_KEY_TYPE key) {
    GENERAL_HASH_MAP_TYPE *result = getHashMapValuePointer(hashMap, key);
    assert(result != 0);
    return *result;
}

bool keyExistsInHashMap (GENERAL_HASH_MAP_TYPE_STRUCT(GENERAL_HASH_MAP_KEY_TYPE, GENERAL_HASH_MAP_TYPE) *hashMap, GENERAL_HASH_MAP_KEY_TYPE key) {
    GENERAL_HASH_MAP_TYPE *result = getHashMapValuePointer(hashMap, key);
    return result != 0;
}

bool getHashMapValueIfExists (GENERAL_HASH_MAP_TYPE_STRUCT(GENERAL_HASH_MAP_KEY_TYPE, GENERAL_HASH_MAP_TYPE) *hashMap, GENERAL_HASH_MAP_KEY_TYPE key, GENERAL_HASH_MAP_TYPE *outValue) {
    GENERAL_HASH_MAP_TYPE *result = getHashMapValuePointer(hashMap, key);
    if(result != 0) {
        *outValue = *result;
    };
    return result != 0;
}

void clearHashMap (GENERAL_HASH_MAP_TYPE_STRUCT(GENERAL_HASH_MAP_KEY_TYPE, GENERAL_HASH_MAP_TYPE) *hashMap) {
    for (unsigned int i = 0; i < hashMap->capacity; ++i) {
        hashMap->entries[i].enteredIntoHashmap = false;
    }
}

GENERAL_HASH_MAP_TYPE_STRUCT(GENERAL_HASH_MAP_KEY_TYPE, GENERAL_HASH_MAP_TYPE) GENERAL_HASH_MAP_INIT(GENERAL_HASH_MAP_KEY_TYPE, GENERAL_HASH_MAP_TYPE) (memory_arena *memory, int capacity) {
    GENERAL_HASH_MAP_TYPE_STRUCT(GENERAL_HASH_MAP_KEY_TYPE, GENERAL_HASH_MAP_TYPE) result = {};
    result.capacity = capacity;
    result.entries = (GENERAL_HASH_MAP_ENTRY_STRUCT(GENERAL_HASH_MAP_KEY_TYPE, GENERAL_HASH_MAP_TYPE) *)allocateMemorySize(memory, capacity * sizeof(GENERAL_HASH_MAP_ENTRY_STRUCT(GENERAL_HASH_MAP_KEY_TYPE, GENERAL_HASH_MAP_TYPE)));
    for (int i = 0; i < capacity; ++i) {
        result.entries[i] = {};
    }
    return result;
}

#undef GENERAL_HASH_MAP_TYPE
#undef GENERAL_HASH_MAP_KEY_TYPE
