#include "stdafx.h"
#include "AssisToolMdl.h"

#include <string>       // std::string
#include <vector>       // std::vector
#include <bcrypt.h>     // Cryptography API
#pragma comment(lib, "bcrypt.lib")

const char CAssisToolMdl::_HexUpperStr[] = "0123456789ABCDEF";
const char CAssisToolMdl::_HexLowerStr[] = "0123456789abcdef";

CAssisToolMdl::CAssisToolMdl(void)
{
}


CAssisToolMdl::~CAssisToolMdl(void)
{
}


assis_error CAssisToolMdl::ConvertStringToHex(CString _Src, CString& _Dst, bool toUpper)
{
	int len = _Src.GetLength();
	int i;
	unsigned char c;
	_Dst = "0x";
	if(toUpper)
	{
		for(i=0;i<len;i++)
		{
			c = _Src.GetAt(i);
			_Dst += _HexUpperStr[c / 16];
			_Dst += _HexUpperStr[c % 16];
		}
	}
	else
	{
		for(i=0;i<len;i++)
		{
			c = _Src.GetAt(i);
			_Dst += _HexLowerStr[c / 16];
			_Dst += _HexLowerStr[c % 16];
		}
	}
	return assis_error_success;
}

assis_error CAssisToolMdl::ConvertHexToString(CString _Src, CString& _Dst)
{
	if(_Src.Mid(0, 2) == "0x")
	{
		_Src.Delete(0, 2);
	}
	int len = _Src.GetLength();
	int i;
	char x;
	int t;
	_Dst.Empty();
	for(i=0;i<len;)
	{
		int m,n;
		t = _Src.GetAt(i++);
		if(t<=0x39 && t>=0x30)
		{
			m = t-0x30;
		}
		else if(t<=0x66 && t>=0x61)
		{
			m = t-0x57;
		}
		else if(t<=0x46 && t>=0x41)
		{
			m = t-0x37;
		}
		else
			continue;

		t = _Src.GetAt(i++);
		if(t<=0x39 && t>=0x30)
		{
			n = t-0x30;
		}
		else if(t<=0x66 && t>=0x61)
		{
			n = t-0x57;
		}
		else if(t<=0x46 && t>=0x41)
		{
			n = t-0x37;
		}
		else
			continue;

		x = m * 16;
		x += n;

		if(x)
			_Dst += x;
	}
	return assis_error_success;
}

assis_error CAssisToolMdl::ConvertStringToAscii(CString _Src, CString& _Dst)
{
	int len = _Src.GetLength();
	int i;
	int c;
	char buffer[6];
	_Dst.Empty();
	for(i=0;i<len-1;i++)
	{
		c = _Src.GetAt(i);
		sprintf_s(buffer, 5, "%d,", c);
		_Dst += buffer;
	}
	c = _Src.GetAt(i);
	sprintf_s(buffer, 5, "%d", c);
	_Dst += buffer;
	return assis_error_success;
}

assis_error CAssisToolMdl::ConvertAsciiToString(CString _Src, CString& _Dst, char split)
{
	int pos1=0, pos2 = _Src.Find(split);
	char x;
	_Dst.Empty();
	while(pos2 != -1)
	{
		x = atoi(_Src.Mid(pos1, pos2-pos1));
		_Dst += x;
		pos1=pos2+1;
		pos2 = _Src.Find(split, pos2+1);
	}
	x = atoi(_Src.Mid(pos1));
	_Dst += x;
	return assis_error_success;
}

assis_error CAssisToolMdl::ConvertBinaryToHex(CString _Src, CString& _Dst)
{
	int len = _Src.GetLength() / 4;
	int i;
	_Dst = "0x";
	int j;
	for(i=0;i<len;i++)
	{
		int m=1;
		unsigned char c = 0;
		for(j=3;j>=0;j--)
		{
			c += (_Src.GetAt(4*i+j) - 0x30) * m;
			m*=2;
		}
		_Dst += _HexUpperStr[c];
	}
	return assis_error_success;
}

assis_error CAssisToolMdl::ConvertHexToBinary(CString _Src, CString& _Dst)
{
	if(_Src.Mid(0, 2) == "0x")
	{
		_Src.Delete(0, 2);
	}
	int len = _Src.GetLength();
	int i,j;
	char c;
	_Dst.Empty();
	for(i=0;i<len;i++)
	{
		c = _Src.GetAt(i);
		if(c<=0x39 && c>=0x30)
		{
			c -= 0x30;
		}
		else if(c<=0x66 && c>=0x61)
		{
			c -= 0x57;
		}
		else if(c<=0x46 && c>=0x41)
		{
			c -= 0x37;
		}
		else
			break;

		char x = 0x08;
		for(j=0;j<4;j++)
		{
			if(c & x)
				_Dst += "1";
			else
				_Dst += "0";
			x>>=1;
		}
		
		if(i%2)
			_Dst += " ";

	}
	return assis_error_success;
}

assis_error CAssisToolMdl::Base64Encode(CString _Src, CString& _Dst)
{
	// 由于Base64一般是对以UTF8编码的字符进行转码，所以先要进行转换

	// 以下对编码的处理方法是通过对Windows记事本程序逆向得到的方案，所以绝对官方，O(∩_∩)O
	// 由于Windows记事本是Unicode编码属性的，所以直接就从Unicode转换成UTF-8了，没有下面第一步转换。

	// 1. 由于本工程属性没有设置成宽字符集，所以CString、string等都是GBK编码的(即都是char类型)，需要先转换成Unicode编码
	// 如果是在工程Unicode属性的工程里，CString、string都是wchar_t类型，就不用这一步的转换了，直接按下面的转成UTF-8即可
	int nb = MultiByteToWideChar(CP_ACP, 0, _Src.GetBuffer(), -1, NULL, 0);
	WCHAR* wBuf = new WCHAR[nb];
	MultiByteToWideChar(CP_ACP, 0, _Src.GetBuffer(), -1, wBuf, nb);

	// 这里将Unicode转成UTF-8
	int nb2 = WideCharToMultiByte(CP_UTF8, 0, wBuf, wcslen(wBuf), NULL, 0, NULL, NULL);
	CHAR* aBuf= new CHAR[nb2];
	memset(aBuf, 0, nb2);
	WideCharToMultiByte(CP_UTF8, 0, wBuf, wcslen(wBuf), aBuf, nb2, NULL, NULL);
	delete [] wBuf;

	// 以下Base64编码以及后面的解码函数是根据某JavaScript版本的函数实现的，应该是没什么问题
	char base64Str[66]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
	int len = nb2;

	
	_Dst.Empty();
	unsigned char chr1, chr2, chr3, enc1, enc2, enc3, enc4;
	int i = 0;

	//input = Base64._utf8_encode(input);

	

	while (i < len / 3) {

		chr1 = aBuf[3*i];
		chr2 = aBuf[3*i+1];
		chr3 = aBuf[3*i+2];

		enc1 = chr1 >> 2;
		enc2 = ((chr1 & 3) << 4) | (chr2 >> 4);
		enc3 = ((chr2 & 15) << 2) | (chr3 >> 6);
		enc4 = chr3 & 63;

		_Dst = _Dst + base64Str[enc1] + base64Str[enc2] + base64Str[enc3] + base64Str[enc4];
		i++;
	}

	if(3 * i < len)
	{
		chr1 = aBuf[3 * i];
		enc1 = chr1 >> 2;
		if(3 * i + 1 < len)
		{
			chr2 = aBuf[ 3 * i + 1];
			enc2 = ((chr1 & 3) << 4) | (chr2 >> 4);
			if(3 * i + 2 < len)
			{
				chr3 = aBuf[3 * i + 2];
				enc3 = ((chr2 & 15) << 2) | (chr3 >> 6);
				enc4 = chr3 & 63;
			}
			else
			{
				chr3 = 0;
				enc3 = ((chr2 & 15) << 2) | (chr3 >> 6);
				enc4 = 64;
			}
		}
		else
		{
			chr2 = 0;
			enc2 = ((chr1 & 3) << 4) | (chr2 >> 4);
			enc3 = enc4 = 64;
		}
		_Dst = _Dst + base64Str[enc1] + base64Str[enc2] + base64Str[enc3] + base64Str[enc4];
	}
	delete [] aBuf;
	return assis_error_success;

}

assis_error CAssisToolMdl::Base64Decode(CString _Src, CString& _Dst)
{
	char base64Str[66]= "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
	int len = _Src.GetLength();
	int i=0;
	_Dst.Empty();
	while(i+2<len)
	{
		unsigned char enc1= (char*)memchr(base64Str, _Src.GetAt(i++), 65) - base64Str;
		unsigned char enc2= (char*)memchr(base64Str, _Src.GetAt(i++), 65) - base64Str;
		unsigned char enc3= (char*)memchr(base64Str, _Src.GetAt(i++), 65) - base64Str;
		unsigned char enc4= (char*)memchr(base64Str, _Src.GetAt(i++), 65) - base64Str;
		char chr1 = (enc1 << 2) | (enc2 >> 4);
		char chr2 = ((enc2 & 15) << 4) | (enc3 >> 2);
		char chr3 = ((enc3 & 3) << 6) | enc4;

		_Dst += chr1;
		if (enc3 != 64) {
			_Dst += chr2;
		}
		if (enc4 != 64) {
			_Dst += chr3;
		}
	}
	// 这里将UTF-8转成Unicode
	int nb = MultiByteToWideChar(CP_UTF8, 0, _Dst.GetBuffer(), -1, NULL, 0);
	WCHAR* wBuf = new WCHAR[nb];
	MultiByteToWideChar(CP_UTF8, 0, _Dst.GetBuffer(), -1, wBuf, nb);

	// 这里将Unicode转成GBK
	int nb2 = WideCharToMultiByte(CP_ACP, 0, wBuf, wcslen(wBuf), NULL, 0, NULL, NULL)+1;
	CHAR* aBuf= new CHAR[nb2];
	memset(aBuf, 0, nb2);
	WideCharToMultiByte(CP_ACP, 0, wBuf, wcslen(wBuf), aBuf, nb2, NULL, NULL);
	_Dst = aBuf;
	delete [] wBuf;
	delete [] aBuf;
	return assis_error_success;
}

assis_error CAssisToolMdl::UrlEncode(CString _Src, CString& _Dst, bool specialOnly, bool toUpper)
{
	int i;
	_Dst.Empty();
	int len = _Src.GetLength();
	unsigned char c;

	if(toUpper)
	{
		for(i=0;i<len;i++)
		{
			c = _Src.GetAt(i);
			if(specialOnly)
			{
				switch(c)
				{
				case '<':
				case '>':
				case ' ':
				case '\'':
					_Dst += "%";
					_Dst += _HexUpperStr[c / 16];
					_Dst += _HexUpperStr[c % 16];
					break;
				default:
					_Dst += c;
					break;
				}
			}
			else
			{
				_Dst += "%";
				_Dst += _HexUpperStr[c / 16];
				_Dst += _HexUpperStr[c % 16];
			}
		}
	}
	else
	{
		for(i=0;i<len;i++)
		{
			c = _Src.GetAt(i);
			if(specialOnly)
			{
				switch(c)
				{
				case '<':
				case '>':
				case ' ':
				case '\'':
					_Dst += "%";
					_Dst += _HexLowerStr[c / 16];
					_Dst += _HexLowerStr[c % 16];
					break;
				default:
					_Dst += c;
					break;
				}
			}
			else
			{
				_Dst += "%";
				_Dst += _HexLowerStr[c / 16];
				_Dst += _HexLowerStr[c % 16];
			}
		}
	}
	return assis_error_success;
}

assis_error CAssisToolMdl::UrlDecode(CString _Src, CString& _Dst)
{
	_Dst.Empty();
	int len = _Src.GetLength();
	int i=0;
	unsigned char m,n;
	unsigned char c, t, q;
	while(i<len)
	{
		c = _Src.GetAt(i);
		if(c=='%')
		{
			t = _Src.GetAt(i+1);
			if(t<=0x39 && t>=0x30)
			{
				m = t-0x30;
			}
			else if(t<=0x66 && t>=0x61)
			{
				m = t-0x57;
			}
			else if(t<=0x46 && t>=0x41)
			{
				m = t-0x37;
			}
			else
			{
				_Dst += c;
				i++;
				continue;
			}
			
			q = _Src.GetAt(i+2);
			if(q<=0x39 && q>=0x30)
			{
				n = q-0x30;
			}
			else if(q<=0x66 && q>=0x61)
			{
				n = q-0x57;
			}
			else if(q<=0x46 && q>=0x41)
			{
				n = q-0x37;
			}
			else
			{
				_Dst += c;
				_Dst += t;
				_Dst += q;
				i+=3;
				continue;
			}

			c = (m << 4) + n;
			i+=2;
		}
		_Dst += c;
		i++;
	}
	return assis_error_success;
}

assis_error CAssisToolMdl::Escape(CString _Src, CString& _Dst)
{
	// Escape实际上就是Unicode编码(大端方式)，所以只需转换成宽字节编码再按格式输出即可
	size_t size = MultiByteToWideChar(CP_ACP, 0, _Src.GetBuffer(), -1, NULL, 0);
	WCHAR* wBuf = new WCHAR[size];
	MultiByteToWideChar(CP_ACP, 0, _Src.GetBuffer(), -1, wBuf, size);
	
	// 以下大端与小端转换是逆向Windows记事本程序得来的
	size_t i=0;
	char* Buf = (char*)wBuf;
	while(i<size)
	{
		BYTE high = Buf[i*2];
		WORD low  = Buf[i*2+1];
		wBuf[i] = low << 8 ^ high;
		i++;
	}

	_Dst.Empty();

	// Upper-case hex digits
	static const char hexDigits[] = "0123456789ABCDEF";

	// For each byte in the hash value buffer
	for (size_t i = 0; i < size; ++i)
	{
		BYTE low  = Buf[i*2];
		BYTE high = Buf[i*2+1];
		if(high)
		{
			_Dst += "%u";
			_Dst += hexDigits[ (high & 0xF0) >> 4 ];
			_Dst += hexDigits[ (high & 0x0F) ];
			_Dst += hexDigits[ (low & 0xF0) >> 4 ];
			_Dst += hexDigits[ (low & 0x0F) ];
		}
		else
		{
			_Dst += low;
		}

	}

	delete[] wBuf;
	return assis_error_success;
}

assis_error CAssisToolMdl::Unescape(CString _Src, CString& _Dst)
{
	_Dst.Empty();
	size_t size = MultiByteToWideChar(CP_ACP, 0, _Src.GetBuffer(), -1, NULL, 0);
	WCHAR* wSrc = new WCHAR[size];
	MultiByteToWideChar(CP_ACP, 0, _Src.GetBuffer(), -1, wSrc, size);

	WCHAR* wDst = new WCHAR[size+1];
	memset(wDst, 0, 2*size+2);

	int i=0, j=0;
	wchar_t p, u, c1, c2, c3, c4, b1, b2, b3, b4;
	while(i<size)
	{
		p = wSrc[i];
		if(p=='%' || p=='\\')
		{
			u = wSrc[i+1];
			if(u=='u')
			{
				c1 = wSrc[i+2];
				if(c1<=0x39 && c1>=0x30)
				{
					b1 = c1-0x30;
				}
				else if(c1<=0x66 && c1>=0x61)
				{
					b1 = c1-0x57;
				}
				else if(c1<=0x46 && c1>=0x41)
				{
					b1 = c1-0x37;
				}
				else
				{
					wDst[j++] = p;
					wDst[j++] = u;
					wDst[j++] = c1;
					i+=3;
					continue;
				}

				c2 = wSrc[i+3];
				if(c2<=0x39 && c2>=0x30)
				{
					b2 = c2-0x30;
				}
				else if(c2<=0x66 && c2>=0x61)
				{
					b2 = c2-0x57;
				}
				else if(c2<=0x46 && c2>=0x41)
				{
					b2 = c2-0x37;
				}
				else
				{
					wDst[j++] = p;
					wDst[j++] = u;
					wDst[j++] = c1;
					wDst[j++] = c2;
					i+=4;
					continue;
				}
				
				c3 = wSrc[i+4];
				if(c3<=0x39 && c3>=0x30)
				{
					b3 = c3-0x30;
				}
				else if(c3<=0x66 && c3>=0x61)
				{
					b3 = c3-0x57;
				}
				else if(c3<=0x46 && c3>=0x41)
				{
					b3 = c3-0x37;
				}
				else
				{
					wDst[j++] = p;
					wDst[j++] = u;
					wDst[j++] = c1;
					wDst[j++] = c2;
					wDst[j++] = c3;
					i+=5;
					continue;
				}
				
				c4 = wSrc[i+5];
				if(c4<=0x39 && c4>=0x30)
				{
					b4 = c4-0x30;
				}
				else if(c4<=0x66 && c4>=0x61)
				{
					b4 = c4-0x57;
				}
				else if(c4<=0x46 && c4>=0x41)
				{
					b4 = c4-0x37;
				}
				else
				{
					wDst[j++] = p;
					wDst[j++] = u;
					wDst[j++] = c1;
					wDst[j++] = c2;
					wDst[j++] = c3;
					wDst[j++] = c4;
					i+=6;
					continue;
				}
				wDst[j++] = (b1 << 12) ^ (b2 << 8) ^ (b3 << 4) ^ b4;
				i+=6;
			}
			else
			{
				wDst[j++] = p;
				wDst[j++] = u;
				i+=2;
			}
		}
		else
		{
			wDst[j++] = p;
			i++;
		}
	}
	wDst[j] = '\0';

	size = WideCharToMultiByte(CP_ACP, 0, wDst, wcslen(wDst), NULL, 0, NULL, NULL);
	CHAR* aDst= new CHAR[++size];
	memset(aDst, 0, size);
	WideCharToMultiByte(CP_ACP, 0, wDst, wcslen(wDst), aDst, size, NULL, NULL);

	_Dst = aDst;
	
	delete [] wSrc;
	delete [] wDst;
	delete [] aDst;

	return assis_error_success;
}

assis_error CAssisToolMdl::Hash(CString _Src, CString& _Dst)
{
	int nb = MultiByteToWideChar(CP_ACP, 0, _Src.GetBuffer(), -1, NULL, 0);
	WCHAR* wBuf = new WCHAR[nb];
	MultiByteToWideChar(CP_ACP, 0, _Src.GetBuffer(), -1, wBuf, nb);

	int nb2 = WideCharToMultiByte(CP_UTF8, 0, wBuf, wcslen(wBuf), NULL, 0, NULL, NULL);
	CHAR* aBuf= new CHAR[nb2];
	memset(aBuf, 0, nb2);
	WideCharToMultiByte(CP_UTF8, 0, wBuf, wcslen(wBuf), aBuf, nb2, NULL, NULL);
	delete [] wBuf;

	LPCWSTR algIdArray[] = { 
		BCRYPT_MD5_ALGORITHM,
		BCRYPT_SHA1_ALGORITHM,
		BCRYPT_SHA256_ALGORITHM,
		BCRYPT_SHA512_ALGORITHM
	};

	size_t size[] = {
		16,				// MD5输出128bit
		20,				// SHA1输出160bit
		32,				// SHA256输出256bit
		64				// SHA512输出512bit
	};

	BCRYPT_ALG_HANDLE alg;
	NTSTATUS result;
	
	DWORD propertyValue;
	DWORD resultSize;

	_Dst.Empty();

	int n=0;
	while(n<sizeof(algIdArray)/4)
	{
		result = ::BCryptOpenAlgorithmProvider(
					&alg,						// algorithm handle
					algIdArray[n],				// hashing algorithm ID
					nullptr,                    // use default provider
					0                           // default flags
					);
	
		if(result < 0)
			return assis_error_fail;
	
	
		result = ::BCryptGetProperty(
				alg, 
				BCRYPT_OBJECT_LENGTH,
				reinterpret_cast<BYTE *>(&propertyValue),
				sizeof(propertyValue),
				&resultSize,
				0);

		std::vector<BYTE> hashObj(propertyValue);
	
		if(result < 0)
			return assis_error_fail;

		BCRYPT_HASH_HANDLE hash;
		result = ::BCryptCreateHash(
				alg,  // handle to parent
				&hash,            // hash object handle
				hashObj.data(),   // hash object buffer pointer
				hashObj.size(),   // hash object buffer length
				nullptr,            // no secret
				0,                  // no secret
				0                   // no flags
				);
     
		if(result < 0)
		{
			return assis_error_fail;
		}

		// 数据多的话以下调用可以放在while内分块循环多次调用
		result = ::BCryptHashData(
				hash, // hash object handle
				(PUCHAR)aBuf,    // safely remove const from buffer pointer
				nb2, // input buffer length in bytes
				0       // no flags
				);

		if(result < 0)
		{
			return assis_error_fail;
		}

		//BYTE hashValue[nBytes]; // SHA-1: 20 bytes = 160 bits
		BYTE* hashValue = new BYTE[size[n]];
		memset(hashValue, 0, size[n]);
		result = ::BCryptFinishHash( 
				hash,             // handle to hash object
				hashValue,          // output buffer for hash value
				size[n],  // size of this buffer
				0                   // no flags
				);

		//
		// Get the hash digest string from hash value buffer.
		//

		// Each byte --> 2 hex chars
		std::string hashDigest;
		hashDigest.resize( size[n] * 2 );

		// Upper-case hex digits
		static const char hexDigits[] = "0123456789abcdef";

		// Index to current character in destination string
		size_t currChar = 0;

		// For each byte in the hash value buffer
		for (size_t i = 0; i < size[n]; ++i)
		{
			// high nibble
			hashDigest[currChar] = hexDigits[ (hashValue[i] & 0xF0) >> 4 ];
			++currChar;

			// low nibble
			hashDigest[currChar] = hexDigits[ (hashValue[i] & 0x0F) ];
			++currChar;
		}

		delete [] hashValue;

		// Releases resources
		::BCryptDestroyHash(hash);
		::BCryptCloseAlgorithmProvider(alg, 0);

		CString out;

		if(n==0)
		{
			out.Format(
				"MD5(32位): %s\r\n"
				"MD5(16位): %s\r\n",
				hashDigest.c_str(),
				hashDigest.substr(8, 16).c_str()
				);
		}
		else
		{
			out.Format(
				"%S: %s\r\n",
				algIdArray[n],
				hashDigest.c_str()
				);
		}

		_Dst+= out;

		n++;
	}

	return assis_error_success;
}