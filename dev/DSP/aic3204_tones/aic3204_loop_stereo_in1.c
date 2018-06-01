/*
 *  Copyright 2010 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  AIC3204 Tone
 *
 */
#include "stdio.h"
#include "usbstk5515.h"
#include "usbstk5515_led.h"
#include "sar.h"

extern Int16 AIC3204_rset( Uint16 regnum, Uint16 regval);
#define Rcv 0x08
#define Xmit 0x20

#include "audioCore.h"

void toggleMute( void );
void toggleFIR( void );
void toggleBypass( void );

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  AIC3204 Loop                                                            *
 *      Output input from STEREO IN 1 through the HEADPHONE jack            *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 aic3204_loop_stereo_in1( )
{
    /* Pre-generated sine wave data, 16-bit signed samples */
    Int16 sinetable[48] = {
        0x0000, 0x10b4, 0x2120, 0x30fb, 0x3fff, 0x4dea, 0x5a81, 0x658b,
        0x6ed8, 0x763f, 0x7ba1, 0x7ee5, 0x7ffd, 0x7ee5, 0x7ba1, 0x76ef,
        0x6ed8, 0x658b, 0x5a81, 0x4dea, 0x3fff, 0x30fb, 0x2120, 0x10b4,
        0x0000, 0xef4c, 0xdee0, 0xcf06, 0xc002, 0xb216, 0xa57f, 0x9a75,
        0x9128, 0x89c1, 0x845f, 0x811b, 0x8002, 0x811b, 0x845f, 0x89c1,
        0x9128, 0x9a76, 0xa57f, 0xb216, 0xc002, 0xcf06, 0xdee0, 0xef4c
    };
    /* Pre-generated sine wave data, 16-bit signed samples */
    Int16 j, i = 0;
    Int16 sample, data1, data2, data3, data4;
    
    Int16 key;
    
    Sample readSample, writeSample, mixSample;
   
    /* Initialize SAR ADC and ULED */
    Init_SAR();
    USBSTK5515_ULED_init( );
       
	/* Initialize audio processor */
    AudioInit();
    //AudioSetEnableFIR( 1 );
	
	if (AudioGetEnableFIR())
	{
        USBSTK5515_ULED_on( 1 );
        USBSTK5515_ULED_on( 2 );
	}
    
    /* Configure AIC3204 */

    AIC3204_rset( 0, 0 );          // Select page 0
    AIC3204_rset( 1, 1 );          // Reset codec
    AIC3204_rset( 0, 1 );          // Select page 1
    AIC3204_rset( 1, 8 );          // Disable crude AVDD generation from DVDD
    AIC3204_rset( 2, 1 );          // Enable Analog Blocks, use LDO power
    AIC3204_rset( 0, 0 );          // Select page 0
    /* PLL and Clocks config and Power Up  */
    AIC3204_rset( 27, 0x0d );      // BCLK and WCLK is set as o/p to AIC3204(Master)
    AIC3204_rset( 28, 0x00 );      // Data ofset = 0
    AIC3204_rset( 4, 3 );          // PLL setting: PLLCLK <- MCLK, CODEC_CLKIN <-PLL CLK
    AIC3204_rset( 6, 7 );          // PLL setting: J=7
    AIC3204_rset( 7, 0x06 );       // PLL setting: HI_BYTE(D=1680)
    AIC3204_rset( 8, 0x90 );       // PLL setting: LO_BYTE(D=1680)
    AIC3204_rset( 30, 0x88 );      // For 32 bit clocks per frame in Master mode ONLY
                                   // BCLK=DAC_CLK/N =(12288000/8) = 1.536MHz = 32*fs
    AIC3204_rset( 5, 0x91 );       // PLL setting: Power up PLL, P=1 and R=1
    AIC3204_rset( 13, 0 );         // Hi_Byte(DOSR) for DOSR = 128 decimal or 0x0080 DAC oversamppling
    AIC3204_rset( 14, 0x80 );      // Lo_Byte(DOSR) for DOSR = 128 decimal or 0x0080
    AIC3204_rset( 20, 0x80 );      // AOSR for AOSR = 128 decimal or 0x0080 for decimation filters 1 to 6
    AIC3204_rset( 11, 0x82 );      // Power up NDAC and set NDAC value to 2
    AIC3204_rset( 12, 0x87 );      // Power up MDAC and set MDAC value to 7
    AIC3204_rset( 18, 0x87 );      // Power up NADC and set NADC value to 7
    AIC3204_rset( 19, 0x82 );      // Power up MADC and set MADC value to 2
    /* DAC ROUTING and Power Up */
    AIC3204_rset(  0, 0x01 );      // Select page 1
    AIC3204_rset( 12, 0x08 );      // LDAC AFIR routed to HPL
    AIC3204_rset( 13, 0x08 );      // RDAC AFIR routed to HPR
    AIC3204_rset(  0, 0x00 );      // Select page 0
    AIC3204_rset( 64, 0x02 );      // Left vol=right vol
    AIC3204_rset( 65, 0x00 );      // Left DAC gain to 0dB VOL; Right tracks Left
    AIC3204_rset( 63, 0xd4 );      // Power up left,right data paths and set channel
    AIC3204_rset(  0, 0x01 );      // Select page 1
    AIC3204_rset( 16, 0x00 );      // Unmute HPL , 0dB gain
    AIC3204_rset( 17, 0x00 );      // Unmute HPR , 0dB gain
    AIC3204_rset(  9, 0x30 );      // Power up HPL,HPR
    AIC3204_rset(  0, 0x00 );      // Select page 0
    USBSTK5515_wait( 500 );        // Wait
    
    /* ADC ROUTING and Power Up */
    AIC3204_rset( 0, 1 );          // Select page 1
    AIC3204_rset( 0x34, 0x30 );    // STEREO 1 Jack
		                           // IN2_L to LADC_P through 40 kohm
    AIC3204_rset( 0x37, 0x30 );    // IN2_R to RADC_P through 40 kohmm
    AIC3204_rset( 0x36, 3 );       // CM_1 (common mode) to LADC_M through 40 kohm
    AIC3204_rset( 0x39, 0xc0 );    // CM_1 (common mode) to RADC_M through 40 kohm
    AIC3204_rset( 0x3b, 0 );       // MIC_PGA_L unmute
    AIC3204_rset( 0x3c, 0 );       // MIC_PGA_R unmute
    AIC3204_rset( 0, 0 );          // Select page 0
    AIC3204_rset( 0x51, 0xc0 );    // Powerup Left and Right ADC
    AIC3204_rset( 0x52, 0 );       // Unmute Left and Right ADC
    
    AIC3204_rset( 0, 0 );    
    USBSTK5515_wait( 200 );        // Wait
    /* I2S settings */
    I2S0_SRGR = 0x0;
    I2S0_CR = 0x8010;    // 16-bit word, slave, enable I2C
    I2S0_ICMR = 0x3f;    // Enable interrupts
    
    /* Play Tone */
//    for ( i = 0 ; i < 5 ; i++ )
    while ( 1 )
    {
        for ( j = 0 ; j < 1000 ; j++ )
        {
            for ( sample = 0 ; sample < 48 ; sample++ )
            {
            	/* Read Digital audio */
            	while((Rcv & I2S0_IR) == 0);  // Wait for interrupt pending flag
                data3 = I2S0_W0_MSW_R;  // 16 bit left channel received audio data
      	        data1 = I2S0_W0_LSW_R;
      	        data4 = I2S0_W1_MSW_R;  // 16 bit right channel received audio data
      	        data2 = I2S0_W1_LSW_R;
      	        
      	        /* Process this sample */
      	        setSample( data3, data1, data4, data2, &readSample );
				//setSample( sinetable[sample], 0, sinetable[sample], 0, &mixSample );
      	        AudioProcessSample( &readSample, &writeSample );
      	        //AudioProcessMixedSample( &readSample, &mixSample, &writeSample );
      	        
				/* Write Digital audio */
      	        while((Xmit & I2S0_IR) == 0);  // Wait for interrupt pending flag
				I2S0_W0_MSW_W = writeSample.LeftMsw;  // 16 bit left channel transmit audio data
      	        I2S0_W0_LSW_W = 0;
      	        I2S0_W1_MSW_W = writeSample.RightMsw; // 16 bit right channel transmit audio data
      	        I2S0_W1_LSW_W = 0;
            }
        
        	/* check for volume keys */
        	key = Get_Sar_Key();
    	    if((key == SW12))   // If SW1 and SW2 pressed
    	    {    
				toggleMute();
				//toggleFIR();
				//toggleBypass();
    	    }   	
   			else if((key == SW1))   // If SW1 pressed
    		{
    			AudioDecrVolume();
    			USBSTK5515_ULED_toggle( 0 );  // Toggle DS2 (GREEN LED)
    		}
    		else if((key == SW2))   // If SW2 pressed
    		{
     			AudioIncrVolume();
    			USBSTK5515_ULED_toggle( 3 );  // Toggle DS5 (BLUE LED)
       		}

    	}
    }
    
    /* Disble I2S */
    I2S0_CR = 0x00;
   
    return 0;
}

void toggleMute( void )
{
    if (AudioGetMute())
    {
    	AudioSetMute( 0 );
    	USBSTK5515_ULED_off( 1 );
    	USBSTK5515_ULED_off( 2 );
    }
    else
    {
    	AudioSetMute( 1 );
    	USBSTK5515_ULED_on( 1 );
    	USBSTK5515_ULED_on( 2 );
    }
}

void toggleFIR( void )
{
    if (AudioGetEnableFIR())
    {
    	AudioSetEnableFIR( 0 );
    	USBSTK5515_ULED_off( 1 );
    	USBSTK5515_ULED_off( 2 );
    }
    else
    {
    	AudioSetEnableFIR( 1 );
    	USBSTK5515_ULED_on( 1 );
    	USBSTK5515_ULED_on( 2 );
    }
}

void toggleBypass( void )
{
    if (AudioGetEnableBypass())
    {
    	AudioSetEnableBypass( 0 );
    	USBSTK5515_ULED_off( 1 );
    	USBSTK5515_ULED_off( 2 );
    }
    else
    {
    	AudioSetEnableBypass( 1 );
    	USBSTK5515_ULED_on( 1 );
    	USBSTK5515_ULED_on( 2 );
    }
}