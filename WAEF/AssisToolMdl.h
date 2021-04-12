#pragma once

enum assis_error
{
	assis_error_success,
	assis_error_fail
};

class CAssisToolMdl
{
public:
	CAssisToolMdl(void);
	~CAssisToolMdl(void);
	assis_error ConvertStringToHex(CString _Src, CString& _Dst, bool toUpper = false);
	assis_error ConvertHexToString(CString _Src, CString& _Dst);
	assis_error ConvertStringToAscii(CString _Src, CString& _Dst);
	assis_error ConvertAsciiToString(CString _Src, CString& _Dst, char split=',');
	assis_error ConvertBinaryToHex(CString _Src, CString& _Dst);
	assis_error ConvertHexToBinary(CString _Src, CString& _Dst);
	assis_error Base64Encode(CString _Src, CString& _Dst);
	assis_error Base64Decode(CString _Src, CString& _Dst);
	assis_error UrlEncode(CString _Src, CString& _Dst, bool specialOnly = true, bool toUpper = false);
	assis_error UrlDecode(CString _Src, CString& _Dst);
	assis_error Escape(CString _Src, CString& _Dst);
	assis_error Unescape(CString _Src, CString& _Dst);
	assis_error Hash(CString _Src, CString& _Dst);

private:
	const static char _HexUpperStr[];
	const static char _HexLowerStr[];
};

