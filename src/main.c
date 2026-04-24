#include <stdlib.h>
#include <stdio.h>
#define row 8
#define column 8
#define RED   "\033[31m"
#define WHITE "\033[37m"
#define RESET "\033[0m"
#define BROWN "\033[38;5;94m"

enum
{
    TYPE_EMPTY = 0,
    TYPE_PAWN = 1,
    TYPE_KNIGHT = 2,
    TYPE_BISHOP = 3,
    TYPE_ROOK = 4,
    TYPE_QUEEN = 5,
    TYPE_KING = 6
};

typedef enum
{
    EMPTY = 0,

    WHITE_PAWN   = (TYPE_PAWN << 1) | 0,
    WHITE_KNIGHT = (TYPE_KNIGHT << 1) | 0,
    WHITE_BISHOP = (TYPE_BISHOP << 1) | 0,
    WHITE_ROOK   = (TYPE_ROOK << 1) | 0,
    WHITE_QUEEN  = (TYPE_QUEEN << 1) | 0,
    WHITE_KING   = (TYPE_KING << 1) | 0,

    BLACK_PAWN   = (TYPE_PAWN << 1) | 1,
    BLACK_KNIGHT = (TYPE_KNIGHT << 1) | 1,
    BLACK_BISHOP = (TYPE_BISHOP << 1) | 1,
    BLACK_ROOK   = (TYPE_ROOK << 1) | 1,
    BLACK_QUEEN  = (TYPE_QUEEN << 1) | 1,
    BLACK_KING   = (TYPE_KING << 1) | 1,
} ChessPiece;

ChessPiece A[64] =
{
    BLACK_ROOK, BLACK_KNIGHT, BLACK_BISHOP, BLACK_QUEEN, BLACK_KING, BLACK_BISHOP, BLACK_KNIGHT, BLACK_ROOK,
    BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN,
    EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
    EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
    EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
    EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
    WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN,
    WHITE_ROOK, WHITE_KNIGHT, WHITE_BISHOP, WHITE_QUEEN, WHITE_KING, WHITE_BISHOP, WHITE_KNIGHT, WHITE_ROOK
};

char getPieceChar(ChessPiece p)
{
    if (p == EMPTY) return '.';
    int type = (p >> 1);
    int isBlack = p & 1;

    char c;
    switch(type)
    {
    case TYPE_PAWN:
        c = 'P';
        break;
    case TYPE_KNIGHT:
        c = 'N';
        break;
    case TYPE_BISHOP:
        c = 'B';
        break;
    case TYPE_ROOK:
        c = 'R';
        break;
    case TYPE_QUEEN:
        c = 'Q';
        break;
    case TYPE_KING:
        c = 'K';
        break;
    default:
        return '-';
    }
    return c;
}

void out()
{
    printf("\n   ");
    for(char i='a'; i<='h'; i++) printf(BROWN"%c "RESET, i);
    printf("\n");

    for(int i=0; i<row; i++)
    {
        printf(BROWN"%i  "RESET, 8-i);
        for(int j=0; j<column; j++)
        {
            if((A[i*row+j]& 1)==0)
                printf("%c ", getPieceChar(A[i*row+j]));
            else
                printf(RED"%c "RESET, getPieceChar(A[i*row+j]));

        }
        printf(BROWN" %i\n"RESET, 8-i);
    }

    printf("   ");
    for(char i='a'; i<='h'; i++) printf(BROWN"%c "RESET, i);
    printf("\n\n");
}
// сделать базовый принцип ходьбы фигур
// функци€ приема координат фигуры и места х у

int Color(ChessPiece p)
{
    return (p & 1);
}

int LogicPAWN(int x,int y)
{
    if(A[x]!=WHITE_PAWN && A[x]!=BLACK_PAWN)
        return 0;

    if(A[x]==WHITE_PAWN)
    {
        if((x-8==y) && A[y]==EMPTY)
            return 1;
        if((x-16==y) && A[y]==EMPTY && A[x-8]==EMPTY && x/8==6)
            return 1;
        if(((x-7==y) || (x-9==y)) && A[y]!=EMPTY && Color(A[y])!=0 && x/8-y/8==1)
            return 2;
        return 0;
    }
    else
    {
        if((x+8==y) && A[y]==EMPTY)
            return 1;
        if((x+16==y) && A[y]==EMPTY && A[x+8]==EMPTY && x/8==1)
            return 1;
        if(((x+7==y) || (x+9==y)) && A[y]!=EMPTY && Color(A[y])!=1 && y/8-x/8==1)
            return 2;
        return 0;
    }
}

int replace(int x, int y)
{
    A[y]=A[x];
    A[x]=EMPTY;
}

int Figaro(int x, int y,int B1_or_W0)
{
    int type = (A[x] >> 1);
    switch(type)
    {
    case TYPE_PAWN:
        if (LogicPAWN(x,y))
            replace(x,y);
        else
            return 0;
        break;
    case TYPE_KNIGHT:
        break;
    case TYPE_BISHOP:
        break;
    case TYPE_ROOK:
        break;
    case TYPE_QUEEN:
        break;
    case TYPE_KING:
        break;
    default:
        return 0;
    }
    int p[64];
    for(int i=0; i<64; i++)
        p[i]=0;
    for(int i=0; i<64; i++)
    {
        if(A[i]==EMPTY || Color(A[i])!=B1_or_W0)
            continue;
        type = (A[i] >> 1);
        switch(type)
        {
        case TYPE_PAWN:
            for(int j=0; j<64; j++)
            {
                int a=LogicPAWN(i,j);
                if(a!=0 && p[j]!=2)
                    p[j]=a;
            }
            break;
        case TYPE_KNIGHT:
            break;
        case TYPE_BISHOP:
            break;
        case TYPE_ROOK:
            break;
        case TYPE_QUEEN:
            break;
        case TYPE_KING:
            break;
        default:
            return 0;
        }
    }
    for(int i=0; i<8; i++)
    {
        printf("\n");
        for(int j=0; j<8; j++)
        {
            printf("%d ",p[i*row+j]);
        }
    }
}

int cordfinder(int n, char temp)
{
    return (8-n)*row+(temp-'a');
}
// Ќадо получить ходы, проверь правильность, записать, помен€ть сторону хода
void move()
{
    while(1)
    {
        out();
        int startcord, endcord;
        printf("\n Enter your move(for example e2e4):");
        char spos[3], epos[3];
        scanf("%2s%2s", spos, epos);
        //printf("%s", spos);
        char temp=spos[0];
        int n=spos[1]-'0';
        int f1=0, f2=0;
        for(int i=1; i<=8; i++)
            if(n==i)
                f1=1;
        for(char i='a'; i<='h'; i++)
            if(temp==i)
                f2=1;
        if(f1==1 && f2==1)
            startcord=cordfinder(n, temp);
        else
            continue;
        temp=epos[0];
        n=epos[1]-'0';
        f1=0;
        f2=0;
        for(int i=1; i<=8l; i++)
            if(n==i)
                f1=1;
        for(char i='a'; i<='h'; i++)
            if(temp==i)
                f2=1;
        if(f1==1 && f2==1)
            endcord=cordfinder(n, temp);
        else
            continue;
        system("cls");
        printf("start cord=%i, end cord=%i", startcord, endcord);
        Figaro(startcord,endcord,Color(A[startcord]));
    }
}

int main()
{
    move();
    return 0;
}

