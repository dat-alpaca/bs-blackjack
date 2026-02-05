#pragma once

#define KEY_UP 1000
#define KEY_DOWN 1001
#define KEY_ENTER 1002

#define CURSOR_UP(n) "\x1b[" #n "A"
#define CURSOR_DOWN(n) "\x1b[" #n "B"
#define CURSOR_HIDE "\x1b[?25l"
#define CURSOR_SHOW "\x1b[?25h"