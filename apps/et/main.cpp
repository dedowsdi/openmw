#include "command.hpp"

int main(int argc, char *argv[])
{
    ET::Command command(std::cout);
    command.execute(argc, argv);

    return 0;
}
