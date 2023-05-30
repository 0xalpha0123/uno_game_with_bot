#include "game/uno_game.h"
#include "player/simple_player.h"
#include "utils/logger.h"
#include "utils/stats.h"
#include "stdio.h"

using namespace std;

int main()
{
    // Ваш main должен выглядеть примерно так

    UnoGame game;
    Logger logger;
    SimplePlayer player1("Vasya"), player2("Kolya");
    game.setRandomGeneratorSeed(2);
    game.addPlayer(&player1);
    game.addPlayer(&player2);
    game.addObserver(&logger);
    game.runGame();

    int i = 0;
    cin >> i;
}

void statisticTest()
{
    UnoGame game;
    SimplePlayer player1("Vasya"), player2("Kolya");
    game.addPlayer(&player1);
    game.addPlayer(&player2);

    StatsObserver result = runGames(game, 100'000);
    StatsObserver::MV winMV = result.getWinsMV();
    std::cout << "Vasya won in " << winMV.mean[0] * 100 << "% of games" << std::endl;
}

