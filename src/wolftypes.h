#ifndef _wolftypes_
#define _wolftypes_

#define true 1
#define false 0

#define MAXPREY    32
#define MAXWOLVES  10

typedef enum _itemType {
	it_prey,
	it_wolf,
	it_earth,
	it_bush,
	it_tree
} itemType;

typedef enum _runmode {
	wRunModeNone, 
	wRunModeNormal, 
	wRunModeFlee
} wRunMode;

typedef struct _pos {
	char x;
	char y;
} position;

typedef struct _thing {
	position pos;
	wRunMode runmode;
	itemType type;
	struct _thing *hunter;
	struct _thing *next;
	struct _thing *prev;
} thing;

#endif