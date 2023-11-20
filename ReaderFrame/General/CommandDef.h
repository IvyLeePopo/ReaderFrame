#pragma once

//数据帧定义解析
typedef enum eProtocolConst
{
	PROTOCOL_HEADER = 0xDF,					//协议起始符
	PROTOCOL_END = 0xFD,					//协议休止符

	PROTOCOL_NUMBER = 0x00,			//协议帧编号

	PROTOCOL_CONVERT_CHAR = 0x00,			//协议转译公共字符

	PROTOCOL_HEAND_CONVERT_CHAR1 = 0xDE,	//协议起始符转译字符1
	PROTOCOL_HEAND_CONVERT_CHAR2 = 0x01,	//协议起始符转译字符2

	PROTOCOL_END_CONVERT_CHAR1 = 0xFC,		//协议休止符转译字符1
	PROTOCOL_END_CONVERT_CHAR2 = 0x01		//协议休止符转译字符2
}eProtocolType_t;

//主命令类型
typedef enum eTopCmdType
{
	TOP_CMD_TYPE = 0x58       //主命令类型，帧标记，其值:58(H)
}eTopCmdType_t;

//子命令类型
typedef enum eSubCmdType
{	
	//TOPCMD:58(H)
	SUB_CMD_TYPE_OpenReader = 0x01,			//打开 IC 卡读写器
	SUB_CMD_TYPE_CloseReader = 0x02,		//关闭 IC 卡读写器
	SUB_CMD_TYPE_SetInitPPS = 0x03,			//设置 IC 卡读写器通讯速率（可选）
	SUB_CMD_TYPE_SetInitTimeOut = 0x04,     //设置 IC 卡读写器超时时间（可选）
	SUB_CMD_TYPE_LEDDisplay = 0x05,         //设置读写器发光二极管
	SUB_CMD_TYPE_AudioControl = 0x06,       //设置读写器蜂鸣器
	SUB_CMD_TYPE_GetStatus = 0x07,          //获取设备状态
	SUB_CMD_TYPE_ReaderVersion = 0x08,      //获取设备版本信息
	SUB_CMD_TYPE_GetVersion = 0x09,         //获取设备动态库版本
	SUB_CMD_TYPE_GetStatusMsg = 0x10,       //获取错误码详细描述
	SUB_CMD_TYPE_OpenCard = 0x11,           //打开卡片
	SUB_CMD_TYPE_CloseCard = 0x12,          //关闭卡片
	SUB_CMD_TYPE_CPUCommand = 0x13,         //CPU 卡通用指令函数
	SUB_CMD_TYPE_SamReset = 0x14,           //SAM 卡复位
	SUB_CMD_TYPE_SamCommand = 0x15,         //SAM 卡通用指令函数
	SUB_CMD_TYPE_LoadKey = 0x16,            //M1 卡载入密钥（可选）
	SUB_CMD_TYPE_ReadBlock = 0x17,          //M1 卡读块函数（可选）
	SUB_CMD_TYPE_WriteBlock = 0x18,         //M1 卡写块函数（可选）
}eSubCmdType_t;


typedef enum enReaderApiErrorCode
{
    READER_API_NO_ERROR = 0,					//正常
    READER_API_ERROR_NO_CARD = -1,				//无卡
    READER_API_ERROR_OPEN_CARD_FAIL = -2,		//打开卡片失败
    READER_API_ERROR_CLOSE_CARD_FAIL = -3,		//关闭卡片失败
    READER_API_ERROR_NO_RESPONSE = -100,		//设备无响应
    READER_API_ERROR_INPUT_PARAMETER = -1000,	//传入参数错误
    READER_API_ERROR_DEV_IN_BUSY = -1001,		//设备被占用
    READER_API_ERROR_OPEN_DEV_FAIL = -1002,		//设备打开失败
    READER_API_ERROR_SET_RED_LED_FAIL = -1003,	//红色灯设置错误
    READER_API_ERROR_SET_GRN_LED_FAIL = -1004,	//绿色灯设置错误
    READER_API_ERROR_SET_BLE_LED_FAIL = -1005,	//蓝色灯设置错误
    READER_API_ERROR_OTHER = -2000,				//其他错误
    READER_API_ERROR_DISCONNECT = -2001,             //连接服务器错误
                                                //-2000 以上:预留
}enReaderApiErrorCode_t;
