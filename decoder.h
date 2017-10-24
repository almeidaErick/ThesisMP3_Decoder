#include "ff.h"


#define FREQUENCIES 576
#define SBLIMIT 32
#define SSLIMIT 18
#define HTN 34
#define HUFFBITS unsigned long int
#define MXOFF 250
#define MPG_MD_JOINT_STEREO 1
//--------------------------------------------------------------------------
#define POW2CACHENEG (16*4)		/* # of cached numbers for n < 0   */
#define POW2CACHEPOS (5*4)		/* # of cached numbers for n > 0   */
#define PI 3.14159265358979
#define PI4	0.78539816339744
#define PI12 0.261799388
#define PI36 0.087266463
#define PI64 0.049087385
#define SCALE 32767
#define	BUFFERSIZE 8192		/* x 2 bytes */


//--------------------------------Register controllers-------------------------------------------
#define INPUT_ADDRESS 24
#define OUTPUT_ADDRESS 16
#define BLOCK_TYPE_ADDRESS 14
#define DONE_WRITING 13
#define INPUT_RAM_MODE 12

/*
    NOTES ON FUNCTIONS TESTED
    * Get bits function works perfect (bit counting and byte counting works as expected).
    * load_buffer_side_info functions works as expected, use only when reading side information.


*/


/*
    MODE:
    00 -> Stereo
    01 -> Joint Stereo
    10 -> Dual Channel
    11 -> Single Channel
*/

/*
    FREQUENCY:
    00 -> 44100 Hz
    01 -> 48000 Hz
    10 -> 32000 Hz
    11 -> reserv
*/

/*
    Side Information:
    17 bytes -> if it is single channel mode
    32 bytes -> otherwise
*/

/*
    STRUCTS

*/

typedef struct
{
    short bit_rate; //save bit rate index for later use in decoding of each frame
    short bit_rate_index;
    unsigned short freq_rate; //save value for frequency rate
    unsigned int sampling_frequency_index;
    char bit_padding; //save if there is padding or not in the frame
    char private;
    int frame_length; //keep the lenght of the frame
    int ini_bit_rate; // bit rate for the first valid frame in the mp3 file
    int ini_freq_rate; // sampling frequency for the first valid frame in the mp3 file
    char stereo_mode; // channel mode
    char extension_mode; // extension mode (joint stereo only)
    char copyright;
    char original;
    char emphasis;
    char get_stereo;
} FrameHeader;

typedef struct
{
    unsigned main_data_begin;//9bits (where the main data of a frame begins )
    unsigned private_bits;//5bits
    struct {
        unsigned scfsi[4];//4bits
        struct gr_info_type {
            unsigned part2_3_length;//12bits
            unsigned big_values;//9bits
            unsigned global_gain;//8bits
            unsigned scalefac_compress;//4bits
            unsigned window_switching_flag;//1bits
            unsigned block_type;//2bits
            unsigned mixed_block_flag;//1bit
            unsigned table_select[3];//5 bits each
            unsigned subblock_gain[3];//3 bits each
            unsigned region0_count;//4bits
            unsigned region1_count;//3bits
            unsigned preflag;//1bits
            unsigned scalefac_scale;//1bits
            unsigned count1table_select;//1bits
        } gr[2];
    } ch[2];
} SideInfoType;

typedef struct
{
    FIL *ptr; //File descriptor for reading the mp3 file
    unsigned char max_buffer[32]; //Buffer tnat contains the bytes for the side information
    unsigned char *get_bit; //pointer of bits for each byte in the buffer
    char bit_count; //bits counting for each byte (if bit_count==8 then next byte)
    int count_bytes; //how many bytes have been read
    int total_bytes; //how many bytes have been saved on the buffer

    int read_new;
    unsigned char read_char; //char read from file descriptor
    char bad_block;
    char is_buffer_aval; //check if read_char has a recent character read from file descriptor
    long total_number_bits;
}NewBuffer;

//-------------------Scalefactor Definition-------------------------;
typedef struct
{
    struct {
        int l[22]; // [cb]
        int s[3][13]; // [window][cb]
    }ch[2]; // [ch]
} scalefac_type;


   //---------------------------------------------------------------------------------
   struct huffcodetab {
       char tablename[3]; /*string, containing table_description */
       unsigned int xlen; /*max. x-index+ */
       unsigned int ylen; /*max. y-index+ */
       unsigned int linbits; /*number of linbits */
       unsigned int linmax; /*max number to be stored in linbits */
       int ref; /*a positive value indicates a reference*/
       HUFFBITS *table; /*pointer to array[xlen][ylen] */
       unsigned char *hlen; /*pointer to array[xlen][ylen] */
       unsigned char (*val)[2];/*decoder tree */
       unsigned int treelen; /*length of decoder tree */
   };



   /*
       FUNCTIONS
   */

   void frame_details();
   void frame_stereo_dec(FILE *flptr, FrameHeader* header);
   int get_side_info(NewBuffer* load_info, FrameHeader* header_info, SideInfoType* side_info);
   unsigned long int get_bits2(NewBuffer* load_info, unsigned long bits_num);

   int read_decoder_table();
   void initialize_huffman();
   int huffman_decoder(struct huffcodetab *h, int *x, int *y, int *v, int *w, NewBuffer* load_info);
   void huffman_decode(long int is[SBLIMIT][SSLIMIT], SideInfoType* side_info, int ch, int gr,
                       FrameHeader* header_info, NewBuffer* load_info);
   void requantize(SideInfoType* side_info, scalefac_type *scalefac, FrameHeader* header_info,
                   long int is[SBLIMIT][SSLIMIT], int ch, int gr, float xr[SBLIMIT][SSLIMIT]);
   void get_scale_factors(NewBuffer* load_info, SideInfoType* side_info, scalefac_type *scalefac, int gr, int ch);
   void stereo_decoding(float xr[2][SBLIMIT][SSLIMIT], float lr[2][SBLIMIT][SSLIMIT],
           scalefac_type *scalefac, struct gr_info_type *gr_info, FrameHeader* header_info);
   void reorder(float xr[SBLIMIT][SSLIMIT], float ro[SBLIMIT][SSLIMIT],
           struct gr_info_type *gr_info, FrameHeader* header_info);
   void antialias(float xr[SBLIMIT][SSLIMIT], float hybridIn[SBLIMIT][SSLIMIT],
           struct gr_info_type *gr_info, FrameHeader* header_info);
   void hybrid(
           float fsIn[SSLIMIT], float tsOut[SSLIMIT], int sb, int ch,
           struct gr_info_type *gr_info, FrameHeader* header_info);
   void inv_mdct(float in[18], float out[36], int block_type);
   void SubBandSynthesis(float *bandPtr, int ch, short *samples);
   void create_syn_filter(double doublefilter[64][SBLIMIT]);
   unsigned long hsstell(NewBuffer* load_info);
   static void swapWordsInLong(short *loc, int words);
   void SwapBytesInWords(short *loc, int words);
   void out_fifo(short pcm_sample[2][SSLIMIT][SBLIMIT], FIL *outFile, unsigned long	*psampFrames,
           FrameHeader* header_info);
