/**	
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2014
 * | 
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |  
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * | 
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 */
#include "tm_stm32_stdio.h"
#include "tm_stm32_fatfs.h"	   /* FORK USED */

/* Create main __stdout for printf */
/* Set funcPointer to 0 */
__weak FILE __stdout = {0, 0};
__weak FILE __stdin = {0, 0};
__weak FILE __stderr;

/* stdio.h related function */
int fputc(int ch, FILE* f) {
	/* Check if it was called from printf or fprintf */
	
	if (f->outputFuncPointer != 0) {
		/* Call user custom function */
		return f->outputFuncPointer(ch, f);
	}
	
	/* Printf was called probably = stdout */
	return TM_STDIO_StdoutHandler(ch, f);
}

int fgetc(FILE* f) {
	/* Check if user want data from custom stream */
	
	if (f->inputFuncPointer != 0) {
		/* Call user custom function */
		return f->inputFuncPointer(f);
	}
	
	/* Standard stream (stdin) check */
	return TM_STDIO_StdinHandler(f);
}

void TM_STDIO_SetOutputFunction(FILE* f, int (*funcPointer)(int, FILE *)) {
	/* Set pointer to output function for specific file pointer */
	f->outputFuncPointer = funcPointer;
}

void TM_STDIO_SetInputFunction(FILE* f, int (*inputFuncPointer)(FILE *)) {
	/* Set pointer to input function for specific file pointer */
	f->inputFuncPointer = inputFuncPointer;
}

__weak int TM_STDIO_StdoutHandler(int c, FILE* f) {
	/* NOTE : This function Should not be modified, when the callback is needed,
            the TM_STDIO_StdoutHandler could be implemented in the user file
	*/
	return 0;
}

__weak int TM_STDIO_StdinHandler(FILE* f) {
	/* NOTE : This function Should not be modified, when the callback is needed,
            the TM_STDIO_StdoutHandler could be implemented in the user file
	*/
	return 0;
}

/* ----------------------------------------------------------------------------------
 * ----------------------------------------------------------------------------------
 * Following is a reworked code for STM32F7
 * - adding fopen, fclose, fread i/o functions implementation
 *
 */ 

FILE* fopen ( const char * filename, const char * mode ) {
		/* Check if it was called from fopen */
	
//	if (f->outputFuncPointer != 0) {
//		/* Call user custom function */
//		return f->outputFuncPointer(ch, f);
//	}
	
	/* Fopen was called probably = stdout */
  FILE* f;
	f = (FILE*) malloc(sizeof(FILE));
	f->fil = (TM_STDIO_FileOpenHandler(filename, mode));
	return f;
}

int fclose ( FILE * stream ) {
	int res; /*res now can be 0 or NOT(0) */
	res = TM_STDIO_FileCloseHandler(stream);
	free(stream);
	stream = NULL;
	return res;
}

__weak FIL* TM_STDIO_FileOpenHandler(const char * filename, const char * mode) {
	/* NOTE : This function Should not be modified, when the callback is needed,
            the TM_STDIO_FileOpenHandler could be implemented in the user file
	*/
	return 0;
}

__weak int TM_STDIO_FileCloseHandler(FILE * stream) {
	/* NOTE : This function Should not be modified, when the callback is needed,
            the TM_STDIO_FileCloseHandler could be implemented in the user file
	*/
	return 0;
}

int fputs ( const char * str, FILE * stream ) {
	/*return number of characters written on success, (EOF) -1 on failure. */
		
	return TM_STDIO_FileFputsHandler(str, stream);
}

char * fgets ( char * str, int num, FILE * stream ) {
	/*return str on success, a null pointer on failure. */
		
	return TM_STDIO_FileFgetsHandler(str, num, stream);
}

long int ftell ( FILE * stream ) {
	/* return the current value of the position indicator on success, -1L on failure. */
	return TM_STDIO_FileFtellHandler(stream);
}

int fseek ( FILE * stream, long int offset, int origin ) {
	/* return zero on success, non-zero value on failure. */
	return TM_STDIO_FileFseekHandler(stream, offset, origin);
}

__weak int TM_STDIO_FileFputsHandler( const char * str, FILE * stream ) {
	/* NOTE : This function Should not be modified, when the callback is needed,
            the TM_STDIO_FileFputsHandler could be implemented in the user file
	*/
	return 0;
}

__weak char * TM_STDIO_FileFgetsHandler( char * str, int num, FILE * stream ) {
	/* NOTE : This function Should not be modified, when the callback is needed,
            the TM_STDIO_FileFgetsHandler could be implemented in the user file
	*/
	return 0;
}

__weak long int TM_STDIO_FileFtellHandler( FILE * stream ) {
	/* NOTE : This function Should not be modified, when the callback is needed,
            the TM_STDIO_FileFtellHandler could be implemented in the user file
	*/
	return 0;
}

__weak int TM_STDIO_FileFseekHandler( FILE * stream, long int offset, int origin ) {
	/* NOTE : This function Should not be modified, when the callback is needed,
            the TM_STDIO_FileFseekHandler could be implemented in the user file
	*/
	return 0;
}

size_t fwrite ( const void * ptr, size_t size, size_t count, FILE * stream ) {
	/* return the total number of elements written on success, error indicator (ferror) will be set on failure. */
	return TM_STDIO_FileFwriteHandler(ptr, size, count, stream);
}

size_t fread ( void * ptr, size_t size, size_t count, FILE * stream ) {
	/* return the total number of elements read on success, error indicator (ferror) will be set on failure. */
	return TM_STDIO_FileFreadHandler(ptr, size, count, stream);
}

__weak size_t TM_STDIO_FileFwriteHandler( const void * ptr, size_t size, size_t count, FILE * stream ) {
	/* NOTE : This function Should not be modified, when the callback is needed,
            the TM_STDIO_FileFwriteHandler could be implemented in the user file
	*/
	return 0;
}

__weak size_t TM_STDIO_FileFreadHandler( void * ptr, size_t size, size_t count, FILE * stream ) {
	/* NOTE : This function Should not be modified, when the callback is needed,
            the TM_STDIO_FileFreadHandler could be implemented in the user file
	*/
	return 0;
}
