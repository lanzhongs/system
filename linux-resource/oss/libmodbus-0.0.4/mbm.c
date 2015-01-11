#include <modbus.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>

/* master main function :
- trame informations
- data in
- data out
- pointer function called when master send a data on serial port (can be NULL if not use)
- pointer function called when master receive a data on serial port (can be NULL if not use)*/
int Mb_master(Mbm_trame, int [] , int []);

/* commun functions */
int Mb_open_device(char [], int , int , int ,int );		/* open device and configure it */
void Mb_close_device();												/* close device and restore old parmeters */
int Mb_test_crc(unsigned char[] ,int, int);						/* check crc16 */
int Mb_calcul_crc(unsigned char[] ,int, int);						/* compute crc16 */


int Mb_device = -1;				/* device tu use */
byte *Mbm_result;			/* byte readed on the serial port : answer of the slave */
int revert = 0;				/* flip crc sequence */
int getSenVal(Mbm_trame trame, Sen_value *senInfo, int data_in[], int data_out[])
{
	int result = -1;
	int output[128];
	
	if(Mb_device == -1)

	{
		/* open device */
   		Mb_device = Mb_open_device("/dev/ttyO0",9600,0,8,1);
		
	}
	if(Mb_device == -1)
	{
		senInfo->state = result;
		return result;
	}
	result=Mb_master(trame,data_in,output);
   /* return 0 if ok */
   if (result<0)
   {
      if (result==-1) printf("error : unknow function\n");
      if (result==-2) printf("crc error\n");
      if (result==-3) printf("timeout error\n");
      if (result==-4) printf("error : bad slave answer\n");
   }
   else
   {
	  senInfo->state = 0;
	  printf("ok\n");
   }
   	if(data_out)
		memcpy(data_out, output, 32);
   senInfo->state = result;
	return result;
}

int setSenVal(Mbm_trame trame, Sen_value *senInfo)
{

	return 0;
}


int contrlSen(Mbm_trame trame, void *pBuf)
{
	
		return 0;
}


/************************************************************************************
		Mb_test_crc : check the crc of a packet
*************************************************************************************
input :
-------
trame  : packet with is crc
n      : lenght of the packet without tht crc
                              ^^^^^^^
answer :
--------
1 = crc fealure
0 = crc ok
************************************************************************************/
int Mb_test_crc(byte trame[],int n, int revert)
{
	unsigned int crc, i, j, carry_flag, a;
	crc = 0xffff;
	for (i = 0; i < n; i++)
	{
		crc = crc ^ trame[i];
		for (j = 0; j < 8; j++)
		{
			a = crc;
			carry_flag = a & 0x0001;
			crc = crc >> 1;
			if (carry_flag == 1)
				crc = crc ^ 0xa001;
		}
	}
   if (Mb_verbose)
      printf("test crc %0x %0x\n", (crc & 255),(crc >> 8));
   if(!revert)
   {
	if ((trame[n+1] != (crc & 0xff)) || (trame[n] != (crc >> 8)))
      return 1;
	else
      return 0;
   }
   else
   {
	   if ((trame[n+1] != (crc >> 8)) || (trame[n] != (crc & 0xff)))
		 return 1;
	   else
      	 return 0;
   }
 
}

/************************************************************************************
		Mb_calcul_crc : compute the crc of a packet and put it at the end
*************************************************************************************
input :
-------
trame  : packet with is crc
n      : lenght of the packet without tht crc
                              ^^^^^^^
answer :
--------
crc
************************************************************************************/
int Mb_calcul_crc(byte trame[],int n, int revert)
{
	unsigned int crc,i,j,carry_flag,a;
	crc = 0xffff;
	for (i = 0; i < n; i++)
	{
		crc = crc ^ trame[i];
		for (j = 0; j < 8; j++)
		{
			a = crc;
			carry_flag = a & 0x0001;
			crc = crc >> 1;
			if (carry_flag == 1)
				crc = crc ^ 0xa001;
		}
		
	}
	if(!revert)
	{
		trame[n]=crc>>8;
		trame[n+1]=crc & 0xff;
	}
	else
	{
		trame[n] = crc & 0xff;
		trame[n+1] = crc >> 8;

	}
	return crc;
}
/************************************************************************************
		Mb_close_device : Close the device
*************************************************************************************
input :
-------
Mb_device : device descriptor

no output
************************************************************************************/
void Mb_close_device(int Mb_device)
{
  close(Mb_device);
}

/************************************************************************************
		Mb_open_device : open the device
*************************************************************************************
input :
-------
Mbc_port   : string with the device to open (/dev/ttyS0, /dev/ttyS1,...)
Mbc_speed  : speed (baudrate)
Mbc_parity : 0=don't use parity, 1=use parity EVEN, -1 use parity ODD
Mbc_bit_l  : number of data bits : 7 or 8 	USE EVERY TIME 8 DATA BITS
Mbc_bit_s  : number of stop bits : 1 or 2    ^^^^^^^^^^^^^^^^^^^^^^^^^^

answer  :
---------
device descriptor
************************************************************************************/
int Mb_open_device(char Mbc_port[20], int Mbc_speed, int Mbc_parity, int Mbc_bit_l, int Mbc_bit_s)
{
  int fd;
 
  /* open port */
  fd = open(Mbc_port,O_RDWR | O_NOCTTY | O_NONBLOCK) ;
  if(fd<0)
  {
    perror("Open device failure\n") ;
    exit(-1) ;
  }

  /* settings port */
  bzero(&Mb_tio,sizeof(&Mb_tio));

  switch (Mbc_speed)
  {
     case 0:
        Mb_tio.c_cflag = B0;
        break;
     case 50:
        Mb_tio.c_cflag = B50;
        break;
     case 75:
        Mb_tio.c_cflag = B75;
        break;
     case 110:
        Mb_tio.c_cflag = B110;
        break;
     case 134:
        Mb_tio.c_cflag = B134;
        break;
     case 150:
        Mb_tio.c_cflag = B150;
        break;
     case 200:
        Mb_tio.c_cflag = B200;
        break;
     case 300:
        Mb_tio.c_cflag = B300;
        break;
     case 600:
        Mb_tio.c_cflag = B600;
        break;
     case 1200:
        Mb_tio.c_cflag = B1200;
        break;
     case 1800:
        Mb_tio.c_cflag = B1800;
        break;
     case 2400:
        Mb_tio.c_cflag = B2400;
        break;
     case 4800:
        Mb_tio.c_cflag = B4800;
        break;
     case 9600:
        Mb_tio.c_cflag = B9600;
        break;
     case 19200:
        Mb_tio.c_cflag = B19200;
        break;
     case 38400:
        Mb_tio.c_cflag = B38400;
        break;
     case 57600:
        Mb_tio.c_cflag = B57600;
        break;
     case 115200:
        Mb_tio.c_cflag = B115200;
        break;
     default:
        Mb_tio.c_cflag = B9600;
  }
  switch (Mbc_bit_l)
  {
     case 7:
        Mb_tio.c_cflag = Mb_tio.c_cflag | CS7;
        break;
     case 8:
     default:
        Mb_tio.c_cflag = Mb_tio.c_cflag | CS8;
        break;
  }
  switch (Mbc_parity)
  {
     case 1:
        Mb_tio.c_cflag = Mb_tio.c_cflag | PARENB;
        break;
     case -1:
        Mb_tio.c_cflag = Mb_tio.c_cflag | PARENB | PARODD;
        break;
     case 0:
     default:
        Mb_tio.c_iflag = IGNPAR;
        break;
  }
  Mb_tio.c_iflag &= ~ICRNL;
  if (Mbc_bit_s==2)
     Mb_tio.c_cflag = Mb_tio.c_cflag | CSTOPB;
#if 1  
  Mb_tio.c_cflag = Mb_tio.c_cflag | CLOCAL | CREAD;
  Mb_tio.c_oflag = 0;
  Mb_tio.c_lflag = 0;
#endif
  /* clean port */
  tcflush(fd, TCIFLUSH);

  //fcntl(fd, F_SETFL, FASYNC);
  /* activate the settings port */
  if (tcsetattr(fd,TCSANOW,&Mb_tio) <0)
  {
    perror("Can't set terminal parameters ");
    return -1 ;
  }
  
  /* clean I & O device */
  tcflush(fd,TCIOFLUSH);

   if (Mb_verbose)
   {
      printf("setting ok:\n");
      printf("device        %s\n",Mbc_port);
      printf("speed         %d\n",Mbc_speed);
      printf("data bits     %d\n",Mbc_bit_l);
      printf("stop bits     %d\n",Mbc_bit_s);
      printf("parity        %d\n",Mbc_parity);
   }
   return fd ;
}

char *Mb_version(void)
{
   return VERSION;
}

int Csm_get_data(int len, int timeout)
{
	int i,flag;
	byte read_data;
	fd_set  rdfds;
	struct timeval tv;
	int ret;
	
	flag =i= 0;
	if (Mb_verbose)
		fprintf(stderr,"in get data\n");

	do{
		tv.tv_sec = timeout;
		tv.tv_usec = 0;
		FD_ZERO(&rdfds);
		FD_SET(Mb_device,&rdfds);
		ret = select(Mb_device+1, &rdfds, NULL, NULL, &tv);
		if(ret < 0) 
			perror("select");
		else if(ret > 0){
			if(read(Mb_device, &read_data, 1) == 1){
				if(!flag && !read_data)
					continue;
				flag = 1;
				Mbm_result[i]=read_data;
				if (Mb_verbose)
					fprintf(stderr,"receiving byte :i=%d data=0x%02x \n",i,read_data);
				len--;
				i++;
			}
		}else{
			printf("timeout\n");
			break;//timeout;
		}
	}while(len);

	if (Mb_verbose)
		fprintf(stderr,"receiving data done\n");
	return(1);
}
		
int Csm_send_and_get_result(unsigned char trame[], int timeout, int long_emit, int longueur)
{
	//int i;
	int ret;
	Mbm_result = trame;
	
	if (Mb_verbose)
		fprintf(stderr,"start writing \n");

	/* send data */
	write(Mb_device,trame,long_emit);

  if (Mb_verbose)
		fprintf(stderr,"write ok\n");

	if (tcsetattr(Mb_device,TCSANOW,&Mb_tio) <0) {
		perror("Can't set terminal parameters ");
		return 0;
	}
	ret = Csm_get_data(longueur, timeout);
	if (tcsetattr(Mb_device,TCSANOW,&Mb_tio) <0) {
		perror("Can't set terminal parameters ");
		return 0 ;
	}
	
	return ret;
}


/************************************************************************************
					Mbm_master : comput and send a master packet
*************************************************************************************
input :
-------
Mb_trame	  : struct describing the packet to comput
						device		: device descriptor
						slave 		: slave number to call
						function 	: modbus function
						address		: address of the slave to read or write
						length		: lenght of data to send
data_in	  : data to send to the slave
data_out	  : data to read from the slave
timeout	  : timeout duration in ms
ptrfoncsnd : function to call when master send data (can be NULL if you don't whant to use it)
ptrfoncrcv : function to call when master receive data (can be NULL if you don't whant to use it)
*************************************************************************************
answer :
--------
0 : OK
-1 : unknow modbus function
-2 : CRC error in the slave answer
-3 : timeout error
-4 : answer come from an other slave
*************************************************************************************/
int Mb_master(Mbm_trame Mbtrame,int data_in[], int data_out[])
{
	int i,longueur,long_emit;
	int slave, function, adresse, nbre;
	byte trame[256];

	slave=Mbtrame.slave;
	function=Mbtrame.function;
	adresse=Mbtrame.address;
	nbre=Mbtrame.length;
	
	if((slave >= 1) && (slave <= 32))
	{
		revert = 1;
	}			
	else if((slave >= 33) && (slave <= 64))
	{
		revert = 0;
	}
	else if((slave >= 65) && (slave <= 96))
	{
		revert = 0;
	}
	
	switch (function)
	{
		case 0x03:
		case 0x04:
			/* read n byte */
			trame[0]=slave;
			trame[1]=function;
			trame[2]=adresse>>8;
			trame[3]=adresse&0xFF;
			trame[4]=nbre>>8;
			trame[5]=nbre&0xFF;
			/* comput crc */
			Mb_calcul_crc(trame,6,revert);
			/* comput length of the packet to send */
			long_emit=8;
			break;
		
		case 0x05: //write a single coil
			trame[0]=slave;
			trame[1]=function;
			trame[2]=adresse>>8;
			trame[3]=adresse&0xFF;
			trame[4]=data_in[0]>>8;
			trame[5]=data_in[0]&0xFF;
			/* comput crc */
			Mb_calcul_crc(trame,6,revert);
			/* comput length of the packet to send */
			long_emit=8;
			break;
			
		case 0x06:
			/* write one byte */
			trame[0]=slave;
			trame[1]=function;
			trame[2]=adresse>>8;
			trame[3]=adresse&0xFF;
			trame[4]=data_in[0]>>8;
			trame[5]=data_in[0]&0xFF;
			/* comput crc */
			Mb_calcul_crc(trame,6,revert);
			/* comput length of the packet to send */
			long_emit=8;
			break;

		case 0x07:
			/* read status */
			trame[0]=slave;
			trame[1]=function;
			/* comput crc */
			Mb_calcul_crc(trame,2,revert);
			/* comput length of the packet to send */
			long_emit=4;
			break;
			
		case 0x08:
			/* line test */
			trame[0]=slave;
			trame[1]=0x08;
			trame[2]=0;
			trame[3]=0;
			trame[4]=0;
			trame[5]=0;
			Mb_calcul_crc(trame,6,revert);
			/* comput length of the packet to send */
			long_emit=8;
			break;
			
		case 0x10:
			/* write n byte  */
			trame[0]=slave;
			trame[1]=0x10;
			trame[2]=adresse>>8;
			trame[3]=adresse&0xFF;
			trame[4]=nbre>>8;
			trame[5]=nbre&0xFF;
			trame[6]=nbre*2;
			for (i=0;i<nbre;i++)
			{
				trame[7+i*2]=data_in[i]>>8;
				trame[8+i*2]=data_in[i]&0xFF;
			}
			/* comput crc */
			Mb_calcul_crc(trame,7+nbre*2,revert);
			/* comput length of the packet to send */
			long_emit=(nbre*2)+9;
			break;
		default:
			return -1;
	}
	if (Mb_verbose) 
	{
		fprintf(stderr,"send packet length %d\n",long_emit);
		for(i=0;i<long_emit;i++)
			fprintf(stderr,"send packet[%d] = %02x\n",i,trame[i]);
	}
	
	/* comput length of the slave answer */
	switch (function)
	{
		case 0x03:
		case 0x04:
			if((slave >= 1) && (slave <= 32))
			{
				revert = 1;
				longueur = 17;
			}			
			else if((slave >= 33) && (slave <= 64))
			{
				longueur = 18;
			}
			else if((slave >= 65) && (slave <= 96))
			{
				longueur = 19;
			}
			break;
		
		case 0x05:
		case 0x06:
		case 0x08:
		case 0x10:
		longueur=8;
			break;

		case 0x07:
		longueur=5;
			break;

		default:
			return -1;
			break;
	}

	/* send packet & read answer of the slave
		answer is stored in trame[] */
	for(i = 0;i < 4; i++){
		if(Csm_send_and_get_result(trame,Mbtrame.timeout,long_emit,longueur)){
			i = 1;
			break;
		}
	}
	if(i != 1) 
		return -3;	/* timeout error */




  	if (Mb_verbose)
	{
		fprintf(stderr,"answer :\n");
		for(i=0;i<longueur;i++)
			fprintf(stderr,"answer packet[%d] = %0x\n",i,trame[i]);
	}
	
	if (trame[0]!=slave)
		return -4;	/* this is not the right slave */

	switch (function)
	{
		case 0x03:
		case 0x04:
			/* test received data */
			if (trame[1]!=0x03 && trame[1]!=0x04)
				return -2;
			if (Mb_test_crc(trame, longueur-2, revert))
				return -2;
			/* data are ok */
			if (Mb_verbose)
				fprintf(stderr,"Reader data \n");
			for(i=0;i<18;i++){
				if(isdigit(trame[i]) || (trame[i]== '.') ||
					(trame[i] == ' '))
				printf("%c",trame[i]);
			}
			printf("\n");
			for (i=0;i<nbre;i++)
			{
				data_out[i]=(trame[3+(i*2)]<<8)+trame[4+i*2];
				if (Mb_verbose)
					fprintf(stderr,"data %d = %0x\n",i,data_out[i]);
			}
			break;
			
		case 0x05: //write a single coil
			/* test received data */
			if (trame[1]!=0x05)
				return -2;
			if (Mb_test_crc(trame,6,revert))
				return -2;
			/* data are ok */
			if (Mb_verbose)
				fprintf(stderr,"data stored succesfull !\n");
			break;
		
		case 0x06:
			/* test received data */
			if (trame[1]!=0x06)
				return -2;
			if (Mb_test_crc(trame,6,revert))
				return -2;
			/* data are ok */
			if (Mb_verbose)
				fprintf(stderr,"data stored succesfull !\n");
			break;

		case 0x07:
			/* test received data */
			if (trame[1]!=0x07)
				return -2;
			if (Mb_test_crc(trame,3,revert))
				return -2;
			/* data are ok */
			data_out[0]=trame[2];	/* store status in data_out[0] */
			if (Mb_verbose)
				fprintf(stderr,"data  = %0x\n",data_out[0]);
			break;

		case 0x08:
			/* test received data */
			if (trame[1]!=0x08)
				return -2;
			if (Mb_test_crc(trame,6,revert))
				return -2;
			/* data are ok */
			if (Mb_verbose)
				fprintf(stderr,"Loopback test ok \n");
			break;

		case 0x10:
			/* test received data */
			if (trame[1]!=0x10)
				return -2;
			if (Mb_test_crc(trame,6,revert))
				return -2;
			/* data are ok */
			if (Mb_verbose)
				fprintf(stderr,"%d setpoint stored succesfull\n",(trame[4]<<8)+trame[5]);
			break;

		default:
			return -1;
			break;
	}
	return 0;
}
