#pragma once

enum {
  SCREEN_WIDTH  = 960,
  SCREEN_HEIGHT = 544
};
#define DEBUG 1

#define SPRITES_DIR "app0:\\content\\sprites\\"
#define LEVELS_DIR "app0:\\content\\configs\\levels\\"
#define CFG_EXT ".cfg"
#define PNG_EXT ".png"
#define GAME_CONFIG_PATH "app0:\\content\\configs\\game.cfg"
#define BUF_SIZE 64

#if DEBUG
#define printf psvDebugScreenPrintf
#endif

