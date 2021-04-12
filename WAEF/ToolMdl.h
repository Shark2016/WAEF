#pragma once

#include <string>
using namespace std;

#include <curl\curl.h>

enum toolmdl_error
{
	toolmdl_error_success,
	toolmdl_error_fail,

	toolmdl_error_notfind,
	toolmdl_error_unknowhost,
	toolmdl_error_notstandardheader,

	toolmdl_error_unknowcms,
};

class CToolMdl
{
public:
	CToolMdl(void);
	~CToolMdl(void);
	long static request_get(const string& url, string& content, string cookie="");
	long static request_post(const string& url, string& post_data, string& content, string cookie="");
	toolmdl_error static request_head(const string& url, string& header);
	long static signature(const string& url, string& signature);
	toolmdl_error static parse_header(const string& header, string& state, string& server, string& powerby);
	toolmdl_error static sub_string(const string& content, const string& start, const string& stop, string& rtn, int& startpos);
	toolmdl_error static resolve_ip(char* host_name, char* ip_address, const size_t size_ip_address);
	string static utf8togb(string source);

private:
	size_t static write_callback( void *ptr, size_t size, size_t nmemb, string &userp);
	size_t static signature_callback( void *ptr, size_t size, size_t nmemb, void *userp);
};

