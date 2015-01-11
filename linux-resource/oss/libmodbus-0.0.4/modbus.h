/*
#ifndef __modbus__

#define __modbus__ 1
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <ctype.h>


#define 	VERSION "0.0.4"
#define		PH_DEVICE	1
#define		DO_DEVICE	33
#define		WN_DEVICE	65

struct termios Mb_tio;								/* new serail port setting */

int Mb_verbose;										/* print debug informations */

typedef unsigned char byte;						/* create byte type */

/* master structure */
typedef struct {
   int slave; 											/* number of the slave to call*/
   int function; 										/* modbus function to emit*/
   int address;										/* slave address */
   int length;											/* data length */
   int timeout;										/* timeout in second */
} Mbm_trame;

/* sense device info */
typedef struct
{
	//״ֵ̬
	//-1,tty init fail;-2,slave device not respond;-3,function code or data error; -4,received data error
	int state;
	//��Ҫֵ
	//�������֣���Ϊ��
	int fival;
	//С�����֣���ȡ3����Ч����
	int ffva;
	//����Ҫֵ���ַ���ģʽ
	unsigned char fcval[8];

	//����Ҫֵ������ͬ��
	int sival;
	int sfval;
	unsigned char scval[8];
	unsigned int reserv[8];	 
} Sen_value;

int getSenVal(Mbm_trame trame, Sen_value *senInfo, int data_in[], int data_out[]);
int setSenVal(Mbm_trame trame, Sen_value *senInfo);
int contrlSen(Mbm_trame trame, void *pBuf);
