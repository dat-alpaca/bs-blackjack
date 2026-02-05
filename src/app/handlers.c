#include "handlers.h"

#include "app/client_data.h"
#include "core/terminal.h"
#include "core/keys.h"
#include "core/colors.h"
#include "platform.h"

void handle_main_menu(game_data* game, ui_data* ui, client_data* clientData)
{
	BS_ASSERT_NULL(game);
	BS_ASSERT_NULL(ui);
	BS_ASSERT_NULL(clientData);

	while (true)
	{
		ui_print_menu(game, ui, clientData);
		int ch = terminal_read_key();

		if (ch == 'q')
		{
			ui->isRunning = false;
			break;
		}

		else if (ch == KEY_UP)
			ui_cycle_option(ui, false);

		else if (ch == KEY_DOWN)
			ui_cycle_option(ui, true);

		else if (ch == KEY_ENTER)
		{
			if (ui->option == UI_OPTION_QUIT)
			{
				ui->isRunning = false;
				break;
			}
			else
			{
				ui->currentView = UI_VIEW_PLAYING;
				break;
			}
		}
	}
}
void handle_playing(game_data* game, ui_data* ui, client_data* clientData)
{
	BS_ASSERT_NULL(game);
	BS_ASSERT_NULL(ui);
	BS_ASSERT_NULL(clientData);

	// Initialization:
	initialize_game(game);

	// Starter hands:
	game_draw_card(&game->deck, &game->userData);
	game_draw_card(&game->deck, &game->userData);

	game_draw_card(&game->deck, &game->dealerData);
	game_draw_card(&game->deck, &game->dealerData);

	// Player's turn:
	while (true)
	{
		printf(CLEAR_SCREEN);
		ui_print_header(clientData);
		printf("It's " RED "your " RESET "turn now\n\n");

		printf("Your cards are:\n");
		ui_print_cards(&game->userData);
		printf("%s%d" RESET "\n", "Total Sum: " YELLOW, game->userData.score);
		printf("\n");

		printf("The dealer cards are:\n");
		printf("* ");
		ui_print_card(game->dealerData.hand[0]);
		printf("\n* ?\n");

		printf("Press 'H' to Hit, and 'S' to Stand\n");
		printf("\n");

		int ch = terminal_read_key();

		if (ch == 'q')
		{
			game->result = GAME_RESULT_EXIT;
			ui->isRunning = false;
			break;
		}

		else if (ch == 'h' || ch == 'H')
			game_draw_card(&game->deck, &game->userData);

		else if (ch == 's' || ch == 'S')
			break;

		if (game->userData.score > 21)
		{
			game->result = GAME_RESULT_DEFEAT;
			break;
		}
		else if (game->userData.score == 21)
		{
			game->result = GAME_RESULT_VICTORY;
			break;
		}
	}

	if (game->result == GAME_RESULT_EXIT)
		return;

	else if (game->result == GAME_RESULT_DEFEAT)
	{
		++clientData->totalDefeats;
		ui_print_lose(game, clientData);
		return;
	}

	else if (game->result == GAME_RESULT_VICTORY)
	{
		++clientData->totalVictories;
		ui_print_win(game, clientData);
		return;
	}

	// Dealer's turn:
	while (game->dealerData.score < 17)
	{
		printf(CLEAR_SCREEN);
		ui_print_header(clientData);

		printf("Your cards are:\n");
		ui_print_cards(&game->userData);
		printf("%s%d" RESET "\n", "Total Sum: " YELLOW, game->userData.score);
		printf("\n");

		printf("The dealer cards are:\n");
		ui_print_cards(&game->dealerData);
		printf("%s%d" RESET "\n", "Total Sum: " YELLOW, game->dealerData.score);

		printf("Press any key to continue...");
		printf("\n");

		int ch = terminal_read_key();
		if (ch == 'q')
		{
			ui->isRunning = false;
			game->result = GAME_RESULT_EXIT;
			break;
		}

		if (game->dealerData.score > 21)
		{
			game->result = GAME_RESULT_VICTORY;
			break;
		}
		else if (game->dealerData.score == 21 || game->dealerData.score > game->userData.score)
		{
			game->result = GAME_RESULT_DEFEAT;
			break;
		}

		game_draw_card(&game->deck, &game->dealerData);
	}

	if (game->result == GAME_RESULT_EXIT)
		return;

	if (game->result == GAME_RESULT_VICTORY || game->dealerData.score > 21 || game->userData.score > game->dealerData.score)
	{
		++clientData->totalVictories;
		ui_print_win(game, clientData);
		return;
	}
	else if (game->result == GAME_RESULT_DEFEAT || game->userData.score < game->dealerData.score)
	{
		++clientData->totalDefeats;
		ui_print_lose(game, clientData);
		return;
	}

	++clientData->totalDraws;
	ui_print_draw(game, clientData);
}

bool handle_replay()
{
	printf("Would you like to play another game? (y / n)\n");

	while (true)
	{
		int ch = terminal_read_key();
		if (ch == 'y')
			return true;
        
        if (ch == 'n')
            return false;
	}
}