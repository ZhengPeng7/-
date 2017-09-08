//作者QQ号:1945993626
#include <SDL2/SDL.h>
#include <stdio.h>
#include <time.h>

#define M 15
#define N 15

#define S 3 //STATE
#define J 6 //JOIN_DEGREE //0,1,2,3,4,5 // >=5 视同为=5
#define F 3 //FREE_DEGREE

#define HUMAN 1
#define MACHINE 2

#define DIRECT 4

#define MAX_LEVEL 5 // 1,3,5,7,...

#define MAX_INT 999999999

typedef enum {false, true}bool;

typedef struct _STEP
{
	int x; //0...(M-1)
	int y; //0...(N-1)
} STEP;

typedef struct _BOARD
{
	int step_id;   //0...(M*N-1)
	STEP step[M*N];
	
	int state[M][N]; //0,1,2<==>' ','W','B'
	int power[M][N];
	int situation[S][J][F];

	int dx[4][2];
	int dy[4][2];

	int temp_X;
	int temp_Y;
} BOARD;


int SCREEN_WIDTH=961;
int SCREEN_HEIGHT=961;

SDL_Window * gWindow = NULL;
SDL_Surface * gScreenSurface = NULL;

SDL_Surface * board_surface = NULL;
SDL_Surface * piece_black_surface = NULL;
SDL_Surface * piece_white_surface = NULL;
SDL_Surface * piece_cursor_surface = NULL;
SDL_Rect rect_piece={0,0,64,64}, rect_cursor={0,0,64,64};

void myLog(int state, int x, int y, int r)
{
	FILE * stream;
	if( (stream  = fopen( "log.txt", "a+" )) == NULL )
	{
		printf( "The file 'log.txt' was not opened\n" );
		return;
	}
	if (x<0 || x>=15 || y<0 || y>=15 )fprintf(stream, "\n\n");
	else fprintf(stream,"\n%s %02d %02d %d", (state==1? "W":"B"), x+1, y+1, r);
	fclose( stream );
}

void myPrint(BOARD * board)
{
	int i,j;
/*
	printf("\n");
	for(i=0;i<J;i++)
	{
		for(j=0;j<F;j++)
		{
			printf("%02d ",board->situation[1][i][j]);
		}
		printf(" --- ");
		for(j=0;j<F;j++)
		{
			printf("%02d ",board->situation[2][i][j]);
		}
		printf(" --- ");
				
		printf("\n");
	}
*/
	printf("\n   ");
	for(j=0;j<N;j++)
	{
		printf(" %02d",j);
	}
	printf("\n");
	for(i=0;i<M;i++)
	{
		printf("%02d  ",i);
		for(j=0;j<N;j++)
		{
			printf("%d  ",board->state[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

void power_on(BOARD * board, int x, int y)
{
	int i,j;
	int x1, y1, dx1, dy1;
	int x2, y2, dx2, dy2;

	board->power[x][y]+=4;
	for(i=0;i<DIRECT;i++)
	{
		dx1=board->dx[i][0];
		dy1=board->dy[i][0];
		for(j=0;j<2;j++)
		{
			x1=x+dx1*(j+1);
			y1=y+dy1*(j+1);
			if(x1>=0 && x1<M && y1>=0 && y1<N)
			{
				board->power[x1][y1]+=(2-j);
			}
		}
		
		dx2=board->dx[i][1];
		dy2=board->dy[i][1];
		for(j=0;j<2;j++)
		{
			x2=x+dx2*(j+1);
			y2=y+dy2*(j+1);
			if(x2>=0 && x2<M && y2>=0 && y2<N)
			{
				board->power[x2][y2]+=(2-j);
			}
		}
	}
}

void power_off(BOARD * board, int x, int y)
{
	int i,j;
	int x1, y1, dx1, dy1;
	int x2, y2, dx2, dy2;

	board->power[x][y]-=4;
	for(i=0;i<DIRECT;i++)
	{
		dx1=board->dx[i][0];
		dy1=board->dy[i][0];
		for(j=0;j<2;j++)
		{
			x1=x+dx1*(j+1);
			y1=y+dy1*(j+1);
			if(x1>=0 && x1<M && y1>=0 && y1<N)
			{
				board->power[x1][y1]-=(2-j);
			}
		}
		
		dx2=board->dx[i][1];
		dy2=board->dy[i][1];
		for(j=0;j<2;j++)
		{
			x2=x+dx2*(j+1);
			y2=y+dy2*(j+1);
			if(x2>=0 && x2<M && y2>=0 && y2<N)
			{
				board->power[x2][y2]-=(2-j);
			}
		}
	}
}

//不包含(x,y)
void join_degree(BOARD * board, int x, int y, int dx, int dy, int state, int * join_degree, int * free_degree)
{
	*join_degree=0;
	*free_degree=1;
	
	while(1==1)
	{
		x=x+dx;
		y=y+dy;
		if(x<0 || x>=M || y<0 || y>=N)
		{
			break;
		}
		if(board->state[x][y]!=state)
		{
			if(board->state[x][y]==0)
			{
				*free_degree=2;
			}
			break;
		}
		*join_degree=*join_degree+1;
	}
	if(*join_degree>5)
	{
		*join_degree=5;
	}
}

int adjusted(int a)
{
	if(a>5)
	{
		return 5;
	}
	else
	{
		return a;
	}
}

void put_down(BOARD * board, int x, int y, int state)
{
	int i;
	int x1, y1, dx1, dy1, state1;
	int x2, y2, dx2, dy2, state2;
	int join_degree1, free_degree1;
	int join_degree2, free_degree2;

	x=x%M;
	y=y%N;
	if(board->state[x][y]!=0)
	{
		return;
	}
	if(state==0)
	{
		return;
	}
	
	board->state[x][y]=state;
	board->step[board->step_id].x=x;
	board->step[board->step_id].y=y;
	board->step_id++;
	
	power_on(board, x, y);
	
	//situation adjust
	for(i=0;i<DIRECT;i++)
	{
		dx1=board->dx[i][0];
		dy1=board->dy[i][0];
		x1=x+dx1;
		y1=y+dy1;
		if(x1<0 || x1>=M || y1<0 || y1>=N)
		{
			state1=3;
		}
		else
		{
			state1=board->state[x1][y1];
			if(state1==1 || state1==2)
			{
				join_degree(board, x, y, dx1, dy1, state1, &join_degree1, &free_degree1);
				board->situation[state1][join_degree1][free_degree1]--;
			}
		}

		dx2=board->dx[i][1];
		dy2=board->dy[i][1];
		x2=x+dx2;
		y2=y+dy2;
		if(x2<0 || x2>=M || y2<0 || y2>=N)
		{
			state2=3;
		}
		else
		{
			state2=board->state[x2][y2];
			if(state2==1 || state2==2)
			{
				join_degree(board, x, y, dx2, dy2, state2, &join_degree2, &free_degree2);
				board->situation[state2][join_degree2][free_degree2]--;
			}
		}

		if(state1==state2)
		{
			if(state1==0)
			{
				board->situation[state][1][2]++;
			}
			else if(state1==3)
			{
				board->situation[state][1][0]++;
			}
			else //if(state1==1 || state1==2)
			{
				if(state!=state1)
				{
					board->situation[state][1][0]++;
				}
				else
				{
					board->situation[state][adjusted(join_degree1+join_degree2+1)][free_degree1+free_degree2-2]++;
				}
			}
		}
		else //(state1!=state2)
		{
			if(state1==0)
			{
				if(state2==3)
				{
					board->situation[state][1][1]++;
				}
				else
				{
					if(state==state2)
					{
						board->situation[state2][adjusted(join_degree2+1)][free_degree2]++;
					}
					else
					{
						board->situation[state2][join_degree2][free_degree2-1]++;
						board->situation[state][1][1]++;
					}
				}
			}
			else if(state2==0)
			{
				if(state1==3)
				{
					board->situation[state][1][1]++;
				}
				else
				{
					if(state==state1)
					{
						board->situation[state1][adjusted(join_degree1+1)][free_degree1]++;
					}
					else
					{
						board->situation[state1][join_degree1][free_degree1-1]++;
						board->situation[state][1][1]++;
					}
				}
			}
			else if(state1==3 && state2!=0)
			{
				if(state==state2)
				{
					board->situation[state2][adjusted(join_degree2+1)][free_degree2-1]++;
				}
				else
				{
					board->situation[state2][join_degree2][free_degree2-1]++;
					board->situation[state][1][0]++;
				}
			}
			else if(state2==3 && state1!=0)
			{
				if(state==state1)
				{
					board->situation[state1][adjusted(join_degree1+1)][free_degree1-1]++;
				}
				else
				{
					board->situation[state1][join_degree1][free_degree1-1]++;
					board->situation[state][1][0]++;
				}
			}
			else //(state1==1 && state2==2) || (state1==2 && state2==1)
			{
				if(state==state2)
				{
					board->situation[state2][adjusted(join_degree2+1)][free_degree2-1]++;
					board->situation[state1][join_degree1][free_degree1-1]++;
				}
				else
				{
					board->situation[state2][join_degree2][free_degree2-1]++;
					board->situation[state1][adjusted(join_degree1+1)][free_degree1-1]++;
				}
			}
		}
	}
}

void take_up(BOARD * board)
{
	int i,x,y,state;
	int x1, y1, dx1, dy1, state1;
	int x2, y2, dx2, dy2, state2;
	int join_degree1, free_degree1;
	int join_degree2, free_degree2;

	if(board->step_id==0)
	{
		return;
	}
	board->step_id--;
	x=board->step[board->step_id].x;
	y=board->step[board->step_id].y;
	state=board->state[x][y];
	if(state==0)
	{
		return;
	}
	board->state[x][y]=0;
	power_off(board, x, y);

	//situation_adjust
	for(i=0;i<DIRECT;i++)
	{
		dx1=board->dx[i][0];
		dy1=board->dy[i][0];
		x1=x+dx1;
		y1=y+dy1;
		if(x1<0 || x1>=M || y1<0 || y1>=N)
		{
			state1=3;
		}
		else
		{
			state1=board->state[x1][y1];
			if(state1==1 || state1==2)
			{
				join_degree(board, x, y, dx1, dy1, state1, &join_degree1, &free_degree1);
				board->situation[state1][join_degree1][free_degree1]++;
			}
		}

		dx2=board->dx[i][1];
		dy2=board->dy[i][1];
		x2=x+dx2;
		y2=y+dy2;
		if(x2<0 || x2>=M || y2<0 || y2>=N)
		{
			state2=3;
		}
		else
		{
			state2=board->state[x2][y2];
			if(state2==1 || state2==2)
			{
				join_degree(board, x, y, dx2, dy2, state2, &join_degree2, &free_degree2);
				board->situation[state2][join_degree2][free_degree2]++;
			}
		}

		if(state1==state2)
		{
			if(state1==0)
			{
				board->situation[state][1][2]--;
			}
			else if(state1==3)
			{
				board->situation[state][1][0]--;
			}
			else //if(state1==1 || state1==2)
			{
				if(state!=state1)
				{
					board->situation[state][1][0]--;
				}
				else
				{
					board->situation[state][adjusted(join_degree1+join_degree2+1)][free_degree1+free_degree2-2]--;
				}
			}
		}
		else //(state1!=state2)
		{
			if(state1==0)
			{
				if(state2==3)
				{
					board->situation[state][1][1]--;
				}
				else
				{
					if(state==state2)
					{
						board->situation[state2][adjusted(join_degree2+1)][free_degree2]--;
					}
					else
					{
						board->situation[state2][join_degree2][free_degree2-1]--;
						board->situation[state][1][1]--;
					}
				}
			}
			else if(state2==0)
			{
				if(state1==3)
				{
					board->situation[state][1][1]--;
				}
				else
				{
					if(state==state1)
					{
						board->situation[state1][adjusted(join_degree1+1)][free_degree1]--;
					}
					else
					{
						board->situation[state1][join_degree1][free_degree1-1]--;
						board->situation[state][1][1]--;
					}
				}
			}
			else if(state1==3 && state2!=0)
			{
				if(state==state2)
				{
					board->situation[state2][adjusted(join_degree2+1)][free_degree2-1]--;
				}
				else
				{
					board->situation[state2][join_degree2][free_degree2-1]--;
					board->situation[state][1][0]--;
				}
			}
			else if(state2==3 && state1!=0)
			{
				if(state==state1)
				{
					board->situation[state1][adjusted(join_degree1+1)][free_degree1-1]--;
				}
				else
				{
					board->situation[state1][join_degree1][free_degree1-1]--;
					board->situation[state][1][0]--;
				}
			}
			else //(state1==1 && state2==2) || (state1==2 && state2==1)
			{
				if(state==state2)
				{
					board->situation[state2][adjusted(join_degree2+1)][free_degree2-1]--;
					board->situation[state1][join_degree1][free_degree1-1]--;
				}
				else
				{
					board->situation[state2][join_degree2][free_degree2-1]--;
					board->situation[state1][adjusted(join_degree1+1)][free_degree1-1]--;
				}
			}
		}
	}
}

/*
int evaluate(BOARD * board, int state, int level, int alpha, int beta)
{
	int i,j,self,enemy,t,a,b;

	self=state;
	enemy=(state)%2+1;
	
	for(i=0;i<M;i++)
	{
		for(j=0;j<N;j++)
		{
			if(board->state[i][j]==0 && board->power[i][j]>=2)
			{
				put_down(board, i, j, self);
				if(board->situation[self][5][2]>0 || board->situation[self][5][1]>0 || board->situation[self][5][0]>0)
				{
					t=(level%2==0 ? -MAX_INT+level : +MAX_INT-level);
				}
				else
				if(board->situation[enemy][4][2]>0 || board->situation[enemy][4][1]>0) //use or not use?
				{
					t=(level%2==0 ? +MAX_INT-level : -MAX_INT+level);
				}
				else
				{
					if((level+1)==MAX_LEVEL)
					{
						//评估level+1局势(not this level)
						t=0;
						for(a=1;a<J-1;a++)
						{
							for(b=1;b<F;b++)
							{
								t=t+(((board->situation[enemy][a][b] - board->situation[self][a][b])*a*b)<<(3*(a+b-2)));
							}
						}
					}
					else
					{
						t=evaluate(board, enemy, level+1, alpha, beta);
					}
				}
				take_up(board);

				if(level%2==0)
				{
					if(beta>t) //min
					{
						beta=t;
						if (level == 0) //get the best x&y
						{
							board->temp_X=i; //x坐标
							board->temp_Y=j; //y坐标
						}
					}
					if(t<=alpha)
					{
						return t;
					}
				}
				else
				{
					if(alpha<t) //max
					{
						alpha=t;
					}
					if(t>=beta)
					{
						return t;
					}
				}
			}
		}
	}
	
	if(level%2==0)
	{
		return beta;
	}
	else
	{
		return alpha;
	}
}
*/

/*
function alphabeta(node, depth, α, β, Player)         
    if  depth = 0 or node is a terminal node
        return the heuristic value of node
    if  Player = MaxPlayer // 极大节点
        for each child of node // 极小节点
            α := max(α, alphabeta(child, depth-1, α, β, not(Player) ))   
            if β ≤ α // 该极大节点的值>=α>=β，该极大节点后面的搜索到的值肯定会大于β，因此不会被其上层的极小节点所选用了。对于根节点，β为正无穷
                break                             (* Beta cut-off *)
        return α
    else // 极小节点
        for each child of node // 极大节点
            β := min(β, alphabeta(child, depth-1, α, β, not(Player) )) // 极小节点
            if β ≤ α // 该极大节点的值<=β<=α，该极小节点后面的搜索到的值肯定会小于α，因此不会被其上层的极大节点所选用了。对于根节点，α为负无穷
                break                             (* Alpha cut-off *)
        return β 
(* Initial call *)
alphabeta(origin, depth, -infinity, +infinity, MaxPlayer)
*/

//轮到 state 落子, level=0, alpha=-MAX_INT, beta=MAX_INT
int evaluate(BOARD * board, int state, int level, int alpha, int beta)
{
	int i,j,self,enemy,t,a,b;

	self=state;
	enemy=(state)%2+1;
	
	if(level==MAX_LEVEL)
	{
		t=0;
		if(level%2==0)
		{
			for(a=1;a<J-1;a++)
			{
				for(b=1;b<F;b++)
				{
					t=t+(((board->situation[self][a][b] - board->situation[enemy][a][b])*a*b)<<(3*(a+b-2)));
				}
			}
		}
		else
		{
			for(a=1;a<J-1;a++)
			{
				for(b=1;b<F;b++)
				{
					t=t+(((board->situation[enemy][a][b] - board->situation[self][a][b])*a*b)<<(3*(a+b-2)));
				}
			}
		}
		return t;
	}
	

	if(board->situation[enemy][5][2]>0 || board->situation[enemy][5][1]>0 || board->situation[enemy][5][0]>0)
	{
		return (level%2==0 ? -MAX_INT : +MAX_INT);
	}
	else
	if(board->situation[self][4][2]>0 || board->situation[self][4][1]>0) //必胜
	{
		if(level>0) return (level%2==0 ? +MAX_INT : -MAX_INT);
		//else 需要利用后续程序计算temp_X, temp_Y坐标
	}


	if(level%2==0)
	{
		for(i=0;i<M;i++)
		{
			for(j=0;j<N;j++)
			{
				if(board->state[i][j]==0 && board->power[i][j]>=2)
				{
					put_down(board, i, j, self);
					t=evaluate(board, enemy, level+1, alpha, beta);
					if(t>alpha)
					{
						alpha=t;
						if (level==0)
						{
							board->temp_X=i; //x坐标
							board->temp_Y=j; //y坐标
						}
					}
					take_up(board);
					if(beta<=alpha)
					{
						i=M;
						break;
					}
				}
			}
		}

		return alpha;
	}
	else
	{
		for(i=0;i<M;i++)
		{
			for(j=0;j<N;j++)
			{
				if(board->state[i][j]==0 && board->power[i][j]>=2)
				{
					put_down(board, i, j, self);
					t=evaluate(board, enemy, level+1, alpha, beta);
					if(t<beta)beta=t;
					take_up(board);
					if(beta<=alpha)
					{
						i=M;
						break;
					}
				}
			}
		}
		return beta;
	}
}


void board_init(BOARD * board)
{
	int i,j,k;

	board->step_id=0;
	
	for(i=0;i<M;i++)
	{
		for(j=0;j<N;j++)
		{
			board->state[i][j]=0;
			board->power[i][j]=0;
		}
	}

	for(i=0;i<S;i++)
	{
		for(j=0;j<J;j++)
		{
			for(k=0;k<F;k++)
			{
				board->situation[i][j][k]=0;
			}
		}
	}

	//direction
	//(-1,1) (0,1) (1,1)
	//(-1,0) (0,0) (1,0)
	//(-1,-1)(0,-1)(1,-1)
	board->dx[0][0]=-1;
	board->dy[0][0]=0;
	board->dx[0][1]=1;
	board->dy[0][1]=0;

	board->dx[1][0]=0;
	board->dy[1][0]=-1;
	board->dx[1][1]=0;
	board->dy[1][1]=1;

	board->dx[2][0]=-1;
	board->dy[2][0]=1;
	board->dx[2][1]=1;
	board->dy[2][1]=-1;

	board->dx[3][0]=-1;
	board->dy[3][0]=-1;
	board->dx[3][1]=1;
	board->dy[3][1]=1;
}

void board_display(BOARD * board)
{
	SDL_BlitSurface( board_surface, NULL, gScreenSurface, NULL );
	for(int i=0;i<M;i++)
	{
		for(int j=0;j<N;j++)
		{
			rect_piece.x=i*64;
			rect_piece.y=j*64;
			if(board->state[i][j]==2)SDL_BlitSurface( piece_black_surface, NULL, gScreenSurface, &rect_piece );
			else if(board->state[i][j]==1)SDL_BlitSurface( piece_white_surface, NULL, gScreenSurface, &rect_piece );
		}
	}
}

SDL_Surface * loadSurface(char *path )
{
	SDL_Surface* optimizedSurface = NULL;

	SDL_Surface* loadedSurface = SDL_LoadBMP( path);
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path, SDL_GetError() );
		return NULL;
	}
	optimizedSurface = SDL_ConvertSurface( loadedSurface, gScreenSurface->format, 0 );
	if( optimizedSurface == NULL )
	{
		printf( "Unable to optimize image %s! SDL Error: %s\n", path, SDL_GetError() );
		return NULL;
	}
	SDL_FreeSurface( loadedSurface );
	SDL_SetColorKey( optimizedSurface, SDL_TRUE, SDL_MapRGB( optimizedSurface->format, 0, 255, 0 ) ); //transparent color
	return optimizedSurface;
}

bool init()
{
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		return false;
	}

	gWindow = SDL_CreateWindow( "WZQ", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
	if( gWindow == NULL )
	{
		printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
		return false;
	}
	gScreenSurface = SDL_GetWindowSurface( gWindow );
	board_surface=loadSurface( "board.bmp" );
	piece_black_surface=loadSurface( "piece_black.bmp" );
	piece_white_surface=loadSurface( "piece_white.bmp" );
	piece_cursor_surface=loadSurface( "piece_black.bmp" );
	SDL_SetSurfaceAlphaMod(piece_cursor_surface, 64);
	SDL_SetSurfaceBlendMode(piece_cursor_surface, SDL_BLENDMODE_BLEND);

	return true;
}

void close()
{
	if(board_surface!=NULL)
	{
		SDL_FreeSurface( board_surface );
		board_surface = NULL;
	}

	if(piece_black_surface!=NULL)
	{
		SDL_FreeSurface( piece_black_surface );
		piece_black_surface = NULL;
	}

	if(piece_white_surface!=NULL)
	{
		SDL_FreeSurface( piece_white_surface );
		piece_white_surface = NULL;
	}

	if(piece_cursor_surface!=NULL)
	{
		SDL_FreeSurface( piece_cursor_surface );
		piece_cursor_surface = NULL;
	}

	SDL_DestroyWindow( gWindow );
	gWindow = NULL;

	SDL_Quit();
}

int main(int argc, char *argv[])
{
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		int first,x,y,r=0;
		time_t tm;
		BOARD board;
		board_init(&board);
		myLog(-1, -1, -1, -1);

		bool quit = false, win=false;
		SDL_Event e;
		while( !quit )
		{
			while( SDL_PollEvent( &e ) != 0 )
			{
				if( e.type == SDL_QUIT )
				{
					quit = true;
				}
				else 
				if(e.type ==SDL_MOUSEBUTTONUP)
				{
					if(e.button.button==1)
					{
						if(e.button.y>5)
						{
							if(win==false && board.step_id%2==1)
							{
								x=e.button.x/64, y=e.button.y/64;
								put_down(&board, x, y, 2);
								myLog(2, x, y, 0);

								board_display(&board);
								SDL_UpdateWindowSurface( gWindow );

								if(board.situation[1][5][2]>0 || board.situation[1][5][1]>0 || board.situation[1][5][0]>0)
								{
									printf("white win");
									win = true;
								}
								if(board.situation[2][5][2]>0 || board.situation[2][5][1]>0 || board.situation[2][5][0]>0)
								{
									printf("black win");
									win = true;
								}
							}
						}
					}
				}
				else
				if(e.type ==SDL_MOUSEMOTION)
				{
					rect_cursor.x=e.motion.x-32;
					rect_cursor.y=e.motion.y-32;
				}
			}
			if(win ==false && board.step_id%2==0)
			{
				tm = time(0);
				if(board.step_id==0)
				{
					x=M/2;
					y=N/2;
				}
				else
				{
					board.temp_X=-1;
					r=evaluate(&board, 1, 0, -MAX_INT, MAX_INT);
					x=board.temp_X;
					y=board.temp_Y;
				}
				if(x==-1)
				{
					printf("you win");
					win = true;
				}
				else
				{
					put_down(&board, x, y, 1);
					printf("row=%d, col=%d, machine situation:%d, elapsed time:%d\n", x, y, r, (int)(time(0) - tm));
					myLog(1, x, y, r);

					//board_display(&board);

					if(board.situation[1][5][2]>0 || board.situation[1][5][1]>0 || board.situation[1][5][0]>0)
					{
						printf("white win");
						win = true;
					}
					if(board.situation[2][5][2]>0 || board.situation[2][5][1]>0 || board.situation[2][5][0]>0)
					{
						printf("black win");
						win = true;
					}
				}
			}

			board_display(&board);
			SDL_BlitSurface( piece_cursor_surface, NULL, gScreenSurface, &rect_cursor );
			SDL_UpdateWindowSurface( gWindow );
		}
	}
	close();
	return 0;
}
