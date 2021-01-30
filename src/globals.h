#pragma once

enum {
  SCREEN_WIDTH  = 960,
  SCREEN_HEIGHT = 544
};
#define DEBUG 1

#define SPRITES_DIR "app0:\\content\\sprites\\"
#define LEVELS_DIR "app0:\\content\\configs\\levels\\"
#define BG_DIR "app0:\\content\\sprites\\bg\\"
#define CFG_EXT ".cfg"
#define PNG_EXT ".png"
#define GAME_CONFIG_PATH "app0:\\content\\configs\\game.cfg"
#define GAME_FONT_PATH "app0:\\content\\fonts\\arial.ttf"
#define BULLET_SPRITE_ALIAS "bullet"
#define GAME_FONT_SIZE 14
#define BUF_SIZE 64

#define AI_NODE_WIDTH 50
#define AI_NODE_HEIGHT 50

#if DEBUG
#define printf psvDebugScreenPrintf
#endif

