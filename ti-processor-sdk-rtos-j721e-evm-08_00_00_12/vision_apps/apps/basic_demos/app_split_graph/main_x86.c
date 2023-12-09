#include <utils/app_init/include/app_init.h>

int app_split_graph_main(int argc, char* argv[]);

int main(int argc, char* argv[])
{
    int status;

    appInit();
    status = app_split_graph_main(argc, argv);
    appDeInit();

    return status;
}
