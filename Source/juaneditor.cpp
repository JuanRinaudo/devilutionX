#include "all.h"

DEVILUTION_BEGIN_NAMESPACE

BYTE *pEditorPanel;
BOOL juaneditorflag;

static void LevelUp(void *value);
static void LevelUpToMax(void *value);
static void CreateUnique(void *value);
static void CreateRandomItem(void *value);

JMenuItem juanMenuItems[] = {
	// clang-format off
//	  dwFlags,       pszStr,			fnMenu
	{ 0, "Level up",		&LevelUp			},
	{ 0, "Level to max",	&LevelUpToMax		},
	{ 0, "Create unique",   &CreateUnique	    },
	{ 0, "Create random",   &CreateRandomItem	},
	{ 0, NULL,            NULL }
	// clang-format on
};

void InitJuanEditor()
{
	pEditorPanel = LoadFileInMem("Data\\quest.CEL", NULL);
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

int uniqueIndex = 0;

static void LevelUp(void *value)
{
	NetSendCmd(TRUE, CMD_CHEAT_EXPERIENCE);
}

static void LevelUpToMax(void *value)
{
	int currentLevel = plr[myplr].plrlevel;
	int neededLevels = MAXCHARLEVEL - currentLevel;
	for (int i = 0; i < neededLevels; ++i) {
		NetSendCmd(TRUE, CMD_CHEAT_EXPERIENCE);
	}
}

static void CreateUnique(void *value)
{
	SpawnUnique(uniqueIndex, plr[myplr]._px, plr[myplr]._py);
	uniqueIndex = (uniqueIndex + 1) % uniqueItemCount;
}

static void CreateRandomItem(void *value)
{
	CreateRndItem(plr[myplr]._px, plr[myplr]._py, true, true, true);
}

void DrawJuanEditor()
{
	CelDraw(SCREEN_X, 351 + SCREEN_Y, pEditorPanel, 1, SPANEL_WIDTH);
	DrawString(32, 48, "Juan Editor", COL_WHITE);
	//PlaySFX(IS_TITLEMOV);
	int posY = 80;
	JMenuItem *item = juanMenuItems;
	while (item->fnMenu) {
		char color = COL_WHITE;
		if (MouseX > 32 && MouseX < 200 && MouseY > posY - 16 && MouseY < posY) {
			color = COL_RED;
		}
		DrawString(32, posY, item->pszStr, color);
		posY += 20;
		item++;
	}
}

void CheckJuanEditorBtns()
{
	int posY = 80;
	JMenuItem *item = juanMenuItems;
	while (item->fnMenu) {
		if (MouseX > 32 && MouseX < 200 && MouseY > posY - 16 && MouseY < posY) {
			item->fnMenu(0);
		}
		posY += 20;
		item++;
	}
}

DEVILUTION_END_NAMESPACE
