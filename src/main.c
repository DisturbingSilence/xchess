#include <stdio.h>
#include <stdlib.h>
//початкову логіку для виводу поля і ходів
#define row 8
#define column 8
char *A[64]={
    "ROOK", "KNIGHT", "BISHOP", "QUEEN", "KING", "BISHOP", "KNIGHT", "ROOK",
    "PAWN", "PAWN", "PAWN", "PAWN", "PAWN", "PAWN", "PAWN", "PAWN",
    "EMPTY", "EMPTY","EMPTY","EMPTY","EMPTY","EMPTY","EMPTY","EMPTY",
    "EMPTY", "EMPTY","EMPTY","EMPTY","EMPTY","EMPTY","EMPTY","EMPTY",
    "EMPTY", "EMPTY","EMPTY","EMPTY","EMPTY","EMPTY","EMPTY","EMPTY",
    "EMPTY", "EMPTY","EMPTY","EMPTY","EMPTY","EMPTY","EMPTY","EMPTY",
    "PAWN", "PAWN", "PAWN", "PAWN", "PAWN", "PAWN", "PAWN", "PAWN",
    "ROOK", "KNIGHT", "BISHOP", "QUEEN", "KING", "BISHOP", "KNIGHT", "ROOK"
};

void out()
{
    for(char i='a'; i<='h'; i++)
        printf("\t%c ", i);
    printf("\n");
    for(int i=0; i<row; i++)
    {
        printf("%i ", 9-i-1);
        for(int j=0; j<column; j++)
        {
            printf("\t%s ", A[i*row+j]);
        }
        printf("\n");
    }
    for(char i='a'; i<='h'; i++)
        printf("\t%c ", i);
    return;
}



int main()
{
    out();
    return 0;
}
