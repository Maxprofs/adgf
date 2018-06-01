/***********************************************************************

   File:  	image.c

   Author: 	Brian Lingard

   Date:	10/20/95

  High level functions for image processing demo program.

***********************************************************************/


/*  include necessary files 
 */
#include <stdio.h>
#include "libsx.h"

#ifndef _MAIN_H
#include "main.h"		/* already includes "particle.h" */
#endif

#ifndef _CALLBACK_H
#include "callback.h"
#endif


/* 
#define IMAGE_DEBUG 1
*/


/******************************************************************************
*  InitDatabase - initializes global project database                         *
******************************************************************************/
void
InitDatabase(MyProgram *data)
{
   data->project_loaded = FALSE;
   data->project_saved = FALSE;

   data->load_file_name[0] = '\0';
   data->save_file_name[0] = '\0';

   data->load_file = (FILE *) NULL;
   data->save_file = (FILE *) NULL;

   ClearColorMap();

   data->data.current.rsize = RASTER_R_SIZE;
   data->data.current.csize = RASTER_C_SIZE;

   data->data.original.rsize = RASTER_R_SIZE;
   data->data.original.csize = RASTER_C_SIZE;

   ClearImageBuff(&data->data.current);
   ClearImageBuff(&data->data.original);

   data->data.image_is_grey = FALSE;

   data->data.hist.computed = FALSE;

   InitializeLUT(data);

   fprintf(stdout, "... done initialization!\n");
}


/******************************************************************************
*  DumpDatabase - ouputs entire database to a file called "dumpdata.log"      *
******************************************************************************/
void
DumpDatabase(MyProgram *data)
{
   FILE_STRING_T	dumpname;
   FILE			*dumpfile;
   int 			i, j;


   sprintf(dumpname, "%s", DEFAULT_DUMP_FILE);

   dumpfile = fopen(dumpname,"w");
   if (dumpfile == (FILE *) NULL) {
      fprintf(stderr, "DumpDatabase: can't open '%s' as dump file", dumpname);
      exit(1);
   }

   fprintf(dumpfile, "project loaded   = %d\n", data->project_loaded);
   fprintf(dumpfile, "project saved    = %d\n", data->project_saved);

   fprintf(dumpfile, "load file name   = %s\n", data->load_file_name);
   fprintf(dumpfile, "save file name   = %s\n", data->save_file_name);

   fprintf(dumpfile, "\nhistogram:\n");
   for (i = 0; i < MAX_PIXEL_VALUE; i++)
   {
      fprintf(dumpfile, "   i = %03d -- %d\n", i, data->data.hist.array[i]);
   }

   fprintf(dumpfile, "\n END OF FILE\n");

   fclose(dumpfile);
}


/******************************************************************************
*  LoadImage - loads PPM image file and displays it                           *
*                                                                             *                                                                             ******************************************************************************/
void
LoadImage(MyProgram *data)
{
   int rsize, csize;


   ClearImageBuff(&data->data.current);

   rsize = RASTER_R_SIZE;
   csize = RASTER_C_SIZE;

   if ( PPM_read(data->load_file, 
		(char *)&data->data.current.raster[0][0], &rsize, &csize) )
   {
      data->project_loaded = FALSE;
      data->project_saved = FALSE;

      fprintf(stderr, "warning: image file could not be loaded\n");
   }
   else
   {
      data->data.current.rsize = rsize;
      data->data.current.csize = csize;

      fprintf(stdout, "   loaded %d x %d image\n", csize, rsize);

      data->project_loaded = TRUE;
      data->project_saved = TRUE;

      data->data.hist.computed = FALSE;

      CopyImageBuff(&data->data.current, &data->data.original);

      UpdateDisplay(data);
      DrawHistogram(data);
   }
}


/******************************************************************************
*  SaveImage - saves current image buffer to PPM image file                   *
*                                                                             *
******************************************************************************/
void
SaveImage(MyProgram *data)
{
   PPM_write(data->save_file, (char *)&data->data.current.raster[0][0],
		data->data.current.rsize, data->data.current.csize,
		"generated by Image v1.0");
}


/******************************************************************************
*  InitializeLUT - initializes the grey level component look up table         *
*                                                                             *
******************************************************************************/
void
InitializeLUT(MyProgram *data)
{
   int i;

   for (i = 0; i < MAX_GRADIENT_LEVELS; i++)
   {
      data->data.lut[i] = (UBYTE_T) i;
   }
}


/******************************************************************************
*  SetNegate - set LUT to a negatively sloped ramp                            *
*                                                                             *
******************************************************************************/
void
SetNegate(MyProgram *data)
{
   int i;

   for (i = 0; i < MAX_GRADIENT_LEVELS; i++)
   {
      data->data.lut[i] = (UBYTE_T) (MAX_GRADIENT_LEVELS - i);
   }
}


/******************************************************************************
*  SetBWThreshold - sets all entries below threshold to black, else to white  *
*                                                                             *
******************************************************************************/
void
SetBWThreshold(MyProgram *data, int threshold)
{
   int i;

   InitializeLUT(data);

   for (i = 0; i < MAX_GRADIENT_LEVELS; i++)
   {
      if (i >= threshold)
         data->data.lut[i] = (UBYTE_T) MAX_PIXEL_VALUE;
      else
         data->data.lut[i] = (UBYTE_T) 0;
   }
}


/******************************************************************************
*  SetBrightnessFactor - adds a brightness factor to all entries in LUT       *                                                                             *                                                                             *
******************************************************************************/
void
SetBrightnessFactor(MyProgram *data, int factor)
{
   int i, level;

   InitializeLUT(data);

   for (i = 0; i < MAX_GRADIENT_LEVELS; i++)
   {
      level = data->data.lut[i] + factor;

      data->data.lut[i] = (UBYTE_T) CLAMP( level, 0, MAX_PIXEL_VALUE);
   }
}


/******************************************************************************
*  ClearImageBuff - clears image buffer to all black                          *
*                                                                             *
******************************************************************************/
void
ClearImageBuff(IMAGE_S_T *buffer)
{
   int r, c;
   char *ras_p;


   ras_p = (char *)&buffer->raster[0][0];

   for (r = 0; r < buffer->rsize; r++)
   {
      for (c = 0; c < buffer->csize; c++)
      {
         *(ras_p++) = 0;
         *(ras_p++) = 0;
         *(ras_p++) = 0;
      }
   }
}


/******************************************************************************
*  CopyImageBuff - copies source image buffer to destination image buffer     *
*                                                                             *
******************************************************************************/
void
CopyImageBuff(IMAGE_S_T *src, IMAGE_S_T *dst)
{
   int r, c;

   COLOR_S_T *src_p, *dst_p;

   char *src_ras_p, *dst_ras_p;


   src_ras_p = (char *)&src->raster[0][0];
   dst_ras_p = (char *)&dst->raster[0][0];


   for (r = 0; r < src->rsize; r++)
   {
      for (c = 0; c < src->csize; c++)
      {
         *(dst_ras_p++) = *(src_ras_p++);
         *(dst_ras_p++) = *(src_ras_p++);
         *(dst_ras_p++) = *(src_ras_p++);
      }
   }

   dst->rsize = src->rsize;
   dst->csize = src->csize;
}


/******************************************************************************
*  GenerateHistogram - computes the histogram of current image buffer         *
*                                                                             *
******************************************************************************/
void
GenerateHistogram(MyProgram *data)
{
   int x;
   int r, c, maxr, maxc;
   int grey;
   int max_value, max_index;
   int min_value, min_index;

   unsigned char rv, bv, gv;
   char *ras_p;


   ras_p = (char *)&data->data.current.raster[0][0];

   maxr = data->data.current.rsize;
   maxc = data->data.current.csize;


   /*  initialize histogram 
    */
   for (r = 0; r < MAX_GRADIENT_LEVELS; r++)
   {
      data->data.hist.array[r] = 0;
   }

   /*  compute histogram of current image 
    */
   for (r = 0; r < maxr; r++)
   {
      for (c = 0; c < maxc; c++)
      {
         rv = *(ras_p++);
         gv = *(ras_p++);
         bv = *(ras_p++);

         if (data->data.image_is_grey)
         {
            data->data.hist.array[rv] += 1;
         }
         else
         {
            grey = RGB2GREY( rv, gv, bv );

            data->data.hist.array[CLAMP(grey, 0, MAX_PIXEL_VALUE)] += 1;
         }
      }
   }

   /*  compute highest and lowest count in histogram
    */
   max_value = data->data.hist.array[0];
   max_index = 0;
   min_value = data->data.hist.array[0];
   min_index = 0;

   for (x = 1; x < MAX_GRADIENT_LEVELS; x++)
   {
      if (data->data.hist.array[x] > max_value)
      {
         max_value = data->data.hist.array[x];
         max_index = x;
      }
      if (data->data.hist.array[x] < min_value)
      {
         min_value = data->data.hist.array[x];
         min_index = x;
      }
   }

   data->data.hist.max_count = max_value;
   data->data.hist.max_index = max_index;
   data->data.hist.min_count = min_value;
   data->data.hist.min_index = min_index;

   data->data.hist.computed = TRUE;
}


/******************************************************************************
*  Color2GreyImageBuff - converts image in current buffer from color to grey  *
*                                                                             *
******************************************************************************/
void
Color2GreyImageBuff(MyProgram *data)
{
   int r, c, maxr, maxc;

   int grey;

   unsigned char rv, bv, gv;
   char *ras_p, *temp_ras_p;


   ras_p = (char *)&data->data.current.raster[0][0];
   temp_ras_p = (char *)&data->data.temp.raster[0][0];

   maxr = data->data.current.rsize;
   maxc = data->data.current.csize;

   for (r = 0; r < maxr; r++)
   {
      for (c = 0; c < maxc; c++)
      {
         rv = *(ras_p++);
         gv = *(ras_p++);
         bv = *(ras_p++);

         grey = RGB2GREY( rv, gv, bv );

         *(temp_ras_p++) = CLAMP(grey, 0, MAX_PIXEL_VALUE);
         *(temp_ras_p++) = CLAMP(grey, 0, MAX_PIXEL_VALUE);
         *(temp_ras_p++) = CLAMP(grey, 0, MAX_PIXEL_VALUE);
      }
   }

   data->data.temp.rsize = maxr;
   data->data.temp.csize = maxc;

   CopyImageBuff(&data->data.temp, &data->data.current);
}


/******************************************************************************
*  PointProcessImageBuff - applies LUT to pixels in current image buffer      *
*                                                                             *
******************************************************************************/
void
PointProcessImageBuff(MyProgram *data)
{
   int r, c, maxr, maxc;

   unsigned char rv, gv, bv;
   char *ras_p, *temp_ras_p;

   ras_p = (char *)&data->data.current.raster[0][0];
   temp_ras_p = (char *)&data->data.temp.raster[0][0];

   maxr = data->data.current.rsize;
   maxc = data->data.current.csize;

   /* map current color according to LUT entry */
   for (r = 0; r < maxr; r++)
   {
      for (c = 0; c < maxc; c++)
      {
         rv = *(ras_p++);
         gv = *(ras_p++);
         bv = *(ras_p++);

         *(temp_ras_p++) = data->data.lut[rv];
         *(temp_ras_p++) = data->data.lut[gv];
         *(temp_ras_p++) = data->data.lut[bv];
      }
   }

   data->data.temp.rsize = maxr;
   data->data.temp.csize = maxc;

   CopyImageBuff(&data->data.temp, &data->data.current);
}


/******************************************************************************
*  MedianFilterImageBuff - applies median filter to current image buffer      *
*                                                                             *
******************************************************************************/
void
MedianFilterImageBuff(MyProgram *data)
{
   int small, i, r, c, maxr, maxc;

   int row_offset;

   COLOR_S_T *current_p, *temp_p;

   BOOLEAN_T not_done = TRUE;

   COLOR_S_T temp_color, med[9];	/* entry 4 is median after sorting */


   current_p = &data->data.current.raster[0][0];
   temp_p = &data->data.temp.raster[0][0];

   maxr = data->data.current.rsize;
   maxc = data->data.current.csize;

   for (r = 1; r < (maxr - 1); r++)
   {
      for (c = 1; c < (maxc - 1); c++)
      {
         /* copy current pixel and neighbors into median array */
         row_offset = (r-1) * maxc;
         med[0].r = (current_p + row_offset + (c-1))->r;
         med[0].g = (current_p + row_offset + (c-1))->g;
         med[0].b = (current_p + row_offset + (c-1))->b;

         med[1].r = (current_p + row_offset + (c))->r;
         med[1].g = (current_p + row_offset + (c))->g;
         med[1].b = (current_p + row_offset + (c))->b;

         med[2].r = (current_p + row_offset + (c+1))->r;
         med[2].g = (current_p + row_offset + (c+1))->g;
         med[2].b = (current_p + row_offset + (c+1))->b;

         row_offset = (r) * maxc;
         med[3].r = (current_p + row_offset + (c-1))->r;
         med[3].g = (current_p + row_offset + (c-1))->g;
         med[3].b = (current_p + row_offset + (c-1))->b;

         med[4].r = (current_p + row_offset + (c))->r;
         med[4].g = (current_p + row_offset + (c))->g;
         med[4].b = (current_p + row_offset + (c))->b;

         med[5].r = (current_p + row_offset + (c+1))->r;
         med[5].g = (current_p + row_offset + (c+1))->g;
         med[5].b = (current_p + row_offset + (c+1))->b;

         row_offset = (r+1) * maxc;
         med[6].r = (current_p + row_offset + (c-1))->r;
         med[6].g = (current_p + row_offset + (c-1))->g;
         med[6].b = (current_p + row_offset + (c-1))->b;

         med[7].r = (current_p + row_offset + (c))->r;
         med[7].g = (current_p + row_offset + (c))->g;
         med[7].b = (current_p + row_offset + (c))->b;

         med[8].r = (current_p + row_offset + (c+1))->r;
         med[8].g = (current_p + row_offset + (c+1))->g;
         med[8].b = (current_p + row_offset + (c+1))->b;

         /* sort median array */
         for (small = 0; small < 9; small++)
         {
            for (i = (small+1); i < 9; i++)
            {
               if (RGB2GREY(med[small].r, med[small].g, med[small].b)
				> RGB2GREY(med[i].r, med[i].g, med[i].b))
               {
                  temp_color.r = med[small].r;
                  temp_color.g = med[small].g;
                  temp_color.b = med[small].b;

                  med[small].r = med[i].r;
                  med[small].g = med[i].g;
                  med[small].b = med[i].b;

                  med[i].r = temp_color.r;
                  med[i].g = temp_color.g;
                  med[i].b = temp_color.b;
               }
            }
         }

         /* copy median value into temp image buffer */
         row_offset = (r) * maxc;
         (temp_p + row_offset + (c))->r = med[4].r;
         (temp_p + row_offset + (c))->g = med[4].g;
         (temp_p + row_offset + (c))->b = med[4].b;
      }
   }

   data->data.temp.rsize = maxr;
   data->data.temp.csize = maxc;

   CopyImageBuff(&data->data.temp, &data->data.current);
}


/******************************************************************************
*  SetDefaultBlurFilter - negates pixel values of image in current buffer     *
*                                                                             *
******************************************************************************/
void
SetDefaultBlurFilter(MyProgram *data)
{
   int r, c;

   data->data.blur.r = 3;
   data->data.blur.c = 3;

   data->data.blur.divisor = 9.0;

   for (r = 0; r < data->data.blur.r; r++)
      for (c = 0; c < data->data.blur.c; c++)
         data->data.blur.m[r][c] = 1.0;
}


/******************************************************************************
*  BlurImageBuff - apply blurring filter to current image buffer              *
*                                                                             *
******************************************************************************/
void
BlurImageBuff(MyProgram *data)
{
   int maxr, maxc;
   COLOR_S_T *current_p, *temp_p;

   current_p = &data->data.current.raster[0][0];
   temp_p = &data->data.temp.raster[0][0];

   maxr = data->data.current.rsize;
   maxc = data->data.current.csize;

   Convolve(&data->data.blur, maxr, maxc, current_p, temp_p);

   data->data.temp.rsize = maxr;
   data->data.temp.csize = maxc;

   CopyImageBuff(&data->data.temp, &data->data.current);
}


/******************************************************************************
*  SetDefaultSharpenFilter - set filter to be a sharpening filter             *
*                                                                             *
******************************************************************************/
void
SetDefaultSharpenFilter(MyProgram *data)
{
   int r, c;

   data->data.sharp.r = 3;
   data->data.sharp.c = 3;

   data->data.sharp.divisor = 1.0;

   for (r = 0; r < data->data.sharp.r; r++)
      for (c = 0; c < data->data.sharp.c; c++)
         data->data.sharp.m[r][c] = -1.0;

   data->data.sharp.m[1][1] = 9.0;
}


/******************************************************************************
*  SharpenImageBuff - apply sharpening filter to current image buffer         *
*                                                                             *
******************************************************************************/
void
SharpenImageBuff(MyProgram *data)
{
   int maxr, maxc;
   COLOR_S_T *current_p, *temp_p;

   current_p = &data->data.current.raster[0][0];
   temp_p = &data->data.temp.raster[0][0];

   maxr = data->data.current.rsize;
   maxc = data->data.current.csize;

   Convolve(&data->data.sharp, maxr, maxc, current_p, temp_p);

   data->data.temp.rsize = maxr;
   data->data.temp.csize = maxc;

   CopyImageBuff(&data->data.temp, &data->data.current);
}


/******************************************************************************
*  UpdateDisplay - updates screen display following changes in the database   *
******************************************************************************/
void
UpdateDisplay(MyProgram *data)
{
   ClearScreenDisplay();

   if (data->project_loaded)
   {
      DrawImageBuffer(data);

      GenerateHistogram(data);

      DrawHistogram(data);

      DrawLUT(data);
   }
}


/******************************************************************************
*  Convolve - apply convolution filter to image buffer                        *
*                                                                             *
******************************************************************************/
void
Convolve(FILTER_S_T *filter, int maxr, int maxc, 
		COLOR_S_T *src_p, COLOR_S_T *dst_p)
{
   int r, c;
   int row_off1, row_off2, row_off3;
   float r_sum, g_sum, b_sum;


   for (r = 1; r < (maxr - 1); r++)
   {
      for (c = 1; c < (maxc - 1); c++)
      {
         row_off1 = (r-1) * maxc;
         row_off2 = (r) * maxc;
         row_off3 = (r+1) * maxc;

         r_sum =  (src_p + row_off1 + (c-1))->r * filter->m[0][0]
		+ (src_p + row_off1 + (c))->r   * filter->m[0][1]
		+ (src_p + row_off1 + (c+1))->r * filter->m[0][2]
		+ (src_p + row_off2 + (c-1))->r * filter->m[1][0]
		+ (src_p + row_off2 + (c))->r   * filter->m[1][1]
		+ (src_p + row_off2 + (c+1))->r * filter->m[1][2]
		+ (src_p + row_off3 + (c-1))->r * filter->m[1][0]
		+ (src_p + row_off3 + (c))->r   * filter->m[2][1]
		+ (src_p + row_off3 + (c+1))->r * filter->m[2][2];

         (dst_p + row_off2 + c)->r = CLAMP((int) (r_sum / filter->divisor), 0, MAX_PIXEL_VALUE);

         g_sum =  (src_p + row_off1 + (c-1))->g * filter->m[0][0]
		+ (src_p + row_off1 + (c))->g   * filter->m[0][1]
		+ (src_p + row_off1 + (c+1))->g * filter->m[0][2]
		+ (src_p + row_off2 + (c-1))->g * filter->m[1][0]
		+ (src_p + row_off2 + (c))->g   * filter->m[1][1]
		+ (src_p + row_off2 + (c+1))->g * filter->m[1][2]
		+ (src_p + row_off3 + (c-1))->g * filter->m[2][0]
		+ (src_p + row_off3 + (c))->g   * filter->m[2][1]
		+ (src_p + row_off3 + (c+1))->g * filter->m[2][2];

         (dst_p + row_off2 + c)->g = CLAMP((int) (g_sum / filter->divisor), 0, MAX_PIXEL_VALUE);

         b_sum =  (src_p + row_off1 + (c-1))->b * filter->m[0][0]
		+ (src_p + row_off1 + (c))->b   * filter->m[0][1]
		+ (src_p + row_off1 + (c+1))->b * filter->m[0][2]
		+ (src_p + row_off2 + (c-1))->b * filter->m[1][0]
		+ (src_p + row_off2 + (c))->b   * filter->m[1][1]
		+ (src_p + row_off2 + (c+1))->b * filter->m[1][2]
		+ (src_p + row_off3 + (c-1))->b * filter->m[2][0]
		+ (src_p + row_off3 + (c))->b   * filter->m[2][1]
		+ (src_p + row_off3 + (c+1))->b * filter->m[2][2];

         (dst_p + row_off2 + c)->b = CLAMP((int) (b_sum / filter->divisor), 0, MAX_PIXEL_VALUE);
      }
   }
}


/* end of image.c */
