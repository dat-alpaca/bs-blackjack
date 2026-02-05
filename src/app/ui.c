#include "ui.h"
#include "app/client_data.h"
#include "platform.h"
#include "core/colors.h"

#include <stdio.h>

void ui_cycle_option(ui_data* ui, bool reverse)
{
	BS_ASSERT_NULL(ui);

	if (!reverse)
		ui->option = (ui->option + 1) % UI_OPTION_AMOUNT;
	else
		ui->option = (ui->option + UI_OPTION_AMOUNT - 1) % UI_OPTION_AMOUNT;
}

void ui_print_header(client_data* clientData)
{
	BS_ASSERT_NULL(clientData);

	int wins = clientData->totalVictories;
	int draws = clientData->totalDraws;
	int lost = clientData->totalDefeats;

	printf("\n");
	printf(GREEN "-=-= >" RED " Blackjack " GREEN "< =-=-\n" RESET);
	printf("%s%d%s%d%s%d", GREEN "Wins: " RESET, wins, RESET " | " YELLOW "Draws: " RESET, draws, RESET " | " RED "Defeats: " RESET, lost);
	printf("\n\n");
}
void ui_print_options(game_data* game, client_data* clientData)
{
	BS_ASSERT_NULL(game);
	BS_ASSERT_NULL(clientData);

	printf(CLEAR_SCREEN);
	ui_print_header(clientData);

	printf("Use the 'UP' and 'DOWN' arrow keys to switch through the options.\n");
	printf("Press 'Enter' to select the option chosen.\n\n");
}
void ui_print_menu(game_data* game, ui_data* ui, client_data* clientData)
{
	BS_ASSERT_NULL(game);
	BS_ASSERT_NULL(ui);
	BS_ASSERT_NULL(clientData);

	ui_print_options(game, clientData);

	switch (ui->option)
	{
		case UI_OPTION_PLAY:
		{
			printf(RED "Play\n\n" RESET);
			printf(WHITE "Quit\n\n" RESET);
		}
		break;

		case UI_OPTION_QUIT:
		{
			printf(WHITE "Play\n\n" RESET);
			printf(RED "Quit\n\n" RESET);
		}
		break;

		default:
			break;
	}
}

void ui_print_card(int card)
{
	switch (card)
	{
		case 1:
			printf(RED "Ace" RESET);
			break;
		case 2:
			printf(GREEN "Two" RESET);
			break;
		case 3:
			printf(GREEN "Three" RESET);
			break;
		case 4:
			printf(GREEN "Four" RESET);
			break;
		case 5:
			printf(GREEN "Five" RESET);
			break;
		case 6:
			printf(GREEN "Six" RESET);
			break;
		case 7:
			printf(GREEN "Seven" RESET);
			break;
		case 8:
			printf(GREEN "Eight" RESET);
			break;
		case 9:
			printf(GREEN "Nine" RESET);
			break;
		case 10:
			printf(GREEN "Ten" RESET);
			break;
		case 11:
			printf(PURPLE "Jack" RESET);
			break;
		case 12:
			printf(BLUE "Queen" RESET);
			break;
		case 13:
			printf(YELLOW "King" RESET);
			break;
	}
}
void ui_print_cards(player_data* player)
{
	BS_ASSERT_NULL(player);

	for (int i = 0; i < player->index; ++i)
	{
		printf("* ");
		ui_print_card(player->hand[i]);
		printf("\n");
	}
}
void ui_reveal_cards(game_data* game)
{
	BS_ASSERT_NULL(game);

	printf("Your cards are:\n");
	ui_print_cards(&game->userData);
	printf("%s%d" RESET "\n", "Total Sum: " YELLOW, game->userData.score);
	printf("\n");

	printf("The dealer cards are:\n");
	ui_print_cards(&game->dealerData);
	printf("%s%d" RESET "\n", "Total Sum: " YELLOW, game->dealerData.score);
	printf("\n");
}

void ui_print_win(game_data* game, client_data* clientData)
{
	BS_ASSERT_NULL(game);
	BS_ASSERT_NULL(clientData);

	printf(CLEAR_SCREEN);
	ui_print_header(clientData);
	ui_reveal_cards(game);

	printf("\n");
	printf("%s%d\n", GREEN "You Win!" RESET " You got a score of ", game->userData.score);
	printf("The dealer got a score of: %d\n", game->dealerData.score);
	printf("\n");
}
void ui_print_lose(game_data* game, client_data* clientData)
{
	BS_ASSERT_NULL(game);
	BS_ASSERT_NULL(clientData);

	printf(CLEAR_SCREEN);
	ui_print_header(clientData);
	ui_reveal_cards(game);

	printf("%s%d\n", RESET "Oh no! " RED "You Lost! " RESET "You got a score of ", game->userData.score);
	printf("The dealer got a score of: %d\n", game->dealerData.score);
	printf("\n");
}
void ui_print_draw(game_data* game, client_data* clientData)
{
	BS_ASSERT_NULL(game);
	BS_ASSERT_NULL(clientData);

	printf(CLEAR_SCREEN);
	ui_print_header(clientData);
	ui_reveal_cards(game);

	printf("%s%d\n", YELLOW "It's a draw!" RESET " You got a score of ", game->userData.score);
	printf("The dealer also got a score of: %d\n", game->dealerData.score);
	printf("\n");
}