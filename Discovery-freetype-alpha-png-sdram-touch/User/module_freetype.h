#include "tm_stm32_stdio.h"
#include "tm_stm32_fatfs.h"	   /* FORK USED */
#include <math.h>								/* matrix USED */
#include "tm_stm32_lcd.h"      /* draw_image USED */
#include "module_filters.h"    /* draw_image USED */

#include <ft2build.h>
#include FT_FREETYPE_H

#define HEIGHT 200
#define WIDTH 200 
/* origin is the upper left corner */
unsigned char image[HEIGHT][WIDTH];
/* ===================================== */
FT_Library library;   /* handle to library */
FT_Face    face;      /* handle to face object */

const char* getErrorMessage(FT_Error err) {
    #undef __FTERRORS_H__
    #define FT_ERRORDEF( e, v, s )  case e: return s;
    #define FT_ERROR_START_LIST     switch (err) {
    #define FT_ERROR_END_LIST       }
    #include FT_ERRORS_H
    return "(Unknown error)";
}

void show_image( void ) {
  int  i, j;

  for ( i = 0; i < HEIGHT; i++ )
  {
    
		for ( j = 0; j < WIDTH; j++ )
      printf("%c", image[i][j] == 0 ? ' ' : image[i][j] < 128 ? '+' : '*' );
    printf("\r\n");
  }
}

void draw_image(uint16_t x, uint16_t y, uint32_t color) {
  int  i, j;
	BYTE red, green, blue;
	unsigned char fg[4];
	unsigned char bg[4];
	unsigned char result[4];
	
	/* rgb = (r << 11) | (g << 5) | b; */
  uint32_t rgb565, bg_rgb565;
	unsigned char r=0, g=0, b=0;
  for ( i = 0; i < HEIGHT; i++ )
  {    
		for ( j = 0; j < WIDTH; j++ ) {				
			  
			  bg_rgb565 = TM_LCD_GetPixel(x+j,y+i);
			
				/* extract rgb component from bg_rgb565 */
			  red = (( bg_rgb565 & 0xF800) >> 11 ) << 3;
				green = (( bg_rgb565 & 0x7E0) >> 5 ) << 2;
				blue = (( bg_rgb565 & 0x1F )) << 3;  
			
			  fg[0] = 0xff; fg[1] = 0xff; fg[2] = 0xff; fg[3] = image[i][j]; 
        bg[0] = red;    bg[1] = green;    bg[2] = blue;    bg[3] = 0xff;                
        alpha_blend(result, fg, bg);
				r = result[0]; g = result[1]; b = result[2];
			  
			  r = r >> 3; g = g >> 2; b = b >> 3;
				rgb565 = r << 11 | (g << 5) | b;
			  
				TM_LCD_DrawPixel(x+j, y+i, rgb565);
			
		}
  }
}

void empty_image() {
  int  i, j;

  for ( i = 0; i < HEIGHT; i++ )
  {
    
		for ( j = 0; j < WIDTH; j++ ) {
			
				image[i][j] = 0;
		}
  }
}

void draw_bitmap( FT_Bitmap* bitmap, FT_Int x, FT_Int y) {
  FT_Int  i, j, p, q;
  FT_Int  x_max = x + bitmap->width;
  FT_Int  y_max = y + bitmap->rows;

  for ( i = x, p = 0; i < x_max; i++, p++ )
  {
    for ( j = y, q = 0; j < y_max; j++, q++ )
    {
      if ( i < 0      || j < 0       ||
           i >= WIDTH || j >= HEIGHT )
        continue;

      image[j][i] |= bitmap->buffer[q * bitmap->width + p];
    }
  }
}

int init_freetype(void) {
	FT_Error   error;
	error = FT_Init_FreeType( &library );
  if (error == FT_Err_Ok) {
      printf("Library Initialization is successful.\r\n");
  } else {
      printf("ERROR0: Failed to initialize Library.\r\n");
		  printf("ERROR REPORTED: %s\r\n",getErrorMessage(error));		
      return (1);
      
  }
  
  error = FT_New_Face ( library, "SD:./font/SegoeWP.ttf", 0, &face );
  if (error == FT_Err_Unknown_File_Format) {
      printf("ERROR: Font format is unsupported.\r\n");      
		  printf("ERROR REPORTED: %s\r\n",getErrorMessage(error));		
      return (1);      
  } else if (error) {
      printf("ERROR1: Font file could not be opened or read (maybe broken).\r\n");
		  printf("ERROR REPORTED: %s\r\n",getErrorMessage(error));		
      return (1);
  } 
	return 0;
}

int draw_freetype(char* str, int num_chars, uint16_t x, uint16_t y, uint32_t color) {
	FT_Error   error;
	FT_GlyphSlot  slot; 
  //char str[] = "abcdefghijklmn";
  int n; //, num_chars = sizeof(str)-1;
	FT_Matrix     matrix;                 /* transformation matrix */
  FT_Vector     pen;                    /* untransformed origin  */
  /*double        angle = 0.0; / ( 25.0 / 360 ) * 3.14159 * 2;      use 25 degrees     */
	double        angle = ( 90.0 / 360 ) * 3.14159 * 2;
	int           target_height = HEIGHT; 
	
	error = FT_Select_Charmap(face , ft_encoding_unicode);
  if (error) {
      printf("ERROR6: An error occurred while selecting charmap.\r\n");
		  printf("ERROR REPORTED: %s\r\n",getErrorMessage(error));		
      return(1);
  }
  
  error = FT_Set_Char_Size ( face, 50*64, 0, 100, 0 );
  if (error) {
      printf("ERROR2: An error occurred while setting the current Pixel Size.\r\n");
		  printf("ERROR REPORTED: %s\r\n",getErrorMessage(error));		
      return(1);
  } 
   
  slot = face->glyph;
    
  /* set up matrix */
  matrix.xx = (FT_Fixed)( cos( angle ) * 0x10000L );
  matrix.xy = (FT_Fixed)(-sin( angle ) * 0x10000L );
  matrix.yx = (FT_Fixed)( sin( angle ) * 0x10000L );
  matrix.yy = (FT_Fixed)( cos( angle ) * 0x10000L );

  /* the pen position in 26.6 cartesian space coordinates; */
  /* start at (300,200) relative to the upper left corner  */
  pen.x = 60*64; /*300 * 64;*/
  pen.y = 20*64; /*( target_height - 200 ) * 64;*/
  
  
  for ( n = 0; n < num_chars; n++ )
  {
    /* set transformation */
    FT_Set_Transform( face, &matrix, &pen );

    /* load glyph image into the slot (erase previous one) */
    error = FT_Load_Char( face, str[n], FT_LOAD_RENDER );
    if ( error ) {
      printf("ERROR20: An error occurred while loading char.\r\n");
		  printf("ERROR REPORTED: %s\r\n",getErrorMessage(error));		
      return (1);
    } 
      /*continue;                  ignore errors */

    /* now, draw to our target surface (convert position) */
    draw_bitmap( &slot->bitmap,
                 slot->bitmap_left,
                 target_height - slot->bitmap_top );        
    
    /* increment pen position */
    pen.x += slot->advance.x;
    pen.y += slot->advance.y;		
  }
	
	/* Display char-based image */
  /* show_image(); */
	draw_image(x, y, color);
	empty_image();
	return 0;
}

int kill_freetype(void) {
	FT_Done_Face    ( face );
  FT_Done_FreeType( library );

  printf("Program exited successfully.\r\n");
	return 0;
}
/* ======================================================================================= */
int test_freetype(void)
{
  FT_Library library;   /* handle to library */
  FT_Face    face;      /* handle to face object */
  FT_Error   error;
  //FT_UInt    glyph_index;  /* font glyth index */
  //FT_ULong   charcode;     /* holds the character code */
  FT_GlyphSlot  slot; 
  char str[] = "alfa";
  int n, num_chars = sizeof(str)-1;
	FT_Matrix     matrix;                 /* transformation matrix */
  FT_Vector     pen;                    /* untransformed origin  */
  double        angle = 0.0; /*( 25.0 / 360 ) * 3.14159 * 2;      use 25 degrees     */
	int           target_height = HEIGHT; 
  
  //charcode = 0x0061;  /* a letter (Unicode) */
  /* go to Control Panel\Aspect\Characters\Arial --> Find a character  */
	
  error = FT_Init_FreeType( &library );
  if (error == FT_Err_Ok) {
      printf("Library Initialization is successful.\r\n");
  } else {
      printf("ERROR0: Failed to initialize Library.\r\n");
		  printf("ERROR REPORTED: %s\r\n",getErrorMessage(error));		
      return (1);
      
  }
  
  error = FT_New_Face ( library, "SD:./SegoeWP.ttf", 0, &face );
  if (error == FT_Err_Unknown_File_Format) {
      printf("ERROR: Font format is unsupported.\r\n");      
		  printf("ERROR REPORTED: %s\r\n",getErrorMessage(error));		
      return (1);
      
  } else if (error) {
      printf("ERROR1: Font file could not be opened or read (maybe broken).\r\n");
		  printf("ERROR REPORTED: %s\r\n",getErrorMessage(error));		
      return (1);
      
  } 
  
  error = FT_Select_Charmap(face , ft_encoding_unicode);
  if (error) {
      printf("ERROR6: An error occurred while selecting charmap.\r\n");
		  printf("ERROR REPORTED: %s\r\n",getErrorMessage(error));		
      return(1);
  }
  
  error = FT_Set_Char_Size ( face, 50*64, 0, 100, 0 );
  if (error) {
      printf("ERROR2: An error occurred while setting the current Pixel Size.\r\n");
		  printf("ERROR REPORTED: %s\r\n",getErrorMessage(error));		
      return(1);
  } 
   
  slot = face->glyph;
  
  
  /* set up matrix */
  matrix.xx = (FT_Fixed)( cos( angle ) * 0x10000L );
  matrix.xy = (FT_Fixed)(-sin( angle ) * 0x10000L );
  matrix.yx = (FT_Fixed)( sin( angle ) * 0x10000L );
  matrix.yy = (FT_Fixed)( cos( angle ) * 0x10000L );

  /* the pen position in 26.6 cartesian space coordinates; */
  /* start at (300,200) relative to the upper left corner  */
  pen.x = 0; /*300 * 64;*/
  pen.y = 20*64; /*( target_height - 200 ) * 64;*/
  
  
  for ( n = 0; n < num_chars; n++ )
  {
    /* set transformation */
    FT_Set_Transform( face, &matrix, &pen );

    /* load glyph image into the slot (erase previous one) */
    error = FT_Load_Char( face, str[n], FT_LOAD_RENDER );
    if ( error ) {
      printf("ERROR20: An error occurred while loading char.\r\n");
		  printf("ERROR REPORTED: %s\r\n",getErrorMessage(error));		
      return (1);
    } 
      /*continue;                  ignore errors */

    /* now, draw to our target surface (convert position) */
    draw_bitmap( &slot->bitmap,
                 slot->bitmap_left,
                 target_height - slot->bitmap_top );
    
    
    
    /* increment pen position */
    pen.x += slot->advance.x;
    pen.y += slot->advance.y;
  }
	
	/* Display char-based image */
  show_image();
	draw_image(0, 0, 0xffff);
	
  FT_Done_Face    ( face );
  FT_Done_FreeType( library );

  printf("Program exited successfully.\r\n");
	return 0;
}

