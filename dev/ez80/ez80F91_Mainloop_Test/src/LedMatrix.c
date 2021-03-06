/*                                                   Copyright (c) ZiLOG, 2003
 *****************************************************************************
 *  LedMatrix.c (1.0)
 *
 *  functions for LED matrix on eZ80 Development Board
 *****************************************************************************
 */

#include <ez80.h>
#include <string.h>

#include "LedMatrix.h"
#include "LedTimer.h"


/*  LED matrix display pointer, what ever 5 byte string this points to will
 *  be displayed
 */
unsigned char *pcolumn;
unsigned char *p_user_input;
char user_input;


/*****************************************************************************
 *  ledmatrix_init()
 *
 *  setup peripherals to use the LED matix
 */
void ledmatrix_init( void )
{
    /* map LED Matrix at 800000h with Chip Select 2 */
    CS2_LBR = 0x80;
    CS2_UBR = 0xbf;
    CS2_CTL = 0x28;
	/* clear display */
	ledmatrix_clear();

	user_input = 0;
	p_user_input = &matrix_char_map[1][0];
}


/*****************************************************************************
 *  ledmatrix_clear()
 *
 *  clear LED matrix
 */
void ledmatrix_clear( void )
{
	pcolumn = &matrix_char_map[1][0];
}


/*****************************************************************************
 *  ledmatrix_fill()
 *
 *  fill LED matrix
 */
void ledmatrix_fill( void )
{
	pcolumn = &matrix_char_map[0][0];
}


/*****************************************************************************
 *  ledmatrix_putc( character )
 *
 *  ouput a character to the LED Matrix
 */
void ledmatrix_putc( char c )
{
	char index;
	
	/* align ascii character to this programs character map */
    index = c - 31;
	pcolumn = &matrix_char_map[index][0];
}


/*****************************************************************************
 *  ledmatrix_puts( string pointer )
 *
 *  ouput a character string to the LED Matrix
 */
void ledmatrix_puts( char *str )
{
	char *c;
	int i;

	c = str;

	for( i=0; i<strlen(str); i++ )
	{
	    /* place character */
        ledmatrix_putc( *(str + i) );
		wait( 500 );

	    /* briefly clear the display to cause a character change effect */
		ledmatrix_clear();
        wait( 50 );
	}
}


/*****************************************************************************
 *  ledmatrix_spin()
 *
 *  make display appear to spin
 */
void ledmatrix_spin( void )
{
    int i;

  	for( i=0; i<15; i++ )
	{
	
        ledmatrix_putc('l');
		wait( 60 );

        ledmatrix_putc('/');
		wait( 60 );

        ledmatrix_putc('-');
		wait( 60 );

        ledmatrix_putc('\\');
		wait( 60 );

    }
}


/*****************************************************************************
 *  ledmatrix_flash()
 *
 *  This is a flashing display function
 */
void ledmatrix_flash( void )
{
    int i;

  	for( i=0; i<2; i++ )
	{
	
        ledmatrix_fill();
		wait( 200 );

        ledmatrix_clear();
		wait( 200 );
    }
}


/*****************************************************************************
 *  displays each character map entry
 *
 *  flash a bunch of data on the LED Matrix
 */
void ledmatrix_test( void )
{
    int i;

    /* run through the LED matrix character map */
    for( i=31; i<129; i++ )
	{
	    ledmatrix_putc( i );
		wait( 50 );
    }
}


/*****************************************************************************
 *  5x7 LED matrix character map
 *  this maps the column bit assignments (bits 0-4)
 */
unsigned char matrix_char_map[128][7] = {
    /* misc. characters */
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  //  0 - block
    {0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f},  //  0 - space
	{0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x1f, 0x1b},  //  1 - !
    {0x15, 0x15, 0x15, 0x1f, 0x1f, 0x1f, 0x1f},  //  2 - "
	{0x1f, 0x15, 0x00, 0x15, 0x00, 0x15, 0x1f},  //  3 - #
	{0x1b, 0x11, 0x0a, 0x11, 0x0a, 0x11, 0x1b},  //  4 - $
	{0x1f, 0x1e, 0x15, 0x1b, 0x15, 0x0f, 0x1f},  //  5 - %
	{0x11, 0x0e, 0x0e, 0x11, 0x15, 0x0e, 0x10},  //  6 - &
	{0x1b, 0x1b, 0x1b, 0x1f, 0x1f, 0x1f, 0x1f},  //  7 - '
	{0x1d, 0x1b, 0x17, 0x17, 0x17, 0x1b, 0x1d},  //  8 - (
	{0x17, 0x1b, 0x1d, 0x1d, 0x1d, 0x1b, 0x17},  //  9 - )
	{0x1f, 0x0a, 0x11, 0x00, 0x11, 0x0a, 0x1f},  // 10 - *
	{0x1f, 0x1b, 0x1b, 0x00, 0x1b, 0x1b, 0x1f},  // 11 - +
	{0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1b, 0x17},  // 12 - ,
	{0x1f, 0x1f, 0x1f, 0x00, 0x1f, 0x1f, 0x1f},  // 13 - -
	{0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1b},  // 14 - .
	{0x1f, 0x1e, 0x1d, 0x1b, 0x17, 0x0f, 0x1f},  // 15 - /
    /* numbers */
    {0x11, 0x0e, 0x0c, 0x0a, 0x06, 0x0e, 0x11},  // 16 - 0
    {0x1b, 0x13, 0x1b, 0x1b, 0x1b, 0x1b, 0x11},  // 17 - 1
    {0x11, 0x0e, 0x1d, 0x1b, 0x17, 0x0f, 0x00},  // 18 - 2
    {0x11, 0x0e, 0x1e, 0x19, 0x1e, 0x0e, 0x11},  // 19 - 3
    {0x0e, 0x0e, 0x0e, 0x10, 0x1e, 0x1e, 0x1e},  // 20 - 4
    {0x00, 0x0f, 0x0f, 0x01, 0x1e, 0x0e, 0x11},  // 21 - 5
    {0x11, 0x0f, 0x0f, 0x01, 0x0e, 0x0e, 0x11},  // 22 - 6
    {0x00, 0x1e, 0x1e, 0x1d, 0x1b, 0x1b, 0x1b},  // 23 - 7
    {0x11, 0x0e, 0x0e, 0x11, 0x0e, 0x0e, 0x11},  // 24 - 8
    {0x11, 0x0e, 0x0e, 0x10, 0x1e, 0x1d, 0x1b},  // 25 - 9
    /* misc. characters */
    {0x1f, 0x1f, 0x1b, 0x1f, 0x1b, 0x1f, 0x1f},  // 26 - :
	{0x1f, 0x1f, 0x1b, 0x1f, 0x1b, 0x17, 0x1f},  // 27 - ;
	{0x1d, 0x1b, 0x17, 0x0f, 0x17, 0x1b, 0x1d},  // 28 - <
	{0x1f, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x1f},  // 29 - =
	{0x17, 0x1b, 0x1d, 0x1e, 0x1d, 0x1b, 0x17},  // 30 - >
	{0x11, 0x0e, 0x0d, 0x1b, 0x1b, 0x1f, 0x1b},  // 31 - ?
	{0x11, 0x0a, 0x04, 0x04, 0x05, 0x0a, 0x11},  // 32 - @
    /* upper case characters */
    {0x11, 0x0e, 0x0e, 0x0e, 0x00, 0x0e, 0x0e},  // 33 - A
    {0x01, 0x0e, 0x0e, 0x01, 0x0e, 0x0e, 0x01},  // 34 - B
    {0x11, 0x0e, 0x0f, 0x0f, 0x0f, 0x0e, 0x11},  // 35 - C
    {0x01, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x01},  // 36 - D
    {0x00, 0x0f, 0x0f, 0x01, 0x0f, 0x0f, 0x00},  // 37 - E
    {0x00, 0x0f, 0x0f, 0x01, 0x0f, 0x0f, 0x0f},  // 38 - F
    {0x11, 0x0e, 0x0f, 0x08, 0x0e, 0x0e, 0x11},  // 39 - G
    {0x0e, 0x0e, 0x0e, 0x00, 0x0e, 0x0e, 0x0e},  // 40 - H
    {0x00, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x00},  // 41 - I
    {0x00, 0x1d, 0x1d, 0x1d, 0x0d, 0x0d, 0x13},  // 42 - J
    {0x0e, 0x0d, 0x0b, 0x07, 0x0b, 0x0d, 0x0e},  // 43 - K
    {0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x00},  // 44 - L
    {0x0e, 0x04, 0x0a, 0x0a, 0x0e, 0x0e, 0x0e},  // 45 - M
    {0x0e, 0x0e, 0x06, 0x0a, 0x0c, 0x0e, 0x0e},  // 46 - N
    {0x11, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x11},  // 47 - O
    {0x01, 0x0e, 0x0e, 0x01, 0x0f, 0x0f, 0x0f},  // 48 - P
    {0x11, 0x0e, 0x0e, 0x0e, 0x0a, 0x0c, 0x10},  // 49 - Q
    {0x01, 0x0e, 0x0e, 0x01, 0x0b, 0x0d, 0x0e},  // 50 - R
    {0x11, 0x0e, 0x0f, 0x11, 0x1e, 0x0e, 0x11},  // 51 - S
    {0x00, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b},  // 52 - T
    {0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x11},  // 53 - U
    {0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x15, 0x1b},  // 54 - V
    {0x0e, 0x0e, 0x0a, 0x0a, 0x0a, 0x0a, 0x15},  // 55 - W
    {0x0e, 0x0e, 0x15, 0x1b, 0x15, 0x0e, 0x0e},  // 56 - X
    {0x0e, 0x0e, 0x15, 0x1b, 0x1b, 0x1b, 0x1b},  // 57 - Y
    {0x00, 0x1e, 0x1d, 0x1b, 0x17, 0x0f, 0x00},  // 58 - Z
    /* misc. characters */
    {0x03, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x03},  // 59 - [
    {0x1f, 0x0f, 0x17, 0x1b, 0x1d, 0x1e, 0x1f},  // 60 - backslash
	{0x1c, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1c},  // 61 - ]
	{0x1b, 0x15, 0x0e, 0x1f, 0x1f, 0x1f, 0x1f},  // 62 - ^
	{0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x00},  // 63 - _
	{0x1b, 0x1b, 0x1b, 0x1f, 0x1f, 0x1f, 0x1f},  // 64 - '
	/* lower case characters */
    {0x1f, 0x1f, 0x19, 0x16, 0x16, 0x16, 0x18},  // 65 - a
    {0x17, 0x17, 0x11, 0x16, 0x16, 0x16, 0x11},  // 66 - b
    {0x1f, 0x1f, 0x19, 0x16, 0x17, 0x16, 0x19},  // 67 - c
    {0x1e, 0x1e, 0x18, 0x16, 0x16, 0x16, 0x18},  // 68 - d
    {0x1f, 0x1f, 0x19, 0x10, 0x17, 0x16, 0x19},  // 69 - e
    {0x1d, 0x1a, 0x1b, 0x11, 0x1b, 0x1b, 0x1b},  // 70 - f
    {0x1f, 0x19, 0x16, 0x16, 0x18, 0x16, 0x19},  // 71 - g
    {0x17, 0x17, 0x11, 0x16, 0x16, 0x16, 0x16},  // 72 - h
    {0x1f, 0x1f, 0x1b, 0x1f, 0x1b, 0x1b, 0x1b},  // 73 - i
    {0x1f, 0x1d, 0x1f, 0x1d, 0x1d, 0x1d, 0x13},  // 74 - j
    {0x17, 0x17, 0x15, 0x13, 0x13, 0x15, 0x16},  // 75 - k
    {0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b},  // 76 - l
    {0x1f, 0x1f, 0x05, 0x0a, 0x0a, 0x0a, 0x0a},  // 77 - m
    {0x1f, 0x1f, 0x11, 0x16, 0x16, 0x16, 0x16},  // 78 - n
    {0x1f, 0x1f, 0x19, 0x16, 0x16, 0x16, 0x19},  // 79 - o
    {0x1f, 0x11, 0x16, 0x16, 0x11, 0x17, 0x17},  // 80 - p
    {0x1f, 0x18, 0x16, 0x16, 0x18, 0x1e, 0x1e},  // 81 - q
    {0x1f, 0x1f, 0x11, 0x16, 0x17, 0x17, 0x17},  // 82 - r
    {0x1f, 0x1f, 0x18, 0x17, 0x19, 0x1e, 0x11},  // 83 - s
    {0x1f, 0x1f, 0x1b, 0x11, 0x1b, 0x1b, 0x1b},  // 84 - t
    {0x1f, 0x1f, 0x16, 0x16, 0x16, 0x16, 0x18},  // 85 - u
    {0x1f, 0x1f, 0x16, 0x16, 0x16, 0x16, 0x19},  // 86 - v
    {0x1f, 0x1f, 0x0a, 0x0a, 0x0a, 0x0a, 0x15},  // 87 - w
    {0x1f, 0x1f, 0x0e, 0x15, 0x1b, 0x15, 0x0e},  // 88 - x
    {0x1f, 0x1a, 0x1a, 0x1a, 0x1d, 0x1b, 0x17},  // 89 - y
    {0x1f, 0x1f, 0x10, 0x1d, 0x1b, 0x17, 0x10},  // 90 - z
    {0x1d, 0x1b, 0x1b, 0x17, 0x1b, 0x1b, 0x1d},  // 91 - {
	{0x1b, 0x1b, 0x1b, 0x1f, 0x1b, 0x1b, 0x1b},  // 92 - |
	{0x17, 0x1b, 0x1b, 0x1d, 0x1b, 0x1b, 0x17},  // 93 - }
	{0x1f, 0x1a, 0x15, 0x1f, 0x1f, 0x1f, 0x1f}   // 94 - ~
	};
