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

int LogicROOK(int x,int y)
{
    if(A[x]!=WHITE_ROOK && A[x]!=BLACK_ROOK)
        return 0;

    else
    {
        if(x/8!=y/8 && abs(x-y)%8==0 && (A[y]==EMPTY || Color(A[y])!=Color(A[x])))
        {
            if(x<y)
            {
                for(int i=x+8; i!=y; i=i+8)
                    if(A[i]!=EMPTY)
                        return 0;
            }
            if(y<x)
            {
                for(int i=x-8; i!=y; i=i-8)
                    if(A[i]!=EMPTY)
                        return 0;
            }
            if(A[y]==EMPTY)
                return 1;
            return 2;
        }
        if(y/8==x/8 && (A[y]==EMPTY || Color(A[y])!=Color(A[x])) )
        {
            if(x<y)
            {
                for(int i=x+1; i!=y; i++)
                    if(A[i]!=EMPTY)
                        return 0;
            }
            if(y<x)
            {
                for(int i=x-1; i!=y; i--)
                    if(A[i]!=EMPTY)
                        return 0;
            }
            if(A[y]==EMPTY)
                return 1;
            return 2;
        }
        return 0;
    }
}

int LogicKNIGHT(int x,int y)
{
    if(A[x]!=WHITE_KNIGHT && A[x]!=BLACK_KNIGHT)
        return 0;
    if((y==x-17 || y==x-15  || y==x+17 || y==x+15) && abs(x/8-y/8)==2 || ((y==x+10 || y==x-6)
            && x/8==(x+2)/8 || (y==x-10 || y==x+6) && x/8==(x-2)/8) && abs(x/8-y/8)==1)
    {
        if(A[y]==EMPTY)
            return 1;
        if(Color(A[y])!=Color(A[x]))  return 2;
    }
    return 0;
}

int LogicBISHOP(int x,int y)
{
    if(A[x]!=WHITE_BISHOP && A[x]!=BLACK_BISHOP)
        return 0;
    if(y<x && y/8!=x/8 && (x-7)%8!=0)
        for(int i=x-7; i>=0; i=i-7)
        {
            if(i==y)
            {
                if(A[y]!=EMPTY)
                    if(Color(A[y])!=Color(A[x]))
                        return 2;
                    else return 0;
                return 1;
            }
            if(A[i]!=EMPTY)
                break;
            if((i-7)%8==0)
                break;
        }
    if(y>x && y/8!=x/8 && (x-7)%8!=0)
        for(int i=x+9; i<=63; i=i+9)
        {
            if(i==y)
            {
                if(A[y]!=EMPTY)
                    if(Color(A[y])!=Color(A[x]))
                        return 2;
                    else return 0;
                return 1;
            }
            if(A[i]!=EMPTY)
                break;
            if((i-7)%8==0)
                break;
        }
    if(y<x && y/8!=x/8 && x%8!=0)
        for(int i=x-9; i>=0; i=i-9)
        {
            if(i==y)
            {
                if(A[y]!=EMPTY)
                    if(Color(A[y])!=Color(A[x]))
                        return 2;
                    else return 0;
                return 1;
            }
            if(A[i]!=EMPTY)
                break;
            if(i%8==0)
                break;
        }
    if(y>x && y/8!=x/8 && x%8!=0)
        for(int i=x+7; i<=63; i=i+7)
        {
            if(i==y)
            {
                if(A[y]!=EMPTY)
                    if(Color(A[y])!=Color(A[x]))
                        return 2;
                    else return 0;
                return 1;
            }
            if(A[i]!=EMPTY)
                break;
            if(i%8==0)
                break;
        }
    return 0;
}

int LogicQUEEN(int x,int y)
{
    if(A[x]!=WHITE_QUEEN && A[x]!=BLACK_QUEEN)
        return 0;
    if(y<x && y/8!=x/8 && (x-7)%8!=0)
        for(int i=x-7; i>=0; i=i-7)
        {
            if(i==y)
            {
                if(A[y]!=EMPTY)
                    if(Color(A[y])!=Color(A[x]))
                        return 2;
                    else return 0;
                return 1;
            }
            if(A[i]!=EMPTY)
                break;
            if((i-7)%8==0)
                break;
        }
    if(y>x && y/8!=x/8 && (x-7)%8!=0)
        for(int i=x+9; i<=63; i=i+9)
        {
            if(i==y)
            {
                if(A[y]!=EMPTY)
                    if(Color(A[y])!=Color(A[x]))
                        return 2;
                    else return 0;
                return 1;
            }
            if(A[i]!=EMPTY)
                break;
            if((i-7)%8==0)
                break;
        }
    if(y<x && y/8!=x/8 && x%8!=0)
        for(int i=x-9; i>=0; i=i-9)
        {
            if(i==y)
            {
                if(A[y]!=EMPTY)
                    if(Color(A[y])!=Color(A[x]))
                        return 2;
                    else return 0;
                return 1;
            }
            if(A[i]!=EMPTY)
                break;
            if(i%8==0)
                break;
        }
    if(y>x && y/8!=x/8 && x%8!=0)
        for(int i=x+7; i<=63; i=i+7)
        {
            if(i==y)
            {
                if(A[y]!=EMPTY)
                    if(Color(A[y])!=Color(A[x]))
                        return 2;
                    else return 0;
                return 1;
            }
            if(A[i]!=EMPTY)
                break;
            if(i%8==0)
                break;
        }
    if(x/8!=y/8 && abs(x-y)%8==0 && (A[y]==EMPTY || Color(A[y])!=Color(A[x])))
    {
        if(x<y)
        {
            for(int i=x+8; i!=y; i=i+8)
                if(A[i]!=EMPTY)
                    return 0;
        }
        if(y<x)
        {
            for(int i=x-8; i!=y; i=i-8)
                if(A[i]!=EMPTY)
                    return 0;
        }
        if(A[y]==EMPTY)
            return 1;
        return 2;
    }
    if(y/8==x/8 && (A[y]==EMPTY || Color(A[y])!=Color(A[x])) )
    {
        if(x<y)
        {
            for(int i=x+1; i!=y; i++)
                if(A[i]!=EMPTY)
                    return 0;
        }
        if(y<x)
        {
            for(int i=x-1; i!=y; i--)
                if(A[i]!=EMPTY)
                    return 0;
        }
        if(A[y]==EMPTY)
            return 1;
        return 2;
    }
    return 0;
}

/*int LogicKING(int x,int y)
{
    if(A[x]!=WHITE_KING && A[x]!=BLACK_KING)
        return 0;
    if(y==x-9 && (x-8)>=0 && (x-9)/8==(x-8)/8
}*/

int replace(int x, int y)
{
    A[y]=A[x];
    A[x]=EMPTY;
}

int* KingMove( int i)
{
    int type = (A[i] >> 1);
    int* p=(int*)malloc(sizeof(int)*64);
    for(int b=0; b<64; b++)
        p[b]=0;
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
            for(int j=0; j<64; j++)
            {
                int a=LogicKNIGHT(i,j);
                if(a!=0 && p[j]!=2)
                    p[j]=a;
            }
            break;
        case TYPE_BISHOP:
            for(int j=0; j<64; j++)
            {
                int a=LogicBISHOP(i,j);
                if(a!=0 && p[j]!=2)
                    p[j]=a;
            }
            break;
        case TYPE_ROOK:
            for(int j=0; j<64; j++)
            {
                int a=LogicROOK(i,j);
                if(a!=0 && p[j]!=2)
                    p[j]=a;
            }
            break;
        case TYPE_QUEEN:
            for(int j=0; j<64; j++)
            {
                int a=LogicQUEEN(i,j);
                if(a!=0 && p[j]!=2)
                    p[j]=a;
            }
            break;
        case TYPE_KING:
            for(int j=0; j<64; j++)
            {
                    p[j]=0;
            }
            break;
        default:
            return 0;
        }
    for(int i=0; i<8; i++)
    {
        printf("\n");
        for(int j=0; j<8; j++)
        {
            printf("%d ",p[i*row+j]);
        }
    }
    printf("\n");
    return p;
}

int Figaro(int x, int y)
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
        if (LogicKNIGHT(x,y))
            replace(x,y);
        else
            return 0;
        break;
    case TYPE_BISHOP:
        if (LogicBISHOP(x,y))
            replace(x,y);
        else
            return 0;
        break;
    case TYPE_ROOK:
        if (LogicROOK(x,y))
            replace(x,y);
        else
            return 0;
        break;
    case TYPE_QUEEN:
        if (LogicQUEEN(x,y))
            replace(x,y);
        else
        break;
    case TYPE_KING:
        break;
    default:
        return 0;
    }
}

int cordfinder(int n, char temp)
{
    return (8-n)*row+(temp-'a');
}
/*
This function returns possible moves for both black and white.
First comes the position, then -1,
then possible moves, then -1 again and repeats further starting from the position
and at the end there is -2 indicating that this is the end.
[pos],[-1],[0 0 0 0 0 0 0 0 , [-1],....,[-2]
            0 0 0 0 0 0 0 0
            0 0 0 0 0 0 0 0
            0 0 0 0 0 0 0 0
            0 0 0 0 0 0 0 0
            0 0 0 0 2 1 2 0
            0 0 0 0 0 0 0 0
            0 0 0 0 0 0 0 0]
on the field of possible moves 1 is a normal move, 2 is a capture of a piece*/
int* Rapsodia()
{
    int Ar[33], k=0;
    for(int i=0;i<64;i++)
        if(A[i]!=EMPTY)
            Ar[k++]=i;
    Ar[k]=-1;
    int* Arr=(int*)malloc(sizeof(int)*k*67+sizeof(int));
    for(int i=0; Ar[i]!=-1; i++)
    {
        Arr[i*67]=Ar[i];
        Arr[i*67+1]=-1;
        int* p=KingMove(Ar[i]);
        for(int j=1;j<65;j++)
        Arr[i*67+j]=p[j-1];
        free(p);
        Arr[i*67+1]=-1;
    }
    Arr[k*67]=-2;
    printf("/nArr:");
    for(int i=0;i<k*67+1;i++)
        printf("%d",Arr[i]);
        return(Arr);
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
        Figaro(startcord,endcord);
        int* p=KingMove(endcord);
        int* k=Rapsodia();
            free(k);
            free(p);
    }
}


int main()
{
    move();
    return 0;
}

