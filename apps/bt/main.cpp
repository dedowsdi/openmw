#include "command.hpp"

int main(int argc, char *argv[])
{
    BT::Command command;
    command.execute(argc, argv);

    return 0;
}
