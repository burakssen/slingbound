#include <iostream>
#include "Game.h"

int main()
{
    try
    {
        Game::Initialize();
        Game &game = Game::GetInstance();
        game.Run();
        Game::Shutdown();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
