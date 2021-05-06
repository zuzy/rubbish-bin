#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

int main(int argc, char **argv)
{
    int opt;
    int digit_optind = 0;
    int option_index = 0;
    //    char *optstring = "abcdrs";
    char *optstring = "a:b:c:d:r:s";
    static struct option long_options[] = {
        {"reqarg", required_argument, NULL, 'r'},
        {"noarg", no_argument, NULL, 'n'},
        {"optarg", optional_argument, NULL, 'o'},
        {"xarg", optional_argument, NULL, 0},
        {0, 0, 0, 0}};

    // while ((opt = getopt_long(argc, argv, optstring, long_options, &option_index)) != -1)
    while ((opt = getopt_long(argc - 2, argv + 2, optstring, long_options, &option_index)) != -1)
    {
        printf("-------------------------\n");
        printf("opt = %c\n", opt);
        printf("optarg = %s\n", optarg);
        printf("optind = %d\n", optind);
        printf("argv[optind - 1] = %s\n", argv[optind - 1]);
        printf("option_index = %d\n", option_index);
    }

    return 0;
}
