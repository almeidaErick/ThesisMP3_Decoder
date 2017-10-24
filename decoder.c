#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "tables.h"
#include "math.h"
#include "decoder.h"
#include <xsdps.h>
#include "adventures_with_ip.h"
#include "xtime_l.h"


u32 *baseaddr_p = (u32*) IMDCT_BASE;



unsigned int counter;



struct huffcodetab ht[HTN]; /* array of all huffcodtable heasers */
                               /* 0...31 Huffman code table 0..31 */
                               /* 32,33 count-tables */





//-----------------------------------------------------------------------------------------
long int is[SBLIMIT][SSLIMIT];


//----------------------------------------------------------------------------------------
float ro[2][SBLIMIT][SSLIMIT];
float lr[2][SBLIMIT][SSLIMIT];
float re[SBLIMIT][SSLIMIT];
float hybridIn[SBLIMIT][SSLIMIT];/* Hybrid filter input */
float hybridOut[SBLIMIT][SSLIMIT];/* Hybrid filter out */
float polyPhaseIn[SBLIMIT]; /* PolyPhase Input. */
short pcm_sample[2][SSLIMIT][SBLIMIT];
unsigned short pcm_sample_last[2][SSLIMIT][SBLIMIT];




float Ci[8]={-0.6,-0.535,-0.33,-0.185,-0.095,-0.041,-0.0142,-0.0037};





//--------------------Bit allocation for scalefactor information---------------------
//table 25, page 173
int slen[2][16] = {{0, 0, 0, 0, 3, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4},
                   {0, 1, 2, 3, 0, 1, 2, 3, 1, 2, 3, 1, 2, 3, 2, 3}};


//-----------------Boundaries between high and low frequencies------------------
//table 26, page 174
struct {
   int l[5];
   int s[3];} sfbtable = {{0, 6, 11, 16, 21},
                          {0, 6, 12}};


   //--------------------------------------------------------------------------------------
   struct  {
	   int l[23];
	   int s[14];} sfBandIndex[6] =
         {{{0,6,12,18,24,30,36,44,54,66,80,96,116,140,168,200,238,284,336,396,464,522,576},
          {0,4,8,12,18,24,32,42,56,74,100,132,174,192}},
         {{0,6,12,18,24,30,36,44,54,66,80,96,114,136,162,194,232,278,330,394,464,540,576},
          {0,4,8,12,18,26,36,48,62,80,104,136,180,192}},
         {{0,6,12,18,24,30,36,44,54,66,80,96,116,140,168,200,238,284,336,396,464,522,576},
          {0,4,8,12,18,26,36,48,62,80,104,134,174,192}},

         {{0,4,8,12,16,20,24,30,36,44,52,62,74,90,110,134,162,196,238,288,342,418,576},
          {0,4,8,12,16,22,30,40,52,66,84,106,136,192}},
         {{0,4,8,12,16,20,24,30,36,42,50,60,72,88,106,128,156,190,230,276,330,384,576},
          {0,4,8,12,16,22,28,38,50,64,80,100,126,192}},
         {{0,4,8,12,16,20,24,30,36,44,54,66,82,102,126,156,194,240,296,364,448,550,576},
          {0,4,8,12,16,22,30,42,58,78,104,138,180,192}}};

//-----------------------------------------------------------------------------------------
unsigned short sample_rate[4] = {44100, 48000, 32000, 0};

//-----------------------------------------------------------------------------------------
short bit_rate_table[16]= {0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 0};

//-----------------------------------------------------------------------------------------
HUFFBITS dmask = (HUFFBITS)1 << (sizeof(HUFFBITS)*8-1);


//---------------------------------global variable for huffmn tables-------------------
   struct huffinfo hufftab[HTN] = {
       {  0,  0,  0,  0, -1, NULL},		/* 0*/
       {  7,  2,  2,  0, -1, treedata_1},		/* 1*/
       { 17,  3,  3,  0, -1, treedata_2},		/* 2*/
       { 17,  3,  3,  0, -1, treedata_3},		/* 3*/
       {  0,  0,  0,  0, -1, NULL},		/* 4*/
       { 31,  4,  4,  0, -1, treedata_5},		/* 5*/
       { 31,  4,  4,  0, -1, treedata_6},		/* 6*/
       { 71,  6,  6,  0, -1, treedata_7},		/* 7*/
       { 71,  6,  6,  0, -1, treedata_8},		/* 8*/
       { 71,  6,  6,  0, -1, treedata_9},		/* 9*/
       {127,  8,  8,  0, -1, treedata_10},		/* 10*/
       {127,  8,  8,  0, -1, treedata_11},		/* 11*/
       {127,  8,  8,  0, -1, treedata_12},		/* 12*/
       {511, 16, 16,  0, -1, treedata_13},		/* 13*/
       {  0,  0,  0,  0, -1, NULL},		/* 14*/
       {511, 16, 16,  0, -1, treedata_15},		/* 15*/
       {511, 16, 16,  1, -1, treedata_16},		/* 16*/
       {511, 16, 16,  2, 16, NULL},		/* 17*/
       {511, 16, 16,  3, 16, NULL},		/* 18*/
       {511, 16, 16,  4, 16, NULL},		/* 19*/
       {511, 16, 16,  6, 16, NULL},		/* 20*/
       {511, 16, 16,  8, 16, NULL},		/* 21*/
       {511, 16, 16, 10, 16, NULL},		/* 22*/
       {511, 16, 16, 13, 16, NULL},		/* 23*/
       {512, 16, 16,  4, -1, treedata_24},		/* 24*/
       {512, 16, 16,  5, 24, NULL},		/* 25*/
       {512, 16, 16,  6, 24, NULL},		/* 26*/
       {512, 16, 16,  7, 24, NULL},		/* 27*/
       {512, 16, 16,  8, 24, NULL},		/* 28*/
       {512, 16, 16,  9, 24, NULL},		/* 29*/
       {512, 16, 16, 11, 24, NULL},		/* 30*/
       {512, 16, 16, 13, 24, NULL},		/* 31*/
       { 31,  1, 16,  0, -1, treedata_32},		/* 32*/
       { 31,  1, 16,  0, -1, treedata_33}		/* 33*/
   };

   //----------------------------------------------------------------------------------------
   static int pretab[22] = {0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,2,2,3,3,3,2,0};



   void get_scale_factors(NewBuffer* load_info, SideInfoType* side_info, scalefac_type *scalefac, int gr, int ch) {
       int	sfb, i, window;
       struct gr_info_type	*gr_info = &(side_info->ch[ch].gr[gr]);

       if (gr_info->window_switching_flag && (gr_info->block_type == 2)) {

           if (gr_info->mixed_block_flag) {	/* MIXED */ /* NEW - ag 11/25 */
               for (sfb = 0; sfb < 8; sfb++)
                   scalefac->ch[ch].l[sfb] =
                   get_bits2(load_info, slen[0][gr_info->scalefac_compress]);
               for (sfb = 3; sfb < 6; sfb++)
                   for (window=0; window<3; window++)
                       scalefac->ch[ch].s[window][sfb] =
                       get_bits2(load_info, slen[0][gr_info->scalefac_compress]);
               for (sfb = 6; sfb < 12; sfb++)
                   for (window=0; window<3; window++)
                       scalefac->ch[ch].s[window][sfb] =
                       get_bits2(load_info, slen[1][gr_info->scalefac_compress]);
               for (sfb=12,window=0; window<3; window++)
                   scalefac->ch[ch].s[window][sfb] = 0;

           } else {				/* SHORT*/

               for (i=0; i<2; i++)
                   for (sfb = sfbtable.s[i]; sfb < sfbtable.s[i+1]; sfb++)
                       for (window=0; window<3; window++)
                       scalefac->ch[ch].s[window][sfb] =
                           get_bits2(load_info, slen[i][gr_info->scalefac_compress]);
               for (sfb=12,window=0; window<3; window++)
                   scalefac->ch[ch].s[window][sfb] = 0;
           }

       } else {   /* LONG types 0,1,3 */


           for (i=0; i<4; i++) {
               if ((side_info->ch[ch].scfsi[i] == 0) || (gr == 0))
               for (sfb = sfbtable.l[i]; sfb < sfbtable.l[i+1]; sfb++)
                   scalefac->ch[ch].l[sfb] =
                   get_bits2(load_info, slen[(i<2)?0:1][gr_info->scalefac_compress]);
           }
           scalefac->ch[ch].l[21] = 0; //------------------------------------------Here change 22 for 21


       /*

           for (sfb=0; sfb < 11; sfb++)
               scalefac->ch[ch].l[sfb] = get_bits2(load_info,slen[0][gr_info->scalefac_compress]);
           for (sfb=11;sfb < 21;sfb++)
               scalefac->ch[ch].l[sfb] = get_bits2(load_info,slen[1][gr_info->scalefac_compress]);
           scalefac->ch[ch].l[21] = 0;

       */



       }
   }



   void create_syn_filter(double doublefilter[64][SBLIMIT]) {
       register int i, k;
       for(i = 0; i < 64; i++) {
           for(k = 0; k < 32; k++) {
               doublefilter[i][k] = 1e9 * cos((PI64*i+PI4)*(2*k+1));
               if (doublefilter[i][k] >= 0) {
                   /*modff(filter[i][k]+0.5, &filter[i][k]);*/
                   doublefilter[i][k] = floor(doublefilter[i][k] + 0.5);
               } else {
                   /*modff(filter[i][k]-0.5, &filter[i][k]);*/
                   doublefilter[i][k] = floor(doublefilter[i][k] - 0.5);
               }
               doublefilter[i][k] *= 1e-9;
           }
       }
   }




   /* Shift left and right channels for AIFF output */
   // code from program

   /*
   static void swapWordsInLong(short *loc, int words)
   {
       int i;
       unsigned short	 dst0, dst1;
       unsigned short	*src = (unsigned short*)loc;

       for (i = 0; i < words; i += 2) {
   	dst0 = src[0];
   	dst1 = src[1];
   	*src++ = dst1;
   	*src++ = dst0;
       }
   }*/


   void SwapBytesInWords(short *loc, int words)
   {
       int		   i;
       unsigned char *src = (unsigned char *)loc;
       unsigned char  tmp;

       for (i = 0; i < words; i++) {
           tmp = src[0];
           src[0] = src[1];
           src[1] = tmp;
           src += 2;
       }
   }



   /*
   void SwapBytesInWords( short *loc, int words)
   {
    int i;
    short thisval;
    char *dst, *src;
    src = (char *) &thisval;
    for ( i = 0; i < words; i++ )
    {
       thisval = *loc;
       dst = (char *) loc++;
       dst[0] = src[1];
       dst[1] = src[0];
    }
   }*/




   // Code from thesis
   /*
   void swapWordsInLong( short *loc, int words)
   {
       int i;
       short thisval;
       char *dst, *src;
       src = (char *) &thisval;
       for ( i = 0; i < words; i++ ){
           thisval = *loc;
           dst = (char *) loc++;
           dst[0] = src[1];
           dst[1] = src[0];
       }
   } */


   void out_fifo(short pcm_sample[2][SSLIMIT][SBLIMIT], FIL *outFile, unsigned long	*psampFrames, FrameHeader* header_info) {
       int stereo = header_info->get_stereo;
       int	i, j, l;
       static short int outsamp[BUFFERSIZE];
       static long k = 0;
       unsigned int count;
//       FILE* left;
//       FILE* right;
//
//       if((left = fopen("left.txt", "w")) == NULL) {
//    	   xil_printf("Error! opening file");
//           exit(1);
//       } else {
//    	   xil_printf("Se abrio el archivo\r\n");
//       }
//
//
//       if((right = fopen("right.txt", "w")) == NULL) {
//    	   xil_printf("Error! opening file");
//           exit(1);
//       } else {
//    	   xil_printf("Se abrio el archivo\r\n");
//       }

       //xil_printf("SAMPLES: ");

       for(i = 0; i < SSLIMIT; i++) {
           for(j = 0; j < SBLIMIT; j++) {
               (*psampFrames)++;
               for(l = 0; l < stereo; l++) {
            	   //xil_printf("VALOR DE K: %lu\n\r", k);
                   if(!(k & (BUFFERSIZE-1)) && k) {
                       //Samples are big-endian. If thisis a little-endian machine we must swap
                       //SwapBytesInWords(outsamp, BUFFERSIZE);
//                       if(l == 0) {
//                           fwrite(outsamp, 2, BUFFERSIZE, outFile);
//                       }
                       f_write(outFile, outsamp, BUFFERSIZE, count);
                       //xil_printf(outFile, "%d %d", outsamp[0], outsamp[1]);
                       k = 0;
                   }

                   //pcm_sample_last[l][i][j] = pcm_sample[l][i][j] + 32765;

                   //if(pcm_sample[l][i][j] < 0) {
                       //pcm_sample_last[l][i][j] = pcm_sample[l][i][j] + 32765;
                   //    pcm_sample_last[l][i][j] = pcm_sample[l][i][j] & (0x8000);

                   //} else {
                   //    pcm_sample_last[l][i][j] = pcm_sample[l][i][j];
                   //}
   /*
                   if(l == 0) {
                       xil_printf(outFile, "%d, ", pcm_sample_last[l][i][j]);
                   } else {
                       xil_printf(right, "%d ", pcm_sample_last[l][i][j]);
                   }*/
                   //xil_printf(outFile, "%d ", pcm_sample[l][i][j]);
                   outsamp[k++] = pcm_sample[l][i][j];
                   //xil_printf("%d, ", (int)pcm_sample[l][i][j]);
               }
           }
       }
       //xil_printf("\n\r");

//       fclose(left);
//       fclose(right);
   }



   void SubBandSynthesis(float *bandPtr, int ch, short *samples) {
       register int i, j, k;
       int iii, jjj;
       register float *bufOffsetPtr,sum;
       register long foo;
       static int init = 1;
       typedef float NN[64][32];
       typedef double MM[64][32];
       static NN *filter;
       static MM *doublefilter;
       typedef float BB[2][1024];
       static BB *buf;
       static int bufOffset[2] = { 64, 64 };

       if(init) {
           buf = (BB *) malloc(sizeof(BB));
           doublefilter = (MM *) malloc(sizeof(MM));
           filter = (NN *) malloc(sizeof(NN));
           create_syn_filter(*doublefilter);
           for (iii = 0; iii < 64; iii++) {
               for (jjj = 0; jjj < 32; jjj++)
                   (*filter)[iii][jjj] = (float) (*doublefilter)[iii][jjj];
               ;
           }
           init = 0;
       }
       bufOffset[ch] = (bufOffset[ch] - 64) & 0x3ff;
       bufOffsetPtr = &((*buf)[ch][bufOffset[ch]]);
       //xil_printf("BAND PTR: ");
       for (i = 0; i < 64; i++) {
           sum = 0;
           for (k = 0; k < 32; k++){
        	   if(i == 1){
        	   //xil_printf("%d, ", (int)(bandPtr[k] * 32750));
        	   }
               sum += bandPtr[k] * 32767 *  (*filter)[i][k];
           }
           bufOffsetPtr[i] = sum;

       }


       for (j = 0; j < 32; j++) {
           sum = 0;
           for (i = 0; i < 16; i++) {
               k = j + (i << 5);
               sum += dewindow[k] * 32767 * (*buf)[ch][((k + (((i + 1) >> 1) << 6))
               + bufOffset[ch]) & 0x3ff];
           }



           // I add this here..!!!
           sum = sum / 32767.0;

           //xil_printf("%d, ", (int)(sum));

           if (sum > 0) {
               //foo = (long) (sum / (float) SCALE + (float) 0.5);
               foo = (long) (sum + 0.5);

           } else {
               //foo = (long) (sum / (float) SCALE - (float) 0.5);
               foo = (long) (sum - 0.5);
           }
           //xil_printf("%d, ", (int)foo);
//           if (foo >= (long) SCALE) {
//               samples[j] = (short) (SCALE - 1);
//           } else if (foo < (long) -SCALE) {
//               samples[j] = (short) (-SCALE);
//           } else
//               samples[j] = (short) foo;

           //xil_printf("%d, ", samples[j]);
           // faster..!!!!
           samples[j] = (int)foo;
           //xil_printf("%d, ", samples[j]);
           //xil_printf("SAMPLE: %d\n\r", samples[j]);
           //pcm_output[frameNum-1][gr_global][ch_global][ss_global][j]=samples[j];
       }
       //xil_printf("\n\r");
   }



   void inv_mdct(
              float in[18],
              float out[36],
              int	block_type) {
	   int i; // used only for loops
	   u32 hold = 0;

	   u32 *register_1 = baseaddr_p+0; // pointer for Input of 32 bits
	   u32 *register_2 = baseaddr_p+1; // pointer for output of 32 bits
	   u32 *register_3 = baseaddr_p+2; // address_in(8-bits), address_out(8-bits), block_type(2-bits), done_writing_input(1-bit), mode_input_output(1-bit) m=1 to write, m=0 to read
	   u32 *register_4 = baseaddr_p+3; // pointer to check when the output samples are ready

	   /*
	    *  write here samples to input RAM
	    *
	    */
	   for (i = 0; i < 18; i++) {
		   // setting hold as: input address defined by i, defining block type value, and setting input block ram as writing mode.
		   hold = (i << INPUT_ADDRESS) | (block_type << BLOCK_TYPE_ADDRESS) | (1 << INPUT_RAM_MODE); //not neccesary to specify done_writing bit and address_out bits
		   *(register_3) = hold;
		   *(register_1) = i+1; // send input here

		   //xil_printf("value: %d, block_type: %d\n\r", *(register_3), block_type);
	   }

	   xil_printf("value: %d, block_type: %d\n\r", *(register_3), block_type);

	   /*
	    * Before sending signal of finishing writing to input ram, remember to change the input block ram mode to read
	    * check register_3
	    */


	   /*
	    * change mode of operation of input block ram... change to read mode
	    * this section is proved to be working
	   	*/

	   *(register_3) = *(register_3) ^ (1 << INPUT_RAM_MODE);

	   /**
	    * send signal of finish writing here..!!
	    * this signal has been tested, if not present the program will wait "forever"
	    * this section is proved to be working
	    */
	   *(register_3) = *(register_3) | (1 << DONE_WRITING);

	   /*
	    * wait until the FPGA has done its working
	    */
	   //xil_printf("output before: %d, send input: %d, block type: %d\n\r", *(register_4), *(register_3), block_type);
	   while(*(register_4) != 1) {
		   //xil_printf("output during: %d, send input: %d, block type: %d\n\r", *(register_4), *(register_3), block_type);
	   }

	   /*
	    * Read samples from output ram
	    */

	   //*(register_3) = 0;
	   for (i = 0; i < 36; i++) {
		   hold = (1 << DONE_WRITING) | (i << OUTPUT_ADDRESS); //only the output address to read nees to be set here
		   *(register_3) = hold;
		   //if(*(register_2) != 0) {
		   if(block_type != 0) {
		   	   //xil_printf("OUTPUT SAMPLE: %d, INDEX: %d, BLOCK TYPE: %d\n\r", *(register_2), i, block_type);
		   }

		   //xil_printf("OUTPUT ADDRESS: %d\n\r", *(register_3));
	   }

	   //xil_printf("output after: %d, send input: %d, block type: %d\n\r", *(register_4), *(register_3), block_type);
	   *(register_3) = 0; //activate reset mode for output block...!!
	   //xil_printf("output done: %d\n\r", *(register_4));


   }




//   void inv_mdct(
//           float in[18],
//           float out[36],
//           int	block_type) {
//
//       int i,m,N,p;
//       float tmp[12],sum;
//       for(i = 0; i < 36; i++)
//           out[i] = 0;
//
//       if(block_type == 2) {
//           N = 12;
//           for(i = 0; i < 3; i++) {
//               for(p = 0; p < N; p++) {
//                   sum = 0.0;
//                   for(m = 0; m < N/2; m++) {
//                       int k = (m) + (m<<1);
//                       sum += in[i+k] * (cos_tab_1[p][m]);
//                   }
//                   tmp[p] = sum * win[block_type][p] ;
//               }
//               for(p = 0; p < N; p++) {
//                   int k = (i<<1) + (i<<2);
//                   out[k+p+6] += tmp[p];
//
//               }
//           }
//       } else {
//           N=36;
//           for(p= 0;p<N;p++){
//               sum = 0.0;
//               for(m=0;m<N/2;m++){
//                   int k = ((p*m)<<2) + (p<<1) + (m<<5)+(m<<2)+(m<<1)+19;
//                   while(k>=144){
//                       k = k - 144;
//                   }
//                   sum += in[m] * COS[k];
//               }
//               out[p] = sum * win[block_type][p];
//           }
//       }
////       xil_printf("OUTPUT: ");
////       for(i = 0; i < 32; i++) {
////    	   xil_printf("%d, ", out[i]);
////       }
////       xil_printf("\n\r");
//   }



   void hybrid(
           float fsIn[SSLIMIT], /* freq samples persubband in */
           float tsOut[SSLIMIT], /* time samples per subband out */
           int sb,
           int ch,
           struct gr_info_type *gr_info,
           FrameHeader* header_info) {

       int ss;
       float rawout[36];
       static float prevblck[2][SBLIMIT][SSLIMIT];
       static int init = 1;
       int bt;

       if (init == 1) {
           int i, j, k;

           for (i = 0; i < 2; i++)
               for (j = 0; j < SBLIMIT; j++)
                   for (k = 0; k < SSLIMIT; k++)
                       prevblck[i][j][k] = 0.0;
           init = 0;
       }

       bt = (gr_info->window_switching_flag && gr_info->mixed_block_flag &&
               (sb < 2)) ? 0 : gr_info->block_type;

       //inv_mdct
       inv_mdct(fsIn, rawout, bt);
       /* Overlap addition */

       //xil_printf("OUTPUT: ");

       for (ss = 0; ss < SSLIMIT; ss++) {		/* 18 */
           tsOut[ss] = rawout[ss] + prevblck[ch][sb][ss];
           prevblck[ch][sb][ss] = rawout[ss+18];
           //xil_printf("%d, ", (int)prevblck[ch][sb][ss]);
       }
       //xil_printf("\n\r");
   }




   void antialias(
           float xr[SBLIMIT][SSLIMIT],
           float hybridIn[SBLIMIT][SSLIMIT],
           struct gr_info_type *gr_info,
           FrameHeader* header_info) {

       static int init = 1;
       static float ca[8],cs[8];
       float bu,bd; /* upper and lower butterfly inputs */
       int ss,sb,sblim;

       if (init == 1) {
           int	i;
           float sq;
           for (i = 0; i < 8; i++) {
               sq = sqrt(1.0 + Ci[i] * Ci[i]);
               cs[i] = 1.0 / sq;
               ca[i] = Ci[i] / sq;
           }
           init = 0;
       }
       /* Clear all inputs */
       for(sb=0;sb<SBLIMIT;sb++)
           for(ss=0;ss<SSLIMIT;ss++)
               hybridIn[sb][ss] = xr[sb][ss];

       if  (gr_info->window_switching_flag && (gr_info->block_type == 2) &&
   			!gr_info->mixed_block_flag )
   	    return;

       if ( gr_info->window_switching_flag && gr_info->mixed_block_flag &&
   			(gr_info->block_type == 2))
   	    sblim = 1;
       else
   	    sblim = SBLIMIT-1;

       /*
        * 31 alias-reduction operations between each pair of sub-bands
        * with 8 butterflies between each pair
        */
       for (sb = 0; sb < sblim; sb++)
           for (ss = 0; ss < 8; ss++) {
               bu = xr[sb][17-ss];
               bd = xr[sb+1][ss];
               hybridIn[sb][17-ss] = (bu * cs[ss]) - (bd * ca[ss]);
               hybridIn[sb+1][ss] = (bd * cs[ss]) + (bu * ca[ss]);
           }
   }



   void reorder(
           float xr[SBLIMIT][SSLIMIT],
           float ro[SBLIMIT][SSLIMIT],
           struct gr_info_type *gr_info,
           FrameHeader* header_info) {

       int	sfreq;
       int	sfb, sfb_start, sfb_lines;
       int	sb, ss, window, freq, src_line, des_line;
       //sfreq = fr_ps->header->sampling_frequency + (fr_ps->header->version * 3);
       sfreq = header_info->sampling_frequency_index + 3;


       for (sb=0; sb<SBLIMIT; sb++)
           for (ss=0; ss<SSLIMIT; ss++)
               ro[sb][ss] = 0.0;

       if(gr_info->window_switching_flag && (gr_info->block_type == 2)) {
           if(gr_info->mixed_block_flag) {
               /* No reorder for low 2 subbands */
   	        for (sb=0 ; sb < 2 ; sb++)
   		        for (ss=0 ; ss < SSLIMIT ; ss++) {
   		            ro[sb][ss] = xr[sb][ss];
   		        }
   	        /* Reordering for rest switched short */
               for (sfb = 3, sfb_start = sfBandIndex[sfreq].s[3],
                       sfb_lines=sfBandIndex[sfreq].s[4] - sfb_start;
                       sfb < 13; sfb++,sfb_start=sfBandIndex[sfreq].s[sfb],
                       (sfb_lines=sfBandIndex[sfreq].s[sfb+1] - sfb_start))
   		        for(window=0; window<3; window++)
   		            for(freq=0;freq<sfb_lines;freq++) {
                           src_line = sfb_start*3 + window*sfb_lines + freq;
                           des_line = (sfb_start*3) + window + (freq*3);
                           ro[des_line/SSLIMIT][des_line%SSLIMIT] =
                               xr[src_line/SSLIMIT][src_line%SSLIMIT];
   		            }
           } else {    /* pure short */
               for (sfb=0,sfb_start=0,sfb_lines=sfBandIndex[sfreq].s[1];
                       sfb < 13; sfb++,sfb_start=sfBandIndex[sfreq].s[sfb],
                       (sfb_lines=sfBandIndex[sfreq].s[sfb+1] - sfb_start))
   		        for (window=0; window<3; window++)
   		            for(freq=0;freq<sfb_lines;freq++) {
                           src_line = sfb_start*3 + window*sfb_lines + freq;
                           des_line = (sfb_start*3) + window + (freq*3);
                           ro[des_line/SSLIMIT][des_line%SSLIMIT] =
                               xr[src_line/SSLIMIT][src_line%SSLIMIT];
   		            }

           }
       } else {    /* long blocks */
           for (sb=0 ; sb < SBLIMIT ; sb++)
   	        for (ss=0 ; ss < SSLIMIT ; ss++)
   		        ro[sb][ss] = xr[sb][ss];
       }
   }



   void stereo_decoding(
           float xr[2][SBLIMIT][SSLIMIT],
           float lr[2][SBLIMIT][SSLIMIT],
           scalefac_type *scalefac,
           struct gr_info_type *gr_info,
           FrameHeader* header_info) {

       int	sfreq;
       int	stereo = header_info->get_stereo;
       int	ms_stereo =
   				(header_info->stereo_mode == MPG_MD_JOINT_STEREO) &&
   				(header_info->extension_mode & 0x2);
       int	i_stereo =
   				(header_info->stereo_mode == MPG_MD_JOINT_STEREO) &&
   				(header_info->extension_mode & 0x1);
       int js_bound; /* frequency line that marksthe beggining of the zero part */
       int	sfb, next_sfb_boundary;
       int	i, j, sb, ss, ch;
       short is_pos[SBLIMIT*SSLIMIT];
       float is_ratio[SBLIMIT*SSLIMIT];
       float io;
       float k[2][SBLIMIT*SSLIMIT];


       //int lsf = (fr_ps->header->version == MPEG_PHASE2_LSF);
       int lsf = 0;  // change here..!!!

       if ((gr_info->scalefac_compress % 2) == 1)
   	    io = 0.707106781188;
       else
   	    io = 0.840896415256;

       //sfreq = fr_ps->header->sampling_frequency + (fr_ps->header->version * 3);
       sfreq = header_info->sampling_frequency_index + 3;

       /* Intialization */
       for (i = 0; i < SBLIMIT*SSLIMIT; i++ )
   	    is_pos[i] = 7;

       if((stereo == 2) && i_stereo) {
           if(gr_info->window_switching_flag && (gr_info->block_type == 2)) {  // short block and mixed block
               if(gr_info->mixed_block_flag) { // mixed block
                   int max_sfb = 0;
                   for(j = 0; j < 3; j++) {
                       int sfbcnt;
                       sfbcnt = 2;
                       for(sfb=12; sfb >=3; sfb--) {
                           int lines;
                           lines = sfBandIndex[sfreq].s[sfb+1] -
                               sfBandIndex[sfreq].s[sfb];
                           i = 3*sfBandIndex[sfreq].s[sfb] + (j+1) * lines - 1;
                           while(lines > 0) {
                               if(xr[1][i/SSLIMIT][i%SSLIMIT] != 0.0) {
                                   sfbcnt = sfb;
                                   sfb = -10;
                                   lines = -10;
                               }
                               lines--;
                               i--;
                           }
                       }
                       sfb = sfbcnt + 1;

                       if ( sfb > max_sfb )
                           max_sfb = sfb;

                       while(sfb<12) {
                           sb = sfBandIndex[sfreq].s[sfb+1] -
                               sfBandIndex[sfreq].s[sfb];
                           i = 3*sfBandIndex[sfreq].s[sfb] + j * sb;
                           for( ; sb > 0; sb--) {
                               is_pos[i] = scalefac->ch[1].s[j][sfb];
                               if(is_pos[i] != 7) {
                                   if(lsf) {
                                       // add here unctions for LSF
                                   } else {
                                       is_ratio[i] = tan((float)is_pos[i] * (PI / 12));
                                   }
                               }
                               i++;
                           }
                           sfb++;
                       }
                       sb = sfBandIndex[sfreq].s[12]-sfBandIndex[sfreq].s[11];
                       sfb = 3*sfBandIndex[sfreq].s[11] + j * sb;
                       sb = sfBandIndex[sfreq].s[13]-sfBandIndex[sfreq].s[12];

                       i = 3*sfBandIndex[sfreq].s[11] + j * sb;
                       for( ; sb > 0; sb--) {
                           is_pos[i] = is_pos[sfb];
                           is_ratio[i] = is_ratio[sfb];
                           k[0][i] = k[0][sfb];
                           k[1][i] = k[1][sfb];
                           i++;
                       }
                   }
                   if(max_sfb <= 3) {
                       i = 2;
                       ss = 17;
                       sb = -1;
                       while(i >= 0) {
                           if(xr[1][i][ss] != 0.0) {
                               sb = i*18+ss;
   			                i = -1;
                           } else {
                               ss--;
                               if(ss < 0) {
                                   i--;
   				                ss = 17;
                               }
                           }
                       }
                       i = 0;
                       while ( sfBandIndex[sfreq].l[i] <= sb )
   			            i++;
                       sfb = i;
   		            i = sfBandIndex[sfreq].l[i];
                       for( ; sfb<8; sfb++) {
                           sb = sfBandIndex[sfreq].l[sfb+1]-sfBandIndex[sfreq].l[sfb];
                           for( ; sb > 0; sb--) {
                               is_pos[i] = scalefac->ch[1].l[sfb];
                               if(is_pos[i] != 7) {
                                   if(lsf) {
                                       // add here unctions for LSF
                                   } else {
                                       is_ratio[i] = tan((float)is_pos[i] * (PI / 12));
                                   }
                               }
                               i++;
                           }
                       }
                   }
               } else { //end mixed block ---> short block
                   for(j=0; j<3; j++) {
                       int sfbcnt;
   		            sfbcnt = -1;
                       for(sfb=12; sfb >=0; sfb--) {
                           int lines;
                           lines = sfBandIndex[sfreq].s[sfb+1]-sfBandIndex[sfreq].s[sfb];
                           i = 3*sfBandIndex[sfreq].s[sfb] + (j+1) * lines - 1;
                           while(lines > 0) {
                               if(xr[1][i/SSLIMIT][i%SSLIMIT] != 0.0) {
                                   sfbcnt = sfb;
                                   sfb = -10;
                                   lines = -10;
                               }
                               lines--;
   			                i--;
                           }
                       }
                       sfb = sfbcnt + 1;
                       while(sfb<12) {
                           sb = sfBandIndex[sfreq].s[sfb+1]-sfBandIndex[sfreq].s[sfb];
   			            i = 3*sfBandIndex[sfreq].s[sfb] + j * sb;
                           for( ; sb > 0; sb--) {
                               is_pos[i] = scalefac->ch[1].s[j][sfb];
                               if(is_pos[i] != 7) {
                                   if(lsf) {
                                       // add here unctions for LSF
                                   } else {
                                       is_ratio[i] = tan( (float)is_pos[i] * (PI/ 12));
                                   }
                               }
                               i++;
                           }
                           sfb++;
                       }
                       sb = sfBandIndex[sfreq].s[12]-sfBandIndex[sfreq].s[11];
                       sfb = 3*sfBandIndex[sfreq].s[11] + j * sb;
                       sb = sfBandIndex[sfreq].s[13]-sfBandIndex[sfreq].s[12];

                       i = 3*sfBandIndex[sfreq].s[11] + j * sb;
                       for( ; sb > 0; sb--) {
                           is_pos[i] = is_pos[sfb];
                           is_ratio[i] = is_ratio[sfb];
                           k[0][i] = k[0][sfb];
                           k[1][i] = k[1][sfb];
                           i++;
                       }
                   }
               } // end short block
           } else { // long block
               i = 31;
               ss = 17;
               sb = 0;
               while(i >= 0) {
                   if(xr[1][i][ss] != 0.0) {
                       sb = i*18+ss;
   		            i = -1;
                   } else {
                       ss--;
                       if(ss < 0) {
                           i--;
   			            ss = 17;
                       }
                   }
               }
               i = 0;
               while ( sfBandIndex[sfreq].l[i] <= sb )
   		        i++;
               sfb = i;
   	        i = sfBandIndex[sfreq].l[i];
               for( ; sfb<21; sfb++) {
                   sb = sfBandIndex[sfreq].l[sfb+1] - sfBandIndex[sfreq].l[sfb];
                   for( ; sb > 0; sb--) {
                       is_pos[i] = scalefac->ch[1].l[sfb];
                       if(is_pos[i] != 7) {
                           if(lsf) {
                               // add here unctions for LSF
                           } else {
                               is_ratio[i] = tan((float)is_pos[i] * (PI / 12));
                           }
                       }
                       i++;
                   }
               }
               sfb = sfBandIndex[sfreq].l[20];
               for ( sb = 576 - sfBandIndex[sfreq].l[21]; sb > 0; sb-- ) {
                   is_pos[i] = is_pos[sfb];
                   is_ratio[i] = is_ratio[sfb];
                   k[0][i] = k[0][sfb];
                   k[1][i] = k[1][sfb];
                   i++;
               }
           }
       }
       //end if ((stereo == 2) && i_stereo )
       for (ch = 0; ch < 2; ch++)
           for (sb = 0; sb < SBLIMIT; sb++)
               for (ss = 0; ss < SSLIMIT; ss++)
                   lr[ch][sb][ss] = 0;

       if(stereo == 2) {
           for(sb = 0; sb < SBLIMIT; sb++) {
               for(ss = 0; ss < SSLIMIT; ss++) {
                   i = (sb*18)+ss;
                   if ( is_pos[i] == 7 ) {
                       if ( ms_stereo ) {
                           lr[0][sb][ss] = (xr[0][sb][ss]+xr[1][sb][ss])/1.41421356;
                           lr[1][sb][ss] = (xr[0][sb][ss]-xr[1][sb][ss])/1.41421356;
                       } else {
                           lr[0][sb][ss] = xr[0][sb][ss];
                           lr[1][sb][ss] = xr[1][sb][ss];
                       }
                   } else if (i_stereo ) {
                       if ( lsf ) {
                           lr[0][sb][ss] = xr[0][sb][ss] * k[0][i];
                           lr[1][sb][ss] = xr[0][sb][ss] * k[1][i];
                       } else {
                           lr[0][sb][ss] = xr[0][sb][ss] * (is_ratio[i]/(1+is_ratio[i]));
                           lr[1][sb][ss] = xr[0][sb][ss] * (1/(1+is_ratio[i]));
                       }
                   } else {
                	   fprintf(stderr, "Error in stereo processing\n");
                   }
               }
           }
       } else {   /* mono , bypass xr[0][][] to lr[0][][]*/
           for (sb = 0; sb < SBLIMIT; sb++)
               for(ss = 0; ss < SSLIMIT; ss++)
                   lr[0][sb][ss] = xr[0][sb][ss];
       }
   }




   /* Return the current bit stream length (in bits) */
   unsigned long hsstell(NewBuffer* load_info)
   {
     return(load_info->total_number_bits);
   }




   // This function can be used anywhere to get the number of bits that any variable need
   // not a single buffer is used, just a variable is used to keep the last character read
   // from the mp3 file.
   unsigned long int get_bits2(NewBuffer* load_info, unsigned long bits_num) {
       unsigned long bits_read = 0;
       int i;
       load_info->total_number_bits += bits_num;
       if(load_info->is_buffer_aval == 0) { //check if the variable read_char has a valid character
    	   if(f_read(load_info->ptr, &load_info->read_char, 1, &counter) == 0){
               load_info->is_buffer_aval = 1;
           } else {
               return 0; //here rise an exception
           }
       }
       for(i = 0; i < bits_num; i++) {

           if((load_info->read_new == 1) && (load_info->bit_count == 0)){
        	   if(f_read(load_info->ptr, &load_info->read_char, 1, &counter) != 0){
                   return 0;
               }
               load_info->count_bytes++;
               load_info->read_new = 0;
           }

           bits_read = bits_read << 1;

           bits_read = (bits_read) | (((load_info->read_char)&0x80)>>7);
           load_info->read_char = load_info->read_char << 1;

           if(load_info->bit_count < 7) {
               load_info->bit_count++;
           } else { // no more bits to read in the byte, get next byte
               load_info->bit_count = 0;
               load_info->read_new = 1;
           }
       }
       return bits_read;
   }



   void requantize(SideInfoType* side_info, scalefac_type *scalefac, FrameHeader* header_info,
   long int is[SBLIMIT][SSLIMIT], int ch, int gr, float xr[SBLIMIT][SSLIMIT]) {
       int ss, sb, cb = 0, sfreq;
       int next_cb_boundary, cb_begin = 0, cb_width = 0, sign;
       float temp;
       struct gr_info_type	*gr_info = &(side_info->ch[ch].gr[gr]);

       sfreq = header_info->sampling_frequency_index + 3;

       //-----------------------assign scalefactorband index--------------------------
       /* Choose correct scalefactor band per block type, initalize boundary */

       if (gr_info->window_switching_flag && (gr_info->block_type == 2))
           if (gr_info->mixed_block_flag)
               next_cb_boundary = sfBandIndex[sfreq].l[1];	/* LONG blocks: 0,1,3 */
           else {
               next_cb_boundary = sfBandIndex[sfreq].s[1]*3;	/* Pure SHORT block */
               cb_width = sfBandIndex[sfreq].s[1];
               cb_begin = 0;
   	} else
   	    next_cb_boundary = sfBandIndex[sfreq].l[1];	/* LONG blocks: 0,1,3 */

       for(sb = 0 ; sb < SBLIMIT ; sb++) {         /* 32 */
           for(ss = 0 ; ss < SSLIMIT ; ss++) {     /* 18 */
               //--------------scale factor band transition---------------
               if((sb * SSLIMIT) + ss == next_cb_boundary) {
                   if(gr_info->window_switching_flag && (gr_info->block_type == 2)) {
                       if(gr_info->mixed_block_flag) { // mixed blocks
                           if(((sb * 18) + ss) == sfBandIndex[sfreq].l[8]) {
                               next_cb_boundary=sfBandIndex[sfreq].s[4] * 3;
                               cb = 3;
                               cb_width = sfBandIndex[sfreq].s[cb+1] -
                                   sfBandIndex[sfreq].s[cb];
                               cb_begin = sfBandIndex[sfreq].s[cb] * 3;
                           } else if(((sb * 18) + ss) < sfBandIndex[sfreq].l[8]) {
                               next_cb_boundary = sfBandIndex[sfreq].l[(++cb) + 1];
                           } else {
                               next_cb_boundary = sfBandIndex[sfreq].s[(++cb) + 1] * 3;
   			                cb_width = sfBandIndex[sfreq].s[cb + 1] - sfBandIndex[sfreq].s[cb];
   			                cb_begin = sfBandIndex[sfreq].s[cb] * 3;
                           }
                       } else { //short blocks
                           next_cb_boundary = sfBandIndex[sfreq].s[(++cb) + 1] * 3;
   			            cb_width = sfBandIndex[sfreq].s[cb+1] - sfBandIndex[sfreq].s[cb];
   			            cb_begin = sfBandIndex[sfreq].s[cb]*3;
                       }
                   } else { /* long blocks */
                       next_cb_boundary = sfBandIndex[sfreq].l[(++cb) + 1];
                   }
               }
               //-----------------------compute scaling for each block--------------------
               //xr[sb][ss] = pow(2.0, (0.25 * (gr_info->global_gain - 210.0)));
               xr[sb][ss] = powertable1[gr_info->global_gain];
               if(gr_info->window_switching_flag && (
   			    ((gr_info->block_type == 2) &&
   			    (gr_info->mixed_block_flag == 0)) ||
   			    ((gr_info->block_type == 2) &&
   			    gr_info->mixed_block_flag && (sb >= 2)) )) { //short blocks

                   //xr[sb][ss] *= pow(2.0, 0.25 * -8.0 * gr_info->subblock_gain[(((sb*18)+ss) - cb_begin)/cb_width]);
                   xr[sb][ss] *= powershort1[gr_info->subblock_gain[(((sb*SSLIMIT)+ss) - cb_begin)/cb_width]];
                   /*xr[sb][ss] *= pow(2.0, 0.25 * -2.0 *
   			        (1.0+gr_info->scalefac_scale) *
   			        (*scalefac)[ch].s[(((sb*18)+ss) -
   			        cb_begin)/cb_width][cb]);*/
                   xr[sb][ss] *= powershort2[(1+gr_info->scalefac_scale)*(scalefac->ch[ch].s[(((sb*SSLIMIT)+ss) -
                       cb_begin)/cb_width][cb])];//-0.25*2

               } else {    /* LONG block types 0,1,3 & */
   			            /* 1st 2 subbands of switched blocks */

                   /*xr[sb][ss] *= pow(2.0, -0.5 * (1.0+gr_info->scalefac_scale)
   					* (scalefac->ch[ch].l[cb]
   					+ gr_info->preflag * pretab[cb]));*/
                   xr[sb][ss] *=powertable2[(1+gr_info->scalefac_scale)*(scalefac->ch[ch].l[cb] + gr_info->preflag * pretab[cb])];

               }
               /* Scale quantized value */
               sign = (is[sb][ss]<0) ? 1 : 0;
               //xr[sb][ss] *= pow(abs(is[sb][ss]), 4.0/3.0);
               xr[sb][ss] *= powertable3[abs(is[sb][ss])];
               if (sign){
                   xr[sb][ss] = -xr[sb][ss];
               }
           }
       }

   }




   int huffman_decoder(
    struct huffcodetab *h,	/* pointer to huffman code record*/
    /* unsigned */ int *x, 	/* returns decoded x value*/
    /* unsigned */ int *y,	/* returns decoded y value*/
    int *v,
    int *w,
    NewBuffer* load_info)
{
    HUFFBITS	level;
    int		point = 0;
    int		error = 1;
    int bit_used_count = 0;

    level = dmask;

    if (h->val == NULL){
	    return 2;
    }

    /* Table 0 needs no bits */
    if ( h->treelen == 0) {
        *x = *y = 0;
        return bit_used_count;
    }

    /* Lookup in Huffman table */

    do {
        if (h->val[point][0]==0) {	/* end of tree */
            *x = h->val[point][1] >> 4;
            *y = h->val[point][1] & 0xf;

            error = 0;
            break;
        }
        if (get_bits2(load_info, 1)) {
            bit_used_count++;
            while (h->val[point][1] >= MXOFF) point += h->val[point][1];
            point += h->val[point][1];
        } else {
            while (h->val[point][0] >= MXOFF) point += h->val[point][0];
            point += h->val[point][0];
            bit_used_count++;
        }
        level >>= 1;
    } while (level || (point < ht->treelen) );

    /* Check for error */

    if (error) { /* set x and y to a medium value as a simple concealment */
    	fprintf(stderr, "Illegal Huffman code in data.\n");
        *x = ((h->xlen-1) << 1);
        *y = ((h->ylen-1) << 1);
    }

    /* Process sign encodings for quadruples tables */

    if (h->tablename[0] == '3' && (h->tablename[1] == '2' || h->tablename[1] == '3')) {
        *v = (*y>>3) & 1;
        *w = (*y>>2) & 1;
        *x = (*y>>1) & 1;
        *y = *y & 1;

        /* v, w, x and y are reversed in the bitstream. */
        /* Switch them around to make test bistream work.*/

        /* {int i=*v; *v=*y; *y=i; i=*w; *w=*x; *x=i;}  MI */

        if (*v) {
            bit_used_count++;
            if (get_bits2(load_info, 1) == 1)
            *v = -*v;
        }
        if (*w) {
            bit_used_count++;
            if (get_bits2(load_info, 1) == 1)
            *w = -*w;
        }
        if (*x) {
            bit_used_count++;
            if (get_bits2(load_info, 1) == 1)
            *x = -*x;
        }
        if (*y) {
            bit_used_count++;
            if (get_bits2(load_info, 1) == 1)
            *y = -*y;
        }
        //xil_printf("USE ESTO MIJARES>>!!!! 1111111111111111 : %d\n\r", bit_used_count);
        return bit_used_count;
    } else {
        /* Process sign and escape encodings for dual tables */

        /* x and y are reversed in the test bitstream.*/
        /* Reverse x and y here to make test bitstream work.*/

        /*    removed 11/11/92 -ag
        *	{int i=*x; *x=*y; *y=i;}
        */

        if (h->linbits){
            if ((h->xlen-1) == *x){
                *x += get_bits2(load_info, h->linbits);
                bit_used_count += h->linbits;
            }
        }
        if (*x) {
            bit_used_count++;
            if (get_bits2(load_info, 1) == 1)
            *x = -*x;
        }
        if (h->linbits) {
            if ((h->ylen-1) == *y) {
                *y += get_bits2(load_info, h->linbits);
                bit_used_count += h->linbits;
            }
        }
        if (*y) {
            bit_used_count++;
            if (get_bits2(load_info, 1) == 1)
            *y = -*y;
        }
        return bit_used_count;
    }
    return error;
}



// Entire huffman decoding process
void huffman_decode(long int is[SBLIMIT][SSLIMIT], SideInfoType* side_info, int ch, int gr,
                    FrameHeader* header_info, NewBuffer* load_info) {
    int i, x, y;
    int v, w;
    int bit_used=load_info->total_number_bits;
    //int bit_used = 0;
    struct huffcodetab *h;
    int region1Start;
    int region2Start;
    int samplefreq;
    int currentBit, grBits;
    int part2_length;
    struct gr_info_type	*gr_info = &(side_info->ch[ch].gr[gr]);

    samplefreq = header_info->sampling_frequency_index + 3; // (header_info->ID(1 bit) is always 1 for MPEG1 Layer 3)

    //initialize_huffman();

    /* calculate part2_length */
    if(gr_info->block_type == 2) {
        if(gr_info->mixed_block_flag) {
            part2_length=17*slen[0][gr_info->scalefac_compress]
                        +18*slen[1][gr_info->scalefac_compress];

        } else {
            part2_length=18*slen[0][gr_info->scalefac_compress]
                        +18*slen[1][gr_info->scalefac_compress];
        }
    } else { /* Long types 0, 1, 3 */
        part2_length=11*slen[0][gr_info->scalefac_compress]
                    +10*slen[1][gr_info->scalefac_compress];
    }


    if((side_info->ch[ch].gr[gr].window_switching_flag) &&
        (side_info->ch[ch].gr[gr].block_type == 2)) {

        /* Region2 */
        region1Start = 36;	/* sfb[9/3]*3=36 */
        region2Start = 576;	/* No Region2 for short block case */

    } else {/* Find region boundary for long block case */
        region1Start = sfBandIndex[samplefreq].l[side_info->ch[ch].gr[gr].region0_count + 1]; /* MI */
	    region2Start = sfBandIndex[samplefreq].l[side_info->ch[ch].gr[gr].region0_count +
			side_info->ch[ch].gr[gr].region1_count + 2]; /* MI */
    }

    /* Read bigvalues area */
    //xil_printf("\n\rHUFFMAN START: ");
    for (i = 0; i < side_info->ch[ch].gr[gr].big_values * 2; i += 2) {
        if (i < region1Start) {
            //xil_printf("Region 1 HERE...!!!\n\r");
            h = &ht[side_info->ch[ch].gr[gr].table_select[0]];
        } else if (i<region2Start) {
            //xil_printf("Region 2 HERE...!!!\n\r");
            h = &ht[side_info->ch[ch].gr[gr].table_select[1]];
        } else {
            //xil_printf("Region 3 HERE...!!!\n\r");
            h = &ht[side_info->ch[ch].gr[gr].table_select[2]];
        }

        //bit_used += huffman_decoder(h, &x, &y, &v, &w, load_info); //----------------------------------------------
        huffman_decoder(h, &x, &y, &v, &w, load_info);

        is[i / SSLIMIT][i % SSLIMIT] = x;
        is[(i+1) / SSLIMIT][(i+1) % SSLIMIT] = y;
        //xil_printf("%d, %d, ", x, y);
    }

    h = &ht[(side_info->ch[ch].gr[gr].count1table_select) + 32];
    while ((hsstell(load_info) < (side_info->ch[ch].gr[gr].part2_3_length - part2_length)) && (i < SSLIMIT*SBLIMIT)) {
        huffman_decoder(h, &x, &y, &v, &w, load_info);

        is[i / SSLIMIT][i % SSLIMIT] = v;
        is[(i+1) / SSLIMIT][(i+1) % SSLIMIT] = w;
        is[(i+2) / SSLIMIT][(i+2) % SSLIMIT] = x;
        is[(i+3) / SSLIMIT][(i+3) % SSLIMIT] = y;
        //xil_printf("%d, %d, %d, %d\n\r", v, w, x, y);
        i += 4;
    }


    grBits = part2_length + side_info->ch[ch].gr[gr].part2_3_length;

    /*
    if ( bit_used < grBits )
	    get_bits2( load_info, grBits - bit_used );*/

    /* Zero out rest */
    for (; i < SSLIMIT * SBLIMIT; i++){
	    is[i / SSLIMIT][i % SSLIMIT] = 0;
        //xil_printf("BIT SIZE: %d\n\r", i);
    }

    /*
    xil_printf("the 576 sampling value isthe following:\n");
    for(x=0;x<SBLIMIT;x++){
        xil_printf("subband %d :",x);
        for(y=0;y<SSLIMIT;y++)
        xil_printf("%ld ",is[x][y]);
        xil_printf("\n");
    }*/

}


// read tables
int read_decoder_table() {
    int n, nn, t;
    for(n = 0; n < HTN; n++){
        sprintf(ht[n].tablename, "%d", n);
        ht[n].treelen	= hufftab[n].treelen;
        ht[n].xlen	= hufftab[n].xlen;
        ht[n].ylen	= hufftab[n].ylen;
        ht[n].linbits	= hufftab[n].linbits;
        ht[n].linmax = (1 << ht[n].linbits) - 1;

        /* Check table name   */
        sscanf(ht[n].tablename, "%u", &nn);
        if (nn != n) {
            fprintf(stderr, "wrong table number %u\n", n);
            return -2;
        }
        t = hufftab[n].ref;
        if (t >= 0) {
            ht[n].ref		= t;
            ht[n].val		= ht[t].val;
            ht[n].treelen	= ht[t].treelen;
            if ((ht[n].xlen != ht[t].xlen) || (ht[n].ylen != ht[t].ylen)) {
            	fprintf(stderr,"wrong table %u reference\n",n);
                return -3;
            }
        } else if (t == -1) {
            unsigned char *p;
            int i;

            ht[n].ref	= -1;
            ht[n].val	= (unsigned char (*)[2])
            calloc(2 * (ht[n].treelen)+1, sizeof(unsigned char));
            if (ht[n].val == NULL) {
            	fprintf(stderr, "tables.c: heap error at table %d\n", n);
            	fprintf(stderr, "attempting malloc %d %u\n",
                                2 * (ht[n].treelen), sizeof(unsigned char));
                exit(-10);
            }
            p = hufftab[n].data;
            for (i = 0; i < ht[n].treelen; i++) {
                ht[n].val[i][0] = *p++;
                ht[n].val[i][1] = *p++;
            }
        } else {
        	fprintf(stderr,"huffman decodertable error at table %d\n",n);
        }
    }
    return n;
}


// initialize reading tables
void initialize_huffman() {
    static int	 huffman_initialized = 0;
    if (huffman_initialized)
	    return;
    if(read_decoder_table() != HTN){
    	fprintf(stderr,"decoder table read error\n");
	    exit(4);
    } else {
    	xil_printf("Se inicio bien mijares\n\r");
    }
    huffman_initialized = 1;
}


int get_side_info(NewBuffer* load_info, FrameHeader* header_info, SideInfoType* side_info) {
    int ch, gr, i;
    int stereo = header_info->get_stereo;

    // An extention of MPEG-1 audio layer 3 for lower sampling rates targeting bit rates from 32-64 kbps
    // LSF not taken into account in this function.


    if(header_info->bit_rate > 64) {
        //load_info->total_bytes = 32;
        //xil_printf("OTROS \n\r");
        side_info->main_data_begin = get_bits2(load_info, 9);
	    if (stereo == 1) {// if mono mode
            //exit(0);
	        side_info->private_bits = get_bits2(load_info,5);
        }else{ // every other mode
        	xil_printf("------------------------------------------------------------------\n\r");
	        side_info->private_bits = get_bits2(load_info,3);
        }

        for (ch = 0; ch < stereo; ch++)
	        for (i = 0; i < 4; i++)
		        side_info->ch[ch].scfsi[i] = get_bits2(load_info, 1);

	    for (gr = 0; gr < 2 ; gr++) {
	        for (ch = 0; ch < stereo; ch++) {
                side_info->ch[ch].gr[gr].part2_3_length = get_bits2(load_info, 12);
                side_info->ch[ch].gr[gr].big_values = get_bits2(load_info, 9);
                //xil_printf("Big Values: %d\n\r", side_info->ch[ch].gr[gr].big_values);
                if (side_info->ch[ch].gr[gr].big_values > FREQUENCIES/2) { //number of big value pairs should never exceed FREQUENCIES/2 (page 173)
			            //fprintf(stderr, "ERROR MIJARES\n");
                		xil_printf("ERROR MIJARES\n\r");
                        side_info->ch[ch].gr[gr].big_values = FREQUENCIES/2; // above commented lines
                        //load_info->bad_block++;
			            //exit(0);
		        }
                side_info->ch[ch].gr[gr].global_gain = get_bits2(load_info, 8);
                //xil_printf("GLOBAL GAIN: %d\n\r", side_info->ch[ch].gr[gr].global_gain);
                side_info->ch[ch].gr[gr].scalefac_compress  = get_bits2(load_info, 4);
                side_info->ch[ch].gr[gr].window_switching_flag = get_bits2(load_info, 1);
		        if (side_info->ch[ch].gr[gr].window_switching_flag) {
		            side_info->ch[ch].gr[gr].block_type = get_bits2(load_info, 2);
		            side_info->ch[ch].gr[gr].mixed_block_flag = get_bits2(load_info, 1);
		            for (i = 0; i < 2; i++){
                        side_info->ch[ch].gr[gr].table_select[i] = get_bits2(load_info, 5);
                    }

		            for (i = 0; i < 3; i++){
                        side_info->ch[ch].gr[gr].subblock_gain[i] = get_bits2(load_info, 3);
                    }

                    /*
                    * Set region_count parameters since they are
                    * implicit in this case.
                    */
		            if (side_info->ch[ch].gr[gr].block_type == 0) {
		            	fprintf(stderr, "Side info bad: block_type 0 in split block\n");
                        load_info->bad_block++;
                        //return 1;
			            //exit(0);
		            } else if (side_info->ch[ch].gr[gr].block_type == 2
			            && side_info->ch[ch].gr[gr].mixed_block_flag == 0)
			            side_info->ch[ch].gr[gr].region0_count = 8; /* MI 9; */
		            else
			            side_info->ch[ch].gr[gr].region0_count = 7; /* MI 8; */
		            side_info->ch[ch].gr[gr].region1_count =
				            20 - side_info->ch[ch].gr[gr].region0_count;
		        } else {
		            for (i = 0; i < 3; i++){
                        side_info->ch[ch].gr[gr].table_select[i] = get_bits2(load_info, 5);
                    }
                    side_info->ch[ch].gr[gr].region0_count = get_bits2(load_info, 4); // in the book says to add 1 (+ 1) page 175
                    side_info->ch[ch].gr[gr].region1_count = get_bits2(load_info, 3); // in the book says to add 1 (+ 1) page 175
                    side_info->ch[ch].gr[gr].block_type = 0;
		        }
                //xil_printf("block_type value: %d , window_switching value: %d , big_value: %d\n\r", side_info->ch[ch].gr[gr].block_type, side_info->ch[ch].gr[gr].window_switching_flag, side_info->ch[ch].gr[gr].big_values);
                side_info->ch[ch].gr[gr].preflag = get_bits2(load_info, 1);
                side_info->ch[ch].gr[gr].scalefac_scale = get_bits2(load_info, 1);
                side_info->ch[ch].gr[gr].count1table_select = get_bits2(load_info, 1);
	        }
	    }
    } else {
        // LSF here
    }
    if(load_info->bad_block == 0) {
    	//xil_printf("no hay bad block mijares\n\r");
        return 1;
    } else {
    	//xil_printf("si hay bad block mijares\n\r");
        return 0;
    }

}



void load_header(NewBuffer* load_info, FrameHeader* header){
    header->bit_rate_index = get_bits2(load_info, 4);
    header->sampling_frequency_index = get_bits2(load_info, 2);
    header->bit_padding = get_bits2(load_info, 1);
    header->private = get_bits2(load_info, 1);
    header->stereo_mode = get_bits2(load_info, 2);
    header->extension_mode = get_bits2(load_info, 2);
    header->copyright = get_bits2(load_info, 1);
    header->original = get_bits2(load_info, 1);
    header->emphasis = get_bits2(load_info, 2);

    header->freq_rate = sample_rate[header->sampling_frequency_index];
    header->bit_rate = bit_rate_table[header->bit_rate_index];

    header->get_stereo = (header->stereo_mode==0x3)?1:2;
}


/*
    Main function to read each frame
*/
void frame_details(){
    int i = 0;
    int j = 0;
    int bit_count = 0;
    unsigned short syncChar;
    unsigned char byte_read;
    //XTime tStart, tEnd;
    FIL musicout; /* decode to file */
    FIL file1;
    FRESULT result;

    unsigned long sample_frames;

    int ch, gr;

    //FILE *musicout;		/* decode to file */

    FrameHeader readFrame;
    NewBuffer* start_frame;
    SideInfoType sideInfo;
    scalefac_type scaleFactorRead;

    start_frame->bad_block = 0;
    start_frame->is_buffer_aval = 0;
    start_frame->bit_count = 0;
    start_frame->count_bytes = 0;
    start_frame->read_new = 0;
    start_frame->total_number_bits = 4;

    initialize_huffman();

    /*Open file here*/
        result = f_open(&musicout, "pres.txt", FA_OPEN_ALWAYS | FA_WRITE);
        if(result != 0) {
        	xil_printf("No musicout: %d\r\n", result);

        } else {
        	xil_printf("Si musicout: %d\r\n", result);
        }


    /*Open file here*/
    result = f_open(&file1, "test.mp3", FA_READ);
    if(result != 0) {
    	xil_printf("No mp3: %d\r\n", result);
    } else {
    	xil_printf("Si mp3: %d\r\n", result);
    }

    start_frame->ptr = &file1;




//    /*Open file here*/
//    result = f_open(&musicout, "musicout4.txt", FA_CREATE_ALWAYS);
//    if(result != 0) {
//    	xil_printf("No musicout: %d\r\n", result);
//
//    } else {
//    	xil_printf("Si musicout: %d\r\n", result);
//    }

    sample_frames = 0;

    result = 0;

    //while(f_read(start_frame->ptr, &byte_read, 1, &counter) == 0){
    //while(result == 0){
    while((result == 0) && (i < 4000)) {
    	result = f_read(start_frame->ptr, &start_frame->read_char, 1, &counter);
    	//xil_printf("char: %c\r\n", start_frame->read_char);
    	syncChar = (syncChar << 8) | start_frame->read_char;
        if(i == 0) {
            readFrame.ini_bit_rate = readFrame.bit_rate;
            readFrame.ini_freq_rate = readFrame.freq_rate;
        }

        /*
        0xfffb ->
                * 11 bits for synchronize each frame (all 1s)
                * 2 bits for showing MPEG version (all 1s for MPEG version 1)
                * 2 bits for showing the layer encoded for the audio file (01 -> Layer III)
                * 1 bit to show if the frame is protected by CRC checking (1 -> assuming no crc protection, maybe change this later)
        */
        if(syncChar == 0xfffb) {
        	//xil_printf("ENTRO CHUCHA\r\n");
        	/* If input from the terminal is 'q', then return to menu.
        		 * Else, continue streaming. */
        	if(XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET) == 'q') {
        		f_close(&musicout);
        		f_close(start_frame->ptr);
        		menu();
        	}

            load_header(start_frame, &readFrame);

            if((readFrame.freq_rate == readFrame.ini_freq_rate) && (readFrame.bit_rate == readFrame.ini_bit_rate)){
            //if(readFrame.freq_rate != 0) {
            	//XTime_GetTime(&tStart);
                readFrame.frame_length = (((144*(int)readFrame.bit_rate*1000)/((int)readFrame.freq_rate))) + (int)readFrame.bit_padding;
                if(get_side_info(start_frame, &readFrame, &sideInfo)) {
                    i++;
                    for(gr = 0; gr < 2; gr++){
                        for(ch = 0; ch < readFrame.get_stereo; ch++){ //check < 2 with header info..!!! this is a test
                            get_scale_factors(start_frame, &sideInfo, &scaleFactorRead, gr, ch);
                            huffman_decode(is, &sideInfo, ch, gr, &readFrame, start_frame);
                            requantize(&sideInfo, &scaleFactorRead, &readFrame, is, ch, gr, ro[ch]);
                        }
                        //Here Stereo decoding..!!!
                        stereo_decoding(ro, lr, &scaleFactorRead, &(sideInfo.ch[0].gr[gr]), &readFrame);
                        for(ch = 0; ch < readFrame.get_stereo; ch++) {
                            /*
                                reorder
                                antialias
                            */
                            reorder(lr[ch], re, &(sideInfo.ch[ch].gr[gr]), &readFrame);
                            antialias(re, hybridIn, &(sideInfo.ch[ch].gr[gr]), &readFrame); /* antialias butterflies*/
                            unsigned char sb=0;
                            unsigned char ss=0;
                            //xil_printf("Hibrid: ");
                            for(sb = 0; sb < SBLIMIT; sb++) {   /* hybrid synthesis*/
                                /*
                                    hybrid here
                                */

                                hybrid(hybridIn[sb], hybridOut[sb], sb, ch, &(sideInfo.ch[ch].gr[gr]), &readFrame);
                                //xil_printf("SYNTHESIS DONE MIJARES\n\r");
                            }


                            for(ss = 0; ss < 18; ss++) {
                                for(sb = 0; sb < SBLIMIT; sb++) {
                                    if((ss%2) && (sb%2)) {
                                        hybridOut[sb][ss] = -hybridOut[sb][ss];
                                        //xil_printf("%d, ", (int)(hybridOut[sb][ss] * 32750));
                                    }
                                }
                            }

                            //xil_printf("\n\r");

                            //xil_printf("START: ");
                            for(ss = 0; ss < 18; ss++) {
                                for (sb = 0; sb < SBLIMIT; sb++) {
                                    polyPhaseIn[sb] = hybridOut[sb][ss];
                                    //xil_printf("%d, ", (int)(polyPhaseIn[sb]* 32750));
                                }
                                /*
                                    Here add subBandSynthesis..!!!!
                                */
                                SubBandSynthesis(polyPhaseIn, ch, &((pcm_sample)[ch][ss][0]));
                                //xil_printf("%d, ", (pcm_sample)[ch][ss][0]);
                            }
                            //xil_printf("\n\r");
                            //xil_printf("SYNTHESIS DONE MIJARES\n\r");

                        }
                        //xil_printf("PRINTING PCM SAMPLES>>!!\n\r");
                        out_fifo(pcm_sample, &musicout, &sample_frames, &readFrame);
                    }

                    xil_printf("Bit rate: %d, Freq rate: %d, Padding: %d, length: %d, Stereo: %d. Extension: %d\n\r", readFrame.bit_rate, readFrame.freq_rate, readFrame.bit_padding, readFrame.frame_length, readFrame.stereo_mode, i);


                    //xil_printf("Bit rate: %d, Freq rate: %d, Padding: %d, length: %d, Extension: %d\n\r", readFrame.bit_rate, readFrame.freq_rate, readFrame.bit_padding, readFrame.frame_length, i);
                    //xil_printf("number bad blocks: %d\n\r", start_frame->bad_block);

                    //xil_printf("TOTAL NUMBER OF BYTES: %d, STEREO: %d\n\r", start_frame->count_bytes, readFrame.get_stereo);
                }
            }
            //XTime_GetTime(&tEnd);
            //xil_printf("Output took %llu clock cycles.\n\r", 2*(tEnd - tStart));
            //xil_printf("Output took %.2f us.\n\r",
            //           1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND/1000000));
            start_frame->is_buffer_aval = 0;
            start_frame->bit_count = 0;
            start_frame->count_bytes = 0;

            start_frame->bad_block = 0; //this is for test..!!!
            start_frame->read_new = 0;
            start_frame->total_number_bits = 4;
        }
    }
    xil_printf("NO ENTRO CHUCHA: %d\r\n", result);
    f_close(&musicout);
    f_close(start_frame->ptr);
}








