#include "TwProtocol.h"

# include "json/autolink.h"
# include "json/value.h"
# include "json/reader.h"
# include "json/writer.h"
# include "json/features.h"

#include <QDebug>

//json合法性检查
#define CHECKJSON(strData)\
    if(strData.empty())\
        return false;\
    Json::Value jsonRoot;\
    Json::Reader jsonReader;\
    if(!jsonReader.parse(strData,jsonRoot))\
    {\
        return false;\
    }

// 获取 JSON 值(单字节字符串)
#define GETSTRINGFROMJSON(key, root, des) \
    if(!root[key].isNull() && root[key].isString()) \
    { \
        des = root[key].asString();\
    }else{\
        des = "";\
    }


#define DATA_OFFSET 7

#define TWPD_STARTFLAG  0xDF    //帧开始取值
#define TWPD_ENDFLAG    0xFD    //帧结束取值
#define TWPD_STARTFLAG_ESCAPE_FIRST 0xDE
#define TWPD_STARTFLAG_ESCAPE_SECOND 0x01
#define TWPD_ENDFLAG_ESCAPE_FIRST 0xFC
#define TWPD_ENDFLAG_ESCAPE_SECOND 0x01
#define TWPD_COM1 0x01
#define TWPD_COM2 0x02

bool TW_Protocol::ConstructFrame(eSubCmdType_t subCmdType, int nReaderCom, const string & jsonData, std::vector<unsigned char>& escapePackage)
{
    std::vector<unsigned char> package;
	unsigned short len = static_cast<unsigned short>(jsonData.size());
    unsigned char chLen1 = 0xFF & (len >> 8);
    unsigned char chLen2 = 0xFF & len;
    package.push_back(chLen1);
    package.push_back(chLen2);

	if (1 == nReaderCom)
        package.push_back(TWPD_COM1);
	else if (2 == nReaderCom)
        package.push_back(TWPD_COM2);

    package.push_back(eProtocolConst::PROTOCOL_NUMBER);
    package.push_back(eTopCmdType::TOP_CMD_TYPE);
    package.push_back(subCmdType);

	std::vector<unsigned char> data;
	std::copy(jsonData.begin(), jsonData.end(), std::back_inserter(data));
    std::copy(data.begin(), data.end(), std::back_inserter(package));

    unsigned short crc_code = crc16(package.begin(), package.end());

    unsigned char crc_code1 = 0xFF & (crc_code >> 8);
    unsigned char crc_code2 = 0xFF & crc_code;
    package.push_back(crc_code1);
    package.push_back(crc_code2);

    //
    escapePackage.clear();
    escapePackage.push_back(eProtocolConst::PROTOCOL_HEADER);

    for (size_t i = 0; i < package.size(); ++i)
    {
        escapePackage.push_back(package[i]);
    }

    escapePackage.push_back(eProtocolConst::PROTOCOL_END);

	return true;
}

bool TW_Protocol::ParseProtocolData(int nReaderCom, const QByteArray &byteArray, eSubCmdType_t correctSubCmd, string &jsonData)
{
    if (byteArray.size() <= 10)
    {
        qDebug() << __FUNCTION__ << "byteArray.size() <= 10";
        return false;
    }

    std::vector<unsigned char> dst(byteArray.data(), byteArray.data() + byteArray.size());

    if (eProtocolConst::PROTOCOL_HEADER != dst.at(0) ||
        eProtocolConst::PROTOCOL_NUMBER != dst.at(4))
    {
        qDebug() << __FUNCTION__ << "header or number is wrong";
        return false;
    }

    // 开始解析data
    if (1 == nReaderCom)
    {
        if (TWPD_COM1 != dst.at(3))
        {
            qDebug() << __FUNCTION__ << "nReaderCom is wrong";
            return false;
        }
    }
    else if (2 == nReaderCom)
    {

        if (TWPD_COM2 != dst.at(3))
        {
            qDebug() << __FUNCTION__ << "nReaderCom is wrong";
            return false;
        }
    }
    else
    {
        if (eProtocolConst::PROTOCOL_NUMBER != dst.at(3))
        {
            qDebug() << __FUNCTION__ << "number is wrong";
            return false;
        }
    }

    if (correctSubCmd != dst.at(6))
    {
        qDebug() << __FUNCTION__ << "SubCmd is wrong";
        return false;
    }

    unsigned int nDstLen = (0xFF00 & (dst.at(1) << 8)) + (0xFF & dst.at(2));
    if(nDstLen < 1)
    {
        qDebug() << __FUNCTION__ << "nDstLen=0";
        return false;
    }

    if (eProtocolConst::PROTOCOL_END != dst.at(dst.size()-1))
    {
        qDebug() << __FUNCTION__ << "0xFD is not match";
        return false;
    }

    jsonData.clear();
    for (unsigned int index = 0; index < nDstLen; index++)
    {
        jsonData += dst.at(index + DATA_OFFSET);
    }

    qDebug() << __FUNCTION__ << "jsonData : " << QString::fromStdString(jsonData);

    return true;
}

// CRC校验
bool TW_Protocol::CRC16(const u8* byte, int len, u8* cHi, u8* cLo)
{
	u8	CRC16Hi = 0xFF;
	u8	CRC16Lo = 0xFF;
	u8	CH = 0xA0;
	u8	CL = 0x01;
	u8	SaveHi = 0;
	u8	SaveLo = 0;
	int     i;
	u8   Flag;

	for (i = 1; i < len; i++)
	{
		CRC16Lo = CRC16Lo ^ byte[i];
		for (Flag = 0; Flag < 8; Flag++)
		{
			SaveHi = CRC16Hi;
			SaveLo = CRC16Lo;
			CRC16Hi = CRC16Hi >> 1;
			CRC16Lo = CRC16Lo >> 1;
			if ((SaveHi & 1) == 1)
				CRC16Lo |= 0x80;
			if ((SaveLo & 1) == 1)
			{
				CRC16Hi = CRC16Hi ^ CH;
				CRC16Lo = CRC16Lo ^ CL;
			}
		}
	}
	*cHi = CRC16Hi;
	*cLo = CRC16Lo;

	return true;
}

unsigned short TW_Protocol::crc16(const std::vector<u8>::const_iterator & it_first, const std::vector<u8>::const_iterator & it_second)
{
	if (it_first == it_second)
	{
		return 0;
	}
	u8    CRC16Hi = 0xFF;
	u8    CRC16Lo = 0xFF;
	u8    CH = 0xA0;
	u8    CL = 0x01;
	u8    SaveHi = 0;
	u8    SaveLo = 0;

	for (std::vector<u8>::const_iterator it = it_first; it != it_second; ++it)
	{
		CRC16Lo = CRC16Lo ^ (*it);

		for (int Flag = 0; Flag < 8; Flag++)
		{
			SaveHi = CRC16Hi;
			SaveLo = CRC16Lo;
			CRC16Hi = CRC16Hi / 2; //高位右移一位
			CRC16Lo = CRC16Lo / 2; //低位右移一位
			if ((SaveHi & 1) == 1)
				CRC16Lo |= 0x80;
			if ((SaveLo & 1) == 1)
			{
				CRC16Hi = CRC16Hi ^ CH;
				CRC16Lo = CRC16Lo ^ CL;
			}
		}//for(int Flag = 0;Flag < 8;Flag++)
	}

	return (unsigned short)(CRC16Hi * 0x100 + CRC16Lo);
}

unsigned short TW_Protocol::crc16_Websocket(const u8* byte, int len)
{
	if (len < 1)
	{
		return 0;
	}

	BYTE    CRC16Hi = 0xFF;
	BYTE    CRC16Lo = 0xFF;
	BYTE    CH = 0xA0;
	BYTE    CL = 0x01;
	BYTE    SaveHi = 0;
	BYTE    SaveLo = 0;

	for (int i = 1; i < len; i++)
	{
		CRC16Lo = CRC16Lo ^ byte[i];

		for (int Flag = 0; Flag < 8; Flag++)
		{
			SaveHi = CRC16Hi;
			SaveLo = CRC16Lo;
			CRC16Hi = CRC16Hi / 2; //高位右移一位
			CRC16Lo = CRC16Lo / 2; //低位右移一位
			if ((SaveHi & 1) == 1)
				CRC16Lo |= 0x80;
			if ((SaveLo & 1) == 1)
			{
				CRC16Hi = CRC16Hi ^ CH;
				CRC16Lo = CRC16Lo ^ CL;
			}
		}
	}

	return (WORD)(CRC16Hi * 0x100 + CRC16Lo);
}

bool TW_Protocol::EscapeComProtocol(const std::vector<unsigned char>& src, std::vector<unsigned char>&dst)
{
	dst.clear();
	dst.push_back(eProtocolConst::PROTOCOL_HEADER);

	for (size_t i = 0; i < src.size(); ++i)
    {
        if (src[i] == TWPD_STARTFLAG)
        {
            dst.push_back(TWPD_STARTFLAG_ESCAPE_FIRST);
            dst.push_back(TWPD_STARTFLAG_ESCAPE_SECOND);
        }
        else if (src[i] == TWPD_STARTFLAG_ESCAPE_FIRST)
        {
            dst.push_back(TWPD_STARTFLAG_ESCAPE_FIRST);
            dst.push_back(0x00);
        }
        else if (src[i] == TWPD_ENDFLAG)
        {
            dst.push_back(TWPD_ENDFLAG_ESCAPE_FIRST);
            dst.push_back(TWPD_ENDFLAG_ESCAPE_SECOND);
        }
        else if (src[i] == TWPD_ENDFLAG_ESCAPE_FIRST)
        {

            dst.push_back(TWPD_ENDFLAG_ESCAPE_FIRST);
            dst.push_back(0x00);
        }
        else
        {
            dst.push_back(src[i]);
        }
    }

	dst.push_back(eProtocolConst::PROTOCOL_END);

	return true;
}

bool TW_Protocol::UnEscapeComProtocol(const std::vector<unsigned char>& src, std::vector<unsigned char>&dst)
{
	dst.clear();

	if (src.empty())
	{
		return true;
	}
	size_t i = 0;
	while (i < (src.size() - 1)) /**/
	{
		if (src[i] == TWPD_STARTFLAG_ESCAPE_FIRST)
		{
			if (src[i + 1] == TWPD_STARTFLAG_ESCAPE_SECOND)
			{
				dst.push_back(TWPD_STARTFLAG);
			}
			else if (src[i + 1] == 0x00)
			{
				dst.push_back(TWPD_STARTFLAG_ESCAPE_FIRST);
			}
			else
			{
				return false;
			}
			i += 2;
		}
		else if (src[i] == TWPD_ENDFLAG_ESCAPE_FIRST)
		{
			if (src[i + 1] == TWPD_ENDFLAG_ESCAPE_SECOND)
			{
				dst.push_back(TWPD_ENDFLAG);
			}
			else if (src[i + 1] == 0x00)
			{
				dst.push_back(TWPD_ENDFLAG_ESCAPE_FIRST);
			}
			else
			{
				return false;
			}
			i += 2;
		}
		else
		{
			dst.push_back(src[i]);
			i++;
		}
	}
	/*last char unescape*/
	if (src.back() == TWPD_STARTFLAG_ESCAPE_FIRST
		|| src.back() == TWPD_ENDFLAG_ESCAPE_FIRST)
	{
		return false;
	}
	else
	{
		dst.push_back(src.back());
	}


	return true;
}

bool TW_Protocol::generateGeneralJson(string & strJson, const int nHandle)
{
	Json::Value jsonRoot;
	Json::FastWriter jsonWriter;

	jsonRoot["HandleCode"] = std::to_string(nHandle);

	strJson = jsonWriter.write(jsonRoot);

	return true;
}

bool TW_Protocol::parseGeneralRespJsonData(const string & strJson, string & strReturnCode)
{
	CHECKJSON(strJson);
	GETSTRINGFROMJSON("ReturnCode", jsonRoot, strReturnCode);
	return true;
}

//1、打开 IC 卡读写器
bool TW_Protocol::generateOpenReaderJson(string & strJson, const int nMode, const char * sParas)
{
	Json::FastWriter jsonWriter;
	Json::Value Root;
	Json::Value OpenReader;

	OpenReader["nMode"] = std::to_string(nMode);
	OpenReader["sParas"] = sParas;
	Root["OpenReader"] = OpenReader;

	strJson = jsonWriter.write(Root);

	return true;
}
bool TW_Protocol::parseOpenReaderRespJson(const string& strJson, string& nHandleCode)
{
	if (strJson.empty())
		return false;

	Json::Reader jsonReader;
	Json::Value Root;
	Json::Value OpenReaderResp;

	if (!jsonReader.parse(strJson, Root))
	{
		return false;
	}
	if (Root["OpenReaderResp"].isNull() || Root["OpenReaderResp"].type() != Json::objectValue)
	{
		return false;
	}

	OpenReaderResp = Root["OpenReaderResp"];
	if (OpenReaderResp["HandleCode"].isNull() || OpenReaderResp["HandleCode"].type() != Json::stringValue)
	{
		return false;
	}

	nHandleCode = OpenReaderResp["HandleCode"].asString();

	return true;
}

// 2、关闭 IC 卡读写器
bool TW_Protocol::generateCloseReaderJson(string& strJson, const int nHandle)
{
	Json::FastWriter jsonWriter;
	Json::Value Root;
	Json::Value CloseReader;

	CloseReader["HandleCode"] = std::to_string(nHandle);
	Root["CloseReader"] = CloseReader;

	strJson = jsonWriter.write(Root);

	return true;
}
bool TW_Protocol::parseCloseReaderRespJson(const string& strJson, string& ReturnCode)
{
	if (strJson.empty())
		return false;

	Json::Reader jsonReader;
	Json::Value Root;
	Json::Value CloseReaderResp;

	if (!jsonReader.parse(strJson, Root))
	{
		return false;
	}
	if (Root["CloseReaderResp"].isNull() || Root["CloseReaderResp"].type() != Json::objectValue)
	{
		return false;
	}

	CloseReaderResp = Root["CloseReaderResp"];
	if (CloseReaderResp["ReturnCode"].isNull() || CloseReaderResp["ReturnCode"].type() != Json::stringValue)
	{
		return false;
	}

	ReturnCode = CloseReaderResp["ReturnCode"].asString();

	return true;
}

// 3、设置 IC 卡读写器通讯速率（可选）
bool TW_Protocol::generateSetInitPPSJson(string & strJson, const int nHandle, const char * sPPS)
{
	Json::FastWriter jsonWriter;
	Json::Value Root;
	Json::Value SetInitPPS;

	SetInitPPS["HandleCode"] = std::to_string(nHandle);
	SetInitPPS["sPPS"] = sPPS;
	Root["SetInitPPS"] = SetInitPPS;

	strJson = jsonWriter.write(Root);

	return true;
}
bool TW_Protocol::parseSetInitPPSRespJson(const string& strJson, string& ReturnCode)
{
	if (strJson.empty())
		return false;

	Json::Reader jsonReader;
	Json::Value Root;
	Json::Value SetInitPPSResp;

	if (!jsonReader.parse(strJson, Root))
	{
		return false;
	}
	if (Root["SetInitPPSResp"].isNull() || Root["SetInitPPSResp"].type() != Json::objectValue)
	{
		return false;
	}

	SetInitPPSResp = Root["SetInitPPSResp"];
	if (SetInitPPSResp["ReturnCode"].isNull() || SetInitPPSResp["ReturnCode"].type() != Json::stringValue)
	{
		return false;
	}

	ReturnCode = SetInitPPSResp["ReturnCode"].asString();

	return true;
}

// 4、设置 IC 卡读写器超时时间（可选）
bool TW_Protocol::generateSetInitTimeOutJson(string & strJson, const int nHandle, const char * sTimeOut)
{
	Json::FastWriter jsonWriter;
	Json::Value Root;
	Json::Value SetInitTimeOut;

	SetInitTimeOut["HandleCode"] = std::to_string(nHandle);
	SetInitTimeOut["sTimeOut"] = sTimeOut;
	Root["SetInitTimeOut"] = SetInitTimeOut;

	strJson = jsonWriter.write(Root);

	return true;
}
bool TW_Protocol::parseSetInitTimeOutRespJson(const string& strJson, string& ReturnCode)
{
	if (strJson.empty())
		return false;

	Json::Reader jsonReader;
	Json::Value Root;
	Json::Value SetInitTimeOutResp;

	if (!jsonReader.parse(strJson, Root))
	{
		return false;
	}
	if (Root["SetInitTimeOutResp"].isNull() || Root["SetInitTimeOutResp"].type() != Json::objectValue)
	{
		return false;
	}

	SetInitTimeOutResp = Root["SetInitTimeOutResp"];
	if (SetInitTimeOutResp["ReturnCode"].isNull() || SetInitTimeOutResp["ReturnCode"].type() != Json::stringValue)
	{
		return false;
	}

	ReturnCode = SetInitTimeOutResp["ReturnCode"].asString();

	return true;
}

// 5、设置读写器发光二极管（可选）
bool TW_Protocol::generateLEDDisplayJson(string & strJson, const int nHandle, const int nRed, const int nGreen, const int nBlue)
{
	Json::FastWriter jsonWriter;
	Json::Value Root;
	Json::Value LEDDisplay;

	LEDDisplay["HandleCode"] = std::to_string(nHandle);
	LEDDisplay["nRed"] = std::to_string(nRed);
	LEDDisplay["nGreen"] = std::to_string(nGreen);
	LEDDisplay["nBlue"] = std::to_string(nBlue);
	Root["LEDDisplay"] = LEDDisplay;

	strJson = jsonWriter.write(Root);

	return true;
}
bool TW_Protocol::parseLEDDisplayRespJson(const string& strJson, string& ReturnCode)
{
	if (strJson.empty())
		return false;

	Json::Reader jsonReader;
	Json::Value Root;
	Json::Value LEDDisplayResp;

	if (!jsonReader.parse(strJson, Root))
	{
		return false;
	}
	if (Root["LEDDisplayResp"].isNull() || Root["LEDDisplayResp"].type() != Json::objectValue)
	{
		return false;
	}

	LEDDisplayResp = Root["LEDDisplayResp"];
	if (LEDDisplayResp["ReturnCode"].isNull() || LEDDisplayResp["ReturnCode"].type() != Json::stringValue)
	{
		return false;
	}

	ReturnCode = LEDDisplayResp["ReturnCode"].asString();

	return true;
}

// 6、设置读写器蜂鸣器
bool TW_Protocol::generateAudioControlJson(string & strJson, const int nHandle, const int nTimes, const int nVoice)
{
	Json::FastWriter jsonWriter;
	Json::Value Root;
	Json::Value AudioControl;

	AudioControl["HandleCode"] = std::to_string(nHandle);
	AudioControl["nTimes"] = std::to_string(nTimes);
	AudioControl["nVoice"] = std::to_string(nVoice);
	Root["AudioControl"] = AudioControl;

	strJson = jsonWriter.write(Root);

	return true;
}
bool TW_Protocol::parseAudioControlRespJson(const string& strJson, string& ReturnCode)
{
	if (strJson.empty())
		return false;

	Json::Reader jsonReader;
	Json::Value Root;
	Json::Value AudioControlResp;

	if (!jsonReader.parse(strJson, Root))
	{
		return false;
	}
	if (Root["AudioControlResp"].isNull() || Root["AudioControlResp"].type() != Json::objectValue)
	{
		return false;
	}

	AudioControlResp = Root["AudioControlResp"];
	if (AudioControlResp["ReturnCode"].isNull() || AudioControlResp["ReturnCode"].type() != Json::stringValue)
	{
		return false;
	}

	ReturnCode = AudioControlResp["ReturnCode"].asString();

	return true;
}

// 7、获取设备状态
bool TW_Protocol::generateGetStatusJson(string& strJson, const int nHandle)
{
	Json::FastWriter jsonWriter;
	Json::Value Root;
	Json::Value GetStatus;

	GetStatus["HandleCode"] = std::to_string(nHandle);
	Root["GetStatus"] = GetStatus;

	strJson = jsonWriter.write(Root);

	return true;
}
bool TW_Protocol::parseGetStatusRespJson(const string& strJson, string& StatusCode)
{
	if (strJson.empty())
		return false;

	Json::Reader jsonReader;
	Json::Value Root;
	Json::Value GetStatusResp;

	if (!jsonReader.parse(strJson, Root))
	{
		return false;
	}
	if (Root["GetStatusResp"].isNull() || Root["GetStatusResp"].type() != Json::objectValue)
	{
		return false;
	}

	GetStatusResp = Root["GetStatusResp"];
	if (GetStatusResp["ReturnCode"].isNull() || GetStatusResp["ReturnCode"].type() != Json::stringValue)
	{
		return false;
	}

	StatusCode = GetStatusResp["ReturnCode"].asString();

	return true;
}

//8、 获取设备版本信息
bool TW_Protocol::generateReaderVersionJson(string& strJson, const int nHandle)
{
	Json::FastWriter jsonWriter;
	Json::Value Root;
	Json::Value ReaderVersion;

	ReaderVersion["HandleCode"] = std::to_string(nHandle);
	Root["ReaderVersion"] = ReaderVersion;

	strJson = jsonWriter.write(Root);

	return true;
}
bool TW_Protocol::parseReaderVersionRespJson(const string& strJson, string& sRDVersion, string& sAPIVersion)
{
	if (strJson.empty())
		return false;

	Json::Reader jsonReader;
	Json::Value Root;
	Json::Value ReaderVersionResp;

	if (!jsonReader.parse(strJson, Root))
	{
		return false;
	}
	if (Root["ReaderVersionResp"].isNull() || Root["ReaderVersionResp"].type() != Json::objectValue)
	{
		return false;
	}

	ReaderVersionResp = Root["ReaderVersionResp"];
	if (ReaderVersionResp["sRDVersion"].isNull() || ReaderVersionResp["sRDVersion"].type() != Json::stringValue)
	{
		return false;
	}
	sRDVersion = ReaderVersionResp["sRDVersion"].asString();
	if (ReaderVersionResp["sAPIVersion"].isNull() || ReaderVersionResp["sAPIVersion"].type() != Json::stringValue)
	{
		return false;
	}
	sAPIVersion = ReaderVersionResp["sAPIVersion"].asString();

	return true;
}

//9、 获取设备动态库版本
bool TW_Protocol::generateGetVersionJson(string& strJson, const int nHandle)
{
	Json::FastWriter jsonWriter;
	Json::Value Root;
	Json::Value GetVersion;

	GetVersion["HandleCode"] = std::to_string(nHandle);
	Root["GetVersion"] = GetVersion;

	strJson = jsonWriter.write(Root);

	return true;
}
bool TW_Protocol::parseGetVersionRespJson(const string& strJson, string& Version)
{
	if (strJson.empty())
		return false;

	Json::Reader jsonReader;
	Json::Value Root;
	Json::Value GetVersionResp;

	if (!jsonReader.parse(strJson, Root))
	{
		return false;
	}
	if (Root["GetVersionResp"].isNull() || Root["GetVersionResp"].type() != Json::objectValue)
	{
		return false;
	}

	GetVersionResp = Root["GetVersionResp"];
	if (GetVersionResp["Version"].isNull() || GetVersionResp["Version"].type() != Json::stringValue)
	{
		return false;
	}
	Version = GetVersionResp["Version"].asString();

	return true;
}

//10、获取错误码详细描述
bool TW_Protocol::generateGetStatusMsgJson(string& strJson, const int StatusCode)
{
	Json::FastWriter jsonWriter;
	Json::Value Root;
	Json::Value GetStatusMsg;

	GetStatusMsg["StatusCode"] = std::to_string(StatusCode);
	Root["GetStatusMsg"] = GetStatusMsg;

	strJson = jsonWriter.write(Root);

	return true;
}
bool TW_Protocol::parseGetStatusMsgRespJson(const string& strJson, string& StatusMsg)
{
	if (strJson.empty())
		return false;

	Json::Reader jsonReader;
	Json::Value Root;
	Json::Value GetStatusMsgResp;

	if (!jsonReader.parse(strJson, Root))
	{
		return false;
	}
	if (Root["GetStatusMsgResp"].isNull() || Root["GetStatusMsgResp"].type() != Json::objectValue)
	{
		return false;
	}

	GetStatusMsgResp = Root["GetStatusMsgResp"];
	if (GetStatusMsgResp["StatusMsg"].isNull() || GetStatusMsgResp["StatusMsg"].type() != Json::stringValue)
	{
		return false;
	}
	StatusMsg = GetStatusMsgResp["StatusMsg"].asString();

	return true;
}

//11、打开卡片
bool TW_Protocol::generateOpenCardJson(string& strJson, const int nHandle)
{
	Json::FastWriter jsonWriter;
	Json::Value Root;
	Json::Value OpenCard;

	OpenCard["HandleCode"] = std::to_string(nHandle);
	Root["OpenCard"] = OpenCard;

	strJson = jsonWriter.write(Root);

	return true;
}
bool TW_Protocol::parseOpenCardRespJson(const string& strJson, string& ReturnCode, string& pCardPlace, string& sPhysicsCardno)
{
	if (strJson.empty())
		return false;

	Json::Reader jsonReader;
	Json::Value Root;
	Json::Value OpenCardResp;

	if (!jsonReader.parse(strJson, Root))
	{
		return false;
	}
	if (Root["OpenCardResp"].isNull() || Root["OpenCardResp"].type() != Json::objectValue)
	{
		return false;
	}

	OpenCardResp = Root["OpenCardResp"];
	if (OpenCardResp["ReturnCode"].isNull() || OpenCardResp["ReturnCode"].type() != Json::stringValue)
	{
		return false;
	}
	ReturnCode = OpenCardResp["ReturnCode"].asString();
	if (OpenCardResp["pCardPlace"].isNull() || OpenCardResp["pCardPlace"].type() != Json::stringValue)
	{
		return false;
	}
	pCardPlace = OpenCardResp["pCardPlace"].asString();
	if (OpenCardResp["sPhysicsCardno"].isNull() || OpenCardResp["sPhysicsCardno"].type() != Json::stringValue)
	{
		return false;
	}
	sPhysicsCardno = OpenCardResp["sPhysicsCardno"].asString();

	return true;
}

//12、关闭卡片
bool TW_Protocol::generateCloseCardJson(string& strJson, const int nHandle)
{
	Json::FastWriter jsonWriter;
	Json::Value Root;
	Json::Value CloseCard;

	CloseCard["HandleCode"] = std::to_string(nHandle);
	Root["CloseCard"] = CloseCard;

	strJson = jsonWriter.write(Root);

	return true;
}
bool TW_Protocol::parseCloseCardRespJson(const string& strJson, string& ReturnCode)
{
	if (strJson.empty())
		return false;

	Json::Reader jsonReader;
	Json::Value Root;
	Json::Value CloseCardResp;

	if (!jsonReader.parse(strJson, Root))
	{
		return false;
	}
	if (Root["CloseCardResp"].isNull() || Root["CloseCardResp"].type() != Json::objectValue)
	{
		return false;
	}

	CloseCardResp = Root["CloseCardResp"];
	if (CloseCardResp["ReturnCode"].isNull() || CloseCardResp["ReturnCode"].type() != Json::stringValue)
	{
		return false;
	}
	ReturnCode = CloseCardResp["ReturnCode"].asString();

	return true;
}

//13、CPU 卡通用指令函数
bool TW_Protocol::generateCPUCommandJson(string& strJson, const int nHandle, const char* sCommand)
{
	Json::FastWriter jsonWriter;
	Json::Value Root;
	Json::Value CPUCommand;

	CPUCommand["HandleCode"] = std::to_string(nHandle);
	CPUCommand["sCommand"] = sCommand;
	Root["CPUCommand"] = CPUCommand;

	strJson = jsonWriter.write(Root);

	return true;
}
bool TW_Protocol::parseCPUCommandRespJson(const string& strJson, string& ReturnCode, string& sReply)
{
	if (strJson.empty())
		return false;

	Json::Reader jsonReader;
	Json::Value Root;
	Json::Value CPUCommandResp;

	if (!jsonReader.parse(strJson, Root))
	{
		return false;
	}
	if (Root["CPUCommandResp"].isNull() || Root["CPUCommandResp"].type() != Json::objectValue)
	{
		return false;
	}

	CPUCommandResp = Root["CPUCommandResp"];
	if (CPUCommandResp["ReturnCode"].isNull() || CPUCommandResp["ReturnCode"].type() != Json::stringValue)
	{
		return false;
	}
	ReturnCode = CPUCommandResp["ReturnCode"].asString();

	if (CPUCommandResp["sReply"].isNull() || CPUCommandResp["sReply"].type() != Json::stringValue)
	{
		return false;
	}
	sReply = CPUCommandResp["sReply"].asString();

	return true;
}

//14、CPU 卡通用指令函数
bool TW_Protocol::generateSamResetJson(string& strJson, const int nHandle, const int nSockID, const int nProtocolType)
{
	Json::FastWriter jsonWriter;
	Json::Value Root;
	Json::Value SamReset;

	SamReset["HandleCode"] = std::to_string(nHandle);
	SamReset["nSockID"] = std::to_string(nSockID);
	SamReset["nProtocolType"] = std::to_string(nProtocolType);
	Root["SamReset"] = SamReset;

	strJson = jsonWriter.write(Root);

	return true;
}
bool TW_Protocol::parseSamResetRespJson(const string& strJson, string& ReturnCode)
{
	if (strJson.empty())
		return false;

	Json::Reader jsonReader;
	Json::Value Root;
	Json::Value SamResetResp;

	if (!jsonReader.parse(strJson, Root))
	{
		return false;
	}
	if (Root["SamResetResp"].isNull() || Root["SamResetResp"].type() != Json::objectValue)
	{
		return false;
	}

	SamResetResp = Root["SamResetResp"];
	if (SamResetResp["ReturnCode"].isNull() || SamResetResp["ReturnCode"].type() != Json::stringValue)
	{
		return false;
	}

	ReturnCode = SamResetResp["ReturnCode"].asString();

	return true;
}

bool TW_Protocol::generateSamCommandJson(string& strJson, const int nHandle, const int nSockID, const char* sCommand)
{
	Json::FastWriter jsonWriter;
	Json::Value Root;
	Json::Value SamCommand;

	SamCommand["HandleCode"] = std::to_string(nHandle);
	SamCommand["nSockID"] = std::to_string(nSockID);
	SamCommand["sCommand"] = sCommand;
	Root["SamCommand"] = SamCommand;

	strJson = jsonWriter.write(Root);

	return true;
}
bool TW_Protocol::parseSamCommandRespJson(const string& strJson, string& ReturnCode, string& sReply)
{
	if (strJson.empty())
		return false;

	Json::Reader jsonReader;
	Json::Value Root;
	Json::Value SamCommandResp;

	if (!jsonReader.parse(strJson, Root))
	{
		return false;
	}
	if (Root["SamCommandResp"].isNull() || Root["SamCommandResp"].type() != Json::objectValue)
	{
		return false;
    }

	SamCommandResp = Root["SamCommandResp"];
	if (SamCommandResp["ReturnCode"].isNull() || SamCommandResp["ReturnCode"].type() != Json::stringValue)
	{
		return false;
	}
	ReturnCode = SamCommandResp["ReturnCode"].asString();

	if (SamCommandResp["sReply"].isNull() || SamCommandResp["sReply"].type() != Json::stringValue)
	{
		return false;
	}
	sReply = SamCommandResp["sReply"].asString();

	return true;
}

//16、M1卡载入密钥
bool TW_Protocol::generateLoadKeyJson(string& strJson, const int nHandle, const int nKeyType, const int nSec, const char* sKeyValue)
{
	Json::FastWriter jsonWriter;
	Json::Value Root;
	Json::Value LoadKey;

	LoadKey["HandleCode"] = std::to_string(nHandle);
	LoadKey["nKeyType"] = std::to_string(nKeyType);
	LoadKey["nSec"] = std::to_string(nSec);
	LoadKey["sKeyValue"] = sKeyValue;
	Root["LoadKey"] = LoadKey;

	strJson = jsonWriter.write(Root);

	return true;
}
bool TW_Protocol::parseLoadKeyRespJson(const string& strJson, string& ReturnCode)
{
	if (strJson.empty())
		return false;

	Json::Reader jsonReader;
	Json::Value  Root;
	Json::Value  LoadKeyResp;

	if (!jsonReader.parse(strJson, Root))
	{
		return false;
	}
	if (Root["LoadKeyResp"].isNull() || Root["LoadKeyResp"].type() != Json::objectValue)
	{
		return false;
	}

	LoadKeyResp = Root["LoadKeyResp"];
	if (LoadKeyResp["ReturnCode"].isNull() || LoadKeyResp["ReturnCode"].type() != Json::stringValue)
	{
		return false;
	}

	ReturnCode = LoadKeyResp["ReturnCode"].asString();

	return true;
}

//17、M1卡读块函数
bool TW_Protocol::generateReadBlockJson(string& strJson, const int nHandle, const int nKeyType, const int nBlock)
{
	Json::FastWriter jsonWriter;
	Json::Value Root;
	Json::Value ReadBlock;

	ReadBlock["HandleCode"] = std::to_string(nHandle);
	ReadBlock["nKeyType"] = std::to_string(nKeyType);
	ReadBlock["nBlock"] = std::to_string(nBlock);
	Root["ReadBlock"] = ReadBlock;

	strJson = jsonWriter.write(Root);

	return true;
}
bool TW_Protocol::parseReadBlockRespJson(const string& strJson, string& ReturnCode, string& sReply)
{
	if (strJson.empty())
		return false;

	Json::Reader jsonReader;
	Json::Value  Root;
	Json::Value  ReadBlockResp;

	if (!jsonReader.parse(strJson, Root))
	{
		return false;
	}
	if (Root["ReadBlockResp"].isNull() || Root["ReadBlockResp"].type() != Json::objectValue)
	{
		return false;
	}

	ReadBlockResp = Root["ReadBlockResp"];
	if (ReadBlockResp["ReturnCode"].isNull() || ReadBlockResp["ReturnCode"].type() != Json::stringValue)
	{
		return false;
	}
	ReturnCode = ReadBlockResp["ReturnCode"].asString();

	if (ReadBlockResp["sReply"].isNull() || ReadBlockResp["sReply"].type() != Json::stringValue)
	{
		return false;
	}
	sReply = ReadBlockResp["sReply"].asString();

	return true;
}

//18、M1卡写块函数
bool TW_Protocol::generateWriteBlockJson(string& strJson, const int nHandle, const int nKeyType, const int nBlock, const char* sData)
{
	Json::FastWriter jsonWriter;
	Json::Value Root;
	Json::Value WriteBlock;

	WriteBlock["HandleCode"] = std::to_string(nHandle);
	WriteBlock["nKeyType"] = std::to_string(nKeyType);
	WriteBlock["nBlock"] = std::to_string(nBlock);
	WriteBlock["sData"] = sData;
	Root["WriteBlock"] = WriteBlock;

	strJson = jsonWriter.write(Root);

	return true;
}

bool TW_Protocol::parseWriteBlockRespJson(const string& strJson, string& ReturnCode)
{
	if (strJson.empty())
		return false;

	Json::Reader jsonReader;
	Json::Value  Root;
	Json::Value  WriteBlockResp;

	if (!jsonReader.parse(strJson, Root))
	{
		return false;
	}
	if (Root["WriteBlockResp"].isNull() || Root["WriteBlockResp"].type() != Json::objectValue)
	{
		return false;
	}

	WriteBlockResp = Root["WriteBlockResp"];
	if (WriteBlockResp["ReturnCode"].isNull() || WriteBlockResp["ReturnCode"].type() != Json::stringValue)
	{
		return false;
	}

	ReturnCode = WriteBlockResp["ReturnCode"].asString();

	return true;
}
