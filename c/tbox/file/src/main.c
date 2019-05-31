#include "tbox/tbox.h"

int main(int argc, char** argv)
{
    tb_file_link(argv[1], argv[2]);
    tb_file_info_t info;
    tb_file_info(argv[1], &info);
    return 0;
}
