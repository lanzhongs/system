#include <common.h>
#include <command.h>
#include <net.h>
#include <flash.h>
#include <jffs2/jffs2.h>
#include <malloc.h>
#include <nand.h>
#include <spi_flash.h>

#define UPFILE_NAME           "ldfirm.upg"
#define MAX_FILE_NUM          32
#define PAGE_SIZE		    	4096		/* 4KB */
#define CFG_MAGIC          	0x43594954  /*CYIT */
#define RESERVED_FEA_NUMS	    16
#define JFFS2_IMG_ADDR		0x82000000

extern int g_spi_boot;

/* file header of upgrade files */
typedef struct{	                /* 64 bytes */
	char 	fileName[32];	    /* 文件名 */
	int	    fileLen;            /* 文件长度 */
	int  	checkSum;           /* 校验和 */
	int	    startOffset;        /* 起始位置 */
	unsigned char	res[20];	
}UPGRADE_FILE_HEADER;

/* file header of digicap.mav */
typedef struct {/* 64 bytes */
	unsigned int	magic_number;			        /* 0x43594954 CYIT */
	unsigned int	header_check_sum;		        /* 文件头校验和 */
	unsigned int	header_length;			        /* 文件头长度 */
	unsigned int	file_nums;                      /* 文件个数 */
	unsigned int	upg_version;                    /* 升级包版本*/	
	unsigned short	language;                       /* 见SYS_UPG_DEV_LANG_E*/
	unsigned short	device_class;			        /* 见SYS_UPG_DEV_TYPE_E */
	unsigned short	area;	                        /* 1 – hikvision  */
	unsigned char	res[38];
	UPGRADE_FILE_HEADER  fileHeader[0];
}FIRMWARE_HEADER;

/*--------------------------------------------------------------------------------------------------*/
#ifndef MAX_ETHERNET
#define MAX_ETHERNET			2
#endif

#ifndef MACADDR_LEN
#define MACADDR_LEN         	6
#endif

#define PRODDATELEN         	8			/* 生产日期 */
#define PRODNUMLEN          	9			/* 9位数的序列号 */
#define SERIALNO_LEN        	48			/* 序列号长度 */
#define RESERVED_FEA_NUMS		16			/* reserved feature numbers */
#define IGNORE_VALUE32		0xffffffff
#define IGNORE_VALUE8			0xff


/* 系统信息 */
typedef struct {/* 256 bytes */
/* 0  */unsigned int 	    magicNumber;		                    /* 幻数 */
/* 4  */unsigned int	    paraChecksum;	                        /* 检查和 */
/* 8  */unsigned int	    paraLength;		                    /* 结构长度 */	/* 检查和、长度从'encryptVer'开始计算 */
/* 12 */unsigned int	    encryptVer;							/* 加密版本:用于控制此结构的更改 */

/* 以下4项用户升级控制:必须与升级文件包中的内容一致 */

/* 16 */unsigned int		language;								/* 语言 */
/* 20 */unsigned int		device_class;							/* 产品类型, 1 -- DS9000 DVR, ... */
/* 24 */unsigned int		oemCode;								/* oem 代码: 1 -- hikvision自己, ... */
/* 28 */unsigned char		reservedFeature[RESERVED_FEA_NUMS];	/* 保留的产品特性，用于升级控制 */

/* 44 */unsigned short 	    encodeChans;		                    /* 编码路数 */
/* 46 */unsigned short 	    decodeChans;		                    /* 解码路数 */
/* 48 */unsigned short		ipcChans;								/* IPC通道数 */
/* 50 */unsigned short		ivsChans;								/* 智能通道数 */
/* 52 */unsigned char	    picFormat;			                    /* 编码最大分辨率 0--CIF, 1--2CIF, 2--4CIF */
/* 53 */unsigned char 	    macAddr[MAX_ETHERNET][MACADDR_LEN];	/* 物理地址 */
/* 65 */unsigned char 	    prodDate[PRODDATELEN];	                /* 生产日期 */
/* 73 */unsigned char 	    prodNo[PRODNUMLEN];		            /* 产品序号 */
/* 82 */unsigned char 	    devHigh;			                    /* 高度 1--1U, 2--2U                     */
/* 83 */unsigned char 	    cpuFreq;			                    /* ARM主频: 1--400Mhz, 2--500Mhz ..      */
/* 84 */unsigned char 	    dspFreq;			                    /* DSP主频: 1--700Mhz, 2--900Mhz ..      */
/* 85 */unsigned char 	    zone;				                    /* 销售地区: 1--大陆 2--港台 3--海外 ... */
/* 86 */unsigned char	    webSupport;			                /* 支持WEB */
/* 87 */unsigned char 	    voipSupport;		                    /* 支持VOIP */
/* 88 */unsigned char 	    usbNums;			                    /* USB个数: 0、1、2 */
/* 89 */unsigned char 	    lcdSupport;			                /* 支持LCD */
/* 90 */unsigned char	    voNums;			                    /* 本地VO个数: 0、1、2 */
/* 91 */unsigned char 	    vganums;			                    /* VGA个数: 0、1、2 */
/* 92 */unsigned char 	    vtSupport;			                    /* 支持语音对讲  */
/* 93 */unsigned char		videoMaxtrix;							/* 视频矩阵输出: 0 -- 无，1 -- 16进4出 ... */
/* 94 */unsigned char 	    extendedDecoder;						/* 多路解码扩展板 */
/* 95 */unsigned char 	    extendedIVS;							/* 智能视频分析扩展板 */
/* 96 */unsigned char 	    extendedAlarmOut;						/* 报警输出扩展板 */
		unsigned char		res1[3];
/*100 */unsigned short	    devType;			                    /* 设备型号，2个字节 */
		unsigned char		res2[2];
/*104 */unsigned int        ubootAdrs;			                    /* uboot存放flash地址       */
/*108 */unsigned int        ubootSize;                             /* uboot大小                */
/*112 */unsigned int        ubootCheckSum;		                /* uboot校验值              */
/*116 */unsigned int        tinyKernelAdrs;		                /* tinyKernel存放flash地址  */
/*120 */unsigned int        tinyKernelSize;                       /* tinyKernel大小           */
/*124 */unsigned int        tinyKernelCheckSum;	                /* tinyKernel校验值         */
/*128 */unsigned char	    devModel[64];		                    /* 产品型号:考虑国标型号，扩充到64字节 */
/*192 */unsigned char	    res3[64];
} BOOT_PARMS;
/*--------------------------------------------------------------------------------------------------*/

struct mtd_partition {
	char *name;		        /* identifier string */
	u_int32_t size;		    /* partition size */
	u_int32_t offset;		/* offset within the master MTD space */
	u_int32_t mask_flags;	/* master MTD flags to mask out for this partition */
};

#ifndef CONFIG_TI814X_MIN_CONFIG
/****************************************************************************
 Function: check_byte_sum
 
 Description:count checksum
 
 Input:  1)char *pdata: input data
            2)int len: data length
 
 Output: N/A
 
 Return: 1)sum:input date checksum
*****************************************************************************/
unsigned int check_byte_sum(char *pdata, int len)
{
	unsigned int sum = 0;
	int i;

	for(i=0; i<len; i++)
	{
		sum += ((unsigned int)pdata[i]) & 0xff;
	}

	return sum;
}

/****************************************************************************
 Function: convert_data
 
 Description:covert data with xor 
 
 Input:  1)char *src: source data
            2)int len: length of source data
            
 Output: 1)char *dst: destination to put output data
 
 Return: 1) -1: failed
             2) 0: success
*****************************************************************************/
int convert_data(char *src, char *dst, int len)
{
    /* 固定的幻数，用于异或变换 */
    unsigned char magic[] =
	{0xe3,	0xf8,	0xf4,	0xf0,	0xe7,	0xf3,	0xe8,	0xf2,	
	 0xe4,	0xeb,	0xe6,	0xef,	0xee,	0xe5,	0xe4,	0xe2,	
	 0xf6,	0xf1,	0xe9,	0xf5,	0xf1,	0xea,	0xec,	0xee,	
	 0xe4,	0xfa,	0xe8,	0xed,	0xf9,	0xf5,	0xe1,	0xee};
	
	int i, j;
	int magiclen, startmagic;

	if(src==NULL || dst==NULL) {
		printf("Invalid input param: src = 0x%x, dst = 0x%x\n", (u_int)src, (u_int)dst);
		return -1;
	}

	magiclen = sizeof(magic);
	for(i=0, startmagic=0; i<len; startmagic=(startmagic+1)%magiclen) {
		/* 用startmagic控制每次内循环magic的起始位置 */
		for(j=0; j<magiclen && i<len; j++, i++) {
			*dst++ = *src++ ^ magic[(startmagic+j)%magiclen];	/* 进行异或变换 */
		}
	}
	
	return 0;
}

#if 1
/****************************************************************************
 Function: do_update
 
 Description: Do software update, make update files to jffs2 format and 
    write to one mtd partition of flash.
	
 Input:  1) cmd_tbl_t *cmdtp: command internal data
 	2) int flag: command flag
 	3) int argc: number of arguments supplied to the command
 	4) char *argv[]: arguments list
 	
 Output: N/A
 
 Return: 1)1: success
 	2)0: TFTP error or flash operation error
 	3)-1: upgrade argumnets mismatch
*****************************************************************************/
//void set_panel_ready(int led_on);

#ifdef CONFIG_FLASH_CFI_DRIVER
extern flash_info_t flash_info[]; /* cfi_flash.c */
#endif

struct mtd_partition nand_mtd_parts[] = 
{
	{
	.name = "flash_sys0",
	.offset = 0x0000000,	
	.size   =  0x4000000 ,/* 64M */
	.mask_flags = 0,	
	},

	{
	.name = "flash_sys1",
	.offset = 0x4000000,
	.size   =  0x4000000 ,/* 64M */
	.mask_flags = 0,	
	},
	
	{
	.name = "data_config",
	.offset = 0x8000000,
	.size   =  0x8000000 ,/* 128M */
	.mask_flags = 0,	
	},
/*
	{
	.name = "data_log",
	.offset = 0x10000000,	//256M
	.size   = 0x10000000 ,	//256M
	.mask_flags = 0,	
	},*/	
};


#define ROOT_DIR 			"/nand/"
#define FLAG_FILE_NAME	"version.bin"
#include <nand.h>
#include <spi_flash.h> 
extern struct spi_flash *spi_flash_init(void);
extern int yaffs2_StartUp(int startblock, int endblock);
extern int yaffs_mount(const char *path);
extern int yaffsfs_GetError(void);
extern void yaffs_set_mount_flag(int flg);
extern int yaffs_get_mount_flag(void);
extern void cmd_yaffs_umount(char *mp);
extern void reset_cpu (ulong addr);//reset system

void umount_partition(void)
{
    cmd_yaffs_umount(ROOT_DIR);
}

int  mount_partition(int part_num)
{
    int start_block, end_block;
    struct mtd_info *mtd = &nand_info[0];

    if(part_num < 0 || part_num > 1)
    {
        printf("%s param error : %d \n", __FUNCTION__, part_num);
        return -1;
    }

    if(1 == yaffs_get_mount_flag())
    {
        printf("partition has mounted, now umount first!\n");
	  umount_partition();
    }
    	
    start_block = (uint32_t)nand_mtd_parts[part_num].offset / mtd->erasesize;
    end_block = (uint32_t)( nand_mtd_parts[part_num].offset + nand_mtd_parts[part_num].size) / mtd->erasesize -1;

    yaffs2_StartUp(start_block, end_block);
	
    debug("start block: %d end block: %d\n", start_block, end_block);
	
    int retval = yaffs_mount(ROOT_DIR);
    if( retval != -1)
    	yaffs_set_mount_flag(1);
    else
    {
        printf("Error mounting %s/%s, return value: %d\n", 
                   ROOT_DIR, nand_mtd_parts[part_num].name, yaffsfs_GetError());
        return -1;
    }

    return 0;
}
#endif
int do_nand_update(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	char file_name[256] = {0};
	u_long file_pos = 0;

	int ftpsize, header_len, header_checksum;
	UPGRADE_FILE_HEADER *uheader;
	FIRMWARE_HEADER dst_fheader, *fheader, *pheader = NULL;
	BOOT_PARMS bp;
	
	int  i,k, ret, offset = 0;
	u_int start_addr, len, pre_size;
	int is_find = 0;/* 判断是否找到匹配设备的升级包 */
	int package_idx = 0;


	
	copy_filename(BootFile, UPFILE_NAME, sizeof(BootFile));
	load_addr = ((_bss_end + 0xfffff)/0x100000)*0x100000 + 0x100000;/* modify for big update package */

	/* Download by TFTP */
	if((ftpsize = NetLoop(TFTP)) < 0) {	
		return -1;   
	}


	/////////////////////////////////////////查找正确的升级包////////////////////////////
	/* Convert firmware header */
	while(offset < ftpsize) {
		fheader = (FIRMWARE_HEADER *)(load_addr + offset);
		memset(&dst_fheader, 0, sizeof(FIRMWARE_HEADER));
		convert_data((char *)fheader, (char *)&dst_fheader, sizeof(FIRMWARE_HEADER));
		/* Check file numbers is correct */
		if(dst_fheader.file_nums == 0 || dst_fheader.file_nums > MAX_FILE_NUM) {
			printf("file header error\n");
			return -1;
		}

		header_len = sizeof(FIRMWARE_HEADER) + dst_fheader.file_nums * sizeof(UPGRADE_FILE_HEADER);
		/* 因每个升级包的文件数量不一定相同，故需每次申请 */
		pheader = (FIRMWARE_HEADER *)malloc(header_len);
		if(NULL == pheader) {
			printf("malloc failed\n");
			return -1;
		}
		
		/* Convert whole file header */
		memset(pheader, 0, header_len);
		convert_data((char *)fheader, (char *)pheader, header_len);
		header_checksum = check_byte_sum((char *)pheader + 12 , header_len - 12);

		/* Check file header is correct? */
		if( pheader->magic_number != CFG_MAGIC ||
			pheader->header_length != header_len ||
			pheader->header_check_sum != header_checksum) {
			printf("file header error\n");
			free(pheader);
			return -1;
		}

		is_find = 1;	
		if(is_find) {
			break;
		}

		uheader = (UPGRADE_FILE_HEADER *)(pheader + 1);
		uheader +=  pheader->file_nums - 1;
		offset += uheader->startOffset + uheader->fileLen;/* 跳转到下一个升级包 */

		free(pheader);
		package_idx++;
	}

	if(0 == is_find) {
		printf("The board info: language - 0x%X device_class - 0x%X oemCode - 0x%X\n", 
			bp.language, bp.device_class, bp.oemCode);
		printf("upgrade packet mismatch, please select correct packet\n");
		return -1;
	}
	printf("The number of correct packet is: %d \n", package_idx);
	


	//////////////////////////////////////删除整个分区上的内容///////////////////////////
	start_addr = nand_mtd_parts[0].offset;
	len = nand_mtd_parts[0].size + nand_mtd_parts[1].size;

	printf("start to erase nand flash addr: 0x%X, len: 0x%X ... \n", start_addr, len);
	nand_erase_options_t opts;
	memset(&opts, 0, sizeof(opts));
	opts.offset = start_addr;
	opts.length = len ;
	extern nand_info_t nand_info[CONFIG_SYS_MAX_NAND_DEVICE];
	ret = nand_erase_opts(&nand_info[0], &opts);
	printf("%s\n", ret ? "ERROR" : "OK");
	/////////////////////////////////////////////////////////////////////////////////////


	
	//////////////////////////////////////////写入升级文件///////////////////////////////

	struct mtd_info *mtd = &nand_info[0];
	pre_size = mtd->size;	/* mount first part */

	for(k = 0; k < 2; k ++)
    {
        printf("\nstart to write files to partition %d...\n", k);

        mtd->size = nand_mtd_parts[k].offset+ nand_mtd_parts[k].size;
	
        if(-1 == mount_partition(k))
        {
            printf("mount partition %d failed...\n", k);
            goto ERR_HANDLE;
        }

        uheader = (UPGRADE_FILE_HEADER *)(pheader + 1);		
        
        for(i = 0; i < pheader->file_nums; i ++)
        {
            file_pos = load_addr + uheader->startOffset;
            memset(file_name, 0x0, sizeof(file_name));
            sprintf(file_name, "%s%s", ROOT_DIR,  uheader->fileName);

            ret = cmd_yaffs_mwrite_file(file_name, (char*)file_pos, uheader->fileLen);
            if(0 != ret)
            {
                printf("Error: write %s len %d failed! Errno:%d\n", file_name, uheader->fileLen, yaffsfs_GetError());
                return  -1;
            }
            printf("\t%d/%d.  write %s ok.\n", i + 1,pheader->file_nums + 1, file_name);
            uheader ++;
        }
        
        //write flag file to nand flash
        len = 0;
        memset(file_name, 0x0, sizeof(file_name));
        sprintf(file_name, "%s%s", ROOT_DIR,  FLAG_FILE_NAME);
        printf("\t%d/%d.  write flag file %s ok\n",i + 1,pheader->file_nums + 1, file_name);
        ret = cmd_yaffs_mwrite_file(file_name, (char*)&len, sizeof(len));
        if(0 != ret)
        {
            printf("Error: write %s len %d failed!\n", file_name,  sizeof(len));
            return  -1;
        }
        printf("update partition %d success!\n", k);

        umount_partition();
    }
	
    mtd->size = pre_size;
	
    printf("\nUpgrade success!\n");
    /* upgrade success, reset the machine */
	printf("Press ENTER key to reboot\n");
    readline(" ");
 	reset_cpu(0);
	return 0;
	
ERR_HANDLE:
	printf("\nUpgrade failed !\n");
    /* upgrade success, reset the machine */
    printf("Press ENTER key to reboot and reupgrade\n");
	readline(" ");
	reset_cpu(0);
	return -1;
	
}

int do_update(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{	
	extern int g_spi_boot;
	if(g_spi_boot) {
		return do_nand_update(cmdtp,flag,argc,argv);
	}

}

U_BOOT_CMD(update,1,0,do_update,"update  - update ldfirm.upg",NULL);


static int do_format(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{	
	extern int g_spi_boot;
	int scrub = 0;

	if(g_spi_boot) {
		extern nand_info_t nand_info[CONFIG_SYS_MAX_NAND_DEVICE];
		nand_erase_options_t opts;
		u_int start_addr = 0;
		u_int len = nand_info[0].size;
		int ret;
		
		memset(&opts, 0, sizeof(opts));
		opts.offset = start_addr;
		opts.length = len ;
		
		opts.scrub = scrub; //是否擦除坏块
		printf("opts.scrub = %d\n",opts.scrub);
		ret = nand_erase_opts(&nand_info[0], &opts);
		printf("%s\n", ret ? "ERROR" : "OK");
		
	} 
	return 0;
}

U_BOOT_CMD(format,2,1,do_format,"format  - format nand except bootloader area",NULL);
#endif
static void user_confirm(void) 
{
	char x;
	printf("*******************************************************\n"
            "*    ATTENTION: PLEASE READ THIS NOTICE CAREFULLY!    *\n"
            "* It is a DANGEROUS process,if it failed,the device   *\n"
			"* maybe unusable.Do not reset the  device,or disrupt  *\n"
            "* the process.If you do not know how to deal with the *\n"
			"* condition,poweroff the device now.To start continue *\n"
			"* press SPACE key.                                    *\n"
			"*******************************************************\n\n");
	do {    
		while (!tstc());
        x = getc();
		if (x == ' ') {
			break;
		}
	} while(1);	
}
static int do_updateboot(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{		
	extern int g_spi_boot;
	struct spi_flash *sf;
	int rc,uboot_sz;
	u_long start_addr,end_addr;//buf_addr;

#ifndef CONFIG_TI814X_MIN_CONFIG
	printf("Updater u-boot\n");
	user_confirm();	
# ifdef CONFIG_TI814X
	copy_filename(BootFile, "u-boot.bin", sizeof(BootFile));
#else
	copy_filename(BootFile, "u-boot.noxip.bin.spi", sizeof(BootFile));
#endif
	load_addr = CONFIG_SYS_LOAD_ADDR;
	if((uboot_sz = NetLoop(TFTP)) < 0)
		return 1;

	if(g_spi_boot) {
		struct spi_flash *sf;
		sf = spi_flash_probe(0, 0, 1000000, 3);
		if (sf) {
# ifdef CONFIG_TI814X
			spi_flash_erase(sf, CONFIG_MIN_UBOOT_SIZE, CONFIG_COM_UBOOT_SIZE);
			spi_flash_write(sf, CONFIG_MIN_UBOOT_SIZE, uboot_sz, (void *)load_addr);
# else
			spi_flash_erase(sf, 0, CONFIG_ENV_OFFSET);
			spi_flash_write(sf, 0, uboot_sz, (void *)load_addr);
# endif
			printf("\n\tupdate common uboot success!\n\n");
			//spi_flash_free(sf);
		} else {
			printf("update spi-uboot failed\n");
			return -1;
		}		
		return 0;
	}
#else
	sf = spi_flash_probe(0, 0, 1000000, 3);
	if (!sf) {
		printf("not found spi flash,update spi-uboot failed\n");
		return -1;
	}		
	copy_filename(BootFile, "u-boot.min.spi", sizeof(BootFile));
	load_addr = CONFIG_SYS_LOAD_ADDR;
	if((uboot_sz = NetLoop(TFTP)) < 0)
		return 1;
	spi_flash_erase(sf, 0, CONFIG_MIN_UBOOT_SIZE);
	spi_flash_write(sf, 0, uboot_sz, (void *)load_addr);
	printf("\n\tupdate mini uboot success!\n\n");

	copy_filename(BootFile, "u-boot.bin", sizeof(BootFile));
	load_addr = CONFIG_SYS_LOAD_ADDR;
	if((uboot_sz = NetLoop(TFTP)) < 0)
		return 1;
	spi_flash_erase(sf, CONFIG_MIN_UBOOT_SIZE, CONFIG_COM_UBOOT_SIZE);
	spi_flash_write(sf, CONFIG_MIN_UBOOT_SIZE, uboot_sz, (void *)load_addr);
	printf("\n\tupdate common uboot success!\n\n");
#endif

	return 0;
}

U_BOOT_CMD(updateb,1,1,do_updateboot,"updateb - update uboot(u-boot.bin)",NULL);
