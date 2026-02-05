#pragma once
#include "defines.h"

typedef struct deck
{
	int deck[DECK_SIZE];
	int index;
} deck;

void initialize_deck(deck* deck);
void shuffle_deck(deck* deck);