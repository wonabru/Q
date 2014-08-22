// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The bitcoin Developers
// Copyright (c) 2014      wonabru
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


//
#ifndef QCOIN_BASE64_H
#define QCOIN_BASE64_H

#include <string>
#include <vector>

#include "bignum.h"
#include "key.h"
#include "script.h"
#include "allocators.h"

// Decode a base58-encoded string str into byte vector vchRet
// returns true if decoding is successful
inline bool DecodeBase64(const std::string& str, std::vector<unsigned char>& vchRet)
{
    return DecodeBase64(str.c_str(), vchRet);
}

inline std::string Encode64(const std::vector<unsigned char>& vchIn)
{
    std::string vch = "";
    char c = '0';
    for(int i=0;i<(int)vchIn.size();i++)
    {
        c = vchIn[i++];
        vch += c;
    }
    return EncodeBase64(vch);
}


// Encode a byte vector to a base58-encoded string, including checksum
inline std::string EncodeBase64Check(const std::vector<unsigned char>& vchIn)
{
    // add 4-byte hash check to the end
    std::vector<unsigned char> vch(vchIn);
    uint256 hash = Hash(vch.begin(), vch.end());
    vch.insert(vch.end(), (unsigned char*)&hash, (unsigned char*)&hash + 4);
    return Encode64(vch);
}

inline bool DecodeBase64(const char* psz, std::vector<unsigned char>& vchRet)
{
    std::string vch = "";
    char c = '0';
    while(c != '\0')
    {
        c = *psz;
        psz++;
        vch += c;
    }
    std::string ret = DecodeBase64(vch);
    if (memcmp(&ret, &vchRet.end()[-4], 4) != 0)
    {
        vchRet.clear();
        return false;
    }
    return true;
}

// Decode a base58-encoded string psz that includes a checksum, into byte vector vchRet
// returns true if decoding is successful
inline bool DecodeBase64Check(const char* psz, std::vector<unsigned char>& vchRet)
{
    if (!DecodeBase64(psz, vchRet))
        return false;
    if (vchRet.size() < 4)
    {
        vchRet.clear();
        return false;
    }
    uint256 hash = Hash(vchRet.begin(), vchRet.end()-4);
    if (memcmp(&hash, &vchRet.end()[-4], 4) != 0)
    {
        vchRet.clear();
        return false;
    }
    vchRet.resize(vchRet.size()-4);
    return true;
}

// Decode a base58-encoded string str that includes a checksum, into byte vector vchRet
// returns true if decoding is successful
inline bool DecodeBase64Check(const std::string& str, std::vector<unsigned char>& vchRet)
{
    return DecodeBase64Check(str.c_str(), vchRet);
}





/** Base class for all base64-encoded data */
class CBase64Data
{
protected:
    // the version byte
    unsigned char nVersion;

    // the actually encoded data
    typedef std::vector<unsigned char, zero_after_free_allocator<unsigned char> > vector_uchar;
    vector_uchar vchData;

    CBase64Data()
    {
        nVersion = 1;
        vchData.clear();
    }

    void SetData(int nVersionIn, const void* pdata, size_t nSize)
    {
        nVersion = nVersionIn;
        vchData.resize(nSize);
        if (!vchData.empty())
            memcpy(&vchData[0], pdata, nSize);
    }

    void SetData(int nVersionIn, const unsigned char *pbegin, const unsigned char *pend)
    {
        SetData(nVersionIn, (void*)pbegin, pend - pbegin);
    }

public:
    bool SetString(const char* psz)
    {
        std::vector<unsigned char> vchTemp;
        DecodeBase64Check(psz, vchTemp);
        if (vchTemp.empty())
        {
            vchData.clear();
            nVersion = 1;
            return false;
        }
        nVersion = vchTemp[0];
        vchData.resize(vchTemp.size() - 1);
        if (!vchData.empty())
            memcpy(&vchData[0], &vchTemp[1], vchData.size());
        OPENSSL_cleanse(&vchTemp[0], vchData.size());
        return true;
    }

    bool SetString(const std::string& str)
    {
        return SetString(str.c_str());
    }

    std::string ToString() const
    {
        std::vector<unsigned char> vch(1, nVersion);
        vch.insert(vch.end(), vchData.begin(), vchData.end());
        return EncodeBase64Check(vch);
    }

    int CompareTo(const CBase64Data& b64) const
    {
        if (nVersion < b64.nVersion) return -1;
        if (nVersion > b64.nVersion) return 1;
        if (vchData < b64.vchData) return -1;
        if (vchData > b64.vchData) return 1;
        return 0;
    }

    bool operator==(const CBase64Data& b64) const { return CompareTo(b64) == 0; }
    bool operator<=(const CBase64Data& b64) const { return CompareTo(b64) <= 0; }
    bool operator>=(const CBase64Data& b64) const { return CompareTo(b64) >= 0; }
    bool operator< (const CBase64Data& b64) const { return CompareTo(b64) < 0; }
    bool operator> (const CBase64Data& b64) const { return CompareTo(b64) > 0; }
};

/** base64-encoded Qcoin addresses.
* Public-key-hash-addresses have version 0 (or 111 testnet).
* The data vector contains RIPEMD160(SHA256(pubkey)), where pubkey is the serialized public key.
* Script-hash-addresses have version 5 (or 196 testnet).
* The data vector contains RIPEMD160(SHA256(cscript)), where cscript is the serialized redemption script.
*/

#endif // BITCOIN_BASE58_H
