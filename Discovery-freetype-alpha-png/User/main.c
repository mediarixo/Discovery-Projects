/**
 * Keil project for STDIO output & input
 *
 * Before you start, select your target, on the right of the "Load" button
 *
 * @author    Tilen Majerle
 * @email     tilen@majerle.eu
 * @website   http://stm32f4-discovery.com
 * @ide       Keil uVision 5
 * @conf      PLL parameters are set in "Options for Target" -> "C/C++" -> "Defines"
 * @packs     STM32F4xx/STM32F7xx Keil packs are requred with HAL driver support
 * @stdperiph STM32F4xx/STM32F7xx HAL drivers required
 */
/* Include core modules */
#include "stm32fxxx_hal.h"
/* Include my libraries here */
#include "defines.h"
#include "tm_stm32_disco.h"
#include "tm_stm32_delay.h"
#include "tm_stm32_usart.h"

FILE USART6_Stream;

/* Output stream for USART3 function references */
int USART6_Stream_OutputFunction(int ch, FILE* f);
/* Input stream for USART6 function references */
int USART6_Stream_InputFunction(FILE* f);
#include "tm_stm32_stdio.h"
//-------------------
#include "tm_stm32_fatfs.h"
//#include "tm_stm32_lcd.h"
#include "stdio.h"
/* Fatfs structure */
FATFS FS;

//-------------------
#include "module_freetype.h"
#include "tm_stm32_lcd.h"
int calc_h, calc_w;
//-------------------
#include "module_bitmap.h"
#include "module_png.h"

int main(void) {
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	/* Init leds */
	TM_DISCO_LedInit();
	
	/* ------------------------------------------- */
	/* Init LCD */
	TM_LCD_Init();

	/* buggy func */
//	/* Fill LCD with color */
//	TM_LCD_Fill(0xffff);
	
	/* ------------------------------------------- */
	/* Init USART, TX: PC6, RX: PC7, 921600 bauds  */
	TM_USART_Init(USART6, TM_USART_PinsPack_1, 921600);
	
	/* Add output function for USART6_Stream */
	TM_STDIO_SetOutputFunction(&USART6_Stream, USART6_Stream_OutputFunction);
	/* Add input function for USART6_Stream */
	TM_STDIO_SetInputFunction(&USART6_Stream, USART6_Stream_InputFunction);
	
	/* Print something on USART6 
	printf("\r\nTesting Freetype:\r\n"); */
	/* Print something on USART6 
	fprintf(&USART6_Stream, "Hello USART6 user (via fprintf)\r\n");*/
	
	/* -------------------------------------------- */
	/* Try to mount card */
	if (f_mount(&FS, "SD:", 1) == FR_OK) {
		
		printf("\r\nTesting Freetype started.\r\n");
// 		calc_w = TM_LCD_GetWidth() / 2;	/* 480 x 272 px */
//		calc_h = TM_LCD_GetHeight() / 2;			
		
		readBMP("./img/image3.bmp");
		
		
		if (init_freetype() == 0) {
			
			// loading png 48 by 48 pixels at 24 bit color depth
			 decodeOneStep("./img/android.png", 10, 10);
			 decodeOneStep("./img/amazon.png", 68, 10);
			 decodeOneStep("./img/behance.png", 126, 10);
			 decodeOneStep("./img/bing.png", 10, 68);
			 decodeOneStep("./img/box.png", 68, 68);
			 decodeOneStep("./img/buffer.png", 126, 68);
			 decodeOneStep("./img/delicious.png", 10, 126);
			 decodeOneStep("./img/dropbox.png", 68, 126);
			 decodeOneStep("./img/facebook.png", 126, 126);
			
//			if (draw_freetype("18:45", 5, calc_w-100, calc_h-100, 0xffff) == 1)
//				printf("\r\nError in reading/drawing routine.\r\n");
			
			
			
			if (kill_freetype() == 0)
				TM_DISCO_LedOn(LED_ALL);
		}
		
		
//		if (test_freetype() == 0) {
//		/* Turn led ON if fclose is successfull */
//		TM_DISCO_LedOn(LED_ALL);	
//		} else {
//			printf("test_freetype failed.\r\n");
//		}

		printf("\r\nTesting Freetype ended.\r\n");
		/* Unmount SDCARD */
		f_mount(NULL, "SD:", 1);
	}
	/* -------------------------------------------- */
	
	while (1) {
	}
}

/* Handle stdout actions */
int TM_STDIO_StdoutHandler(int ch, FILE* f) {
	/* Send data to USART1 */
	TM_USART_Putc(USART6, (char) ch);
	
	/* Return ch, it means OK */
	return ch;
	/* If you want to return error, then you have to send EOF (-1) */
	//return -1;
}

/* Handle stdin actions */
int TM_STDIO_StdinHandler(FILE* f) {
	/* If any data at USART, return them */
	/* Do your custom implementation here when string ends */
	if (!TM_USART_BufferEmpty(USART6)) {
		return (int)TM_USART_Getc(USART6);
	}

	/* End of data, string is valid */
	/* You have to send -1 at the end of string */
	return -1;
}

/* USART6 output stream handler = custom function name, linked with USART6 
   stream in the beginning of main() function */
int USART6_Stream_OutputFunction(int ch, FILE* f) {
	/* Send char via USART6 */
	TM_USART_Putc(USART6, (char) ch);
	
	/* Return ch, it means OK */
	return ch;
	/* If you want to return error, then you have to send EOF (-1) */
	//return -1;
}

/* Handle USART6 stream input = custom function name, linked with USART6 stream in the beginning of main() function */
int USART6_Stream_InputFunction(FILE* f) {
	/* If any data at USART, return them */
	/* Do your custom implementation here when string ends */
	if (!TM_USART_BufferEmpty(USART6)) {
		return (int)TM_USART_Getc(USART6);
	}
	
	/* End of data, string is valid */
	/* You have to send -1 at the end of string */
	return -1;
}

/* ----------------------------------------------------------------------------------
 * ----------------------------------------------------------------------------------
 * Following is a reworked code for STM32F7
 * - adding fopen, fclose, fread i/o functions implementation
 *
 */ 

void use_uart_debug(FRESULT fres) {
	switch (fres) {
		case FR_OK:             printf("debug0: The function succeeded.\r\n"); break;
		case FR_DISK_ERR:       printf("debug1: An unrecoverable hard error occured in the lower layer.\r\n"); break;
		case FR_INT_ERR:        printf("debug2: Work area (file system object, file object or etc...) has been broken by stack overflow or any other tasks.\r\n");
		                        printf("debug2: There is any error of the FAT structure on the volume.\r\n"); break;
		case FR_NOT_READY:      printf("debug3: The storage device cannot work due to a failure of disk_initialize function due to no medium or any other reason.\r\n"); break;
		case FR_NO_FILE:        printf("debug4: Could not find the file.\r\n"); break;
		case FR_NO_PATH:        printf("debug5: Could not find the path.\r\n"); break;
		case FR_INVALID_NAME:   printf("debug6: The given string is invalid as the path name.\r\n"); break;
		case FR_DENIED:         printf("debug7: The required access was denied.\r\n"); break;
		case FR_EXIST:          printf("debug8: Name collision. Any object that has the same name is already existing.\r\n"); break;
		case FR_INVALID_OBJECT: printf("debug9: The file/directory object is invalid or a null pointer is given.\r\n"); break;
		case FR_WRITE_PROTECTED: printf("debug10: Any write mode operation against the write-protected media.\r\n"); break;
		case FR_INVALID_DRIVE:  printf("debug11: Invalid drive number is specified in the path name.\r\n"); break;
		case FR_NOT_ENABLED:    printf("debug12: Work area for the logical drive has not been registered by f_mount function.\r\n"); break;
		case FR_NO_FILESYSTEM:  printf("debug13: There is no valid FAT volume on the drive.\r\n"); break;
		case FR_MKFS_ABORTED:   printf("debug14: The f_mkfs function aborted before start.\r\n"); break;
		case FR_TIMEOUT:        printf("debug15: The function was canceled due to a timeout of thread-safe control.\r\n"); break;
		case FR_LOCKED:         printf("debug16: The operation to the object was rejected by file sharing control.\r\n"); break;
		case FR_NOT_ENOUGH_CORE:     printf("debug17: Not enough memory for the operation.\r\n"); break;
		case FR_TOO_MANY_OPEN_FILES: printf("debug18: Number of open objects has been reached maximum value and no more object can be opened.\r\n"); break;
		case FR_INVALID_PARAMETER:   printf("debug19: The given parameter is invalid or there is any inconsistent.\r\n"); break;
		default:
			printf("debug20: The return code is not handled.\r\n");
			break;
	}	
}

/* Handle std_open actions */
FIL* TM_STDIO_FileOpenHandler(const char * filename, const char * mode) {	
	FIL* fi;
	FRESULT fres;
	BYTE fmode;
	
	fi = (FIL*) malloc(sizeof(FIL));
	if (fi != NULL) {
		if (strcmp (mode,"rb") == 0) {   
			fmode = FA_OPEN_ALWAYS | FA_READ;	
		}
		if (strcmp (mode,"wb") == 0) {	 
			fmode = FA_OPEN_ALWAYS | FA_WRITE;
		}
		if (strcmp (mode,"r+") == 0) {
			fmode = FA_OPEN_ALWAYS | FA_READ | FA_WRITE;
		}
		
		/* Open a file in SDMMC */
		fres = f_open(fi, filename, fmode);
		
		printf("debug: trying to do f_open...\r\n");
		use_uart_debug(fres);
		
		/* Return OK */
		if (fres == FR_OK) { /*printf("debug: f_open succeeded.\r\n"); */ return fi; }
		else { /*printf("debug: f_open failed.\r\n");*/ return NULL; }
			/* If you want to return error, then you have to send a null pointer (NULL) */		
		
	} else { 
	  return NULL; 
	}

}

/* Handle std_close actions */
int TM_STDIO_FileCloseHandler(FILE * stream) {
	FRESULT fres;
	/* Close a file in SDMMC */
  fres = f_close(stream->fil);
	free(stream->fil);
	stream->fil = NULL;
	/* If you want to return error, then you have to send a null pointer (NULL) */	
	printf("debug: trying to do f_close...\r\n");
	use_uart_debug(fres);
	return fres;
}

int TM_STDIO_FileFputsHandler( const char * str, FILE * stream ) {
	return f_puts(str, stream->fil);
}

char * TM_STDIO_FileFgetsHandler( char * str, int num, FILE * stream ) {
	return f_gets(str, num, stream->fil);
}

long int TM_STDIO_FileFtellHandler( FILE * stream ) {
	stream->ofs = f_tell(stream->fil);
	return stream->ofs;
}

int TM_STDIO_FileFseekHandler( FILE * stream, long int offset, int origin ) {
	/* TO-DO: Checking offset beyond the file size AT WRITE MODE and use f_expand function */
	/*        to allocates (1) a contiguous data area to the file. */
	FSIZE_t new_ofs;
	if (origin == SEEK_SET) { /* Beginning of file */
		new_ofs = offset + 0;
		stream->ofs = new_ofs; /*
		if (offset > f_size(stream->fil)) {
			f_expand(stream->fil, offset, 1);
			offset = stream->ofs = f_size(stream->fil); 
		} */
		return f_lseek(stream->fil, new_ofs); 
	} else
	if (origin == SEEK_CUR) { /* Current position of the file pointer */
		new_ofs = offset + f_tell(stream->fil);		
		stream->ofs = new_ofs; /*
		if (offset > f_size(stream->fil)) {
			f_expand(stream->fil, offset, 1);
			offset = stream->ofs = f_size(stream->fil); 
		} */
		return f_lseek(stream->fil, new_ofs); 
	} else
	if (origin == SEEK_END) { /* End of file */		
		new_ofs = offset + f_size(stream->fil);
    stream->ofs =	new_ofs;	/*
		if (offset > f_size(stream->fil)) {
			f_expand(stream->fil, offset, 1);
			offset = stream->ofs = f_size(stream->fil); 
		} */
		return f_lseek(stream->fil, new_ofs); 
	} else
	return -1; /* none of the orgin is referenced, thus return error. */	
}

size_t TM_STDIO_FileFwriteHandler( const void * ptr, size_t size, size_t count, FILE * stream ) {
	/*  In case of *bw is less than btw, it means the volume got full during the write operation. */
	/*  Remember it depends on the write/read pointer. */
	FRESULT fres;
	UINT* bw;
	fres = f_write(stream->fil, ptr, size*count, bw);
	if ((fres != FR_OK) || (*bw < size)) 
		return 0;
	else
		return *bw;
}

size_t TM_STDIO_FileFreadHandler( void * ptr, size_t size, size_t count, FILE * stream ) {
	/*  In case of *bw is less than btw, it means the volume got full during the write operation. */
	/*  Remember it depends on the write/read pointer. */
	FRESULT fres;
	UINT* bw;
	fres = f_read(stream->fil, ptr, size*count, bw);
	/*
	printf("debug: trying to do f_read...\r\n");
	use_uart_debug(fres);
	*/
	if ((fres != FR_OK) || (*bw < size)) 
		return 0;
	else
		return *bw;
}
