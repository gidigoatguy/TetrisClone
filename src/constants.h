
// chars only store 1 byte, thus taking up very little in memory 
// (it dosen't matter too much on modern machines but i still like saving memory for some reason)

const static char cs_iWindowScale = 2;

const static char cs_iBlockSize = 16;
const static char cs_iBlockGhostOffset = 4;

const static char cs_iWellWidth = 10;
const static char cs_iWellHeight = 16;
const static olc::Pixel cs_pxlWell = olc::Pixel(30, 30, 30);

const static char cs_iWellBlockEmpty = 0;
const static char cs_iWellBlockFilled = 1;
const static char cs_iWellBlockCleared = 2;
const static char cs_iWellBlockFalling = 2;

const static char cs_iTertrominoFallSpd = 1;
const static char cs_iTertrominoMoveDownSpd = 25;
const static char cs_iTertrominoMoveHorSpd = 12;
const static char cs_iTertrominoFallSpdFactor = 10;
const static char cs_iTertrominoColorAdd = 100;

const static olc::Pixel cs_pxlBG = olc::Pixel(10, 10, 10);