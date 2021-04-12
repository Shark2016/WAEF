#pragma once

///////////////////////////////////////////////////////////////////////////////
//
// SHA1.cpp : Console App to hash files using SHA-1.
//
//            Version 1.0 -- 2011, October 29th
//
// Copyright 2011, by Giovanni Dicanio <giovanni.dicanio@gmail.com>
//
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>      // C file management

#include <exception>    // std::exception
#include <ios>          // std::hex
#include <iostream>     // console output
#include <sstream>      // std::ostringstream
#include <stdexcept>    // std::runtime_error
#include <string>       // std::wstring
#include <vector>       // std::vector

#include <Windows.h>    // Win32 Platform SDK
#include <bcrypt.h>     // Cryptography API


#pragma comment(lib, "bcrypt.lib")



//-----------------------------------------------------------------------------
// Checks if input NTSTATUS corresponds to a success code.
//-----------------------------------------------------------------------------
inline bool NtSuccess(NTSTATUS status)
{
    return (status >= 0);
}



//-----------------------------------------------------------------------------
// Class defining private copy constructor and operator=, to ban copies.
//-----------------------------------------------------------------------------
class NonCopyable
{
protected:
    NonCopyable() {}
    ~NonCopyable() {}
private:
    NonCopyable( const NonCopyable& );
    const NonCopyable& operator=( const NonCopyable& );
};



//-----------------------------------------------------------------------------
// Error occurred during cryptographic processing.
//-----------------------------------------------------------------------------
class CryptException : 
    public std::runtime_error
{
public:

    // Constructs the exception with an error message and an error code.
    explicit CryptException(const std::string & message, NTSTATUS errorCode)
        : std::runtime_error( FormatErrorMessage(message, errorCode) ),
          m_errorCode(errorCode)
    {}


    // Returns the error code.
    NTSTATUS ErrorCode() const
    {
        return m_errorCode;
    }
     

    //
    // IMPLEMENTATION
    //
private:
    // Error code from Cryptography API
    NTSTATUS m_errorCode;

    // Helper method to format an error message including the error code.
    static std::string FormatErrorMessage(const std::string & message, NTSTATUS errorCode)
    {
        std::ostringstream os;
        os << message << " (NTSTATUS=0x" << std::hex << errorCode << ")";
        return os.str();
    }
};



//-----------------------------------------------------------------------------
// RAII wrapper to crypt algorithm provider
//-----------------------------------------------------------------------------
class CryptAlgorithmProvider : NonCopyable
{
public:

    // Creates a crypt algorithm provider object.
    // This can be used to create one ore more hash objects to hash some data.
    CryptAlgorithmProvider()
    {
        NTSTATUS result = ::BCryptOpenAlgorithmProvider(
            &m_alg,                     // algorithm handle
            BCRYPT_MD5_ALGORITHM,      // hashing algorithm ID
            nullptr,                    // use default provider
            0                           // default flags
            );
        if ( ! NtSuccess(result) )
            throw CryptException("Can't load default cryptographic algorithm provider.", result);
    }
    

    // Releases resources
    ~CryptAlgorithmProvider()
    {
        ::BCryptCloseAlgorithmProvider(m_alg, 0);
    }


    // Gets raw handle
    BCRYPT_ALG_HANDLE Handle() const
    {
        return m_alg;
    }


    // Gets the value of a DWORD named property
    DWORD GetDWordProperty(const std::wstring & propertyName) const
    {
        DWORD propertyValue;
        DWORD resultSize;

        //
        // Get the value of the input named property
        //

        NTSTATUS result = ::BCryptGetProperty(
            Handle(), 
            propertyName.c_str(),
            reinterpret_cast<BYTE *>(&propertyValue),
            sizeof(propertyValue),
            &resultSize,
            0);
        if ( ! NtSuccess(result) )
            throw CryptException("Can't get crypt property value.", result);

        return propertyValue;
    }


    //
    // IMPLEMENTATION
    //
private:
    // Handle to crypt algorithm provider
    BCRYPT_ALG_HANDLE m_alg;
};



//-----------------------------------------------------------------------------
// Crypt Hash object, used to hash data.
//-----------------------------------------------------------------------------
class CryptHashObject : NonCopyable
{
public:

    // Creates a crypt hash object.
    explicit CryptHashObject(const CryptAlgorithmProvider & provider)
        : m_hashObj( provider.GetDWordProperty(BCRYPT_OBJECT_LENGTH) )
    {
        // Create the hash object
        NTSTATUS result = ::BCryptCreateHash(
            provider.Handle(),  // handle to parent
            &m_hash,            // hash object handle
            m_hashObj.data(),   // hash object buffer pointer
            m_hashObj.size(),   // hash object buffer length
            nullptr,            // no secret
            0,                  // no secret
            0                   // no flags
            );
        if ( ! NtSuccess(result) )
            throw CryptException("Can't create crypt hash object.", result);
    }


    // Releases resources
    ~CryptHashObject()
    {
        ::BCryptDestroyHash(m_hash);
    }

    
    // Hashes the data in the input buffer.
    // Can be called one or more times.
    // When finished with input data, call FinishHash().
    // This method can't be called after FinisHash() is called.
    void HashData(const void * data, unsigned long length) const
    {
        // Hash this chunk of data
        NTSTATUS result = ::BCryptHashData(
            m_hash, // hash object handle
            static_cast<UCHAR *>( const_cast<void *>(data) ),    // safely remove const from buffer pointer
            length, // input buffer length in bytes
            0       // no flags
            );
        if (! NtSuccess(result) )
            throw CryptException("Can't hash data.", result);
    }


    // Finalizes hash calculation.
    // After this method is called, hash value can be got using HashValue() method.
    // After this method is called, the HashData() method can't be called anymore.
    void FinishHash()
    {
        //
        // Retrieve the hash of the accumulated data
        //

        BYTE hashValue[16]; // SHA-1: 20 bytes = 160 bits

        NTSTATUS result = ::BCryptFinishHash( 
            m_hash,             // handle to hash object
            hashValue,          // output buffer for hash value
            sizeof(hashValue),  // size of this buffer
            0                   // no flags
            );
        if ( ! NtSuccess(result) )
            throw CryptException("Can't finalize hashing.", result);
         

        //
        // Get the hash digest string from hash value buffer.
        //

        // Each byte --> 2 hex chars
        m_hashDigest.resize( sizeof(hashValue) * 2 );

        // Upper-case hex digits
        static const char hexDigits[] = "0123456789abcdef";

        // Index to current character in destination string
        size_t currChar = 0;

        // For each byte in the hash value buffer
        for (size_t i = 0; i < sizeof(hashValue); ++i)
        {
            // high nibble
            m_hashDigest[currChar] = hexDigits[ (hashValue[i] & 0xF0) >> 4 ];
            ++currChar;

            // low nibble
            m_hashDigest[currChar] = hexDigits[ (hashValue[i] & 0x0F) ];
            ++currChar;
        }
    }


    // Gets the hash value (in hex, upper-case).
    // Call this method *after* FinishHash(), not before.
    // (If called before, an empty string is returned.)
    std::string HashDigest() const
    {
        return m_hashDigest;
    }


    //
    // IMPLEMENTATION
    //
private:

    // Handle to hash object
    BCRYPT_HASH_HANDLE m_hash;

    // Buffer to store hash object
    std::vector<BYTE> m_hashObj;

    // Hash digest string (hex)
    std::string m_hashDigest;
};

/*

//-----------------------------------------------------------------------------
// Wrapper around C FILE *, for reading binary data from file.
//-----------------------------------------------------------------------------
class FileReader : NonCopyable
{
public:

    // Opens the specified file.
    explicit FileReader(const std::wstring & filename)
    {
        if ( _wfopen_s(&m_file, filename.c_str(), L"rb") != 0)
        {
            throw std::runtime_error("Can't open file for reading.");
        }
    }


    // Closes the file.
    ~FileReader()
    {
        if (m_file != nullptr)
            fclose(m_file);
    }


    // End Of File reached?
    bool EoF() const
    {
        return feof(m_file) ? true : false;
    }


    // Reads bytes from file to a memory buffer.
    // Returns the number of bytes actually read.
    int Read(void * buffer, int bufferSize)
    {
        return fread(buffer, 1, bufferSize, m_file);
    }


    //
    // IMPLEMENTATION
    //
private:
    // Raw C file handle
    FILE * m_file;
};



//-----------------------------------------------------------------------------
// Hashes a file with SHA-1.
// Returns the hash digest, in hex.
//-----------------------------------------------------------------------------

std::string HashFileSHA1(const std::wstring & filename)
{
    // Create the algorithm provider for SHA-1 hashing
    CryptAlgorithmProvider sha1;

    // Create the hash object for the particular hashing
    CryptHashObject hasher(sha1);

    // Object to read data from file
    FileReader file(filename);

    // Read buffer
    std::vector<BYTE> buffer(4*1024);   // 4 KB buffer

    // Reading loop
    while ( ! file.EoF() )
    {
        // Read a chunk of data from file to memory buffer
        int readBytes = file.Read(buffer.data(), buffer.size());

        // Hash this chunk of data
        hasher.HashData(buffer.data(), readBytes);
    }

    // Finalize hashing
    hasher.FinishHash();

    // Return hash digest
    return hasher.HashDigest();
}



//-----------------------------------------------------------------------------
// Print some help lines.
//-----------------------------------------------------------------------------
void PrintHelp()
{
    using std::wcout;
    using std::endl;

    wcout << endl;
    wcout << L"*** SHA1 -- Computes SHA-1 hashes of files ***" << endl;
    wcout << L"                 by Giovanni Dicanio          " << endl;
    wcout << L"            <giovanni.dicanio@gmail.com>      " << endl;
    wcout << L"          http://msmvps.com/blogs/gdicanio/   " << endl;
    wcout << endl;
    wcout << L"Specify the file name to hash as command line parameter." << endl;
    wcout << endl;
}



//-----------------------------------------------------------------------------
// Console App Entry-Point.
//-----------------------------------------------------------------------------
int wmain(int argc, wchar_t* argv[])
{
    static const int kExitOk = 0;
    static const int kExitError = 1;
    
    try
    {
        // Check command line arguments
        if (argc != 2)
        {
            PrintHelp();
        }
        else
        {
            // Print file hash
            std::wstring hash = HashFileSHA1(argv[1]);
            std::wcout << L"SHA-1: " << hash << std::endl;
        }
    }
    catch(const std::exception & e)
    {
        std::cerr << "\n*** ERROR: " << e.what() << std::endl;
        return kExitError;
    }

	return kExitOk;
}
*/


///////////////////////////////////////////////////////////////////////////////

class md5
{
public:
	md5(void);
	~md5(void);
};

