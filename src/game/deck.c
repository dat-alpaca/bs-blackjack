#include <stdlib.h>

#include "deck.h"
#include "platform.h"
#include "core/utils.h"

void shuffle_deck(deck* deck)
{
	BS_ASSERT_NULL(deck);

	for (int i = 0; i < DECK_SIZE; ++i)
	{
		int k = rand() % (i + 1);
		swap(&deck->deck[i], &deck->deck[k]);
	}
}

void initialize_deck(deck* deck)
{
	BS_ASSERT_NULL(deck);

	deck->index = 0;

	int count = 0;
	for (int i = 1; i <= 13; ++i)
	{
		for (int j = 0; j < 4; ++j)
			deck->deck[count++] = i;
	}
}