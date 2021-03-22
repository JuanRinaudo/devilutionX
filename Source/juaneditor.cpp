#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

#define CFLAG_HAS_INPUT 0x1
#define CFLAG_NUMERIC   0x2

#define EDITOR_BUFFER_SIZE 32

BYTE *pEditorPanel;
BOOL juaneditorflag;

int commandIndex = -1;
char debugInputBuffer[ArrayCount(juanMenuItems) - 1][EDITOR_BUFFER_SIZE];
int debugInputCount[ArrayCount(juanMenuItems) - 1];

static void LevelUp(int commandIndex)
{
	char *valueAsChar = (char *)debugInputBuffer[commandIndex];
	int levelUpCount = atoi(valueAsChar);
	if (levelUpCount == 0) {
		levelUpCount = 1;
	}

	for (int i = 0; i < levelUpCount; ++i) {
		NetSendCmd(TRUE, CMD_CHEAT_EXPERIENCE);
	}
}

static void LevelUpToMax(int commandIndex)
{
	int currentLevel = plr[myplr].plrlevel;
	int neededLevels = MAXCHARLEVEL - currentLevel;
	for (int i = 0; i < neededLevels; ++i) {
		NetSendCmd(TRUE, CMD_CHEAT_EXPERIENCE);
	}
}

static void CreateUnique(int commandIndex)
{
	char *valueAsChar = (char *)debugInputBuffer[commandIndex];
	int uniqueIndex = atoi(valueAsChar);

	SpawnUnique(uniqueIndex, plr[myplr]._px, plr[myplr]._py);
}

static void CreateRandomItem(int commandIndex)
{
	char *valueAsChar = (char *)debugInputBuffer[commandIndex];
	int levelIndex = atoi(valueAsChar);

	CreateAnyItemRandom(plr[myplr]._px, plr[myplr]._py, levelIndex, true, true);
}

static void ClearFloorItems(int commandIndex)
{
	numitems = 0;

	for (int i = 0; i < MAXITEMS; i++) {
		item[i]._iPostDraw = FALSE;
		dItem[item[i]._ix][item[i]._iy] = 0;

		itemavail[i] = i;
		itemactive[i] = 0;
	}
}

static void SetMonsterDensity(int commandIndex)
{
	monsterDensityModifier++;
	if (monsterDensityModifier > 8) {
		monsterDensityModifier = 1;
	}

	debugInputBuffer[commandIndex][0] = '0' + monsterDensityModifier;
}

JMenuItem juanMenuItems[] = {
	// clang-format off
//	  dwFlags,							pszStr,	    		    fnMenu
	{ CFLAG_HAS_INPUT | CFLAG_NUMERIC , "Level up"            , &LevelUp		   },
	{ 0								  , "Level to max"        , &LevelUpToMax	   },
	{ CFLAG_HAS_INPUT | CFLAG_NUMERIC , "Create unique"       , &CreateUnique	   },
	{ CFLAG_HAS_INPUT | CFLAG_NUMERIC , "Create random"       , &CreateRandomItem  },
	{ 0                               , "Clear floor items"   , &ClearFloorItems   },
	{ 0                               , "Set monster density" , &SetMonsterDensity },
	{ 0							      , NULL		          , NULL			   },
	{ 0							      , NULL		          , NULL			   },
	{ 0							      , NULL		          , NULL			   },
	{ 0							      , NULL		          , NULL			   },
	{ 0							      , NULL		          , NULL			   },
	{ 0							      , NULL		          , NULL			   },
	{ 0							      , NULL		          , NULL			   },
	{ 0							      , NULL		          , NULL			   },
	{ 0							      , NULL		          , NULL			   },
	{ 0							      , NULL		          , NULL			   }
	// clang-format on
};

void InitJuanEditor()
{
	pEditorPanel = LoadFileInMem("Data\\quest.CEL", NULL);

	for (int i = 0; i < ArrayCount(juanMenuItems) - 1; ++i) {
		debugInputBuffer[i][0] = '-';
		debugInputCount[i] = 0;
	}
}

static void DrawStringCentered(int x, int y, int endX, const char *pszStr, char col)
{
	BYTE c;
	const char *tmp;
	int sx, sy, screen_x, line, widthOffset;

	sx = x + SCREEN_X;
	sy = y + SCREEN_Y;
	widthOffset = endX - x + 1;
	line = 0;
	screen_x = 0;
	tmp = pszStr;
	while (*tmp) {
		c = gbFontTransTbl[(BYTE)*tmp++];
		screen_x += fontkern[fontframe[c]] + 1;
	}
	if (screen_x < widthOffset)
		line = (widthOffset - screen_x) >> 1;
	sx += line;
	while (*pszStr) {
		c = gbFontTransTbl[(BYTE)*pszStr++];
		c = fontframe[c];
		line += fontkern[c] + 1;
		if (c) {
			if (line < widthOffset)
				PrintChar(sx, sy, c, col);
		}
		sx += fontkern[c] + 1;
	}
}

static void DrawString(int x, int y, const char *pszStr, char col)
{
	BYTE c;
	const char *tmp;
	int sx, sy, screen_x, line;

	sx = x + SCREEN_X;
	sy = y + SCREEN_Y;
	line = 0;
	screen_x = 0;
	tmp = pszStr;
	while (*tmp) {
		c = gbFontTransTbl[(BYTE)*tmp++];
		screen_x += fontkern[fontframe[c]] + 1;
	}
	sx += line;
	while (*pszStr) {
		c = gbFontTransTbl[(BYTE)*pszStr++];
		c = fontframe[c];
		line += fontkern[c] + 1;
		if (c) {
			PrintChar(sx, sy, c, col);
		}
		sx += fontkern[c] + 1;
	}
}

void DrawJuanEditor()
{
	CelDraw(SCREEN_X, 351 + SCREEN_Y, pEditorPanel, 1, SPANEL_WIDTH);
	DrawString(32, 48, "Juan Editor", COL_WHITE);
	//PlaySFX(IS_TITLEMOV);
	int posY = 80;
	int index = 0;
	JMenuItem *item = juanMenuItems;
	while (item->fnMenu) {
		if (MouseX > 32 && MouseX < 200 && MouseY > posY - 16 && MouseY < posY) {
			commandIndex = index;
		}

		BOOL hasInput = (item->dwFlags & CFLAG_HAS_INPUT) > 0 || debugInputBuffer[index][0] != '-';
		char color = commandIndex == index ? COL_RED : COL_WHITE;
		DrawString(32, posY, item->pszStr, color);
		if (hasInput) {
			DrawStringCentered(220, posY, 280, debugInputBuffer[index], COL_WHITE);
		}
		posY += 20;
		item++;
		index++;
	}
}

void CheckJuanEditorBtns()
{
	int posY = 80;
	JMenuItem *item = juanMenuItems;
	while (item->fnMenu) {
		if (MouseX > 32 && MouseX < 200 && MouseY > posY - 16 && MouseY < posY) {
			item->fnMenu(commandIndex);
		}
		posY += 20;
		item++;
	}
}

BOOL PressKeysJuanEditor(int vkey)
{
	if (!juaneditorflag)
		return false;

	switch (vkey) {
	case DVL_VK_UP:
		commandIndex--;
		if (commandIndex < 0) {
			commandIndex = ArrayCount(juanMenuItems) - 2;
		}
		break;
	case DVL_VK_DOWN:
		commandIndex++;
		if (commandIndex >= ArrayCount(juanMenuItems) - 1) {
			commandIndex = 0;
		}
		break;
	case DVL_VK_ESCAPE:
		juaneditorflag = false;
		break;
	case DVL_VK_RETURN:
		juanMenuItems[commandIndex].fnMenu(commandIndex);
		break;
	case DVL_VK_BACK:
		if ((juanMenuItems[commandIndex].dwFlags & CFLAG_HAS_INPUT) == 0)
			return false;

		if (debugInputCount[commandIndex] > 0) {
			debugInputCount[commandIndex]--;
			debugInputBuffer[commandIndex][debugInputCount[commandIndex]] = debugInputCount[commandIndex] == 0 ? '-' : '\0';
		}
	}
}

BOOL PressCharJuanEditor(int vkey)
{
	if (!juaneditorflag)
		return false;
	
	if ((juanMenuItems[commandIndex].dwFlags & CFLAG_HAS_INPUT) == 0)
		return false;

	if (commandIndex > -1 && vkey >= ' ' && vkey <= 'z') {
		BOOL numericInput = (juanMenuItems[commandIndex].dwFlags & CFLAG_NUMERIC) > 0;
		if (numericInput && (vkey < '0' || vkey > '9')) {
			return false;
		}

		if (debugInputCount[commandIndex] < EDITOR_BUFFER_SIZE) {

			debugInputBuffer[commandIndex][debugInputCount[commandIndex]] = vkey;
			debugInputCount[commandIndex]++;
		}
		return true;
	}
}

DEVILUTION_END_NAMESPACE
