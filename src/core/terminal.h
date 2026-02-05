#pragma once
#include <termios.h>
#include <unistd.h>

static struct termios originalTerminalOS;

void terminal_initialize();

void terminal_disable_raw_mode();
void terminal_enable_raw_mode();

void terminal_exit(void);

int terminal_read_key();