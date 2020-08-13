%{
    #include <stdio.h>
    void yyerror(char *);
    int num_lines = 0, num_chars = 0;
%}

%%
\n      {
            ++num_lines; ++num_chars;
        }
.       {
            ++num_chars;
        }
%%
int main(int argc, char *argv[]) {
    printf("come on!\n");
    yyin = fopen(argv[1], "r");
    yylex();
    printf("# of lines = %d, # of chars = %d\n", num_lines, num_chars);
    fclose(yyin);
    return 0;
}
