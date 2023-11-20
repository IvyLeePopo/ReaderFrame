//---------------------------------------------------------------------------

#ifndef KeyAlgorithmH
#define KeyAlgorithmH
//---------------------------------------------------------------------------
#include <string>
#include <QByteArray>
using namespace std;

#include "d3des.h"       //DES�㷨ʵ��
#include "sm4.h"         //SM4�㷨ʵ��

//#define USER_SM2

#ifdef USER_SM2
#include "mysm2.h"       //SM2�㷨ʵ��
#endif

template<typename T1,typename T2>
bool HexToBin(const T1* sStr,T2* sDist,int nLen)
{
	QByteArray baStr = QByteArray((const char*)sStr,nLen*2);
	QByteArray baDist = QByteArray::fromHex(baStr);
	memcpy((char*)sDist,baDist.data(),nLen);
	return true;
}

template<typename T1,typename T2>
static bool BinToHex(const T1* sStr,T2* sDist,int nLen)
{
	QByteArray baStr = QByteArray((const char*)sStr,nLen);
	QByteArray baDist = baStr.toHex();
	memcpy((char*)sDist,baDist.data(),nLen*2);
	return true;
}

//�㷨��ʵ�֣�DES��3DES��SM4��SM2��SM3

class TKeyAlgorithm
{
public:
    struct _arr_man
    {
        unsigned char* _p;
        _arr_man(unsigned char* p):_p(p){}
        ~_arr_man(){if(_p)delete[]_p;}
    };
//===================================================================================================================================
//===================================================================================================================================
//===================================================================================================================================
//  ����ΪDES/3DES�㷨

/*
  ���ܣ�ʹ��DES��3DES�������ݼ��� �� 64λ���飬��Կ����ɢ��

  ������data     ���� ��in����������; (Hex��ʽ) (ע�⣺�ڲ��Ჹ������,�����ʽ�����ĳ���+����+������ݣ�
        key      ���� ��in����Կ��    (Hex��ʽ)
        IsTriple ���� ��in��true:3DES��false:DES ��Ĭ��3DES

  ����ֵ�������ĳ���+��������+������ݣ�������  (Hex��ʽ)

  ��ע��������ݵĲ������ο���һ������bin�汾�ġ����������Զ�����ԭ��
*/
static QByteArray  EncryptData(const QByteArray plaintext, const QByteArray key, bool IsTriple=true);

//-----------------------------------------------------------------------------
/*
      ������rvalue   ���� ��out�� �����ĳ���+����+������ݣ������ģ�Bin��ʽ����
            rlen     ���� ��out�� ���ĳ��ȣ��ֽڣ�
            bin      ���� ��in��  ���������� ��Bin��ʽ�� ��ע�⣺�ڲ����Զ�����0x80 00 ...)
            len      ���� ��in��  ���ĳ��ȣ��ֽڣ�
            key      ���� ��in��  ��Կ��     ��Bin��ʽ��
            IsTriple ���� ��in��  true:3DES��false:DES ��Ĭ��3DES

      ����ֵ����

      �����Զ�����ԭ��
           1.data���ݷֽ��8�ֽ����ݿ飬���ΪD1��D2��D3��D4�ȵȡ����һ�����ݿ�ĳ��ȿ��ܲ���8�ֽڡ�
           2.����������ݿ鳤�Ȳ���8�ֽڣ�����������16��������'80'������ﵽ8�ֽڳ��ȣ���ת����һ��������������
             ��16��������'00 'ֱ�����ȴﵽ8�ֽڡ�

*/
static void  EncryptData(unsigned char* rvalue, int& rlen, const unsigned char* bin, int len, const unsigned char key[16], bool IsTriple=true);

//===================================================================================================================================
//-----------------------------------------------------------------------------
/*
    �������ܣ�ʹ��DES��3DES�������ݼ���
    ������  data    ������in���������ģ�Hex��ʽ�� �����ȱ���Ϊ8�ֽڱ���
            key     ������in����Կ    ��Hex��ʽ��
            IsTriple ��-��in��true:3DES��false:DES ��Ĭ��3DES

    ����ֵ��data���ݼ��ܺ������  (Hex��ʽ)
*/
static QByteArray  EncryptWithDes(QByteArray data, QByteArray key, bool IsTriple=true);
//-----------------------------------------------------------------------------
/*
    �������ܣ�ʹ��DES��3DES����8�ֽ����ݿ�
    ������  rvalue   ���� ��out��8�ֽ���������    ��Bin��ʽ��
            data     ���� ��int��8�ֽ���������    ��Bin��ʽ��
            key      ���� ��in����Կ��            ��Bin��ʽ��
            IsTriple ���� ��in�� true:3DES��false:DES ��Ĭ��3DES

    ����ֵ����
*/
static void  EncryptWithDes(unsigned char rvalue[8],const unsigned char data[8],const unsigned char key[16], bool IsTriple=true);

//-----------------------------------------------------------------------------
/*
    �������ܣ�ʹ��DES��3DES�������ݣ�ECBģʽ��
    ������  rvalue   ���� ��out���������ݣ�Bin��ʽ��
            rlen     ���� ��out�����ĳ��ȣ��ֽڣ�
            data     ���� ��in�� �������ݣ�Bin��ʽ�� ,���ȱ���Ϊ8�ֽڱ���
            dlen     ���� ��in�� ���ĳ��ȣ��ֽڣ�
            key      ���� ��in�� ��Կ��  ��Bin��ʽ��
            IsTriple ���� ��in��true:3DES��false:DES ��Ĭ��3DES

    ����ֵ����

    ECBģʽ��
         1.��һ������D1����DES����,�õ���һ������C1
         2.�ڶ�������D2����DES����,�õ��ڶ�������C2��֮��������Դ����ƣ��õ�Cn
         3.��˳����ΪC1C2C3......Cn��Ϊ���ܽ����
*/
static void  EncryptWithECB(unsigned char* rvalue,unsigned& rlen,const unsigned char* data,const unsigned dlen,const unsigned char key[16], bool IsTriple=true);

//-----------------------------------------------------------------------------
/*
    �������ܣ�ʹ��DES��3DES�������ݣ�CBCģʽ��
    ������  rvalue   ���� ��out���������ݣ�Bin��ʽ��
            rlen     ���� ��out�����ĳ��ȣ��ֽڣ�
            data     ���� ��in�� �������ݣ�Bin��ʽ�� ,���ȱ���Ϊ8�ֽڱ���
            dlen     ���� ��in�� ���ĳ��ȣ��ֽڣ�
            atc      ���� ��in�� 8�ֽڵĳ�ʼֵ ��Bin��ʽ��
            key      ���� ��in�� ��Կ��  ��Bin��ʽ��
            IsTriple ���� ��in�� true:3DES��false:DES ��Ĭ��3DES

    ����ֵ����

    CBCģʽ��
         1.��һ������D1���ʼ������I����Ľ������DES���ܵõ���һ������C1����ʼ������IΪȫ�㣩
         2.�ڶ�������D2���һ��ļ��ܽ��C1����Ժ�Ľ������DES���ܣ��õ��ڶ�������C2��֮��������Դ����ƣ��õ�Cn
         3.��˳����ΪC1C2C3......Cn��Ϊ���ܽ����
*/
static void  EncryptWithCBC(unsigned char* rvalue,unsigned& rlen,const unsigned char* data,const unsigned dlen, const unsigned char atc[8],const unsigned char key[16], bool IsTriple=true);

//===================================================================================================================================
/*
    �������ܣ�ʹ��DES��3DES�������ݽ���
    ������  data    ���� ��in���������ݣ�Hex��ʽ����ע�⣬�ڲ����Զ����0x00 .. ��
            key     ���� ��in����Կ    ��Hex��ʽ��
            IsTriple ������in��true:3DES��false:DES ��Ĭ��3DES

    ����ֵ���������� (Hex��ʽ)  ��ע�⣬�����������ݰ����˲������ݣ�

*/
static QByteArray DecryptData(const QByteArray data, const QByteArray key, bool IsTriple=true);

//-----------------------------------------------------------------------------
/*
      ������rvalue   ���� ��out���������ݣ���������ݣ� ��Bin��ʽ����
            rlen     ���� ��out�����ĳ��ȣ��ֽڣ�
            bin      ���� ��in�� �������ݣ�Bin��ʽ��  ��ע�⣬�ڲ����Զ����0x00 .. ��
            len      ���� ��in�� ���ĳ��ȣ��ֽڣ�
            key      ���� ��in�� ��Կ��  ��Bin��ʽ��
            IsTriple ���� ��in�� true:3DES��false:DES ��Ĭ��3DES

      ����ֵ����
*/
static void DecryptData(unsigned char* rvalue, int& rlen, const unsigned char* bin, int len,const unsigned char key[16], bool IsTriple=true);

//===================================================================================================================================
/*
   �������ܣ�ʹ��DES��3DES�������ݽ��� �� 64λ��8�ֽڣ����飬��Կ����ɢ��


    �������ܣ�ʹ��DES��3DES�������ݼ���
    ������  data     ������in�����ģ�Hex��ʽ��,���ȱ���Ϊ8�ֽڱ���
            key      ������in����Կ ��Hex��ʽ��
            IsTriple ������in��true:3DES��false:DES ��Ĭ��3DES

    ����ֵ��data���ݼ��ܺ������  (Hex��ʽ)
*/
static QByteArray  DecryptWithDes(QByteArray data, QByteArray key, bool IsTriple=true);

//-----------------------------------------------------------------------------
/*
    �������ܣ�ʹ��DES��3DES����8�ֽ����ݿ�
    ������  rvalue   ���� ��out��8�ֽ���������  ��Bin��ʽ��
            data     ���� ��in��8�ֽ���������   ��Bin��ʽ��
            key      ���� ��in����Կ��          ��Bin��ʽ��
            IsTriple ���� ��in��true:3DES��false:DES ��Ĭ��3DES

    ����ֵ����
*/
static void  DecryptWithDes(unsigned char rvalue[8],const unsigned char data[8],const unsigned char key[16], bool IsTriple=true);

//-----------------------------------------------------------------------------
/*
    �������ܣ�ʹ��DES��3DES�������ݣ�ECBģʽ��
    ������  rvalue   ���� ��out���������ݣ�Bin��ʽ��
            rlen     ���� ��out�����ĳ��ȣ��ֽڣ�
            data     ���� ��in�� �������ݣ�Bin��ʽ�� ,����Ϊ8�ֽڱ���
            dlen     ���� ��in�� ���ĳ��ȣ��ֽڣ�
            key      ���� ��in�� ��Կ��  ��Bin��ʽ��
            IsTriple ���� ��in�� true:3DES��false:DES ��Ĭ��3DES

    ����ֵ����

    ECBģʽ��
         1.���Ƚ����ݰ���8���ֽ�һ����з���õ�C1C2C3......Cn��
         1.��һ������C1����DES����,�õ���һ������D1
         2.�ڶ�������C2����DES����,�õ��ڶ�������D2��֮��������Դ����ƣ��õ�Dn
         3.��˳����ΪD1D2D3......Dn��Ϊ���ܽ����
*/
static void  DecryptWithECB(unsigned char* rvalue,unsigned& rlen,const unsigned char* data,const unsigned dlen,const unsigned char key[16], bool IsTriple=true);

//-----------------------------------------------------------------------------
/*
    �������ܣ�ʹ��DES��3DES�������ݣ�CBCģʽ��
    ������  rvalue   ���� ��out����������      ��Bin��ʽ��
            rlen     ���� ��out�����ĳ��ȣ��ֽڣ�
            data     ���� ��in�� ��������      ��Bin��ʽ��
            dlen     ���� ��in�� ���ĳ��ȣ��ֽڣ�      ����Ϊ8�ֽڱ���
            atc      ���� ��in�� 8�ֽڵĳ�ʼֵ ��Bin��ʽ��
            key      ���� ��in�� ��Կ��        ��Bin��ʽ��
            IsTriple ���� ��in�� true:3DES��false:DES ��Ĭ��3DES

    ����ֵ����

    CBCģʽ��
            1)	���Ƚ����ݰ���8���ֽ�һ����з���õ�C1C2C3......Cn��
            2)	����һ�����ݽ��н��ܺ����ʼ������I�������õ���һ������D1 ��ע�⣺һ�����Ƚ�������򣩣�
            3)	���ڶ�������C2���н��ܺ����һ���������ݽ������õ��ڶ�������D2
            4)	֮���������ƣ��õ�Dn
            5)	��˳����ΪD1D2D3......Dn��Ϊ���ܽ����
*/
static void  DecryptWithCBC(unsigned char* rvalue,unsigned& rlen,const unsigned char* data,const unsigned dlen, const unsigned char atc[8],const unsigned char key[16], bool IsTriple=true);

//===================================================================================================================================
/*  �������ܣ� ʹ��DES�㷨����MAC

    ��ע��     ���������в����������

    ������1.����������ݿ鳤����8�ֽڵĻ�������������8�ֽڵ�16��������'80 00 00 00 00 00 00 00 '��
              2.����������ݿ鳤�Ȳ���8�ֽڣ�����������16��������'80'������ﵽ8�ֽڳ��ȣ���ת����һ����
                ������������16��������'00 'ֱ�����ȴﵽ8�ֽڡ�

    Hex��ʽ��
         ������  data     ������in�� ����MAC������    (Hex��ʽ��
                 random   ������in�� 8�ֽ����������ʼֵ��      (Hex��ʽ��
                 key      ������in�� ��Կ             (Hex��ʽ��
                 IsTriple ������in�� true:3DES��false:DES ��Ĭ��3DES

        ����ֵ��MAC��
*/
static QByteArray CalcMac(const QByteArray data, const QByteArray random, const QByteArray key, bool IsTriple=true);

//-----------------------------------------------------------------------------
/*
    ��ע��     ���������в����������

    ������1.����������ݿ鳤����8�ֽڵĻ�������������8�ֽڵ�16��������'80 00 00 00 00 00 00 00 '��
              2.����������ݿ鳤�Ȳ���8�ֽڣ�����������16��������'80'������ﵽ8�ֽڳ��ȣ���ת����һ����
                ������������16��������'00 'ֱ�����ȴﵽ8�ֽڡ�

     ������  mac      ������out��4�ֽ�MAC��      ��Bin��ʽ��
             bin      ������in�� ����MAC����     ��Bin��ʽ��
             len      ������in�� bin�ĳ��ȣ��ֽڣ�
             atc      ������in�� 8�ֽ����������ʼֵ����Bin��ʽ��
             key      ������in�� ��Կ            ��Bin��ʽ��
             IsTriple ������in�� true:3DES��false:DES ��Ĭ��3DES

    ����ֵ����
*/
static void CalcMac(unsigned char mac[4], const unsigned char* bin, int len, const unsigned char atc[8], const unsigned char key[16], bool IsTriple=true);


//-----------------------------------------------------------------------------
/*
    �������ܣ����㽻��MAC ��MAC�����ڲ���䣩

         ������  data     ������in�� ����MAC������         (Hex��ʽ��
                 random   ������in�� 8�ֽ����������ʼֵ�� (Hex��ʽ��
                 sessionK ���� (in) 8�ֽڹ�����Կ����  ��Hex��
                 key      ������in�� ��Կ             (Hex��ʽ��
                iAlg     ������in�� �㷨��0-3des��4-sm4

        ����ֵ��MAC��
*/
static QByteArray CalcTradeMac(const QByteArray data, const QByteArray random, const QByteArray sessionKeySeed,const QByteArray key,int iAlg);

//-----------------------------------------------------------------------------
/*
    �������ܣ����㽻��TAC ��TAC�����ڲ���䣩

         ������  data     ������in�� ����TAC������         (Hex��ʽ��
                 random   ������in�� 8�ֽ����������ʼֵ�� (Hex��ʽ��
                 key      ������in�� ��Կ             (Hex��ʽ��
                iAlg      ������in�� �㷨��0-3des��4-sm4

        ����ֵ��TAC��
*/
static QByteArray CalcTradeTac(const QByteArray data, const QByteArray random,const QByteArray key,int iAlg);




//==================================================================================================================================

/* ����Կ�Ƶ�

     �Ƶ��㷨���ο�����ĵ�˵��

     ������   data   ������in�� ��ɢϵ��  ������Ϊ8�ֽڱ����� ��Hex��ʽ��
              key    ������in�� ��Կ ��ֻ����˫������Կ��     ��Hex��ʽ��
              divCnt ������in�� ��ɢ���� (Ĭ��1��  data���ݵ��ֽڳ���/8

     ����ֵ����ɢ�������Կ

     ��ע����ɢϵ��������������ʹ��
*/
static QByteArray  DiversifyKey(QByteArray data, QByteArray key,int divCnt=1 );

//-----------------------------------------------------------------------------
/* ����Կ�Ƶ� 

   �Ƶ��㷨���ο�����ĵ�˵��

   ������   data     ������in��8�ֽڷ�ɢϵ��     ��Hex��ʽ��
            key      ������in����Կ              ��Hex��ʽ��
            IsTriple ������in��true:3DES��false:DES ��Ĭ��3DES

   ����ֵ����ɢ�������Կ

   ��ע���ú���ֻ֧��һ�η�ɢ�������Ҫ���ж�η�ɢ��Ҫ��ε��øú���
*/
static QByteArray  GetSubkey(QByteArray data, QByteArray key,bool IsTriple=true);

//-----------------------------------------------------------------------------
 /* ����Կ�Ƶ�(�Ǹ���Ӧ�ã�
    �Ƶ��㷨���ο�Դ��
*/
static void  GetSubkey(unsigned char* rvalue, int& rlen, unsigned char* data, int dlen, unsigned char* key,bool IsTriple=true);

//-----------------------------------------------------------------------------
 /* ����Կ�Ƶ�(�Ǹ���Ӧ�ã�
    �Ƶ��㷨���ο�Դ��
*/
static QByteArray  GenDiversifiedKey(QByteArray sData, QByteArray sKey, int iDiversifiedLevel);

//==================================================================================================================================

// ����ȡ��

//Hex
static QByteArray  Invert(QByteArray src);
//bin
static void  Invert(char *des, char *src, int length);
static void  Invert(unsigned char *des, unsigned char *src, int length);

//==================================================================================================================================
//==================================================================================================================================
//==================================================================================================================================
//����Ϊ�����㷨 SM4

/*
Hex�汾
    �������ܣ�ʹ��SM4�㷨�������ݼ��ܣ�128λ���飬��Կ����ɢ��

    ������  plaintext ���� ��in�� ��������   ��ע�⣺�ڲ����������ƴװ��
            key       ���� ��out��16�ֽ���Կ

    ����ֵ���������ģ���ʽ���������ݳ��� + �������� + ������ݣ�

    �������  1.����������ǰ������������ݳ���Ld������µ����ݣ�
                2.��16�ֽڷֿ飬���һ���������16�ֽڣ���Ҫ���� 0x80 00 ...
*/
static QByteArray  EncryptData_SM4(const QByteArray plaintext, const QByteArray key);

//-----------------------------------------------------------------------------
/*
Bin�汾
    �������ܣ�ʹ��SM4�㷨�������ݼ��ܣ�128λ���飬��Կ����ɢ��

    ������  rvalue    ���� ��out���������ݣ���ʽ���������� + ������ݣ�
            rlen      ���� ��out�����ĳ��ȣ�ע��
            plaintext ���� ��in�� �������ݣ�ע�⣺�ڲ����������ƴװ��
            key       ���� ��in�� 16�ֽ���Կ

    ����ֵ��

    ������򣺰�16�ֽڷֿ飬���һ���������16�ֽڣ���Ҫ���� 0x80 00 ...
*/
static void  EncryptData_SM4(unsigned char* rvalue, int& rlen, const unsigned char* plaintext, int textlen, const unsigned char key[16]);

//===================================================================================================================================
/*
Hex�汾
    �������ܣ�ʹ��SM4�㷨�������ݼ���
    ������  plaintext ������in���������ݣ�Hex��ʽ��������Ϊ16�ֽڱ���
            key       ������in����Կ

    ����ֵ������  (Hex��ʽ)
*/
static QByteArray  EncryptWithSM4(QByteArray plaintext, QByteArray key);

//-----------------------------------------------------------------------------
/*
Bin�汾
    �������ܣ�ʹ��SM4�㷨����16�ֽ����ݿ�
    ������  rvalue    ���� ��out��16�ֽ���������
            plaintext ���� ��in�� 16�ֽ���������
            key       ���� ��in�� ��Կ��

    ����ֵ����
*/
static void  EncryptWithSM4(unsigned char rvalue[16],const unsigned char plaintext[16],const unsigned char key[16]);

//-----------------------------------------------------------------------------
/*
Bin�汾
    �������ܣ�ʹ��SM4�㷨�������ݣ�ECBģʽ��
    ������  rvalue   ���� ��out���������ݣ�Bin��ʽ��
            rlen     ���� ��out�����ĳ��ȣ��ֽڣ�
            data     ���� ��in�� �������ݣ�Bin��ʽ��  ,����Ϊ16�ֽڱ���
            dlen     ���� ��in�� ���ĳ��ȣ��ֽڣ�
            key      ���� ��in�� ��Կ��

    ����ֵ����

    ECBģʽ��
         1.��һ������D1����SM4����,�õ���һ������C1
         2.�ڶ�������D2����SM4����,�õ��ڶ�������C2��֮��������Դ����ƣ��õ�Cn
         3.��˳����ΪC1C2C3......Cn��Ϊ���ܽ����
*/
static void  EncryptWithECB_SM4(unsigned char* rvalue,unsigned& rlen,const unsigned char* data,const unsigned dlen,const unsigned char key[16]);

//-----------------------------------------------------------------------------
/*

Bin�汾
    �������ܣ�ʹ��SM4�㷨�������ݣ�CBCģʽ��
    ������  rvalue   ���� ��out���������ݣ�Bin��ʽ��
            rlen     ���� ��out�����ĳ��ȣ��ֽڣ�
            data     ���� ��in�� �������ݣ�Bin��ʽ�� ,����Ϊ16�ֽڱ���
            dlen     ���� ��in�� ���ĳ��ȣ��ֽڣ�
            atc      ���� ��in�� 16�ֽڵĳ�ʼֵ
            key      ���� ��in�� ��Կ��

    ����ֵ����

    CBCģʽ��
         1.��һ������D1���ʼ������I����Ľ������SM4���ܵõ���һ������C1����ʼ������IΪȫ�㣩
         2.�ڶ�������D2���һ��ļ��ܽ��C1����Ժ�Ľ������SM4���ܣ��õ��ڶ�������C2��֮��������Դ����ƣ��õ�Cn
         3.��˳����ΪC1C2C3......Cn��Ϊ���ܽ����
*/
static void  EncryptWithCBC_SM4(unsigned char* rvalue,unsigned& rlen,const unsigned char* data,const unsigned dlen, const unsigned char atc[16],const unsigned char key[16]);


//mode: ����ģʽ��0 - ECB������-CBC
//atc: ��ʼ������mode ��0 ʱ��Ч
static QByteArray   EncryptWithSM4(QByteArray plaintext, QByteArray key,int mode,QByteArray atc="");

//===================================================================================================================================
/*
Hex�汾
    �������ܣ�ʹ��SM4�㷨�������ݽ���
    ������  data    ���� ��in���������ݣ�Hex��ʽ����ע�⣬�ڲ����Զ����0x00 .. ��
            key     ���� ��in����Կ

    ����ֵ���������� (Hex��ʽ)  ��ע�⣬�����������ݰ����˲������ݣ�

*/
static QByteArray DecryptData_SM4(const QByteArray data, const QByteArray key);

//-----------------------------------------------------------------------------
/* Bin�汾
       �������ܣ�ʹ��SM4�㷨���ݽ���
      ������rvalue     ���� ��out���������ݣ���������ݣ� ��Bin��ʽ����
            rlen       ���� ��out�����ĳ��ȣ��ֽڣ�
            ciphertext ���� ��in�� �������ݣ�Bin��ʽ��  ��ע�⣬�ڲ����Զ����0x00 .. ��
            textlen    ���� ��in�� ���ĳ��ȣ��ֽڣ�
            key        ���� ��in�� ��Կ��

      ����ֵ����
*/
static void DecryptData_SM4(unsigned char* rvalue, int& rlen, const unsigned char* ciphertext, int textlen,const unsigned char key[16]);

//===================================================================================================================================
/*
Hex�汾

   �������ܣ�ʹ��SM4�������ݽ��� �� 64λ��8�ֽڣ����飬��Կ����ɢ��

    ������  data     ������in�����ģ�Hex��ʽ�� ,����Ϊ16�ֽڱ���
            key      ������in����Կ

    ����ֵ������  (Hex��ʽ)
*/
static QByteArray  DecryptWithSM4(QByteArray data, QByteArray key);

//-----------------------------------------------------------------------------
/*
Bin�汾
    �������ܣ�ʹ��SM4�������ݽ���
    ������  rvalue   ���� ��in��16�ֽ���������
            data     ���� ��in��16�ֽ���������
            key      ���� ��in����Կ��

    ����ֵ����
*/
static void  DecryptWithSM4(unsigned char rvalue[16],const unsigned char data[16],const unsigned char key[16]);

//-----------------------------------------------------------------------------
/*
Bin�汾
    �������ܣ�ʹ��SM4�������ݽ��ܣ�ECBģʽ��
    ������  rvalue   ���� ��out���������ݣ�Bin��ʽ��
            rlen     ���� ��out�����ĳ��ȣ��ֽڣ�
            data     ���� ��in�� �������ݣ�Bin��ʽ��
            dlen     ���� ��in�� ���ĳ��ȣ��ֽڣ�
            key      ���� ��in�� ��Կ��
            IsTriple ���� ��in�� true:3DES��false:DES ��Ĭ��3DES

    ����ֵ����

    ECBģʽ��
         1.���Ƚ����ݰ���16���ֽ�һ����з���õ�C1C2C3......Cn��
         1.��һ������C1����SM4����,�õ���һ������D1
         2.�ڶ�������C2����SM4����,�õ��ڶ�������D2��֮��������Դ����ƣ��õ�Dn
         3.��˳����ΪD1D2D3......Dn��Ϊ���ܽ����
*/
static void  DecryptWithECB_SM4(unsigned char* rvalue,unsigned& rlen,const unsigned char* data,const unsigned dlen,const unsigned char key[16]);

//-----------------------------------------------------------------------------
/*
Bin�汾

    �������ܣ�ʹ��SM4�������ݽ��ܣ�CBCģʽ��
    ������  rvalue   ���� ��out���������ݣ�Bin��ʽ��
            rlen     ���� ��out�����ĳ��ȣ��ֽڣ�
            data     ���� ��in�� �������ݣ�Bin��ʽ��
            dlen     ���� ��in�� ���ĳ��ȣ��ֽڣ�
            atc      ���� ��in�� 16�ֽڵĳ�ʼֵ
            key      ���� ��in�� ��Կ��
            IsTriple ���� ��in�� true:3DES��false:DES ��Ĭ��3DES

    ����ֵ����

    CBCģʽ��
            1)	���Ƚ����ݰ���16���ֽ�һ����з���õ�C1C2C3......Cn��
            2)	����һ�����ݽ��н��ܺ����ʼ������I�������õ���һ������D1 ��ע�⣺һ�����Ƚ�������򣩣�
            3)	���ڶ�������C2���н��ܺ����һ���������ݽ������õ��ڶ�������D2
            4)	֮���������ƣ��õ�Dn
            5)	��˳����ΪD1D2D3......Dn��Ϊ���ܽ����
*/
static void  DecryptWithCBC_SM4(unsigned char* rvalue,unsigned& rlen,const unsigned char* data,const unsigned dlen, const unsigned char atc[16],const unsigned char key[16]);

//===================================================================================================================================
/*
Hex�汾

    �������ܣ� ʹ��SM4����MAC

    ��ע��     ���������в����������

    ������1.����������ݿ鳤����16�ֽڵĻ�������������16�ֽڵ�16��������'80 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00'��
              2.����������ݿ鳤�Ȳ���16�ֽڣ�����������16��������'80'������ﵽ16�ֽڳ��ȣ���ת����һ����
                ������������16��������'00 'ֱ�����ȴﵽ16�ֽڡ�
                

    ������  data     ������in�� ����MAC������
            random   ������in�� 16�ֽ����������ʼֵ������16�ֽڲ�0x00��
            key      ������in�� ��Կ

    ����ֵ��MAC��
*/
static QByteArray CalcMac_SM4(const QByteArray data, const QByteArray random, const QByteArray key);

//==================================================================================================================================
/* 
  Bin��ʽ��
     ������  mac      ������out��4�ֽ�MAC��
             bin      ������in�� ����MAC����
             len      ������in�� bin�ĳ��ȣ��ֽڣ�
             atc      ������in�� 16�ֽ����������ʼֵ����ע�⣺����16�ֽڲ�0��
             key      ������in�� ��Կ
             IsTriple ������in�� true:3DES��false:DES ��Ĭ��3DES

    ����ֵ����
*/
static void CalcMac_SM4(unsigned char mac[4], const unsigned char* bin, int len, const unsigned char atc[16], const unsigned char key[16]);

//==================================================================================================================================
/*

Hex�汾
    SM4�㷨����Կ�Ƶ�

     �Ƶ��㷨���ο�����ĵ�˵��

     ������   data   ������in�� 8�ֽڷ�ɢϵ��
              key    ������in�� ��Կ ��ֻ����˫������Կ��
              divCnt ������in����ɢ������Ĭ��1��

     ����ֵ����ɢ�������Կ



*/
static QByteArray  DiversifyKey_SM4(QByteArray data, QByteArray key,int divCnt=1);

//==================================================================================================================================
/*
Hex�汾
   SM4�㷨����Կ�Ƶ�

   �Ƶ��㷨���ο�����ĵ�˵��

   ������   data     ������in��8�ֽڷ�ɢϵ��  (ʵ�ʷ�ɢϵ��16�ֽڣ� 8�ֽڷ�ɢϵ�� + 8�ֽڷ�ɢϵ��ȡ����
            key      ������in����Կ

   ����ֵ����ɢ�������Կ

   ��ע���ú���ֻ֧��һ�η�ɢ�������Ҫ���ж�η�ɢ��Ҫ��ε��øú���
*/
static QByteArray  GetSubkey_SM4(QByteArray data, QByteArray key);

//==================================================================================================================================
 /* SM4����Կ�Ƶ�(bin�汾)
    ������
        rvalue    -(out)����Կ
        rlen      -(out)����Կ����
        data      -(in) ��ɢ����
        dlen      -(in) ��ɢ���ݳ��ȣ�����Ϊ8�ֽڱ���
        key       -(in) ��Կ

*/
static void  GetSubkey_SM4(unsigned char* rvalue, int& rlen, unsigned char* data, int dlen, unsigned char* key);


//==================================================================================================================================
/* SM4�㷨������Կ����

   �Ƶ��㷨������������In��λȡ���õ���~In��������In||(~In)��˳��������һ�����16�ֽ���������DATA��

   ������   data     ������in��8�ֽڷ�ɢϵ��  (ʵ�ʷ�ɢϵ��16�ֽڣ� 8�ֽڷ�ɢϵ�� + 8�ֽڷ�ɢϵ��ȡ����
            key      ������in����Կ

   ����ֵ��������Կ

*/
static QByteArray  GetSeskey_SM4(QByteArray data, QByteArray key);



//SM4 end
//==================================================================================================================================
//==================================================================================================================================
//==================================================================================================================================
//==================================================================================================================================
//����Ϊ�����㷨 SM2

#ifdef USER_SM2

//------------------------------------------------------------------
//�������ƣ�GenerateKey_SM2
//�������ܣ����ݹ���SM2�㷨���ɹ�˽Կ��
//�����������
//���������pv_key   -˽Կ��ʮ�����ƴ���
//          pb_key   -��Կ(X||Y)��ʮ�����ƴ���

//����ֵ��void
static void __fastcall GenerateKey_SM2(QByteArray& pv_key, QByteArray& pb_key);

//------------------------------------------------------------------
//�������ƣ�Sign_SM2
//�������ܣ�����SM2�㷨�����ݽ���ǩ��
//���������msg        -ԭʼ�������ݣ�ʮ������ֵ��
//        pv_key       -ǩ��˽Կ��ʮ������ֵ��
//        pb_key       -ǩ����Կ"X||Y"��ʮ������ֵ��
//       sig_encoding  -ǩ��������뷽ʽ: 1-"R||S"   2-DER����

//���������sign       -ǩ������������ʽ��sigcodeȷ����ʮ������ֵ��
//          digest     -ժҪ������Դ�ǩ�����ݰ��� SM2�����㷨ʹ�ù淶 �ļ������� ʮ������ֵ

//����ֵ��int 0��ʾǩ���ɹ���������ʾʧ��

static int __fastcall Sign_SM2(QByteArray msg, QByteArray pv_key,QByteArray pb_key,int sig_encoding,
							   QByteArray& sign,QByteArray& digest);

//------------------------------------------------------------------
//�������ƣ�Verify_SM2
//�������ܣ�����SM2�㷨�����ݽ�����ǩ
//���������msg        -��ǩ�����ݣ�ʮ������ֵ��
//        pb_key       -ǩ����Կ"X||Y"��ʮ������ֵ��
//        csig         -ǩ������������ʽ��sigcodeȷ����ʮ������ֵ��
//        sig_encoding -ǩ��������뷽ʽ: 1-"R||S"   2-DER����

//�����������

//����ֵ�� int 0��ʾ��ǩ�ɹ���������ʾʧ��

static int __fastcall Verify_SM2(QByteArray msg, QByteArray pb_key,QByteArray sign,int sig_encoding=1);


//------------------------------------------------------------------
/*
  SM2����ʹ�ý��շ���Կ���ܣ���Կ��һ�������������ɣ�2��32byte��BigInteger�������ߵ�P��x,y����
  SM2��Կ���ܽ����3��������ɣ�SM2���ܹ�����ʹ��������������ͬ������������ÿһ�μ��ܽ������һ����

  ���ݹ����Ƽ���SM2��Բ���߹�Կ�����㷨�����Ȳ������������������ߵ�C1��2��32byte��BigInteger��������ΪSM2���ܽ���ĵ�1���֡�
  ��2�����������������ģ��Ƕ����ĵļ��ܽ�������Ⱥ�����һ����
  ��3�������Ӵ�ֵ������У�����ݡ�
  �������Ƽ���256λ��Բ���ߣ����ļ��ܽ����ԭ���Ȼ��96byte��
*/

//�������ƣ�EncryptData_SM2
//�������ܣ�����SM2�㷨�����ݽ��м���
//���������plaintext  -�����ܵ����ݣ�ʮ������ֵ��
//          pb_key   -��Կ"X||Y"��ʮ������ֵ��

//���������ciphertext   -���ܺ�����ݣ�ʮ������ֵ��C1||C3||C2
//          ���У�C1-64byte, C2-���Ķ�Ӧ���ģ� C3-�Ӵ�ֵ -32byte

//����ֵ��int 0��ʾ�ɹ���������ʾʧ��
static int __fastcall EncryptData_SM2(QByteArray plaintext, QByteArray pb_key, QByteArray& ciphertext);


//------------------------------------------------------------------
//�������ƣ�DecryptData_SM2
//�������ܣ�����SM2�㷨�����ݽ��н���
//���������ciphertext -�����ܵ��������ݣ�ʮ������ֵ�� C1||C3||C2
//          pv_key     -˽Կ������Ϊ32�ֽڣ�ʮ������ֵ��


//���������retdata    -���ܺ�����ݣ�ʮ������ֵ��

//����ֵ��int 0��ʾ�ɹ���������ʾʧ��
static int __fastcall DecryptData_SM2(QByteArray ciphertext, QByteArray pv_key, QByteArray& plaintext);


#endif


};
#endif
