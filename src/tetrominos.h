
using std::cout; // for debugging

class oTetromino {
	
	private:
		
		bool failstate = false;
		
		unsigned char shapeCur = 0;
		unsigned char shapePrev = 0;
		
		int x = 0, y = 0;
		int xprev = 0, yprev = 0;
		
		olc::Pixel color = olc::Pixel(255, 255, 255);
		
		bool canMoveDown = true;
		bool canMoveRight = true;
		bool canMoveLeft = true;
		
		bool placeShape = false;
		
		bool isExisting = true;
	
		char shapeVariations[4][4][4];
		
		float fallTimer = 0;
		float moveHorTimer = 0;
		
		bool pressingDown = false;
		bool pressingRight = false;
		bool pressingLeft = false;
		bool pressingFlipRight = false;
		bool pressingFlipLeft = false;
		bool pressingSlam = false;
		
		void moveRight() { if (canMoveRight) x++; };
		void moveLeft() { if (canMoveLeft) x--; };
		void moveDown() { if (canMoveDown) y++; };
		
		void checkSpin(olc::PixelGameEngine *pge, int loopx, int loopy) {
			drawWellRect(pge, x + loopx, y + loopy);
			if (well[y + loopy][x + loopx] == cs_iWellBlockFilled || x + loopx > cs_iWellWidth) {
				if (loopx == 1  || loopx == 2 || loopy == 1 || loopy == 2) {
					y--;
				}
				if (loopx < 2) { 
					x++;
				} else { 
					x--; 
				}
			}
		}
		void checkWell(int loopx, int loopy) {
			if (well[y + loopy + 1][x + loopx] == cs_iWellBlockFilled) {
				canMoveDown = false;
			}
			if (well[y + loopy][x + loopx + 1] == cs_iWellBlockFilled) {
				canMoveRight = false;
			}
			if (well[y + loopy][x + loopx - 1] == cs_iWellBlockFilled) {
				canMoveLeft = false;
			}
		}
		void checkAndMoveWalls(int loopx, int loopy) {
			if (x + loopx >= cs_iWellWidth) {
				x--;
			}
			if (x + loopx < 0) {
				x++;
			}
			if (y + loopy >= cs_iWellHeight) {
				y--;
			}
		}
		
	protected:
		
		void sendShapes(olc::PixelGameEngine *pge, char shapes[4][4][4], bool falling) {
			if (falling) {
				canMoveDown = true;
				canMoveRight = true;
				canMoveLeft = true;

				for (unsigned char loopy = 0; loopy < 4; loopy++) {
					for (unsigned char loopx = 0; loopx < 4; loopx++) {
						if (xprev + loopx < cs_iWellWidth && well[yprev + loopy][xprev + loopx] != cs_iWellBlockFilled) {
							drawWellRect(pge, xprev + loopx, yprev + loopy);
						}
						if (shapes[shapeCur][loopy][loopx] != ' ') {
							
							// Wall checks
							checkAndMoveWalls(loopx, loopy);
							
							// Well checks
							checkWell(loopx, loopy);
							
							// Check spin
							checkSpin(pge, loopx, loopy);
							
							// Draw
							pge->FillRect(x * cs_iBlockSize + loopx * cs_iBlockSize, y * cs_iBlockSize + loopy * cs_iBlockSize, cs_iBlockSize, cs_iBlockSize , color);
							
							// Place in well
							if (y + loopy >= cs_iWellHeight - 1) {
								canMoveDown = false;
							}
							
						} else {
							continue;
						}
					}
				}
				
				if (placeShape) {
					lockShapeInWell(pge, shapes[shapeCur], x, y);
					isExisting = false;
					for (unsigned char loopy = 0; loopy < 4; loopy++) {
						for (unsigned char loopx = 0; loopx < 4; loopx++) {
							if (shapes[shapeCur][loopy][loopx] != ' ') {
								if (loopy + y < 0 && shapes[shapeCur][loopy][loopx] != ' ')  failstate = true;
							}
						}
					}
				}
			} else {
				pge->FillRect(
					(cs_iWellWidth * cs_iBlockSize) + (cs_iBlockGhostOffset / 2) + 1, 
					(cs_iBlockGhostOffset / 2) + 1, 
					cs_iBlockSize * cs_iWellWidth/2 - cs_iBlockGhostOffset - (cs_iBlockGhostOffset / 2) - 1, 
					cs_iBlockSize * cs_iWellHeight/3 - 1,
					cs_pxlBG
				);
				for (unsigned char loopy = 0; loopy < 4; loopy++) {
					for (unsigned char loopx = 0; loopx < 4; loopx++) {
						if (shapes[shapeCur][loopy][loopx] != ' ') {
							pge->FillRect((cs_iWellWidth + 1) * cs_iBlockSize + loopx * cs_iBlockSize, 1 * cs_iBlockSize + loopy * cs_iBlockSize, cs_iBlockSize, cs_iBlockSize , color);
						}
					}
				}
			}
		};
		
	public:
		
		bool exists() { return isExisting; }
		bool failed() { return failstate; }
		void setColor(olc::Pixel ic) { color = ic; }
		
		void input(bool bFlipRight, bool bFlipLeft, bool bMoveRightPress, bool bMoveRightHold, bool bMoveLeftPress, bool bMoveLeftHold, bool bMoveDown, bool bSlam) {
			
			pressingFlipLeft = bFlipLeft;
			pressingFlipRight = bFlipRight;
			
			if (pressingFlipLeft) { // yes the pressingFlipLeft and pressingFlipRight are redundant. im lazy. sue me
				if (shapeCur - 1 < 0) {
					shapeCur = 3;
				} else {
					shapeCur--;
				}
			}
			
			if (pressingFlipRight) {
				shapeCur++;
				if (shapeCur > 3) shapeCur = 0;
			}
			
			if (bMoveRightPress && canMoveDown) moveHorTimer = 1;
			pressingRight = bMoveRightHold;
			
			if (bMoveLeftPress && canMoveDown) moveHorTimer = 1;
			pressingLeft = bMoveLeftHold;
			
			pressingDown = bMoveDown;
			
			pressingSlam = bSlam;
			
		}
		void create() {
			y = -3;
			x = cs_iWellWidth / 2 - 2;
		}
		void step(float fDelta) {
			
			if (shapePrev != shapeCur) shapePrev = shapeCur;

			if (xprev != x) xprev = x;
			if (yprev != y) yprev = y;
			
			if (pressingDown && cs_iTertrominoMoveDownSpd > (cs_iTertrominoFallSpd + (wellLines / cs_iTertrominoFallSpdFactor))) {
				fallTimer += fDelta * cs_iTertrominoMoveDownSpd;
			} else {
				fallTimer += fDelta * (cs_iTertrominoFallSpd + (wellLines / cs_iTertrominoFallSpdFactor));
			}
			if (fallTimer > 1) {
				if (canMoveDown) {
					fallTimer = 0;
					moveDown();
				} else {
					placeShape = true;
				}
			}
			
			(pressingRight || pressingLeft) ? (moveHorTimer += fDelta * cs_iTertrominoMoveHorSpd) : (moveHorTimer = 0);
			if (moveHorTimer >= 1) {
				if (pressingRight) moveRight();
				if (pressingLeft) moveLeft();
				moveHorTimer = 0;
			}
			
		};
		
		virtual void render(olc::PixelGameEngine *pge) {};
		virtual void setFalling(bool ifalling) {};
		
};

class oTetrominoI : public oTetromino {
	
	private:
		
		char shapeVariations[4][4][4] = {
			{
				{' ', 'I', ' ', ' '},
				{' ', 'I', ' ', ' '},
				{' ', 'I', ' ', ' '},
				{' ', 'I', ' ', ' '},
			},
			{
				{' ', ' ', ' ', ' '},
				{'I', 'I', 'I', 'I'},
				{' ', ' ', ' ', ' '},
				{' ', ' ', ' ', ' '},
			},
			{
				{' ', ' ', 'I', ' '},
				{' ', ' ', 'I', ' '},
				{' ', ' ', 'I', ' '},
				{' ', ' ', 'I', ' '},
			},
			{
				{' ', ' ', ' ', ' '},
				{' ', ' ', ' ', ' '},
				{'I', 'I', 'I', 'I'},
				{' ', ' ', ' ', ' '},
			},
		};
		bool falling = false;
		
	public:
	
		oTetrominoI() {};
		oTetrominoI(bool ifalling): falling(ifalling) {};
		
		void setFalling(bool ifalling) { falling = ifalling; }
		
		void render(olc::PixelGameEngine *pge) {
			
			sendShapes(pge, shapeVariations, falling);

		};
	
};

class oTetrominoT : public oTetromino {
	
	private:
	
		char shapeVariations[4][4][4] = {
			{
				{' ', ' ', ' ', ' '},
				{'T', 'T', 'T', ' '},
				{' ', 'T', ' ', ' '},
				{' ', ' ', ' ', ' '},
			},
			{
				{' ', ' ', 'T', ' '},
				{' ', 'T', 'T', ' '},
				{' ', ' ', 'T', ' '},
				{' ', ' ', ' ', ' '},
			},
			{
				{' ', ' ', ' ', ' '},
				{' ', ' ', 'T', ' '},
				{' ', 'T', 'T', 'T'},
				{' ', ' ', ' ', ' '},
			},
			{
				{' ', ' ', ' ', ' '},
				{' ', 'T', ' ', ' '},
				{' ', 'T', 'T', ' '},
				{' ', 'T', ' ', ' '},
			},
		};
		bool falling = false;
		
	public:
	
		oTetrominoT() {};
		oTetrominoT(bool ifalling): falling(ifalling) {};
	
		void setFalling(bool ifalling) { falling = ifalling; }
	
		void render(olc::PixelGameEngine *pge) {
			
			sendShapes(pge, shapeVariations, falling);

		};
	
};

class oTetrominoO : public oTetromino {
	
	private:
	
		char shapeVariations[4][4][4] = {
			{
				{' ', ' ', ' ', ' '},
				{' ', 'O', 'O', ' '},
				{' ', 'O', 'O', ' '},
				{' ', ' ', ' ', ' '},
			},
			{
				{' ', ' ', ' ', ' '},
				{' ', 'O', 'O', ' '},
				{' ', 'O', 'O', ' '},
				{' ', ' ', ' ', ' '},
			},
			{
				{' ', ' ', ' ', ' '},
				{' ', 'O', 'O', ' '},
				{' ', 'O', 'O', ' '},
				{' ', ' ', ' ', ' '},
			},
			{
				{' ', ' ', ' ', ' '},
				{' ', 'O', 'O', ' '},
				{' ', 'O', 'O', ' '},
				{' ', ' ', ' ', ' '},
			},
		};
		bool falling = false;
		
	public:
	
		oTetrominoO() {};
		oTetrominoO(bool ifalling): falling(ifalling) {};
		
		void setFalling(bool ifalling) { falling = ifalling; }
		
		void render(olc::PixelGameEngine *pge) {
			
			sendShapes(pge, shapeVariations, falling);

		};
	
};

class oTetrominoJ : public oTetromino {
	
	private:
	
		char shapeVariations[4][4][4] = {
			{
				{' ', ' ', 'J', ' '},
				{' ', ' ', 'J', ' '},
				{' ', 'J', 'J', ' '},
				{' ', ' ', ' ', ' '},
			},
			{
				{' ', ' ', ' ', ' '},
				{' ', 'J', ' ', ' '},
				{' ', 'J', 'J', 'J'},
				{' ', ' ', ' ', ' '},
			},
			{
				{' ', ' ', ' ', ' '},
				{' ', 'J', 'J', ' '},
				{' ', 'J', ' ', ' '},
				{' ', 'J', ' ', ' '},
			},
			{
				{' ', ' ', ' ', ' '},
				{'J', 'J', 'J', ' '},
				{' ', ' ', 'J', ' '},
				{' ', ' ', ' ', ' '},
			},			
		};
		bool falling = false;
		
	public:
	
		oTetrominoJ() {};
		oTetrominoJ(bool ifalling): falling(ifalling) {};
		
		void setFalling(bool ifalling) { falling = ifalling; }
		
		void render(olc::PixelGameEngine *pge) {
			
			sendShapes(pge, shapeVariations, falling);

		};
	
};

class oTetrominoL : public oTetromino {
	
	private:
	
		char shapeVariations[4][4][4] = {
			{
				{' ', 'L', ' ', ' '},
				{' ', 'L', ' ', ' '},
				{' ', 'L', 'L', ' '},
				{' ', ' ', ' ', ' '},
			},
			{
				{' ', ' ', ' ', ' '},
				{' ', 'L', 'L', 'L'},
				{' ', 'L', ' ', ' '},
				{' ', ' ', ' ', ' '},
			},
			{
				{' ', ' ', ' ', ' '},
				{' ', 'L', 'L', ' '},
				{' ', ' ', 'L', ' '},
				{' ', ' ', 'L', ' '},
			},
			{
				{' ', ' ', ' ', ' '},
				{' ', ' ', 'L', ' '},
				{'L', 'L', 'L', ' '},
				{' ', ' ', ' ', ' '},
			},
		};
		bool falling = false;
		
	public:
	
		oTetrominoL() {};
		oTetrominoL(bool ifalling): falling(ifalling) {};
		
		void setFalling(bool ifalling) { falling = ifalling; }
	
		void render(olc::PixelGameEngine *pge) {
			
			sendShapes(pge, shapeVariations, falling);

		};
	
};

class oTetrominoS : public oTetromino {
	
	private:
	
		char shapeVariations[4][4][4] = {
			{
				{' ', ' ', ' ', ' '},
				{' ', 'S', 'S', ' '},
				{'S', 'S', ' ', ' '},
				{' ', ' ', ' ', ' '},
			},
			{
				{' ', 'S', ' ', ' '},
				{' ', 'S', 'S', ' '},
				{' ', ' ', 'S', ' '},
				{' ', ' ', ' ', ' '},
			},
			{
				{' ', ' ', ' ', ' '},
				{' ', 'S', 'S', ' '},
				{'S', 'S', ' ', ' '},
				{' ', ' ', ' ', ' '},
			},
			{
				{' ', 'S', ' ', ' '},
				{' ', 'S', 'S', ' '},
				{' ', ' ', 'S', ' '},
				{' ', ' ', ' ', ' '},
			},
		};
		bool falling = false;
		
	public:
	
		oTetrominoS() {};
		oTetrominoS(bool ifalling): falling(ifalling) {};
	
		void setFalling(bool ifalling) { falling = ifalling; }
	
		void render(olc::PixelGameEngine *pge) {
			
			sendShapes(pge, shapeVariations, falling);

		};
	
};

class oTetrominoZ : public oTetromino {
	
	private:
	
		char shapeVariations[4][4][4] = {
			{
				{' ', ' ', ' ', ' '},
				{'Z', 'Z', ' ', ' '},
				{' ', 'Z', 'Z', ' '},
				{' ', ' ', ' ', ' '},
			},
			{
				{' ', ' ', 'Z', ' '},
				{' ', 'Z', 'Z', ' '},
				{' ', 'Z', ' ', ' '},
				{' ', ' ', ' ', ' '},
			},
			{
				{' ', ' ', ' ', ' '},
				{'Z', 'Z', ' ', ' '},
				{' ', 'Z', 'Z', ' '},
				{' ', ' ', ' ', ' '},
			},
			{
				{' ', ' ', 'Z', ' '},
				{' ', 'Z', 'Z', ' '},
				{' ', 'Z', ' ', ' '},
				{' ', ' ', ' ', ' '},
			},
		};
		bool falling = false;
		
	public:
	
		oTetrominoZ() {};
		oTetrominoZ(bool ifalling): falling(ifalling) {};
	
		void setFalling(bool ifalling) { falling = ifalling; }
	
		void render(olc::PixelGameEngine *pge) {
			
			sendShapes(pge, shapeVariations, falling);

		};
	
};






