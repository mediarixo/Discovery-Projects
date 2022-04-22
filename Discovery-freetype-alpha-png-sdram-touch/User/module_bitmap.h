#include "tm_stm32_lcd.h"      /* readBMP USED */
#include "module_filters.h"    /* alpha-blending USED */

void readBMP(char* filename)
{
    int i,j,k;
	  uint32_t rgb565;
		
	  unsigned char info[54];
		int row_padded;
		int width, height;
	
		unsigned char* data;
	  unsigned char tmp;
		unsigned int r, g, b;
	
	  unsigned char fg[4];
	  unsigned char bg[4];
	  unsigned char result[4];
	
    FILE* f = fopen(filename, "rb");
    if (f != NULL) {
        
        fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header
    
        // extract image height and width from header
        width = *(int*)&info[18];
        height = *(int*)&info[22];
    
        row_padded = (width*3 + 3) & (~3);
        data = malloc(sizeof(unsigned char)*row_padded);                
        
        for(i = height-1; i >= 0; i--)
        {
            fread(data, sizeof(unsigned char), row_padded, f);
            
            for(j = 0, k = 0; j < width*3; j += 3, k++)
            {
                // Convert (B, G, R) to (R, G, B)
                tmp = data[j];
                data[j] = data[j+2];
                data[j+2] = tmp;
                r = (int)data[j]; 
                g = (int)data[j+1];
                b = (int)data[j+2];
                
							  fg[0] = 0x00; fg[1] = 0x00; fg[2] = 0x00; fg[3] = 0x40; 
                bg[0] = r;    bg[1] = g;    bg[2] = b;    bg[3] = 0xff;                
                alpha_blend(result, fg, bg);
								r = result[0]; g = result[1]; b = result[2];
								
								r = r >> 3; g = g >> 2; b = b >> 3;							
								rgb565 = r << 11 | (g << 5) | b;
								TM_LCD_DrawPixel(k, i, rgb565);								
            }
        }     
        fclose(f);                
        free(data);
    }
}
