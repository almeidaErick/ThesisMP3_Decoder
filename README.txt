{\rtf1\ansi\ansicpg1252\cocoartf1504\cocoasubrtf830
{\fonttbl\f0\fswiss\fcharset0 Helvetica;}
{\colortbl;\red255\green255\blue255;}
{\*\expandedcolortbl;;}
\paperw11900\paperh16840\margl1440\margr1440\vieww18680\viewh18300\viewkind0
\pard\tx566\tx1133\tx1700\tx2267\tx2834\tx3401\tx3968\tx4535\tx5102\tx5669\tx6236\tx6803\pardirnatural\partightenfactor0

\f0\fs24 \cf0 \

\b ERICK ALMEIDA\
Thesis Project \

\b0 \
Link to GitHub for the entire code used for the project ({\field{\*\fldinst{HYPERLINK "https://github.com/almeidaErick/ThesisMP3_Decoder"}}{\fldrslt https://github.com/almeidaErick/ThesisMP3_Decoder}}).\
\
\
\
\pard\tx566\tx1133\tx1700\tx2267\tx2834\tx3401\tx3968\tx4535\tx5102\tx5669\tx6236\tx6803\pardirnatural\qj\partightenfactor0
\cf0 The application is a bare-metal software implementation of an mp3 decoder, for instance no Linux knowledge is required. The design needs two programs, VIVADO to create the hardware design and SDK to control the hardware using software commands.\
\
										
\b \ul SDK Directory
\b0 \ulnone \
\
Contains all the files used for uploading code to the PS. File such as xparameter.h is not included, since this file is automatially created when the hardware design from Vivado is exported. Files like:\
* adventures_with_ip.c\
* adventures_with_ip.h\
* audio.c\
* audio.h\
* ip_functions.c\
* lms_pcore_addr.h \
\
can be downloaded from the link {\field{\*\fldinst{HYPERLINK "http://www.zynqbook.com/download-tuts.html"}}{\fldrslt http://www.zynqbook.com/download-tuts.html}}, but are added just in case.\
The entire decoder file is called decoder.c, for a better understanding how the functions are being called, check the main function. The file decoder.h contains the structs used for each section of the frame explained on chapter 3 of the thesis. \
Additionally, the file tables.h contains the tables used for the cos, power function. This is useful for a faster decoding analysis. The file mp3Dec.c, contains the software-based mp3 decoder, some of the code is not the same as the PS implementation, since the functions used for print or access to the FAT libraries are not the same.\
\
mp3Dec.c - COMPILATION\
The file can be compiled in a Linux or MAC terminal like, \'93gcc -o mp3 mp3Dec.c -lm\'94.\
The -lm flag means that, the math library needs to be added in the software implementation. \
\
decoder.c - COMPILATION SDK\
The file is automatically compiled along with the adventures_with_ip, audio, ip_functions files. The SDK IDE checks where the main function starts, and only by saving the changes, the code is compiled. Things to be clear:\
- Add FAT libraries in the compilation section (BSP link on SDK project).\
- Add math library compilation flag (Configurations - Add library paths).\
- If the program does not run the following code, then assign more memory to the project (lscript.ld link)\
- The xparameter.h will be created when the hardware design has been exported from Vivado. When the xparameter.h file has been created, check the IP addresses from Vivado with the xparameter.h file from SDK, sometimes the addresses do not update automatically. \
\
\
										
\b \ul VIVADO Directory
\b0 \ulnone \
\
The main file that implements the IMDCT stage, is called \'93imdct_process.vhd\'94. The file contains the logic for processing short and long blocks by using FSM. For the purpose of this project only FSM was the only method considered for implementation, since no other method came out of my mind. Loops are not implemented the same way as software, and does not depend on clock cycles and was really difficult to synchronize with block rams, so that method was not considered at all. \
\'93imdct_process.vhd\'94 already include the block rams created on the files:\
* INPUT_RAM.VHD\
* OUTPUT_RAM.VHD\
* COS_RAM.VHD\
* COS_TAB1_ROM.VHD\
* WIN_ROM.VHD\
\
The registers are mapped on the file called \'93my_imdct_block_v1_0_S00_AXI.vhd\'94, this file was automatically created when the IP was initially created, but some parameters were modified in order to match the input and outputs of the IMDCT IP BLOCK. In order to avoid confusions, if the IMDCT BLOCK want to be created from scratch, just create an IP block from scratch and copy the contents of the file \'93my_imdct_block_v1_0_S00_AXI.vhd\'94 to the new created IP. \
\
The process of integrating is straight forward, the steps are specified below:\
* Follow the tutorial from {\field{\*\fldinst{HYPERLINK "http://www.zynqbook.com/download-tuts.html"}}{\fldrslt http://www.zynqbook.com/download-tuts.html}} called \'93Audio in Vivado IP Integrator\'94 to add the IP block to control the Audio codec from the ZYBO.\
* After the whole tutorial has been followed, start a new IP block implementation, a new Vivado window will open with the files \'93my_imdct_block_v1_0_S00_AXI.vhd\'94 and \'93my_imdct_block_v1_0.vhd\'94. More information is provided in the link above, in the project called \'93Creating IP in HDL\'94.\
* Add a new file that will be containing the controller for the IMDCT decoding stage.\
* Now simply add the files inside of VIVADO Directory to the project.\
* To simulate the IMDCT block, create a new simulation file and copy the content of the file \'93imdct_simulation.vhd\'94 or just simply add the file from the VIVADO Directory. \
* Once everything is working as expected, generate the bitstream on Vivado, this could take several minutes to finish.\
* After the bitstream has been created, go to File and export hardware, make sure you include the bitstream before importing the hardware design.\
* the last step is to launch SDK from Vivado, this will include the hardware design from Vivado and the proper addresses from the respective IP blocks created on Vivado.\
\
\
									
\b \ul PosterPresentation Directory
\b0 \ulnone \
\
This Directory contains the files used for the Poster Presentations, this include:\
* The .pptx file used to support results and design decisions made (also in .pdf and keynote format). \
* The poster in A3 format used for the presentation, in A3 size as .pdf, .pptx , and keynote format. \
* The images used for the poster presentation and the .pptx file.\
\
\
										
\b \ul Final Notes\
\

\b0 \ulnone * The output value produced by the ZYBO decoder is a text file containing the PCM samples decoded from the mp3 file.\
* to test the audio, add the proper header info to each PCM sample, save it as \'93test.wav\'94 and move it to the SD card of the ZYBO,\
* Once the song is located on the sd card, run the program on the ZYBO, and from the terminal write\
	- \'93screen /dev/ttyUSB1 115200\'94 to open the serial interface from the ZYBO. Another method can be used, such as, using the terminal window \
          from SDK.\
* Finally press \'92s\'92 to play the \'93test.wav\'94 file from the sd card to the audio codec.\
\
\
}