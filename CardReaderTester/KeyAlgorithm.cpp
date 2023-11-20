//---------------------------------------------------------------------------




#include "KeyAlgorithm.h"
#include <QString>
#include <QTextCodec>
#include <QDebug>
#include <QObject>
#include <QTimer>
#include "sm4.h"

//==================================================================================================
//==================================================================================================
 // ����ȡ������
QByteArray  TKeyAlgorithm::Invert(QByteArray src)
{
//    char cmdbuf[1024] = {0}, buf[128] = {0};
	int ilen = src.length();
    if(ilen <1) return "";

    char *cmdbuf = new char[ilen+16];
    char *buf = new char[ilen+16];
    _arr_man arr_man((unsigned char *)cmdbuf);
    _arr_man arr_man2((unsigned char *)buf);
    memset(cmdbuf,0x00,ilen+16);
    memset(buf,0x00,ilen+16);

	HexToBin(src.data(), cmdbuf, ilen>>1);
    TKeyAlgorithm::Invert(buf, cmdbuf, ilen>>1);

    memset(cmdbuf, 0, ilen+16);
    BinToHex(buf, cmdbuf, ilen>>1);

	return QByteArray(cmdbuf);;
}
//---------------------------------------------------------------------------
void  TKeyAlgorithm::Invert(char *des, char *src, int length)
{
    TKeyAlgorithm::Invert((unsigned char *)des, (unsigned char *)src, length);
}
//---------------------------------------------------------------------------
void  TKeyAlgorithm::Invert(unsigned char *des, unsigned char *src, int length)
{
    if(src == nullptr || des == nullptr || length < 1){
        return ;
    }
    for(int i = 0; i < length; i++){
        des[i] = ~src[i];
    }
}
//ȡ������  end
//==================================================================================================
//==================================================================================================
//DES��3DES ��������
//---------------------------------------------------------------------------
QByteArray  TKeyAlgorithm::EncryptData(const QByteArray data, const QByteArray key, bool IsTriple)
{
    unsigned char mackey[17]  = {0};
    unsigned char buf[1024*2] = {0};
    unsigned char atc[4]      = {0};

	QByteArray tmp;
    string bin;
    int len;

	len = data.length();
    if(len < 1)    return "";

    tmp = QString().asprintf("%02X%s", len/2, data.data()).toLocal8Bit();    //����ƴװ�����ݳ���+����
	len  = tmp.length()/2;

	HexToBin(tmp.data(),buf,len);

    for(int i=0; i<len; i++)
        bin.push_back(buf[i]);

    len = bin.size();
    memset(buf, 0, sizeof(buf));
    for(int i = 0; i < len; i++)
        buf[i] = (unsigned char)bin[i];

	len = key.length();
	if(len > 1)  HexToBin(key.data(),mackey,len/2);

    unsigned char* rvalue = new unsigned char[bin.size()+16];
    unsigned char* pbuf   = new unsigned char[((bin.size()+16)<<1)+1];
    _arr_man arr_man(rvalue);
    _arr_man arr_man1(pbuf);

    memset(pbuf,0,((bin.size()+16)<<1)+1);
    int rlen=0;
	EncryptData(rvalue,rlen,(unsigned char*)bin.data(),bin.size(),mackey,IsTriple);
    BinToHex(rvalue,pbuf,rlen);

    return (char*)pbuf;

}
//---------------------------------------------------------------------------
void  TKeyAlgorithm::EncryptData(unsigned char* rvalue, int& rlen, const unsigned char* bin, int len,
                                                                              const unsigned char key[16], bool IsTriple)
{
    int cnt;
    unsigned char mackey[17] = {0};

    if(key != nullptr ){
        memset(mackey, 0, sizeof(mackey));
        if(IsTriple){
            memcpy(mackey, key, 16);
        }else{
            memcpy(mackey, key, 8);
        }
    } else return ;

    int ilen = len;
    unsigned char* pbuf = new unsigned char[ilen+16];
    _arr_man arr_man(pbuf);

    memset(pbuf,0,ilen+16);
    memcpy(pbuf,bin,ilen);

    if(ilen%8 != 0) pbuf[ilen++] = 0x80;
    if(ilen%8 != 0){
        cnt = 8 - (ilen%8);
        if(cnt > 0)
              memset(pbuf+ilen,0x00,cnt);
        ilen += cnt;
    }

    D3Des des;

    //��ʼ����ATC
    unsigned char h[9] = {0};
    unsigned char x[9] = {0};

    for(int i=0; i<ilen; i+=8)      //��8�ֽڷֿ����
    {
    	memcpy(x, pbuf+i, 8);
        des.deskey(mackey, EN0);       //����des��Կ��KnL��hexkey�����ӽ��ܵ�ģʽ
//        des.Ddes(x, h);
        des.des(x,h);                 //����deskey()���õ���Կ��ģʽ���� X ���ݽ���des����
        if(IsTriple){
            des.deskey(mackey+8, DE1);
          //  des.Ddes(h, x);         //DES����
            des.des(h,x);
            des.deskey(mackey, EN0);
          //des.Ddes(x,h);
            des.des(x,h);
        }

        memcpy(rvalue+i,h,8);
	}
        rlen=ilen;
	
}
//---------------------------------------------------------------------------
// Triple des
QByteArray  TKeyAlgorithm::EncryptWithDes(QByteArray data, QByteArray key, bool IsTriple)
{
    unsigned char buf[24] = {0};
    unsigned char ckey[17] = {0};
	QByteArray rvalue;
    int length;

	length = key.length();
    if(length > 1){
		HexToBin(key.data(),ckey,length>>1);
//        memcpy(ckey, buf, length/2);
    }
	length = data.length();
    if(length < 1) return "";

    unsigned char rbuf[9] = {0};
    char rhex[17] = {0};
	for(int i = 0; i < data.length(); i += 16)
    {
        memset(rbuf,0,9);
        memset(rhex,0,17);
        memset(buf, 0, sizeof(buf));
		HexToBin(data.data() + i, buf, 8);

        EncryptWithDes(rbuf, buf, ckey, IsTriple);

        BinToHex(rbuf,rhex,8);
        rvalue += rhex;
    }

    return rvalue;
}
//---------------------------------------------------------------------------
void  TKeyAlgorithm::EncryptWithDes(unsigned char* rvalue,const unsigned char* data, const unsigned char* key, bool IsTriple)
{

    unsigned char r[9] = {0};
    unsigned char h[9] = {0};
    unsigned char x[9] = {0};
    unsigned char ckey[17] = {0};
    int length = 8;
    if(IsTriple)   length = 16;

    if(key != nullptr ){
        memset(ckey, 0, sizeof(ckey));
        memcpy(ckey, key, length);
    }else return;

    memcpy(r, data, 8);

    D3Des oD3Des;
    oD3Des.deskey(ckey, EN0);
    oD3Des.des(r, h);
    if(IsTriple){
        oD3Des.deskey(ckey+8, DE1);
        oD3Des.Ddes(h, x);
        oD3Des.deskey(ckey, EN0);
        oD3Des.Ddes(x, h);
    }
    memcpy(rvalue,h,8);

}
//---------------------------------------------------------------------------
void  TKeyAlgorithm::EncryptWithECB(unsigned char* rvalue,unsigned& rlen,const unsigned char* data,const unsigned dlen,const unsigned char key[16], bool IsTriple)
{
    for(rlen=0;rlen<dlen;rlen+=8)
    {
        EncryptWithDes(rvalue+rlen,data+rlen,key,IsTriple);
    }
}
//---------------------------------------------------------------------------
void  TKeyAlgorithm::EncryptWithCBC(unsigned char* rvalue,unsigned& rlen,const unsigned char* data,const unsigned dlen, const unsigned char atc[8],const unsigned char key[16], bool IsTriple)
{
    unsigned char x[9]={0};
    unsigned char h[9]={0};
    memcpy(h,atc,8);
    for(rlen=0;rlen<dlen;rlen+=8)
    {
        memcpy(x,data+rlen,8);
        for(int i=0;i<8;++i)
            x[i]^=h[i];
        EncryptWithDes(h,x,key,IsTriple);
        memcpy(rvalue+rlen,h,8);
    }
}
//DES��3DES ��������  end
//==================================================================================================
//==================================================================================================
//DES��3DES ��������

//---------------------------------------------------------------------------
QByteArray TKeyAlgorithm::DecryptData(const QByteArray data, const QByteArray key, bool IsTriple)
{
    unsigned char mackey[17] = {0};
	QByteArray tmp;
    unsigned char buf[1024*2] = {0};
    string bin;
	unsigned char atc[4] = {0};
    int length, cnt;

    tmp = data;
	length = tmp.length();
    if(length < 1){
        return "";
    }
    tmp = QString().asprintf("%s", data.data()).toLocal8Bit();
	length = tmp.length();
	HexToBin(tmp.data(), buf, length>>1);
    for(int i = 0; i < length/2; i++){
        bin.push_back(buf[i]);
    }
    // ��0ȥ���ַ�������Ϊ8�ı�������
	cnt = bin.size() % 8;
	if(cnt != 0)
		bin.insert(bin.end(), 8 - cnt, 0x00);

    memset(buf, 0, sizeof(buf));
	length = key.length();
    if(length > 1){
		HexToBin(key.data(), buf,length>>1);
        memcpy(mackey, buf, length/2);
    }
    unsigned char* rvalue = new unsigned char[bin.size()+1];
    unsigned char* pbuf = new unsigned char[(bin.size()<<1)+1];
    _arr_man arr_man(rvalue);
    _arr_man arr_man1(pbuf);
     memset(pbuf,0,(bin.size()<<1)+1);
    int rlen=0;
	DecryptData(rvalue,rlen,(unsigned char*)bin.data(), bin.size(),mackey, IsTriple);
    BinToHex(rvalue,pbuf,rlen);
    return (char*)pbuf;
}

//---------------------------------------------------------------------------
void TKeyAlgorithm::DecryptData(unsigned char* rvalue, int& rlen, const unsigned char* bin, int len,const unsigned char key[16], bool IsTriple)
{
    int cnt;
    unsigned char mackey[17] = {0};

    if(key[0] != 0){
        memset(mackey, 0, sizeof(mackey));
        if(IsTriple){
            memcpy(mackey, key, 16);
        }else{
            memcpy(mackey, key, 8);
        }
    }else return ;

    int ilen = len;
    unsigned char* pbuf=new unsigned char[ilen+1];
    _arr_man arr_man(pbuf);
    memset(pbuf,0,ilen+1);
    memcpy(pbuf,bin,ilen);
    //��鲹ȫΪ8�ı���
    cnt = ilen%8;
    for(int i=0;i<cnt;++i)
    {
        pbuf[ilen++] = 0x00;
    }

    D3Des des;

    //��ʼ����ATC
    unsigned char h[9] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    unsigned char x[9] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    memset(rvalue,0,ilen);
    for(size_t i=0; i<ilen; i+=8){
    	memcpy(&x, &pbuf[i], sizeof(x));
        des.deskey(mackey, DE1);
        des.Ddes(x, h);
        if(IsTriple){
            des.deskey(mackey+8, EN0);
            des.Ddes(h, x);
            des.deskey(mackey, DE1);
            des.Ddes(x, h);
        }

        memcpy(&rvalue[i],h,8);
    }
    rlen = ilen;
}
//---------------------------------------------------------------------------
// Triple des
QByteArray  TKeyAlgorithm::DecryptWithDes(QByteArray data, QByteArray key, bool IsTriple)
{
    unsigned char buf[24] = {0};
    unsigned char ckey[17] = {0};
	QByteArray rvalue;
    int length;

	length = key.length();
    if(length > 1){
		HexToBin(key.data(), ckey, length>>1);
//        memcpy(ckey, buf, length>>1);
    } 
	length = data.length();
    if(length < 1)  return "";

    unsigned char rbuf[9]={0};
    char rHex[17]={0};
	for(int i = 0; i < data.length(); i += 16)
    {
        memset(rbuf, 0, sizeof(rbuf));
        memset(rHex, 0, sizeof(rHex));
        memset(buf,  0, sizeof(buf));
		HexToBin(data.data() + i, buf, 8);

        DecryptWithDes(rbuf, buf, ckey, IsTriple);

        BinToHex(rbuf,rHex,8);
        rvalue += rHex;
    }

    return rvalue;
}
//---------------------------------------------------------------------------
void  TKeyAlgorithm::DecryptWithDes(unsigned char* rvalue, const unsigned char* data,const unsigned char* key, bool IsTriple)
{
    unsigned char r[9] = {0};
    unsigned char h[9] = {0};
    unsigned char x[9] = {0};
    unsigned char ckey[17] = {0};

    int length;


    length = 8;
    if(IsTriple){
        length = 16;
    }
    if(key != nullptr && key[0] != '0'){
        memset(ckey, 0, sizeof(ckey));
        memcpy(ckey, key, length);
    }
    memcpy(r, data, 8);


    D3Des oD3Des;
    oD3Des.deskey(ckey, DE1);
    oD3Des.des(r, h);
    if(IsTriple){
        oD3Des.deskey(ckey+8, EN0);
        oD3Des.Ddes(h, x);
        oD3Des.deskey(ckey, DE1);
        oD3Des.Ddes(x, h);
    }

    memcpy(rvalue,h,8);
}
//---------------------------------------------------------------------------
void  TKeyAlgorithm::DecryptWithECB(unsigned char* rvalue,unsigned& rlen,const unsigned char* data,const unsigned dlen,const unsigned char key[16], bool IsTriple)
{
    for(rlen=0;rlen<dlen;rlen+=8)
    {
        DecryptWithDes(rvalue+rlen,data+rlen,key,IsTriple);
    }
}
void  TKeyAlgorithm::DecryptWithCBC(unsigned char* rvalue,unsigned& rlen,const unsigned char* data,const unsigned dlen, const unsigned char atc[8],const unsigned char key[16], bool IsTriple)
{
    unsigned char x[9]={0};
    unsigned char h[9]={0};
    unsigned char d[9]={0};
    memcpy(d,atc,8);

    for(rlen=0;rlen<dlen;rlen+=8)
    {
        memcpy(x,data+rlen,8);
        DecryptWithDes(h,x,key,IsTriple);    //���ݽ���
        for(int i=0;i<8;++i)
            h[i]^=d[i];                     // �Ƚ��ܣ������
        memcpy(d,x,8);                     //�ϴν��ܵ�����
        memcpy(rvalue+rlen,h,8);
    }


}
//DES��3DES ��������  end
//==================================================================================================
//==================================================================================================
//DES��3DES ���� MAC ��

//---------------------------------------------------------------------------
QByteArray TKeyAlgorithm::CalcMac(const QByteArray data, const QByteArray random, const QByteArray key, bool IsTriple)
{
	QByteArray lkey;
    char buf[1024*2] = {0};
    unsigned char atc[9] = {0}, mackey[17] = {0};
    int length;

    lkey = key;

	length = lkey.length();
    if(length < 1) return "";
	HexToBin(lkey.data(),buf,length>>1);
    memcpy(mackey, buf, length>>1);

	length = data.length();
    if(length < 1) return "";
	HexToBin(data.data(),buf,length>>1);
    string bin(buf,length>>1);

	length = random.length();
    if(length < 1)  return "";
	HexToBin(random.data(),buf,length>>1);
    memcpy(atc, buf, length>>1);

    unsigned char mac[4]={0};
    char MAC[9]={0};

	CalcMac(mac,(const unsigned char*)bin.data(), bin.length(), atc, mackey, IsTriple);

    BinToHex(mac,MAC,4);

	return QByteArray(MAC);
}
//---------------------------------------------------------------------------
void TKeyAlgorithm::CalcMac(unsigned char mac[4], const unsigned char* bin, int len, const unsigned char atc[8], const unsigned char key[16], bool IsTriple)
{
    int cnt;
    unsigned char mackey[17] = {0};

    memset(mackey, 0, sizeof(mackey));
    if(IsTriple){
        memcpy(mackey, key, 16);
    }else{
        memcpy(mackey, key, 8);
    }

    int ilen = len;
    unsigned char* pbuf = new unsigned char[ilen+16];
    _arr_man arr_man(pbuf);
    memset(pbuf,0,ilen+16);
    memcpy(pbuf,bin,ilen);

    pbuf[ilen++]=0x80;    //�������ݲ��벻��ȷ
    if(ilen%8!=0){
        cnt = 8 - (ilen % 8);
        for(int i=0;i<cnt;++i)
            pbuf[ilen++] = 0x00;
    }

    D3Des des;
    des.deskey(mackey, EN0);

    //��ʼ����ATC
    unsigned char h[9] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    unsigned char x[9]={0};
    
    memcpy(h, atc, 8); // 8�ֽ������

    for(int i=0; i<ilen; i+=8)
    {
        memset(x, 0, sizeof(x));
        memcpy(x, pbuf+i, 8);
        //���
        for(int j=0; j<sizeof(x); j++){
            x[j] ^= h[j];
        }
        des.Ddes(x, h);
    }


    if(IsTriple){
        des.deskey(mackey+8, DE1);
        des.Ddes(h, x);
        des.deskey(mackey, EN0);
        des.Ddes(x, h);
    }

    memcpy(mac,h,4);
}

//DES��3DES ���� MAC ��  end
//==================================================================================================
//==================================================================================================
//DES��3DES ��Կ��ɢ
//---------------------------------------------------------------------------
QByteArray  TKeyAlgorithm::GetSubkey(QByteArray data, QByteArray key,bool IsTriple)
{
	QByteArray rvalue, des;
    char buf[256] = {0}, src[256] = {0};
    unsigned char ckey[17] = {0};

	if(data.length() ==0)  return key;
	if(key.length() < 1)   return "";

	HexToBin(key.data(),buf,key.length()>>1);
    memcpy(ckey, buf, 16);

    des = TKeyAlgorithm::EncryptWithDes(data, key, IsTriple);

    if(IsTriple)
    {
		HexToBin(data.data(),src,data.length()>>1);
        TKeyAlgorithm::Invert(buf, src, 8);
        unsigned char rbuf[9]={0};
        char rhex[17] = {0};
        TKeyAlgorithm::EncryptWithDes(rbuf,(unsigned char*)buf, ckey,IsTriple);
        BinToHex(rbuf,rhex,8);
        des +=  rhex;
    }
    rvalue = des;

    return rvalue;
}

//---------------------------------------------------------------------------
void  TKeyAlgorithm::GetSubkey(unsigned char* rvalue, int& rlen, unsigned char* data, int dlen, unsigned char* key,bool IsTriple)
{
    unsigned char buf[256] = {0};

    if(dlen < 1){
        return ;
    }
    for(rlen=0;rlen<dlen;rlen+=8)
    {
        memset(buf,0,8);
        TKeyAlgorithm::EncryptWithDes(buf,data+rlen,key,IsTriple);    //3des����
        memcpy(rvalue+rlen,buf,8);
    }
    if(IsTriple)
    {
        TKeyAlgorithm::Invert(buf, data, dlen);            //ȡ��
        unsigned char rbuf[9]={0};
        for(int i=0;i<dlen;i+=8)
        {
            TKeyAlgorithm::EncryptWithDes(rbuf,(unsigned char*)buf+i,key,IsTriple);   //����
            memcpy(rvalue+rlen,rbuf,8);
            rlen += 8;
        }
    }
}
//---------------------------------------------------------------------------
//QByteArray __fastcall TKeyAlgorithm::DiversifyKey(QByteArray data, QByteArray key)
//{
//    return GetSubkey(data, key);
//}
QByteArray  TKeyAlgorithm::DiversifyKey(QByteArray data, QByteArray key,int divCnt )
{
	QByteArray divKey = key;
	QByteArray coefficient;
    for (int i=0;i<divCnt ;i++){

		coefficient = data.mid(16*i,16+16*i);
        divKey =  GetSubkey(coefficient, divKey);
    }

    return divKey;
}
//---------------------------------------------------------------------------
QByteArray  TKeyAlgorithm::GenDiversifiedKey(QByteArray sData, QByteArray sKey, int iDiversifiedLevel)
{
	QByteArray rvalue, tmp;

    rvalue = sKey;
	if(rvalue.length() != 32){
        return "";
    }
    switch(iDiversifiedLevel){
    case 2:
		tmp = sData.mid(0, 8);
        tmp += tmp;
		if(tmp.length() != 16){
            return "";
        }
        rvalue = GetSubkey(tmp, rvalue);
		if(rvalue.length() != 32){
            return "";
        }
    case 1:
        tmp = sData;
		if(tmp.length() != 16){
            return "";
        }
        rvalue = GetSubkey(tmp, rvalue);
		if(rvalue.length() != 32){
            return "";
        }
        break;
    }

    return rvalue;          
}

//DES��3DES ��Կ��ɢ  end
//==================================================================================================
//==================================================================================================
//����Ϊ�����������

// SM4 ��������
//---------------------------------------------------------------------------
QByteArray  TKeyAlgorithm::EncryptData_SM4(const QByteArray data, const QByteArray key)
{
    unsigned char mackey[17] = {0};
	QByteArray tmp;
    char buf[1024*2] = {0};

    string bin;

    int length;

    tmp = data;
	length = tmp.length();
    if(length < 1){
        return "";
    }
    tmp = QString().asprintf("%02X%s", length/2, data.data()).toLocal8Bit();       //�������ݳ���LD + ��������data
	length = tmp.length();

	HexToBin(tmp.data(),buf,length>>1);
    for(int i = 0; i < length/2; i++){
        bin.push_back(buf[i]);
    }
    length = bin.size();
    memset(buf, 0, sizeof(buf));
    for(int i = 0; i < length; i++){
        buf[i] = (unsigned char)bin[i];
    }
	length = key.length();
    if(length > 1){
		HexToBin(key.data(),buf,length>>1);
        memcpy(mackey, buf, length/2);
    }
    unsigned char* rvalue = new unsigned char[bin.size()+16];
    unsigned char* pbuf = new unsigned char[((bin.size()+16)<<1)+1];
    _arr_man arr_man(rvalue);
    _arr_man arr_man1(pbuf);
    memset(pbuf,0,((bin.size()+16)<<1)+1);
    int rlen=0;
	EncryptData_SM4(rvalue,rlen,(unsigned char*)bin.data(),bin.size(),mackey);
    BinToHex(rvalue,pbuf,rlen);
    return (char*)pbuf;
}
//---------------------------------------------------------------------------

void  TKeyAlgorithm::EncryptData_SM4(unsigned char* rvalue, int& rlen, const unsigned char* plaintext, int textlen, const unsigned char key[16])
{

    if(key==nullptr || plaintext==nullptr || textlen<=0){
        return ;
    }
    if(rvalue==nullptr ){
        return ;
    }

    int  nDataLen = textlen;

    while(nDataLen%16 != 0){       //16�ֽڷ���
        nDataLen++;
    }

    unsigned char* pBuf  =  new unsigned char[nDataLen+1];
    _arr_man arr_man(pBuf);

    memset(pBuf, 0, nDataLen+1);
    memcpy(pBuf, plaintext, textlen);

    //text ���Ȳ�Ϊ16������ʱ����Ҫ���� 0x80 00 ...

    int cnt;
    if(textlen%16 != 0)
    {
        pBuf[textlen++] = 0x80;
        cnt = 16 - (textlen%16);
        if(cnt > 0)
            memset(pBuf+textlen,0x00,cnt);

    }

    sm4_context  sm4Text;
    sm4_setkey_enc(&sm4Text,(unsigned char*)key);
    sm4_crypt_ecb(&sm4Text, SM4_ENCRYPT, nDataLen, pBuf, rvalue);

    rlen = nDataLen;


}
//---------------------------------------------------------------------------
QByteArray   TKeyAlgorithm::EncryptWithSM4(QByteArray plaintext, QByteArray key,int mode, QByteArray atc)
{
    unsigned char buf[1024] = {0};
    unsigned char ckey[17] = {0};
    unsigned char iv[17] = {0};
    unsigned char rvalue[1024]={0};
    int length;

	length = key.length();
    if(length > 1)
		HexToBin(key.data(), ckey, length>>1);

	length = plaintext.length();
    if(length < 1)    return "";
	HexToBin(plaintext.data(), buf, length>>1);

    if(mode)
    {
		if(atc.length() != 32) return "";
		HexToBin(atc.data(),iv,atc.length()/2);
    }
    sm4_context  sm4Text;
    sm4_setkey_enc(&sm4Text,ckey);
    if(mode==0)
        sm4_crypt_ecb(&sm4Text, SM4_ENCRYPT, length>>1, buf, rvalue);
    else
        sm4_crypt_cbc(&sm4Text,SM4_ENCRYPT,length>>1, iv,buf, rvalue);

    memset(buf,0x00,1024);
    BinToHex(rvalue,buf,length>>1);

    return (char*)buf;
}
QByteArray  TKeyAlgorithm::EncryptWithSM4(QByteArray plaintext, QByteArray key)
{
    unsigned char buf[24] = {0};
    unsigned char ckey[17] = {0};
	QByteArray rvalue;
    int length;

	length = key.length();
    if(length > 1){
		HexToBin(key.data(), buf, length>>1);
        memcpy(ckey, buf, length>>1);
    }
	length = plaintext.length();
    if(length < 1){
        return "";
    }
	for(int i = 0; i < plaintext.length(); i += 32){

        unsigned char rbuf[17]={0};
        char rHex[33]={0};
        memset(buf, 0, sizeof(buf));
		HexToBin(plaintext.data() + i, buf, 16);
        EncryptWithSM4(rbuf,buf,ckey);         //SM4 �������
        BinToHex(rbuf,rHex,16);
        rvalue += rHex;
    }

    return rvalue;

}
//---------------------------------------------------------------------------
void  TKeyAlgorithm::EncryptWithSM4(unsigned char rvalue[16],const unsigned char plaintext[16],const unsigned char key[16])
{
    if(key==nullptr || plaintext==nullptr || rvalue==nullptr  ){
        return ;
    }

    unsigned char cdata[17] = {0};
    unsigned char ckey[17]  = {0};

    memcpy(ckey, key, 16);
    memcpy(cdata, plaintext, 16);

    sm4_context  sm4Text;
    sm4_setkey_enc(&sm4Text,ckey);
    sm4_crypt_ecb(&sm4Text, SM4_ENCRYPT, 16, cdata, rvalue);


}
//---------------------------------------------------------------------------
void  TKeyAlgorithm::EncryptWithECB_SM4(unsigned char* rvalue,unsigned& rlen,const unsigned char* data,const unsigned dlen,const unsigned char key[16])
{
    for(rlen=0;rlen<dlen;rlen+=16)
    {
        EncryptWithSM4(rvalue+rlen,data+rlen,key);
    }

}
//---------------------------------------------------------------------------
void  TKeyAlgorithm::EncryptWithCBC_SM4(unsigned char* rvalue,unsigned& rlen,const unsigned char* data,const unsigned dlen, const unsigned char atc[16],const unsigned char key[16])
{
    unsigned char cdata[17]={0};
    unsigned char cbuf[17]={0};
    memcpy(cbuf,atc,16);             //��ʼ����

    for(rlen=0;rlen<dlen;rlen+=16)    //16�ֽڷ���
    {
        memcpy(cdata,data+rlen,16);
        for(int i=0;i<16;++i)
            cdata[i] ^= cbuf[i];      //���
        EncryptWithSM4(cbuf,cdata,key);
        memcpy(rvalue+rlen,cbuf,16);
    }
}
//---------------------------------------------------------------------------
// SM4 ��������  end
//==================================================================================================
//==================================================================================================
// SM4 ��������

QByteArray TKeyAlgorithm::DecryptData_SM4(const QByteArray data, const QByteArray key)
{
    unsigned char mackey[17] = {0};
	QByteArray tmp;
    unsigned char buf[1024*2] = {0};
    string bin;
    int length, cnt;

    tmp = data;
	length = tmp.length();
    if(length < 1){
        return "";
    }

	HexToBin(tmp.data(), buf, length>>1);
    for(int i = 0; i < length/2; i++){
        bin.push_back(buf[i]);
    }

    // ��0ȥ���ַ�������Ϊ8�ı�������
	cnt = bin.size()%16;
	if(cnt != 0)
		bin.insert(bin.end(), 16 - cnt, 0x00);

    memset(buf, 0, sizeof(buf));
	length = key.length();
    if(length > 1){
		HexToBin(key.data(), buf,length>>1);
        memcpy(mackey, buf, length/2);
    }
    unsigned char* rvalue = new unsigned char[bin.size()+1];
    unsigned char* pbuf = new unsigned char[(bin.size()<<1)+1];
    _arr_man arr_man(rvalue);
    _arr_man arr_man1(pbuf);
     memset(pbuf,0,(bin.size()<<1)+1);
    int rlen=0;


	DecryptData_SM4(rvalue,rlen,(unsigned char*)bin.data(), bin.size(),mackey);    //����

    BinToHex(rvalue,pbuf,rlen);
    return (char*)pbuf;

}
//---------------------------------------------------------------------------
void TKeyAlgorithm::DecryptData_SM4(unsigned char* rvalue, int& rlen, const unsigned char* ciphertext, int textlen,const unsigned char key[16])
{

    if(key==nullptr || ciphertext==nullptr || textlen<=0){
        return ;
    }

    if(rvalue==nullptr ){
        return ;
    }

    int ilen = textlen;
    unsigned char* pbuf=new unsigned char[ilen+1];
    _arr_man arr_man(pbuf);
    memset(pbuf,0,ilen+1);
    memcpy(pbuf,ciphertext,ilen);

    //��鲹ȫΪ16�ı���  0x00 00 ...
    int cnt = ilen%16;
    for(int i=0;i<cnt;++i){
        pbuf[ilen++] = 0x00;
    }

    sm4_context  sm4Text;
    sm4_setkey_dec(&sm4Text,(unsigned char*)key);
    sm4_crypt_ecb(&sm4Text, SM4_DECRYPT, textlen, (unsigned char*)ciphertext, (unsigned char*)rvalue);

    rlen = ilen;

}
//---------------------------------------------------------------------------
QByteArray  TKeyAlgorithm::DecryptWithSM4(QByteArray data, QByteArray key)
{
    unsigned char mackey[17] = {0};
    unsigned char buf[1024*2] = {0};
    int length;
	QByteArray rvalue;

	length = data.length();
    if(length < 1){
        return "";
    }

	length = key.length();
    if(length > 1){
		HexToBin(key.data(), buf,length>>1);
        memcpy(mackey, buf, length/2);
    }


	for(int i = 0; i < data.length(); i += 32)
    {
        unsigned char rbuf[17]={0};
        char rHex[33]={0};
        memset(buf, 0, sizeof(buf));
		HexToBin(data.data() + i, buf, 16);
        DecryptWithSM4(rbuf,(unsigned char*)buf,mackey);          //SM4 �������
        BinToHex(rbuf,rHex,16);
        rvalue += rHex;
    }

    return rvalue;

}
//---------------------------------------------------------------------------
void  TKeyAlgorithm::DecryptWithSM4(unsigned char* rvalue, const unsigned char* data,const unsigned char* key)
{
    if(key==nullptr || data==nullptr || rvalue==nullptr ){
        return ;
    }

    int ilen = 16;
    unsigned char* pbuf = new unsigned char[ilen+1];
    _arr_man arr_man(pbuf);

    memset(pbuf,0,ilen+1);
    memcpy(pbuf,data,ilen);


    sm4_context  sm4Text;
    sm4_setkey_dec(&sm4Text,(unsigned char*)key);
    sm4_crypt_ecb(&sm4Text, SM4_DECRYPT, ilen, pbuf, (unsigned char*)rvalue);

}
//---------------------------------------------------------------------------
void  TKeyAlgorithm::DecryptWithECB_SM4(unsigned char* rvalue,unsigned& rlen,const unsigned char* data,const unsigned dlen,const unsigned char key[16])
{
    for(rlen=0;rlen<dlen;rlen+=16)
    {
        DecryptWithSM4(rvalue+rlen,data+rlen,key);
    }
}
//---------------------------------------------------------------------------
void  TKeyAlgorithm::DecryptWithCBC_SM4(unsigned char* rvalue,unsigned& rlen,const unsigned char* data,const unsigned dlen, const unsigned char atc[16],const unsigned char key[16])
{
    unsigned char x[17]={0};
    unsigned char h[17]={0};
    unsigned char d[17]={0};
    memcpy(d,atc,16);

    for(rlen=0;rlen<dlen;rlen+=16)
    {
        memcpy(x,data+rlen,16);
        DecryptWithSM4(h,x,key);            //���ݽ���
        for(int i=0;i<16;++i)
            h[i]^=d[i];
        memcpy(d,x,16);                     //�ϴν���ǰ������
        memcpy(rvalue+rlen,h,16);
    }

}
//  SM4 �������� end
//==================================================================================================
//==================================================================================================
// SM4 MAC����

//---------------------------------------------------------------------------
QByteArray TKeyAlgorithm::CalcMac_SM4(const QByteArray data, const QByteArray random, const QByteArray key)
{
	QByteArray lkey;
    char buf[1024*2] = {0};
    unsigned char atc[17] = {0}, mackey[17] = {0};
    int length;

    lkey = key;
    if(lkey == ""){
        return "";
    }
	length = data.length();
    if(length < 1){
        return "";
    }
	HexToBin(data.data(),buf,length>>1);
    string bin(buf,length>>1);

	length = random.length();
    if(length < 1){
        return "";
    }
	HexToBin(random.data(),buf,length>>1);
    memcpy(atc, buf, length>>1);

	length = lkey.length();
    if(length > 1){
		HexToBin(lkey.data(),buf,length>>1);
        memcpy(mackey, buf, length>>1);
    }
    unsigned char mac[4]={0};
    char MAC[9]={0};

	CalcMac_SM4(mac,(const unsigned char*)bin.data(), bin.length(), atc, mackey);

    BinToHex(mac,MAC,4);
	return QByteArray(MAC);

}
void TKeyAlgorithm::CalcMac_SM4(unsigned char mac[4], const unsigned char* bin, int len, const unsigned char atc[16], const unsigned char key[16])
{
    if(key==nullptr || atc==nullptr || bin==nullptr || len<=0){
        return ;
    }
    if(mac==nullptr ){
        return ;
    }

    int  ilen = len;

    unsigned char* pBuf  =  new unsigned char[ilen+32];
    _arr_man arr_man(pBuf);
    memset(pBuf, 0, ilen+32);
    memcpy(pBuf, bin, ilen);

    //������
    pBuf[ilen++]=0x80;
    if(ilen%16!=0){
        int cnt = 16 - (ilen%16);
        for(int i=0;i<cnt;++i)
            pBuf[ilen++] = 0x00;
    }

    unsigned char cTemp[16] = {0};
    unsigned char cData[16] = {0};
                                    
    memcpy(cTemp, atc, 16);         //��ʼֵ

    int i;
    int j;
    for( i=0; i< ilen; i+=16)
    {
        memset(cData, 0, sizeof(cData));
        memcpy(cData, pBuf+i, 16);

        for(j=0; j<sizeof(cTemp); j++)
            cData[j] ^= cTemp[j];           // ���

        memset(cTemp, 0, sizeof(cTemp));
        EncryptWithSM4(cTemp,cData,key);

    }

    memcpy(mac, cTemp, 4);  //ǰ4���ֽ�ΪMAC



}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
QByteArray  TKeyAlgorithm::DiversifyKey_SM4(QByteArray data, QByteArray key,int divCnt)
{
	QByteArray divKey = key;
	QByteArray coefficient;
    for (int i=0;i<divCnt*16 ;i+=16){

		coefficient = data.mid(i,16+i);
        divKey =  GetSubkey_SM4(coefficient, divKey);
    }

    return divKey;
}

QByteArray  TKeyAlgorithm::GetSubkey_SM4(QByteArray data, QByteArray key)
{
	QByteArray rvalue;
    char buf[256] = {0}, src[256] = {0};
    unsigned char ckey[17] = {0};
    char coefficient[17]={0};


	if(data.length()==0){  //����ɢ
        return key;
    }
	if(key.length() < 1){
        return "";
    }

	HexToBin(key.data(),buf,key.length()>>1);
    memcpy(ckey, buf, 16);

	HexToBin(data.data(),src,data.length()>>1);
    memcpy(coefficient,src,8);

    TKeyAlgorithm::Invert(buf, src, 8);
    memcpy(coefficient+8,buf,8);

    memset(buf,0,sizeof(buf));
    BinToHex(coefficient,buf,16);

	rvalue =  QByteArray(buf);

    unsigned char rbuf[17]={0};
    char rhex[33] = {0};
    TKeyAlgorithm::EncryptWithSM4(rbuf,(unsigned char*)coefficient, ckey);

    BinToHex(rbuf,rhex,16);
	rvalue =  QByteArray(rhex);

    return rvalue;
}
//---------------------------------------------------------------------------
void  TKeyAlgorithm::GetSubkey_SM4(unsigned char* rvalue, int& rlen, unsigned char* data, int dlen, unsigned char* key)
{
    unsigned char buf[256] = {0};
    char coefficient[17]={0};
    unsigned char ckey[16] = {0};

    memcpy(ckey,key,16);


    if(dlen ==0){  //����ɢ
        rlen=16;
        memcpy(rvalue,key,16);
        return ;
    }
    for(int i=0;i<dlen;i+=8)
    {
        memset(buf,0,8);
        memset(coefficient,0,17);

        memcpy(coefficient,data+i,8);
        TKeyAlgorithm::Invert(buf, data+i,8);
        memcpy(coefficient+8,buf,8);

        memset(buf,0,16);
        TKeyAlgorithm::EncryptWithSM4(buf,(unsigned char*)coefficient, ckey);    //SM4����
        memcpy(rvalue,buf,16);
//        memcpy(ckey,buf,16);

        rlen = 16;
    }

}
//---------------------------------------------------------------------------
QByteArray  TKeyAlgorithm::GetSeskey_SM4(QByteArray data, QByteArray key)
{
    return GetSubkey_SM4( data, key);
}



QByteArray TKeyAlgorithm::CalcTradeMac(const QByteArray data, const QByteArray random, const QByteArray sessionKeySeed,const QByteArray key,int iAlg)
{
   //1.���������Կ  2.ʹ�ù�����Կ����MAC

   QByteArray sSesKey;

   if (iAlg==4){
       sSesKey = GetSeskey_SM4(sessionKeySeed,key);   //������ԿΪ˫����
       return CalcMac_SM4(data, random, sSesKey);

   }
   else if(iAlg==0){
       sSesKey = EncryptWithDes(sessionKeySeed,key);    //������ԿΪ������
       return CalcMac( data, random, sSesKey,false);
   }
   else
    return "";

}


QByteArray TKeyAlgorithm::CalcTradeTac(const QByteArray data, const QByteArray random,const QByteArray key,int iAlg)
{
    //����3DES�㷨��1.��Կǰ��8�ֽ����2.ʹ������ĵ�������Կ����TAC
    //����SM4�㷨��ֱ��ʹ����Կ����TAC

   if (iAlg==4){
       return CalcMac_SM4(data, random, key);

   }
   else if(iAlg==0){

       char SubKey_bin[24]={0},SubKey_hex[24]={0};
	   HexToBin((char*)key.data(),SubKey_bin,key.length()/2);

       for(int i=0;i<8;++i){
              SubKey_bin[i]^=SubKey_bin[8+i];
              SubKey_bin[8+i] = 0x00;
       }
       BinToHex(SubKey_bin,SubKey_hex,8);

	   return CalcMac( data, random, QByteArray(SubKey_hex),false);     //��ԿΪ������
   }
   else
    return "";
    
}









//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//====================================================================================================
//====================================================================================================
//===================================================================================================

#ifdef USER_SM2
void __fastcall TKeyAlgorithm::GenerateKey_SM2(QByteArray& pv_key, QByteArray& pb_key)
{
    char cpv_key[32*2+1]  = {0};
    char cpb_key_x[32*2+1]= {0};
    char cpb_key_y[32*2+1]= {0};

    mysm2_Geenerate_key(cpv_key, cpb_key_x, cpb_key_y);

	pv_key = QByteArray(cpv_key);

	pb_key = QByteArray(cpb_key_x) +  QByteArray(cpb_key_y);
}
//---------------------------------------------------------------------------
int __fastcall TKeyAlgorithm::Sign_SM2(QByteArray msg, QByteArray pv_key,QByteArray pb_key,int sig_encoding,
							   QByteArray& sign,QByteArray& digest)
{
	unsigned char* pbuf = new unsigned char[msg.length()/2+1];
    _arr_man arr_man(pbuf);
	 memset(pbuf,0,msg.length()/2+1);

    char cpv_key[32+1]    = {0};     //˽Կ���ȣ�32�ֽڣ�
    char cpb_key[64+1+1]  = {0};   //��Կ���ȣ�64�ֽڣ�

    pb_key = "04" + pb_key;         //��Կ "04||X||Y"

	HexToBin(msg.data(),   pbuf,msg.length()/2);
	HexToBin(pv_key.data(),cpv_key,pv_key.length()/2);
	HexToBin(pb_key.data(),cpb_key,pb_key.length()/2);

    char data[1024]   ={0};
    char csign[1024]  ={0};
    char cdigest[1024]={0};

    int  signLen   =0;
    int  digestLen =0;

    int iRtn;
	iRtn = mysm2_Sign(pbuf,msg.length()/2, cpv_key,cpb_key,sig_encoding,csign,&signLen,cdigest,&digestLen);
    if(iRtn == 0)
    {
        BinToHex(csign,data,signLen);
		sign =  QByteArray(data);

        memset(data,0,1024);
        BinToHex(cdigest,data,digestLen);
		digest =  QByteArray(data);
    }

    return iRtn;


}
//---------------------------------------------------------------------------
int __fastcall TKeyAlgorithm::Verify_SM2(QByteArray msg, QByteArray pb_key,QByteArray sign,int sig_encoding)
{

	unsigned char* pMsg = new unsigned char[msg.length()/2+1];
	unsigned char* pSig = new unsigned char[sign.length()/2+1];

    _arr_man arr_man1(pMsg);
    _arr_man arr_man2(pSig);

	memset(pMsg,0,msg.length()/2+1);
	memset(pSig,0,sign.length()/2+1);

    char cpb_key[64+1+1]    = {0};     //��Կ����64�ֽ�

    pb_key = "04" + pb_key;           //��Կ "04||X||Y"

	HexToBin(msg.data(),   pMsg,msg.length()/2);
	HexToBin(sign.data(),   pSig,sign.length()/2);
	HexToBin(pb_key.data(),cpb_key,pb_key.length()/2);

	int iRtn = mysm2_Verify(pMsg ,msg.length()/2, cpb_key, pb_key.length()/2, pSig, sign.length()/2, sig_encoding);

    return iRtn ;

}
//---------------------------------------------------------------------------
int __fastcall TKeyAlgorithm::EncryptData_SM2(QByteArray plaintext, QByteArray pb_key,  QByteArray& ciphertext)
{
	const int plainLen =  plaintext.length()/2;
    unsigned char* pPlain  = new unsigned char[plainLen+1];
    unsigned char* pCipher = new unsigned char[plainLen+96+1];   //���ļ��ܽ����ԭ���Ȼ��96byte��
    int CipherLen =0;

    _arr_man arr_man1(pPlain);
    _arr_man arr_man2(pCipher);

    memset(pPlain, 0,plainLen+1);
    memset(pCipher,0,plainLen+96+1);

    char cpb_key[64+1+1]    = {0};     //��Կ����64�ֽ�

    pb_key = "04" + pb_key;           //��Կ "04||X||Y"

	HexToBin(plaintext.data(),   pPlain,plaintext.length()/2);
	HexToBin(pb_key.data(),cpb_key,pb_key.length()/2);

	int iRtn = mysm2_Encrypt(cpb_key, pPlain, plaintext.length()/2, pCipher, &CipherLen);
    if (iRtn == 0)
    {
        unsigned char *pData = new unsigned char[ 2*(plainLen+96)+1 ];
        _arr_man arr_man3(pData);
        memset(pData, 0, 2*(plainLen+96)+1);

        BinToHex(pCipher,pData,CipherLen);

		ciphertext =  QByteArray((char*)pData, 2*CipherLen);

    }

    return iRtn;

}
//---------------------------------------------------------------------------
int __fastcall  TKeyAlgorithm::DecryptData_SM2(QByteArray ciphertext, QByteArray pv_key, QByteArray& plaintext)
{

	const int cipherLen =  ciphertext.length()/2;
    if(cipherLen < 96) return -1;

    unsigned char* pCipher = new unsigned char[cipherLen+1];   //���ļ��ܽ����ԭ���Ȼ��96byte��
    unsigned char* pPlain  = new unsigned char[cipherLen-96+1];

    int PlainLen =0;

    _arr_man arr_man1(pPlain);
    _arr_man arr_man2(pCipher);

    memset(pPlain, 0,cipherLen-96+1);
    memset(pCipher,0,cipherLen+1);

    char cpv_key[32+1]    = {0};

	HexToBin(ciphertext.data(),   pCipher,ciphertext.length()/2);
	HexToBin(pv_key.data(),cpv_key,pv_key.length()/2);

	int iRtn = mysm2_Decrypt(cpv_key, pCipher, ciphertext.length()/2, pPlain, &PlainLen);
    if (iRtn == 0)
    {
        unsigned char *pData = new unsigned char[ 2*(cipherLen-96)+1 ];
        _arr_man arr_man3(pData);
        memset(pData, 0, 2*(cipherLen-96)+1);

        BinToHex(pPlain,pData,PlainLen);

		plaintext =  QByteArray((char*)pData, 2*PlainLen);

    }

    return iRtn;



}

#endif

