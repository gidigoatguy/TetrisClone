
void drawWellRect(olc::PixelGameEngine *pge, int x, int y) {
	// clear background
	pge->FillRect(
		x * cs_iBlockSize, y * cs_iBlockSize,
		cs_iBlockSize, cs_iBlockSize,
		cs_pxlBG
	);
	
	// Draw Rect
	pge->DrawRect(
		(x * cs_iBlockSize) + (cs_iBlockGhostOffset / 2), 
		(y * cs_iBlockSize) + (cs_iBlockGhostOffset / 2), 
		cs_iBlockSize - cs_iBlockGhostOffset - 1, 
		cs_iBlockSize - cs_iBlockGhostOffset - 1,
		cs_pxlWell
	);
};

void drawHudText(olc::PixelGameEngine *pge, std::string s, int x, int y, char fillOffset) {
	pge->FillRect(x - fillOffset, y - fillOffset, s.length() * 8 + fillOffset * 2, 8 + fillOffset * 2, cs_pxlBG);
	pge->DrawString(x + 1, y + 1, s, olc::Pixel(100, 100, 100));
	pge->DrawString(x, y, s);
};

void drawHud(olc::PixelGameEngine *pge) {
	pge->DrawRect(
		(cs_iWellWidth * cs_iBlockSize) + (cs_iBlockGhostOffset / 2), 
		(cs_iBlockGhostOffset / 2), 
		cs_iBlockSize * cs_iWellWidth/2 - cs_iBlockGhostOffset - (cs_iBlockGhostOffset / 2), 
		cs_iBlockSize * cs_iWellHeight/3,
		cs_pxlWell
	);
	
	std::string sIn = "LVL:   " + std::to_string((int)(wellLines / cs_iTertrominoFallSpdFactor));
	if (wellLines > 9) sIn = "LVL:  " + std::to_string((int)(wellLines / cs_iTertrominoFallSpdFactor));
	if (wellLines > 99) sIn = "LVL: " + std::to_string((int)(wellLines / cs_iTertrominoFallSpdFactor));
	drawHudText(
		pge,
		sIn,
		(cs_iWellWidth * cs_iBlockSize) + 8, 
		(cs_iBlockGhostOffset / 2) + cs_iBlockSize * cs_iWellWidth/2 - cs_iBlockGhostOffset - (cs_iBlockGhostOffset / 2) + 24,
		0
	);
	
	sIn = "LINES:";
	drawHudText(
		pge,
		sIn,
		(cs_iWellWidth * cs_iBlockSize) + 8, 
		(cs_iBlockGhostOffset / 2) + cs_iBlockSize * cs_iWellWidth/2 - cs_iBlockGhostOffset - (cs_iBlockGhostOffset / 2) + 24 + 16,
		0
	);
	
	sIn = "       " + std::to_string((int)(wellLines));
	if (wellLines > 9) sIn = "      " + std::to_string((int)(wellLines));
	if (wellLines > 99) sIn = "     " +std::to_string((int)(wellLines));
	drawHudText(
		pge,
		sIn,
		(cs_iWellWidth * cs_iBlockSize) + 8, 
		(cs_iBlockGhostOffset / 2) + cs_iBlockSize * cs_iWellWidth/2 - cs_iBlockGhostOffset - (cs_iBlockGhostOffset / 2) + 24 + 24,
		0
	);
	
};

void drawWellBG(olc::PixelGameEngine *pge) {
	for (unsigned char y = 0; y < cs_iWellHeight; y++) {
		for (unsigned char x = 0; x < cs_iWellWidth; x++) {
			drawWellRect(pge, x, y);
		}
	}
};

void drawWellBlocks(olc::PixelGameEngine *pge) {
	bool shift = false;
	unsigned char ystart = 0;
	for (unsigned char y = 0; y < cs_iWellHeight; y++) {
		bool hasLine = false;
		for (unsigned char x = 0; x < cs_iWellWidth; x++) {
			if (well[y][x] == cs_iWellBlockFilled) {
				pge->FillRect(x * cs_iBlockSize, y * cs_iBlockSize, cs_iBlockSize, cs_iBlockSize);
			}
		}
	}
};

void checkWellForLines(olc::PixelGameEngine *pge) {
	unsigned char linesToClear[4] = {0};
	unsigned char lineCount = 0;
	for (unsigned char loopy = 0; loopy < cs_iWellHeight; loopy++) {
		char fillCount = 0;
		for (unsigned char loopx = 0; loopx < cs_iWellWidth; loopx++) {
			if (well[loopy][loopx] == cs_iWellBlockFilled) fillCount++;
		}
		if (fillCount >= cs_iWellWidth) {
			linesToClear[lineCount] = loopy;
			lineCount++;
		}
	}
	if (lineCount > 0) {
		for (unsigned char x = 0; x < cs_iWellWidth; x++) {
			well[0][x] = cs_iWellBlockEmpty;
		}
		wellLines += lineCount;
		for (unsigned char i = 0; i < lineCount; i++) {
			char yToClear = linesToClear[i];
			for (unsigned char loopx = 0; loopx < cs_iWellWidth; loopx++) {
				drawWellRect(pge, loopx, yToClear);
				well[yToClear][loopx] = cs_iWellBlockEmpty;
			}
			for (unsigned char y = yToClear; y > 0; y--) {
				for (unsigned char x = 0; x < cs_iWellWidth; x++) {
					if (well[y][x] == cs_iWellBlockFilled) {
						drawWellRect(pge, x, y);
						well[y][x] = cs_iWellBlockEmpty;
						well[y + 1][x] = cs_iWellBlockFilled;
					}
				}
			}
		}
	}
};

void lockShapeInWell(olc::PixelGameEngine *pge, char shapes[4][4], int x, int y) {
	for (unsigned char loopy = 0; loopy < 4; loopy++) {
		for (unsigned char loopx = 0; loopx < 4; loopx++) {
			if (shapes[loopy][loopx] != ' ') {
				well[y + loopy][x + loopx] = cs_iWellBlockFilled;
			}
		}
	}
	checkWellForLines(pge);
};