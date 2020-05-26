#include 	<stdlib.h>
#include 	<string.h>
#include 	"drv_uart.h"
#include 	"stdio.h"
#include	"ff.h"
#include  "ymodem.h"
#include  "stm32f4xx.h"
#include "common.h"
#include "flash_if.h"

#define		MAX_DISK			2
char			_Cmd_Str[40], _Drive[40] = "RPS:\\", _Cmd_Flag = NOT_CMD_MODE;
char			_Drvno[][5] = {"NAND", "USB", "SD"};
char 			_Path[20], _Drv = 0;
FATFS 		Fatfs;  
FILINFO 	Finfo;	//文件信息
char 			Lfname[512];
uint8_t 	tab_1024[1024] = {0};
uint8_t 	FileName[FILE_NAME_LENGTH];
void CmdCls(void)
{
		printf("\033[2J");	
}
void	CmdEnter ()
{
			PutStr("\r\n");	
}
void FileError(char	in_res)
{
	char		i;
	const char *str =
				"OK\0" "DISK_ERR\0" "INT_ERR\0" "NOT_READY\0" "NO_FILE\0" "NO_PATH\0"
				"INVALID_NAME\0" "DENIED\0" "EXIST\0" "INVALID_OBJECT\0" "WRITE_PROTECTED\0"
				"INVALID_DRIVE\0" "NOT_ENABLED\0" "NO_FILE_SYSTEM\0" "MKFS_ABORTED\0" "TIMEOUT\0"
				"LOCKED\0" "NOT_ENOUGH_CORE\0" "TOO_MANY_OPEN_FILES\0";
	if (in_res != FR_OK)
	{
		for (i = 0; i != in_res && *str; i++) 
			while (*str++) ;
		printf("\r\nFAT Error: FILE_%s !!!\r\n", str);
	}
}

void CmdDir(void)
{
	DIR			Dir;
 	long 		p1, s1, s2;
  char 		res = 0, cmd[40];
 	FATFS 	*fs;

 	PutStr("\r\n");

	memset(cmd, 0, 40);
	if (_Cmd_Str[0] == 0)			sprintf(cmd, "%d:\\%s", _Drv, _Path);
	else if ((_Cmd_Str[0] - '0') != _Drv && (_Cmd_Str[0] - '0') >= 0 && (_Cmd_Str[0] - '0') <= 1)			
		sprintf(cmd, "%s", _Cmd_Str);
	else						
		sprintf(cmd, "%d:\\%s\\%s", _Drv, _Path, _Cmd_Str);

  res = f_opendir(&Dir, cmd);	
	if (res != FR_OK)
	{
		FileError(res);					
		return;
	}
  p1 = s1 = s2 = 0;  
  while(((f_readdir(&Dir, &Finfo)) == FR_OK))
 	{  
		if (!Finfo.fname[0])		break;
		if (Finfo.fattrib & AM_DIR) 
		{
			printf("%u/%02u/%02u %02u:%02u  <DIR>              %s\r\n",
		   			(Finfo.fdate >> 9) + 1980, (Finfo.fdate >> 5) & 15, Finfo.fdate & 31,
		   			(Finfo.ftime >> 11), (Finfo.ftime >> 5) & 63, &(Finfo.fname[0]));			
			s2++;
		}
		else 
		{
			s1++; 			p1 += Finfo.fsize;
			printf("%u/%02u/%02u %02u:%02u  %c%c%c%c%c %9lu    %s\r\n",
		   			(Finfo.fdate >> 9) + 1980, (Finfo.fdate >> 5) & 15, Finfo.fdate & 31,
		   			(Finfo.ftime >> 11), (Finfo.ftime >> 5) & 63,
					(Finfo.fattrib & AM_DIR) ? 'D' : '-', (Finfo.fattrib & AM_RDO) ? 'R' : '-',
		   			(Finfo.fattrib & AM_HID) ? 'H' : '-', (Finfo.fattrib & AM_SYS) ? 'S' : '-',
		   			(Finfo.fattrib & AM_ARC) ? 'A' : '-',
					Finfo.fsize, &(Finfo.fname[0]));
		}
	}
	printf("%33lu    File(s)\r\n%33lu    bytes total\r\n%33lu    Dir(s)\r\n", s1, p1, s2);
	if (f_getfree("\\", (DWORD*)&p1, &fs) == FR_OK)
		printf("%33lu    bytes free\r\n", p1 * fs->csize * _MAX_SS);
}
// void CmdCd (void)
// {
// 	char	res;

// 	if (_Path[0] == 0)
// 	{
// 		strcat(_Path, "\\");
// 		strcat(_Path, _Cmd_Str);
// 	}
// 	else
// 	{
// 		if (_Cmd_Str[0] != 0)
// 		{
// 			strcat(_Path, "\\");
// 			strcat(_Path, _Cmd_Str);
// 		}
// 	}
// 	if (_Drv <= MAX_DISK)		sprintf(_Drive, "\r\n%s:\\", _Drvno[_Drv]);	
// 	else						sprintf(_Drive, "\r\nRPS:\\");
// 	strcat(_Drive, _Path + 1);
// 	res = f_chdir(_Path);
// 	if (res != FR_OK)
// 	{
// 	   	FileError(res);
// 		_Path[0] = 0;
// 		f_chdir("\\");
// 		if (_Drv <= MAX_DISK)	sprintf(_Drive, "\r\n%s:\\", _Drvno[_Drv]);	
// 		else					sprintf(_Drive, "\r\nRPS:\\");
// 	}
// }
// void CmdCdout(void)
// {
// 	char	res, i;
// 	int		len;

// 	len = strlen(_Path);
// 	for (i = 0; len >= 0; len --)
// 	{
// 		if (len > 0)
// 		{
// 			if (_Path[len] == '\\')
// 			{
// 				i = 1;
// 				_Path[len] = 0;
// 				if (_Path[len - 1] == '\\')
// 					_Path[len - 1] = 0;
// 				break;
// 			}
// 		}	
// 	}
// 	if (i == 0)
// 	{
// 		_Path[0] = 0;
// 		f_chdir("\\");
// 	 	if (_Drv <= MAX_DISK)	sprintf(_Drive, "\r\n%s:\\", _Drvno[_Drv]);	
// 		else					sprintf(_Drive, "\r\nRPS:\\");
// 		return;
// 	}
// 	if (_Drv <= MAX_DISK)		sprintf(_Drive, "\r\n%s:\\", _Drvno[_Drv]);	
// 	else						sprintf(_Drive, "\r\nRPS:\\");
// 	strcat(_Drive, _Path + 1);
// 	res = f_chdir(_Path);
// 	if (res != FR_OK)
// 	{
// 	   	FileError(res);
// 		_Path[0] = 0;
// 		f_chdir("\\");
// 	 	if (_Drv <= MAX_DISK)	sprintf(_Drive, "\r\n%s:\\", _Drvno[_Drv]);	
// 		else					sprintf(_Drive, "\r\nRPS:\\");
// 	}
// }
// void CmdMkdir(void)
// {
// 	char	res;
// 	res = f_mkdir(_Cmd_Str);
// 	FileError(res);
// 	PutStr("\r\n");
// }
// void	CmdFormat(void)
// {
// 	char	res;

//   res = f_mkfs(0, 1, 0);
// 	if (res == FR_OK)			   
// 		printf("\r\nFormat ok\r\n");
// 	else
// 		FileError(res);
// }
typedef  void (*pFunction)(void);

pFunction 			 Jump_To_Application;
uint32_t 				 JumpAddress;
void	IAP_BOOT(void)
{
		if (((*(__IO uint32_t*)APPLICATION_ADDRESS) & 0x2FFE0000 ) == 0x20000000)
		{ 
				/* Jump to user application */
				JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
				Jump_To_Application = (pFunction) JumpAddress;
				/* Initialize user application's Stack Pointer */
				__set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);		
				Jump_To_Application();
		}			
}

void	Cmdexe(void)
{
		IAP_BOOT();
}
/**
  * @brief  Download a file via serial port
  * @param  None
  * @retval None
  */
void CmdDownload(void)
{
//  uint8_t Number[10] = "          ";
//  int32_t Size = 0;

//  SerialPutString("Waiting for the file to be sent ... (press 'a' to abort)\n\r");
//  Size = Ymodem_Receive(&tab_1024[0]);
//  if (Size > 0)
//  {
//    SerialPutString("\r\nProgramming Completed Successfully!\r\n Name: ");
//    SerialPutString(FileName);
//    Int2Str(Number, Size);
//    SerialPutString("\n\rSize: ");
//    SerialPutString(Number);
//    SerialPutString("Bytes\r\n");
//  }
//  else if (Size == -1)
//  {
//    SerialPutString("\n\n\rThe image size is higher than the allowed space memory!\n\r");
//  }
//  else if (Size == -2)
//  {
//    SerialPutString("\n\n\rVerification failed!\n\r");
//  }
//  else if (Size == -3)
//  {
//    SerialPutString("\r\n\nAborted by user.\n\r");
//  }
//  else
//  {
//    SerialPutString("\n\rFailed to receive the file!\n\r");
//  }
}

/**
  * @brief  Upload a file via serial port.
  * @param  None
  * @retval None
  */
void CmdUpload(void)
{
//  uint8_t status = 0 ; 

//  SerialPutString("\r\nSelect Receive File\n\r");

//  if (GetKey() == CRC16)
//  {
//    /* Transmit the flash image through ymodem protocol */
//    status = Ymodem_Transmit((uint8_t*)APPLICATION_ADDRESS, (const uint8_t*)"UploadedFlashImage.bin", USER_FLASH_SIZE);

//    if (status != 0) 
//    {
//      SerialPutString("\n\rError Occurred while Transmitting File\n\r");
//    }
//    else
//    {
//      SerialPutString("\n\rFile uploaded successfully \n\r");
//    }
//  }
}




void	CmdHelp(void)
{
	printf("\r\n                      IAP System Debug Ver1.01a by 2011-10-10\r\n");
	printf("+-----------------------------------------------------------------------------+\r\n");
	printf("|                                                                             |\r\n");
	printf("|  CLS:     Clears the screen            ?:         Command hlep              |\r\n");
	printf("|  DIR:     Displays list of files       EXE:       Exec app program          |\r\n");
	printf("|  DOWN:    Download chip img to pc      UPDAT:     Update firmware           |\r\n");
	printf("|                                                                             |\r\n");
	printf("+-----------------------------------------------------------------------------+\r\n");
						 
}

//*********************************************************************
typedef struct stMenuEnt {
		char	   *keywd;		// menu lookup keyword
		void	   (*fun)();	// dispatch function
} stMenuEnt;


stMenuEnt _Cmd_Menu[] = {
						{"?"      , CmdHelp			},
						{"\r"     , CmdEnter		},
						{"cls"    , CmdCls			},
						{"exe"    , Cmdexe			},
						{"dir"    , CmdDir			},
						{"down"   , CmdUpload		},
						{"updat"  , CmdDownload	},
						{0        , 0				}
						};

void	GetCmdLine (stComRxBuf *in_buf)
{
	stMenuEnt	*menu;
	int		len, i;

	if (_Cmd_Flag == NOT_CMD_MODE)
	{
		menu = _Cmd_Menu;

		while (menu->keywd)
		{
			len = strlen(menu->keywd);
			if (!strncmp (in_buf->buf, menu->keywd, len))
			{
				if (*(in_buf->buf + len) == ' ')
				{
					for (i = 0; i < 40; i ++)
					{
						if (*(in_buf->buf + i) == '\r')
							break;
					}
					memset(_Cmd_Str, 0x00, 40);
					memcpy(_Cmd_Str, (in_buf->buf + len + 1), i - len - 1);
				}
				break;
			}
			else
				menu++;
		}
		if(menu->keywd)
		{
 			in_buf->tail = 0;
			in_buf->bytes = 0;
			menu->fun();
		
			PutStr(_Drive);
			uart1_putc('>');
		}
		else
		{
				if (in_buf->buf[0] != 0)
				{
					PutStr("\r\n*** Bad command\r\n");
					PutStr(_Drive);
					uart1_putc('>');
				}
		}

		in_buf->tail = 0;
		in_buf->bytes = 0;
		_Cmd_Flag = CMD_MODE;
		memset(in_buf, 0, 100);
	}
}

