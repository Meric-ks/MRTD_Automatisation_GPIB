#include "ui.h"
// plus tard: #include "gpib_ctrl.h"

int main(int argc, char **argv)
{
    // plus tard: gpib_init(...);
    int status = ui_run(argc, argv);
    // plus tard: gpib_close();
    return status;
}
