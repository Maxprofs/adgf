/*****************************************************************************
 *
 *	DrawSdl main program.
 *
 *  Description:
 *  Demo program for drawing with LibSDL.
 *
 *	File:	main.h
 *
 *	Author: Brian Lingard
 *	Date:	02/04/2009
 *
 *
 *****************************************************************************/

#ifndef __main_h
#define __main_h

//////////////////////////////////////////////////////////////////////////////
// defines
//////////////////////////////////////////////////////////////////////////////

#define WIDTH   WIDTH_1024
#define HEIGHT  HEIGHT_768
#define BPP     BPP_16

//////////////////////////////////////////////////////////////////////////////
// prototypes
//////////////////////////////////////////////////////////////////////////////

void PrintBanner();
void Usage( FILE *fp, char *program );
int  DoMenu();

#endif /* __main_h */

