#ifndef MODULE_SDRAM_H
#define MODULE_SDRAM_H

#include "tm_stm32_lcd.h"      /* drawBMP USED */
#include "module_filters.h"    /* alpha-blending USED */
#include "tm_stm32_sdram.h"    /* readBMP USED */


int width = 480, height = 272; /* keep attention not to swap width and height ( emptyBuffer fails ).*/
#define AVD_OFS 4177920 /* using sdram offset to avoid dirty lcd memory/screen. */

void fillScreen(uint32_t rgb565) {
    int i,j,k;	
    for(i = 0; i<height; i++) {
			for(j = 0, k = 0; j < width*3; j += 3, k++) {
							/* Write data to LCD screen */	            
							TM_LCD_DrawPixel(k, i, rgb565);							
      }
    }             
}

void readBuffer() {
    int i,j,k;
	  uint32_t rgb565;		      
    for(i = 0; i<height; i++) {
			for(j = 0, k = 0; j < width*3; j += 3, k++) {
							/* Read data from SDRAM */
	            rgb565 = TM_SDRAM_Read32(AVD_OFS + (4 * (k + width*i)));
							TM_LCD_DrawPixel(k, i, rgb565);							
      }
    }             
}

void writeBuffer() {
	int i,j,k;
	uint32_t rgb565;		 
	for(i = 0; i<height; i++) {            
		for(j = 0, k = 0; j < width*3; j += 3, k++) {                
				rgb565 = TM_LCD_GetPixel(k, i);			
				/* Write to SDRAM */
				TM_SDRAM_Write32(AVD_OFS + (4 * (k + width*i)), rgb565);								
    }
  }     
}

#endif
