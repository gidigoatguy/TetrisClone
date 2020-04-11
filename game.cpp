
#define OLC_PGE_APPLICATION
#include "src\olcPixelGameEngine.h"

#include <stdlib.h>

#include "src\constants.h"

char well[cs_iWellHeight][cs_iWellWidth] = {{0}}; // jar jar is the key to all this
unsigned int wellLines = 0;

#include "src\functions.h"

#include "src\tetrominos.h"
typedef std::unique_ptr<oTetromino> ptr_tetromino;

using std::cout;

class Game : public olc::PixelGameEngine {
	
	public:
	
		Game() {
			sAppName = "Tetris Clone ";
		}
		
	private:
		
		ptr_tetromino tetrominoFalling { new oTetrominoI(true) };
		ptr_tetromino tetrominoNext { new oTetrominoT };
		
		bool running = false;
		
		void reset() {
			
			// chars are 1 byte, so sizeof(well) should be equal to cs_iWellWidth * cs_iWellHeight
			// i extend (3 * cs_iWellHeight) backwards because sometimes the well stores data before [0][0] (normally when a block is above the top of the well)
			// the backwards extention seems to be where wellLines is stored too, so as a side effect this clears wellLines too
			std::fill(&well[-4][0], &well[-4][0] + (cs_iWellWidth * cs_iWellHeight) + (3 * cs_iWellHeight), 0); 
			
			FillRect(0, 0, ScreenWidth(), ScreenHeight(), cs_pxlBG);
			
			drawHud(this);
			drawWellBG(this);
			
			setTetromino((char)(rand() % 7), tetrominoFalling);
			setTetromino((char)(rand() % 7), tetrominoNext);
			
			tetrominoFalling->create();
			tetrominoFalling->setFalling(true);
			tetrominoFalling->setColor(olc::Pixel(255, 0, 0));
			
			tetrominoNext->render(this);
			
		}
		
		void setTetromino(unsigned char index, ptr_tetromino &tetromino) {
			ptr_tetromino tetrominoNew;
			switch(index) {
				default:
				case 0:
					tetrominoNew = ptr_tetromino(new oTetrominoI);
					break;
				case 1:
					tetrominoNew = ptr_tetromino(new oTetrominoO);
					break;
				case 2:
					tetrominoNew = ptr_tetromino(new oTetrominoT);
					break;
				case 3:
					tetrominoNew = ptr_tetromino(new oTetrominoJ);
					break;
				case 4:
					tetrominoNew = ptr_tetromino(new oTetrominoL);
					break;
				case 5:
					tetrominoNew = ptr_tetromino(new oTetrominoS);
					break;
				case 6:
					tetrominoNew = ptr_tetromino(new oTetrominoZ);
					break;
			}
			//tetrominoNew = ptr_tetromino(new oTetrominoI);
			tetromino = std::move(tetrominoNew);
		}
		
	protected:
		
		bool OnUserCreate() override {
			
			FillRect(0, 0, ScreenWidth(), ScreenHeight(), cs_pxlBG);
			
			drawHud(this);
			drawWellBG(this);
			
			std::string s = "Press SPACE to play!";
			drawHudText(this, s, ScreenWidth() / 2 - (s.length() / 2) * 8, ScreenHeight() / 2, 8);
			
			return true;
		}
		
		bool OnUserUpdate(float fDelta) override {
			
			if (GetKey(olc::Key::ESCAPE).bPressed) return false;
			
			if (GetKey(olc::Key::R).bPressed) reset();
			
			if (GetKey(olc::Key::SPACE).bPressed && !running) {
				
				running = true;
				reset();
				
			}
			
			if (running) {
			
				if (tetrominoFalling->failed()) {
				
					running = false;
					
					std::string s = "Game Over!";
					drawHudText(this, s, ScreenWidth() / 2 - (s.length() / 2) * 8, ScreenHeight() / 2 - 16, 8);
					
					s = "Press SPACE to try again!";
					drawHudText(this, s, ScreenWidth() / 2 - (s.length() / 2) * 8, ScreenHeight() / 2 + 16, 8);
					
				} else {
				
					if (tetrominoFalling->exists()) {
						
						tetrominoFalling->input(
							GetKey(olc::Key::E).bPressed, GetKey(olc::Key::Q).bPressed, 
							GetKey(olc::Key::D).bPressed, GetKey(olc::Key::D).bHeld, GetKey(olc::Key::A).bPressed, GetKey(olc::Key::A).bHeld, 
							GetKey(olc::Key::S).bHeld,
							GetKey(olc::Key::SPACE).bPressed
						);
						tetrominoFalling->step(fDelta);
						tetrominoFalling->render(this);
						
					} else {
						
						tetrominoFalling = std::move(tetrominoNext);
						tetrominoFalling->setFalling(true);
						tetrominoFalling->create();
						tetrominoFalling->setColor(olc::Pixel(255, 0, 0));
						
						setTetromino((char)(rand() % 7), tetrominoNext);
						tetrominoNext->render(this);
						
						drawHud(this);
						
						drawWellBlocks(this);
						
					}
					
				}
				
			}
			
			
			return true;
		}
		
		bool OnUserDestroy() override {
				
			// because im using unique_ptr for object storage, the memory will be cleared when those objects leave scope :)	
			
			return true;
		
		}
		
};

int main() {
	
	Game demo;
	if (demo.Construct((cs_iBlockSize * cs_iWellWidth) + (cs_iBlockSize * cs_iWellWidth)/2, cs_iBlockSize * cs_iWellHeight + (cs_iBlockGhostOffset / 2), cs_iWindowScale, cs_iWindowScale)) {
		demo.Start();
	}
	
	return 0;
}