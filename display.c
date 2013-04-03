/******************************************
	display.c: Display subsystem
	by ghost<ghost89413@gmail.com>
		2010.04.19
*******************************************/
#define __DISPLAY_IMP
#include "display.h"
#include "codeset.h"
char* font_path;
SDL_Thread *render_thread;
unsigned color[2];
void     set_point( SDL_Surface* disp , int x , int y , unsigned color )
{
	unsigned* buf = (unsigned*)disp->pixels;
	buf[  y  * disp->pitch / 4 +  x  ] = color;
}
unsigned get_point( SDL_Surface* disp , int x , int y  )
{
	unsigned* buf = (unsigned*)disp->pixels;
	return buf[  y  * disp->pitch / 4 +  x  ] ;
}
void draw_rect( SDL_Surface* screen , int x , int y , int w , int h , unsigned color )
{
	SDL_Rect rect = { x , y , w , h };
	SDL_FillRect( screen , &rect , color );
}
void render_text()
{
	int x , y;
	for( y = 0 ; y < TEXT_H ; y ++ )
		for( x = 0 ; x < TEXT_W ; x ++ )
		{
			uchar ch[3] = { get_text( x , y ) , '\0' , '\0' };
			if( (ch[0] &80) && x + 1 < TEXT_W ) ch[1] = get_text( x + 1  , y);
			if( ch[0] )
			{
				uchar p[10];
				code_convert( "gbk" , "utf8" , (char*)ch , strlen((char*)ch) , (char*)p , 10 );
				SDL_Color text_color = { 0 , 0 , 0 };
				/*#warning("Debug code HERE")
				printf("put %s\n",p);*/
				SDL_Surface* temp = TTF_RenderUTF8_Solid( font , (char*)p , text_color );
				SDL_Rect rect = { x * 8 * PIXEL_SIZE , y * 16 * PIXEL_SIZE };
				draw_rect( screen , rect.x , rect.y , 8 * PIXEL_SIZE , 16 * PIXEL_SIZE , color[0] );
				SDL_BlitSurface( temp , NULL , screen , &rect );
				SDL_FreeSurface(temp);
			}
			if( ch[1] ) x++;
		}
}
void render_graph()
{
	int x , y;
	for( y = 0 ; y < DISP_H  ; y ++ )
		for( x = 0 ; x < DISP_W ; x ++ )
			if( get_pixel( x , y ) )
			{
				int dx,dy;
				for(dx = 0 ; dx < PIXEL_SIZE ; dx ++ )
					for( dy = 0 ; dy < PIXEL_SIZE ; dy ++ )
						set_point( screen , x * PIXEL_SIZE + dx , y * PIXEL_SIZE + dy , color[ get_point( screen , x* PIXEL_SIZE +dx, y* PIXEL_SIZE+dy ) != color[1] ] );

			}	
	}
void render_curs()
{
	int x , y ;
	int dx,dy;
	int c = *memoryP( CUR_S );
	x = ram[ CUR_X ] * PIXEL_SIZE * 8;
	y = ram[ CUR_Y ] * PIXEL_SIZE * 16;
	memoryV(); 
	if( c == 0 ) return;
	for(dx = 0 ; dx < PIXEL_SIZE * 8; dx ++ )
		for( dy = 0 ; dy < PIXEL_SIZE * 16 ; dy ++ )
			set_point( screen , x + dx , y + dy , color[ get_point( screen , x+dx , y+dy ) == color[0] ] );
	 
}
int  render()
{	
	int count = 0;
	while(1)
	{
		usleep(10000);
		//SDL_Delay(40);
		count += 10000;
		if( count > 300000 )
		{
			uchar* cur_s = memoryP( CUR_S );
			if( *cur_s != 2 )
				*cur_s =  !*cur_s;
			count = 0;
			memoryV();
		}
		draw_rect( screen , 0 , 0 , DISP_W * PIXEL_SIZE , DISP_H *PIXEL_SIZE , color[0] );
		if( * memoryP( CUR_S ) == 2 ) /*Graph Mode*/
		{
			memoryV();
			render_text();
			render_graph();
		}
		else
		{
			memoryV();
			render_graph();
			render_text();
			render_curs();
		}
		
		SDL_Flip(screen);
		
		
	}
}
void display_init()
{
	if( SDL_Init(SDL_INIT_VIDEO) < 0 )
	{
		perror("SDL");
		exit(1);
	}
	screen = SDL_SetVideoMode( DISP_W * PIXEL_SIZE , DISP_H * PIXEL_SIZE , 32 , SDL_SWSURFACE );
	if( screen == NULL )
	{
		perror("SDL");
		exit(1);
	}
	TTF_Init();
	font  = TTF_OpenFont(font_path , 65);
	color[0] = SDL_MapRGB( screen->format , 127 , 127 , 127 );
	color[1] = SDL_MapRGB( screen->format , 0 , 0 , 0 );
	render_thread = SDL_CreateThread( render , NULL ); 
}

#ifdef DISPLAY_TEST
#include "io.h"
int main()
{
	memory_init();
	display_init();
	io_init();
	*memoryP(CUR_S) = 2;
	memoryV();
	set_text( 0 , 0 , 'F' );
	set_pixel( 50 , 50 , 1 );
	set_pixel( 30 , 50 , 1 );
	rect( 0 , 0 , 30 , 30 , 1 , 1 );
	ellipse( 30,30 , 30 , 30 , 1 , 1 );
	while(1)
	{
		//usleep(100000);
		//printf("%d\n", inkey() );
		
		put_char( inkey() );
		line( 0,0,50,0,1);
	}
	/*sleep(10);
	printf("%d\n",get_pixel(0,0));*/
	return 0;
}
#endif
