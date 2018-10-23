#include "at_protocol.h"
#include "usart.h"
#include "common.h"
#include "task_net.h"
#include "inventr.h"
#include "SHT2x.h"
#include "bh1750.h"

u8 AT_ECHO = 0;
AT_Command_S AT_CommandBuf[AT_MAX_NUM];

void AT_CommandInit(void)
{
	u8 i = 0;
	
	AT_CommandBuf[i].id = i;
	AT_CommandBuf[i].len = strlen("RST");
	AT_CommandBuf[i].cmd = (char *)mymalloc(sizeof(char) * AT_CommandBuf[i].len + 1);
	memset(AT_CommandBuf[i].cmd,0,AT_CommandBuf[i].len);
	memcpy(AT_CommandBuf[i].cmd,"RST",AT_CommandBuf[i].len);
	i ++;
	
	AT_CommandBuf[i].id = i;
	AT_CommandBuf[i].len = strlen("GMR");
	AT_CommandBuf[i].cmd = (char *)mymalloc(sizeof(char) * AT_CommandBuf[i].len + 1);
	memset(AT_CommandBuf[i].cmd,0,AT_CommandBuf[i].len);
	memcpy(AT_CommandBuf[i].cmd,"GMR",AT_CommandBuf[i].len);
	i ++;
	
	AT_CommandBuf[i].id = i;
	AT_CommandBuf[i].len = strlen("DEVNAME");
	AT_CommandBuf[i].cmd = (char *)mymalloc(sizeof(char) * AT_CommandBuf[i].len + 1);
	memset(AT_CommandBuf[i].cmd,0,AT_CommandBuf[i].len);
	memcpy(AT_CommandBuf[i].cmd,"DEVNAME",AT_CommandBuf[i].len);
	i ++;
	
	AT_CommandBuf[i].id = i;
	AT_CommandBuf[i].len = strlen("DEVID");
	AT_CommandBuf[i].cmd = (char *)mymalloc(sizeof(char) * AT_CommandBuf[i].len + 1);
	memset(AT_CommandBuf[i].cmd,0,AT_CommandBuf[i].len);
	memcpy(AT_CommandBuf[i].cmd,"DEVID",AT_CommandBuf[i].len);
	i ++;
	
	AT_CommandBuf[i].id = i;
	AT_CommandBuf[i].len = strlen("UUID");
	AT_CommandBuf[i].cmd = (char *)mymalloc(sizeof(char) * AT_CommandBuf[i].len + 1);
	memset(AT_CommandBuf[i].cmd,0,AT_CommandBuf[i].len);
	memcpy(AT_CommandBuf[i].cmd,"UUID",AT_CommandBuf[i].len);
	i ++;
	
	AT_CommandBuf[i].id = i;
	AT_CommandBuf[i].len = strlen("OPERATORS");
	AT_CommandBuf[i].cmd = (char *)mymalloc(sizeof(char) * AT_CommandBuf[i].len + 1);
	memset(AT_CommandBuf[i].cmd,0,AT_CommandBuf[i].len);
	memcpy(AT_CommandBuf[i].cmd,"OPERATORS",AT_CommandBuf[i].len);
	i ++;
	
	AT_CommandBuf[i].id = i;
	AT_CommandBuf[i].len = strlen("APN");
	AT_CommandBuf[i].cmd = (char *)mymalloc(sizeof(char) * AT_CommandBuf[i].len + 1);
	memset(AT_CommandBuf[i].cmd,0,AT_CommandBuf[i].len);
	memcpy(AT_CommandBuf[i].cmd,"APN",AT_CommandBuf[i].len);
	i ++;
	
	AT_CommandBuf[i].id = i;
	AT_CommandBuf[i].len = strlen("DOMAIN");
	AT_CommandBuf[i].cmd = (char *)mymalloc(sizeof(char) * AT_CommandBuf[i].len + 1);
	memset(AT_CommandBuf[i].cmd,0,AT_CommandBuf[i].len);
	memcpy(AT_CommandBuf[i].cmd,"DOMAIN",AT_CommandBuf[i].len);
	i ++;
	
	AT_CommandBuf[i].id = i;
	AT_CommandBuf[i].len = strlen("IPADDRESS");
	AT_CommandBuf[i].cmd = (char *)mymalloc(sizeof(char) * AT_CommandBuf[i].len + 1);
	memset(AT_CommandBuf[i].cmd,0,AT_CommandBuf[i].len);
	memcpy(AT_CommandBuf[i].cmd,"IPADDRESS",AT_CommandBuf[i].len);
	i ++;
	
	AT_CommandBuf[i].id = i;
	AT_CommandBuf[i].len = strlen("PORT");
	AT_CommandBuf[i].cmd = (char *)mymalloc(sizeof(char) * AT_CommandBuf[i].len + 1);
	memset(AT_CommandBuf[i].cmd,0,AT_CommandBuf[i].len);
	memcpy(AT_CommandBuf[i].cmd,"PORT",AT_CommandBuf[i].len);
	i ++;
	
	AT_CommandBuf[i].id = i;
	AT_CommandBuf[i].len = strlen("CSQ");
	AT_CommandBuf[i].cmd = (char *)mymalloc(sizeof(char) * AT_CommandBuf[i].len + 1);
	memset(AT_CommandBuf[i].cmd,0,AT_CommandBuf[i].len);
	memcpy(AT_CommandBuf[i].cmd,"CSQ",AT_CommandBuf[i].len);
	i ++;
}


//AT指令帧协议解析,由串口1控制单灯控制器
u16 AT_CommandDataAnalysis(u8 *inbuf,u16 inbuf_len,u8 *outbuf,u8 *hold_reg)
{
	u8 i = 0;
	u16 ret = 0;
	u8 err_code = 1;
	u8 cmd_id = 255;
	u8 respbuf[256];
	
//	xSemaphoreTake(xMutex_AT_COMMAND, portMAX_DELAY);
	
	if((MyStrstr(inbuf, (u8 *)"AT", inbuf_len, 2) != 0xFFFF || \
		MyStrstr(inbuf, (u8 *)"AT+", inbuf_len, 3) != 0xFFFF) &&\
		MyStrstr(inbuf, (u8 *)"\r\n", inbuf_len, 2) != 0xFFFF)
	{
		memset(respbuf,0,256);
		
		if(inbuf_len == 4 || inbuf_len == 6)
		{
			if(inbuf_len == 4)
			{
				err_code = 0;
			}
			
			if(inbuf_len == 6)
			{
				if(MyStrstr(inbuf, (u8 *)"ATE0", inbuf_len, 4) != 0xFFFF)
				{
					AT_ECHO = 0;
					err_code = 0;
				}
				else if(MyStrstr(inbuf, (u8 *)"ATE1", inbuf_len, 4) != 0xFFFF)
				{
					AT_ECHO = 1;
					err_code = 0;
				}
			}
		}
		else
		{
			for(i = 0; i < AT_MAX_NUM; i ++)
			{
				if(MyStrstr(inbuf, (u8 *)AT_CommandBuf[i].cmd, inbuf_len, AT_CommandBuf[i].len) != 0xFFFF)
				{
					if(inbuf_len == AT_CommandBuf[i].len + 3 + 2 || \
						MyStrstr(inbuf, (u8 *)"=", inbuf_len, 1) != 0xFFFF)
					{
						cmd_id = i;
					}
					
					break;
				}
			}
			
			switch(cmd_id)
			{
				case RST:
					err_code = AT_CommandRST(cmd_id,inbuf,inbuf_len,respbuf);
				break;
				
				case GMR:
					err_code = AT_CommandGMR(cmd_id,inbuf,inbuf_len,respbuf);
				break;
				
				case DEVNAME:
					err_code = AT_CommandDEVNAME(cmd_id,inbuf,inbuf_len,respbuf);
				break;
				
				case DEVID:
					err_code = AT_CommandDEVID(cmd_id,inbuf,inbuf_len,respbuf);
				break;
				
				case UUID:
					err_code = AT_CommandUUID(cmd_id,inbuf,inbuf_len,respbuf);
				break;
				
				case OPERATORS:
					err_code = AT_CommandOPERATORS(cmd_id,inbuf,inbuf_len,respbuf);
				break;
				
				case APN:
					err_code = AT_CommandAPN(cmd_id,inbuf,inbuf_len,respbuf);
				break;
				
				case DOMAIN:
					err_code = AT_CommandDOMAIN(cmd_id,inbuf,inbuf_len,respbuf);
				break;
				
				case IPADDRESS:
					err_code = AT_CommandIPADDRESS(cmd_id,inbuf,inbuf_len,respbuf);
				break;
				
				case PORT:
					err_code = AT_CommandPORT(cmd_id,inbuf,inbuf_len,respbuf);
				break;
				
				case CSQ:
					err_code = AT_CommandCSQ(cmd_id,inbuf,inbuf_len,respbuf);
				break;
				
				default:
					
				break;
			}
		}
		
		ret = PackAT_CommandRespone(inbuf,err_code,respbuf,outbuf);
	}
	
//	xSemaphoreGive(xMutex_AT_COMMAND);
	
	return ret;
}

//打包AT指令回复数据
u8 PackAT_CommandRespone(u8 *inbuf,u8 err_code,u8 *respbuf,u8 *outbuf)
{
	u8 len = 0;
	
	if(AT_ECHO)	//开启回显
	{
		sprintf((char *)outbuf, "%s",inbuf);		//填充回显
	}
	
	if(strlen((char *)respbuf))
	{
		strcat((char *)outbuf,(char *)respbuf);		//填充返回的内容
	}
	
	if(err_code == 0)
	{
		strcat((char *)outbuf,"\r\nOK\r\n");		//填充结果OK
	}
	else
	{
		strcat((char *)outbuf,"\r\nERROR\r\n");		//填充结果ERROR
	}
	
	len = strlen((char *)outbuf);
	
	return len;
}

//复位
u8 AT_CommandRST(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf)
{
	u8 ret = 0;
	
	NeedToReset = 1;
	
	return ret;
}

//获取版本信息
u8 AT_CommandGMR(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf)
{
	u8 ret = 1;
	
	if(inbuf_len == AT_CommandBuf[cmd_id].len + 3 + 2)
	{
		sprintf((char *)respbuf, "bootloader: %s\r\nsoftware: %s\r\nhardware: %s",BootLoaderVersion,SoftWareVersion,HardWareVersion);
		ret = 0;
	}
	
	return ret;
}

//获取/设置设备名称
u8 AT_CommandDEVNAME(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf)
{
	u8 ret = 1;
	u8 content[34];

	if(inbuf_len == AT_CommandBuf[cmd_id].len + 3 + 2)
	{
		if(DeviceName != NULL)
		{
			sprintf((char *)respbuf, "device name: %s\r\n",DeviceName);
		}
		else
		{
			sprintf((char *)respbuf, "device name: null\r\n");
		}
		ret = 0;
	}
	else if(MyStrstr(inbuf, (u8 *)"=\"", inbuf_len, 2) != 0xFFFF)
	{
		memset(content,0,34);
		
		if(get_str1(inbuf, "\"", 1, "\"", 2, &content[1]))
		{
			content[0] = strlen((char *)&content[1]);
			
			if(content[0] <= 32)
			{
				CopyStrToPointer(&DeviceName, &content[1],content[0]);
				
				WriteDataFromHoldBufToEeprom(content,DEVICE_NAME_ADD, DEVICE_NAME_LEN - 2);
				
				ret = 0;
			}
		}
	}
	
	return ret;
}

//获取/设置设备ID
u8 AT_CommandDEVID(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf)
{
	u8 ret = 1;
	u8 i = 0;
	u8 content_str[13];
	u8 content_hex[6];
	u8 content_str_len = 0;

	if(inbuf_len == AT_CommandBuf[cmd_id].len + 3 + 2)
	{
		if(DeviceID != NULL)
		{
			sprintf((char *)respbuf, "device id: %02x%02x%02x%02x%02x%02x\r\n",\
				DeviceID[0],DeviceID[1],DeviceID[2],DeviceID[3],DeviceID[4],DeviceID[5]);
		}
		else
		{
			sprintf((char *)respbuf, "device id: null\r\n");
		}
		
		ret = 0;
	}
	else if(MyStrstr(inbuf, (u8 *)"=\"", inbuf_len, 2) != 0xFFFF)
	{
		memset(content_str,0,13);
		memset(content_hex,0,6);
		
		if(get_str1(inbuf, "\"", 1, "\"", 2, content_str))
		{
			content_str_len = strlen((char *)content_str);
			
			for(i = 0; i < 6; i ++)
			{
				content_hex[i] = (content_str[i + 0] - 0x30) * 16 + (content_str[i + 1] - 0x30);
			}
			
			if(content_str_len == 12)
			{
				CopyStrToPointer(&DeviceID, content_hex,content_str_len / 2);
				
				WriteDataFromHoldBufToEeprom(content_hex,DEVICE_ID_ADD, DEVICE_ID_LEN - 2);
				
				ret = 0;
			}
		}
	}
	
	return ret;
}

//获取/设置UUID
u8 AT_CommandUUID(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf)
{
	u8 ret = 1;
	u8 content[37];
	u8 content_len = 0;

	if(inbuf_len == AT_CommandBuf[cmd_id].len + 3 + 2)
	{
		if(DeviceUUID != NULL)
		{
			sprintf((char *)respbuf, "uuid: %s\r\n",DeviceUUID);
		}
		else
		{
			sprintf((char *)respbuf, "uuid: null\r\n");
		}
		ret = 0;
	}
	else if(MyStrstr(inbuf, (u8 *)"=\"", inbuf_len, 2) != 0xFFFF)
	{
		memset(content,0,37);
		
		if(get_str1(inbuf, "\"", 1, "\"", 2, content))
		{
			content_len = strlen((char *)content);
			
			if(content_len == 36)
			{
				CopyStrToPointer(&DeviceUUID, content,content_len);
				
				WriteDataFromHoldBufToEeprom(content,UU_ID_ADD, UU_ID_LEN - 2);
				
				ret = 0;
			}
		}
	}
	
	return ret;
}

//获取/设置运营商
u8 AT_CommandOPERATORS(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf)
{
	u8 ret = 1;
	u8 content;

	if(inbuf_len == AT_CommandBuf[cmd_id].len + 3 + 2)
	{
		sprintf((char *)respbuf, "operators: %d\r\n",Operators);
		ret = 0;
	}
	else if(inbuf_len == AT_CommandBuf[cmd_id].len +3 + 2 + 2 && \
		MyStrstr(inbuf, (u8 *)"=", inbuf_len, 1) != 0xFFFF)
	{
		content = *(inbuf + inbuf_len - 3) - 0x30;
		
		if(content < 3)
		{
			Operators = content;
			
			WriteDataFromHoldBufToEeprom(&Operators,OPERATORS_ADD, OPERATORS_LEN - 2);
			
			ret = 0;
		}
	}
	
	return ret;
}

//获取/设置APN
u8 AT_CommandAPN(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf)
{
	u8 ret = 1;
	u8 content[18];

	if(inbuf_len == AT_CommandBuf[cmd_id].len + 3 + 2)
	{
		if(APName != NULL)
		{
			sprintf((char *)respbuf, "apn: %s\r\n",APName);
		}
		else
		{
			sprintf((char *)respbuf, "apn: null\r\n");
		}
		
		ret = 0;
	}
	else if(MyStrstr(inbuf, (u8 *)"=\"", inbuf_len, 2) != 0xFFFF)
	{
		memset(content,0,18);
		
		if(get_str1(inbuf, "\"", 1, "\"", 2, &content[1]))
		{
			content[0] = strlen((char *)&content[1]);
			
			if(content[0] < 17)
			{
				CopyStrToPointer(&APName, &content[1],content[0]);
				
				WriteDataFromHoldBufToEeprom(content,APN_ADD, APN_LEN - 2);
				
				ret = 0;
			}
		}
	}
	
	return ret;
}

//获取/设置DOMAIN域名
u8 AT_CommandDOMAIN(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf)
{
	u8 ret = 1;
	u8 content[34];

	if(inbuf_len == AT_CommandBuf[cmd_id].len + 3 + 2)
	{
		if(ServerDomain != NULL)
		{
			sprintf((char *)respbuf, "domain: %s\r\n",ServerDomain);
		}
		else
		{
			sprintf((char *)respbuf, "domain: null\r\n");
		}
		
		ret = 0;
	}
	else if(MyStrstr(inbuf, (u8 *)"=\"", inbuf_len, 2) != 0xFFFF)
	{
		memset(content,0,34);
		
		if(get_str1(inbuf, "\"", 1, "\"", 2, &content[1]))
		{
			content[0] = strlen((char *)&content[1]);
			
			if(content[0] < 33)
			{
				CopyStrToPointer(&ServerDomain, &content[1],content[0]);
				
				WriteDataFromHoldBufToEeprom(content,SERVER_DOMAIN_ADD, SERVER_DOMAIN_LEN - 2);
				
				ret = 0;
			}
		}
	}
	
	return ret;
}

//获取/设置服务器IP
u8 AT_CommandIPADDRESS(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf)
{
	u8 ret = 1;
	u8 content[17];

	if(inbuf_len == AT_CommandBuf[cmd_id].len + 3 + 2)
	{
		if(ServerIP != NULL)
		{
			sprintf((char *)respbuf, "server ip: %s\r\n",ServerIP);
		}
		else
		{
			sprintf((char *)respbuf, "server ip: null\r\n");
		}
		ret = 0;
	}
	else if(MyStrstr(inbuf, (u8 *)"=\"", inbuf_len, 2) != 0xFFFF)
	{
		memset(content,0,17);
		
		if(get_str1(inbuf, "\"", 1, "\"", 2, &content[1]))
		{
			content[0] = strlen((char *)&content[1]);
			
			if(content[0] < 16)
			{
				CopyStrToPointer(&ServerIP, &content[1],content[0]);
				
				WriteDataFromHoldBufToEeprom(content,SERVER_IP_ADD, SERVER_IP_LEN - 2);
				
				ret = 0;
			}
		}
	}
	
	return ret;
}

//获取/设置服务器端口号
u8 AT_CommandPORT(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf)
{
	u8 ret = 1;
	u8 content[7];

	if(inbuf_len == AT_CommandBuf[cmd_id].len + 3 + 2)
	{
		if(ServerPort != NULL)
		{
			sprintf((char *)respbuf, "server port: %s\r\n",ServerPort);
		}
		else
		{
			sprintf((char *)respbuf, "server port: null\r\n");
		}
		ret = 0;
	}
	else if(MyStrstr(inbuf, (u8 *)"=\"", inbuf_len, 2) != 0xFFFF)
	{
		memset(content,0,7);
		
		if(get_str1(inbuf, "\"", 1, "\"", 2, &content[1]))
		{
			content[0] = strlen((char *)&content[1]);
			
			if(content[0] < 6)
			{
				CopyStrToPointer(&ServerPort, &content[1],content[0]);
				
				WriteDataFromHoldBufToEeprom(content,SERVER_PORT_ADD, SERVER_PORT_LEN - 2);
				
				ret = 0;
			}
		}
	}
	
	return ret;
}

//获取信号强度
u8 AT_CommandCSQ(u8 cmd_id,u8 *inbuf,u16 inbuf_len,u8 *respbuf)
{
	u8 ret = 1;

	if(inbuf_len == AT_CommandBuf[cmd_id].len + 3 + 2)
	{
		sprintf((char *)respbuf, "csq: %d\r\n",SignalIntensity);
		ret = 0;
	}
	
	return ret;
}
























