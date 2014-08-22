// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Mark developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef BITCOIN_UINT256_H
#define BITCOIN_UINT256_H

#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <string>
#include <vector>
#include <math.h>
#include <stdlib.h>

typedef long long  int64;
typedef unsigned long long  uint64;


inline int Testuint256AdHoc(std::vector<std::string> vArg);



/** Base class without constructors for uint256 and uint160.
 * This makes the compiler let you use it in a union.
 */
template<unsigned int BITS>
class base_uint
{
protected:
    enum { WIDTH=BITS/32 };
    uint32_t pn[WIDTH];
public:

    bool operator!() const
    {
        for (int i = 0; i < WIDTH; i++)
            if (pn[i] != 0)
                return false;
        return true;
    }

    const base_uint operator~() const
    {
        base_uint ret;
        for (int i = 0; i < WIDTH; i++)
            ret.pn[i] = ~pn[i];
        return ret;
    }

    const base_uint operator-() const
    {
        base_uint ret;
        for (int i = 0; i < WIDTH; i++)
            ret.pn[i] = ~pn[i];
        ret++;
        return ret;
    }

    double getdouble() const
    {
        double ret = 0.0;
        double fact = 1.0;
        for (int i = 0; i < WIDTH; i++) {
            ret += fact * pn[i];
            fact *= 4294967296.0;
        }
        return ret;
    }

    base_uint& operator=(uint64 b)
    {
        pn[0] = (unsigned int)b;
        pn[1] = (unsigned int)(b >> 32);
        for (int i = 2; i < WIDTH; i++)
            pn[i] = 0;
        return *this;
    }

    base_uint& operator^=(const base_uint& b)
    {
        for (int i = 0; i < WIDTH; i++)
            pn[i] ^= b.pn[i];
        return *this;
    }

    base_uint& operator&=(const base_uint& b)
    {
        for (int i = 0; i < WIDTH; i++)
            pn[i] &= b.pn[i];
        return *this;
    }

    base_uint& operator|=(const base_uint& b)
    {
        for (int i = 0; i < WIDTH; i++)
            pn[i] |= b.pn[i];
        return *this;
    }

    base_uint& operator^=(uint64 b)
    {
        pn[0] ^= (unsigned int)b;
        pn[1] ^= (unsigned int)(b >> 32);
        return *this;
    }

    base_uint& operator|=(uint64 b)
    {
        pn[0] |= (unsigned int)b;
        pn[1] |= (unsigned int)(b >> 32);
        return *this;
    }

    base_uint& operator<<=(unsigned int shift)
    {
        base_uint a(*this);
        for (int i = 0; i < WIDTH; i++)
            pn[i] = 0;
        int k = shift / 32;
        shift = shift % 32;
        for (int i = 0; i < WIDTH; i++)
        {
            if (i+k+1 < WIDTH && shift != 0)
                pn[i+k+1] |= (a.pn[i] >> (32-shift));
            if (i+k < WIDTH)
                pn[i+k] |= (a.pn[i] << shift);
        }
        return *this;
    }

    base_uint& operator>>=(unsigned int shift)
    {
        base_uint a(*this);
        for (int i = 0; i < WIDTH; i++)
            pn[i] = 0;
        int k = shift / 32;
        shift = shift % 32;
        for (int i = 0; i < WIDTH; i++)
        {
            if (i-k-1 >= 0 && shift != 0)
                pn[i-k-1] |= (a.pn[i] << (32-shift));
            if (i-k >= 0)
                pn[i-k] |= (a.pn[i] >> shift);
        }
        return *this;
    }

    base_uint& operator+=(const base_uint& b)
    {
        uint64 carry = 0;
        for (int i = 0; i < WIDTH; i++)
        {
            uint64 n = carry + pn[i] + b.pn[i];
            pn[i] = n & 0xffffffff;
            carry = n >> 32;
        }
        return *this;
    }

    base_uint& operator-=(const base_uint& b)
    {
        *this += -b;
        return *this;
    }

    base_uint& operator+=(uint64 b64)
    {
        base_uint b;
        b = b64;
        *this += b;
        return *this;
    }

    base_uint& operator-=(uint64 b64)
    {
        base_uint b;
        b = b64;
        *this += -b;
        return *this;
    }


    base_uint& operator++()
    {
        // prefix operator
        int i = 0;
        while (++pn[i] == 0 && i < WIDTH-1)
            i++;
        return *this;
    }

    const base_uint operator++(int)
    {
        // postfix operator
        const base_uint ret = *this;
        ++(*this);
        return ret;
    }

    base_uint& operator--()
    {
        // prefix operator
        int i = 0;
        while (--pn[i] == -1 && i < WIDTH-1)
            i++;
        return *this;
    }

    const base_uint operator--(int)
    {
        // postfix operator
        const base_uint ret = *this;
        --(*this);
        return ret;
    }


    friend inline bool operator<(const base_uint& a, const base_uint& b)
    {
        for (int i = base_uint::WIDTH-1; i >= 0; i--)
        {
            if (a.pn[i] < b.pn[i])
                return true;
            else if (a.pn[i] > b.pn[i])
                return false;
        }
        return false;
    }

    friend inline bool operator<=(const base_uint& a, const base_uint& b)
    {
        for (int i = base_uint::WIDTH-1; i >= 0; i--)
        {
            if (a.pn[i] < b.pn[i])
                return true;
            else if (a.pn[i] > b.pn[i])
                return false;
        }
        return true;
    }

    friend inline bool operator>(const base_uint& a, const base_uint& b)
    {
        for (int i = base_uint::WIDTH-1; i >= 0; i--)
        {
            if (a.pn[i] > b.pn[i])
                return true;
            else if (a.pn[i] < b.pn[i])
                return false;
        }
        return false;
    }

    friend inline bool operator>=(const base_uint& a, const base_uint& b)
    {
        for (int i = base_uint::WIDTH-1; i >= 0; i--)
        {
            if (a.pn[i] > b.pn[i])
                return true;
            else if (a.pn[i] < b.pn[i])
                return false;
        }
        return true;
    }

    friend inline bool operator==(const base_uint& a, const base_uint& b)
    {
        for (int i = 0; i < base_uint::WIDTH; i++)
            if (a.pn[i] != b.pn[i])
                return false;
        return true;
    }

    friend inline bool operator==(const base_uint& a, uint64 b)
    {
        if (a.pn[0] != (unsigned int)b)
            return false;
        if (a.pn[1] != (unsigned int)(b >> 32))
            return false;
        for (int i = 2; i < base_uint::WIDTH; i++)
            if (a.pn[i] != 0)
                return false;
        return true;
    }

    friend inline bool operator!=(const base_uint& a, const base_uint& b)
    {
        return (!(a == b));
    }

    friend inline bool operator!=(const base_uint& a, uint64 b)
    {
        return (!(a == b));
    }



    std::string GetHex() const
    {
        char psz[sizeof(pn)*2 + 1];
        for (unsigned int i = 0; i < sizeof(pn); i++)
            sprintf(psz + i*2, "%02x", ((unsigned char*)pn)[sizeof(pn) - i - 1]);
        return std::string(psz, psz + sizeof(pn)*2);
    }

    void SetHex(const char* psz)
    {
        for (int i = 0; i < WIDTH; i++)
            pn[i] = 0;

        // skip leading spaces
        while (isspace(*psz))
            psz++;

        // skip 0x
        if (psz[0] == '0' && tolower(psz[1]) == 'x')
            psz += 2;

        // hex string to uint
        static const unsigned char phexdigit[256] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,1,2,3,4,5,6,7,8,9,0,0,0,0,0,0, 0,0xa,0xb,0xc,0xd,0xe,0xf,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0xa,0xb,0xc,0xd,0xe,0xf,0,0,0,0,0,0,0,0,0 };
        const char* pbegin = psz;
        while (phexdigit[(unsigned char)*psz] || *psz == '0')
            psz++;
        psz--;
        unsigned char* p1 = (unsigned char*)pn;
        unsigned char* pend = p1 + WIDTH * 4;
        while (psz >= pbegin && p1 < pend)
        {
            *p1 = phexdigit[(unsigned char)*psz--];
            if (psz >= pbegin)
            {
                *p1 |= (phexdigit[(unsigned char)*psz--] << 4);
                p1++;
            }
        }
    }

    void SetHex(const std::string& str)
    {
        SetHex(str.c_str());
    }

    std::string ToString() const
    {
        return (GetHex());
    }

    unsigned char* begin()
    {
        return (unsigned char*)&pn[0];
    }

    unsigned char* end()
    {
        return (unsigned char*)&pn[WIDTH];
    }

    const unsigned char* begin() const
    {
        return (unsigned char*)&pn[0];
    }

    const unsigned char* end() const
    {
        return (unsigned char*)&pn[WIDTH];
    }

    unsigned int size() const
    {
        return sizeof(pn);
    }

    uint64 Get64(int n=0) const
    {
        return pn[2*n] | (uint64)pn[2*n+1] << 32;
    }

//    unsigned int GetSerializeSize(int nType=0, int nVersion=PROTOCOL_VERSION) const
    unsigned int GetSerializeSize(int nType, int nVersion) const
    {
        return sizeof(pn);
    }

    template<typename Stream>
//    void Serialize(Stream& s, int nType=0, int nVersion=PROTOCOL_VERSION) const
    void Serialize(Stream& s, int nType, int nVersion) const
    {
        s.write((char*)pn, sizeof(pn));
    }

    template<typename Stream>
//    void Unserialize(Stream& s, int nType=0, int nVersion=PROTOCOL_VERSION)
    void Unserialize(Stream& s, int nType, int nVersion)
    {
        s.read((char*)pn, sizeof(pn));
    }


    friend class uint160;
    friend class uint256;
    friend class uint128;
    friend inline int Testuint256AdHoc(std::vector<std::string> vArg);
};

typedef base_uint<160> base_uint160;
typedef base_uint<256> base_uint256;
typedef base_uint<888> base_name;
typedef base_uint<192> base_uint192;
typedef base_uint<128> base_uint128;


//
// uint160 and uint256 could be implemented as templates, but to keep
// compile errors and debugging cleaner, they're copy and pasted.
//

//////////////////////////////////////////////////////////////////////////////
//
// uint160
//

/** 160-bit unsigned integer */
class uint160 : public base_uint160
{
public:
    typedef base_uint160 basetype;

    uint160()
    {
        for (int i = 0; i < WIDTH; i++)
            pn[i] = 0;
    }

    uint160(const basetype& b)
    {
        for (int i = 0; i < WIDTH; i++)
            pn[i] = b.pn[i];
    }

    uint160& operator=(const basetype& b)
    {
        for (int i = 0; i < WIDTH; i++)
            pn[i] = b.pn[i];
        return *this;
    }

    uint160(uint64 b)
    {
        pn[0] = (unsigned int)b;
        pn[1] = (unsigned int)(b >> 32);
        for (int i = 2; i < WIDTH; i++)
            pn[i] = 0;
    }

    uint160& operator=(uint64 b)
    {
        pn[0] = (unsigned int)b;
        pn[1] = (unsigned int)(b >> 32);
        for (int i = 2; i < WIDTH; i++)
            pn[i] = 0;
        return *this;
    }

    explicit uint160(const std::string& str)
    {
        SetHex(str);
    }

    explicit uint160(const std::vector<unsigned char>& vch)
    {
        if (vch.size() == sizeof(pn))
            memcpy(pn, &vch[0], sizeof(pn));
        else
            *this = 0;
    }
};

inline bool operator==(const uint160& a, uint64 b)                           { return (base_uint160)a == b; }
inline bool operator!=(const uint160& a, uint64 b)                           { return (base_uint160)a != b; }
inline const uint160 operator<<(const base_uint160& a, unsigned int shift)   { return uint160(a) <<= shift; }
inline const uint160 operator>>(const base_uint160& a, unsigned int shift)   { return uint160(a) >>= shift; }
inline const uint160 operator<<(const uint160& a, unsigned int shift)        { return uint160(a) <<= shift; }
inline const uint160 operator>>(const uint160& a, unsigned int shift)        { return uint160(a) >>= shift; }

inline const uint160 operator^(const base_uint160& a, const base_uint160& b) { return uint160(a) ^= b; }
inline const uint160 operator&(const base_uint160& a, const base_uint160& b) { return uint160(a) &= b; }
inline const uint160 operator|(const base_uint160& a, const base_uint160& b) { return uint160(a) |= b; }
inline const uint160 operator+(const base_uint160& a, const base_uint160& b) { return uint160(a) += b; }
inline const uint160 operator-(const base_uint160& a, const base_uint160& b) { return uint160(a) -= b; }

inline bool operator<(const base_uint160& a, const uint160& b)          { return (base_uint160)a <  (base_uint160)b; }
inline bool operator<=(const base_uint160& a, const uint160& b)         { return (base_uint160)a <= (base_uint160)b; }
inline bool operator>(const base_uint160& a, const uint160& b)          { return (base_uint160)a >  (base_uint160)b; }
inline bool operator>=(const base_uint160& a, const uint160& b)         { return (base_uint160)a >= (base_uint160)b; }
inline bool operator==(const base_uint160& a, const uint160& b)         { return (base_uint160)a == (base_uint160)b; }
inline bool operator!=(const base_uint160& a, const uint160& b)         { return (base_uint160)a != (base_uint160)b; }
inline const uint160 operator^(const base_uint160& a, const uint160& b) { return (base_uint160)a ^  (base_uint160)b; }
inline const uint160 operator&(const base_uint160& a, const uint160& b) { return (base_uint160)a &  (base_uint160)b; }
inline const uint160 operator|(const base_uint160& a, const uint160& b) { return (base_uint160)a |  (base_uint160)b; }
inline const uint160 operator+(const base_uint160& a, const uint160& b) { return (base_uint160)a +  (base_uint160)b; }
inline const uint160 operator-(const base_uint160& a, const uint160& b) { return (base_uint160)a -  (base_uint160)b; }

inline bool operator<(const uint160& a, const base_uint160& b)          { return (base_uint160)a <  (base_uint160)b; }
inline bool operator<=(const uint160& a, const base_uint160& b)         { return (base_uint160)a <= (base_uint160)b; }
inline bool operator>(const uint160& a, const base_uint160& b)          { return (base_uint160)a >  (base_uint160)b; }
inline bool operator>=(const uint160& a, const base_uint160& b)         { return (base_uint160)a >= (base_uint160)b; }
inline bool operator==(const uint160& a, const base_uint160& b)         { return (base_uint160)a == (base_uint160)b; }
inline bool operator!=(const uint160& a, const base_uint160& b)         { return (base_uint160)a != (base_uint160)b; }
inline const uint160 operator^(const uint160& a, const base_uint160& b) { return (base_uint160)a ^  (base_uint160)b; }
inline const uint160 operator&(const uint160& a, const base_uint160& b) { return (base_uint160)a &  (base_uint160)b; }
inline const uint160 operator|(const uint160& a, const base_uint160& b) { return (base_uint160)a |  (base_uint160)b; }
inline const uint160 operator+(const uint160& a, const base_uint160& b) { return (base_uint160)a +  (base_uint160)b; }
inline const uint160 operator-(const uint160& a, const base_uint160& b) { return (base_uint160)a -  (base_uint160)b; }

inline bool operator<(const uint160& a, const uint160& b)               { return (base_uint160)a <  (base_uint160)b; }
inline bool operator<=(const uint160& a, const uint160& b)              { return (base_uint160)a <= (base_uint160)b; }
inline bool operator>(const uint160& a, const uint160& b)               { return (base_uint160)a >  (base_uint160)b; }
inline bool operator>=(const uint160& a, const uint160& b)              { return (base_uint160)a >= (base_uint160)b; }
inline bool operator==(const uint160& a, const uint160& b)              { return (base_uint160)a == (base_uint160)b; }
inline bool operator!=(const uint160& a, const uint160& b)              { return (base_uint160)a != (base_uint160)b; }
inline const uint160 operator^(const uint160& a, const uint160& b)      { return (base_uint160)a ^  (base_uint160)b; }
inline const uint160 operator&(const uint160& a, const uint160& b)      { return (base_uint160)a &  (base_uint160)b; }
inline const uint160 operator|(const uint160& a, const uint160& b)      { return (base_uint160)a |  (base_uint160)b; }
inline const uint160 operator+(const uint160& a, const uint160& b)      { return (base_uint160)a +  (base_uint160)b; }
inline const uint160 operator-(const uint160& a, const uint160& b)      { return (base_uint160)a -  (base_uint160)b; }



class uint128 : public base_uint128
{
public:
    typedef base_uint128 basetype;

    uint128()
    {
        for (int i = 0; i < WIDTH; i++)
            pn[i] = 0;
    }

    uint128(const basetype& b)
    {
        for (int i = 0; i < WIDTH; i++)
            pn[i] = b.pn[i];
    }

    uint128& operator=(const basetype& b)
    {
        for (int i = 0; i < WIDTH; i++)
            pn[i] = b.pn[i];
        return *this;
    }

    uint128(uint64 b)
    {
        pn[0] = (unsigned int)b;
        pn[1] = (unsigned int)(b >> 32);
        for (int i = 2; i < WIDTH; i++)
            pn[i] = 0;
    }


    uint128& operator=(uint64 b)
    {
        pn[0] = (unsigned int)b;
        pn[1] = (unsigned int)(b >> 32);
        for (int i = 2; i < WIDTH; i++)
            pn[i] = 0;
        return *this;
    }

    explicit uint128(const std::string& str)
    {
        SetHex(str);
    }

    explicit uint128(const std::vector<unsigned char>& vch)
    {
        if (vch.size() == sizeof(pn))
            memcpy(pn, &vch[0], sizeof(pn));
        else
            *this = 0;
    }
    explicit uint128(const char * vch)
    {
        unsigned int num[4];
        std::string svch(vch);
        if (svch.size() == 32)
        {
            for(int i = 3;i>=0;i--)
            {
                num[i] = 0;
                for(int j = 7;j>=0;j--)
                {
                    int nn;
                    if(svch[i*8+j] == 'a')
                        nn = 10;
                    else
                    if(svch[i*8+j] == 'b')
                        nn = 11;
                    else
                    if(svch[i*8+j] == 'c')
                        nn = 12;
                    else
                    if(svch[i*8+j] == 'd')
                        nn = 13;
                    else
                    if(svch[i*8+j] == 'e')
                        nn = 14;
                    else
                    if(svch[i*8+j] == 'f')
                        nn = 15;
                    else
                    if(svch[i*8+j] == '0')
                        nn = 0;
                    else
                    if(svch[i*8+j] == '1')
                        nn = 1;
                    else
                    if(svch[i*8+j] == '2')
                        nn = 2;
                    else
                    if(svch[i*8+j] == '3')
                        nn = 3;
                    else
                    if(svch[i*8+j] == '4')
                        nn = 4;
                    else
                    if(svch[i*8+j] == '5')
                        nn = 5;
                    else
                    if(svch[i*8+j] == '6')
                        nn = 6;
                    else
                    if(svch[i*8+j] == '7')
                        nn = 7;
                    else
                    if(svch[i*8+j] == '8')
                        nn = 8;
                    else
                    if(svch[i*8+j] == '9')
                        nn = 9;

                    num[i] += pow(2,(7-j)*4)*nn;
                }
                pn[3-i] = num[i];
                }
        }
        else
            *this = 0;
    }
    inline void setPn(unsigned int value, int n)
    {
        this->pn[n] = value;
    }
};


inline bool operator==(const uint128& a, uint64 b)                           { return (base_uint128)a == b; }
inline bool operator!=(const uint128& a, uint64 b)                           { return (base_uint128)a != b; }
inline const uint128 operator<<(const base_uint128& a, unsigned int shift)   { return uint128(a) <<= shift; }
inline const uint128 operator>>(const base_uint128& a, unsigned int shift)   { return uint128(a) >>= shift; }
inline const uint128 operator<<(const uint128& a, unsigned int shift)        { return uint128(a) <<= shift; }
inline const uint128 operator>>(const uint128& a, unsigned int shift)        { return uint128(a) >>= shift; }

inline const uint128 operator^(const base_uint128& a, const base_uint128& b) { return uint128(a) ^= b; }
inline const uint128 operator&(const base_uint128& a, const base_uint128& b) { return uint128(a) &= b; }
inline const uint128 operator|(const base_uint128& a, const base_uint128& b) { return uint128(a) |= b; }
inline const uint128 operator+(const base_uint128& a, const base_uint128& b) { return uint128(a) += b; }
inline const uint128 operator-(const base_uint128& a, const base_uint128& b) { return uint128(a) -= b; }

inline bool operator<(const base_uint128& a, const uint128& b)          { return (base_uint128)a <  (base_uint128)b; }
inline bool operator<=(const base_uint128& a, const uint128& b)         { return (base_uint128)a <= (base_uint128)b; }
inline bool operator>(const base_uint128& a, const uint128& b)          { return (base_uint128)a >  (base_uint128)b; }
inline bool operator>=(const base_uint128& a, const uint128& b)         { return (base_uint128)a >= (base_uint128)b; }
inline bool operator==(const base_uint128& a, const uint128& b)         { return (base_uint128)a == (base_uint128)b; }
inline bool operator!=(const base_uint128& a, const uint128& b)         { return (base_uint128)a != (base_uint128)b; }
inline const uint128 operator^(const base_uint128& a, const uint128& b) { return (base_uint128)a ^  (base_uint128)b; }
inline const uint128 operator&(const base_uint128& a, const uint128& b) { return (base_uint128)a &  (base_uint128)b; }
inline const uint128 operator|(const base_uint128& a, const uint128& b) { return (base_uint128)a |  (base_uint128)b; }
inline const uint128 operator+(const base_uint128& a, const uint128& b) { return (base_uint128)a +  (base_uint128)b; }
inline const uint128 operator-(const base_uint128& a, const uint128& b) { return (base_uint128)a -  (base_uint128)b; }

inline bool operator<(const uint128& a, const base_uint128& b)          { return (base_uint128)a <  (base_uint128)b; }
inline bool operator<=(const uint128& a, const base_uint128& b)         { return (base_uint128)a <= (base_uint128)b; }
inline bool operator>(const uint128& a, const base_uint128& b)          { return (base_uint128)a >  (base_uint128)b; }
inline bool operator>=(const uint128& a, const base_uint128& b)         { return (base_uint128)a >= (base_uint128)b; }
inline bool operator==(const uint128& a, const base_uint128& b)         { return (base_uint128)a == (base_uint128)b; }
inline bool operator!=(const uint128& a, const base_uint128& b)         { return (base_uint128)a != (base_uint128)b; }
inline const uint128 operator^(const uint128& a, const base_uint128& b) { return (base_uint128)a ^  (base_uint128)b; }
inline const uint128 operator&(const uint128& a, const base_uint128& b) { return (base_uint128)a &  (base_uint128)b; }
inline const uint128 operator|(const uint128& a, const base_uint128& b) { return (base_uint128)a |  (base_uint128)b; }
inline const uint128 operator+(const uint128& a, const base_uint128& b) { return (base_uint128)a +  (base_uint128)b; }
inline const uint128 operator-(const uint128& a, const base_uint128& b) { return (base_uint128)a -  (base_uint128)b; }

inline bool operator<(const uint128& a, const uint128& b)               { return (base_uint128)a <  (base_uint128)b; }
inline bool operator<=(const uint128& a, const uint128& b)              { return (base_uint128)a <= (base_uint128)b; }
inline bool operator>(const uint128& a, const uint128& b)               { return (base_uint128)a >  (base_uint128)b; }
inline bool operator>=(const uint128& a, const uint128& b)              { return (base_uint128)a >= (base_uint128)b; }
inline bool operator==(const uint128& a, const uint128& b)              { return (base_uint128)a == (base_uint128)b; }
inline bool operator!=(const uint128& a, const uint128& b)              { return (base_uint128)a != (base_uint128)b; }
inline const uint128 operator^(const uint128& a, const uint128& b)      { return (base_uint128)a ^  (base_uint128)b; }
inline const uint128 operator&(const uint128& a, const uint128& b)      { return (base_uint128)a &  (base_uint128)b; }
inline const uint128 operator|(const uint128& a, const uint128& b)      { return (base_uint128)a |  (base_uint128)b; }
inline const uint128 operator+(const uint128& a, const uint128& b)      { return (base_uint128)a +  (base_uint128)b; }
inline const uint128 operator-(const uint128& a, const uint128& b)      { return (base_uint128)a -  (base_uint128)b; }




//////////////////////////////////////////////////////////////////////////////
//
// uint256
//

/** 256-bit unsigned integer */
class uint256 : public base_uint256
{
public:
    typedef base_uint256 basetype;

    uint256()
    {
        for (int i = 0; i < WIDTH; i++)
            pn[i] = 0;
    }

    uint256(const basetype& b)
    {
        for (int i = 0; i < WIDTH; i++)
            pn[i] = b.pn[i];
    }

    uint256& operator=(const basetype& b)
    {
        for (int i = 0; i < WIDTH; i++)
            pn[i] = b.pn[i];
        return *this;
    }

    uint256(uint64 b)
    {
        pn[0] = (unsigned int)b;
        pn[1] = (unsigned int)(b >> 32);
        for (int i = 2; i < WIDTH; i++)
            pn[i] = 0;
    }

    uint256& operator=(uint64 b)
    {
        pn[0] = (unsigned int)b;
        pn[1] = (unsigned int)(b >> 32);
        for (int i = 2; i < WIDTH; i++)
            pn[i] = 0;
        return *this;
    }

    explicit uint256(const std::string& str)
    {
        SetHex(str);
    }

    explicit uint256(const std::vector<unsigned char>& vch)
    {
        if (vch.size() == sizeof(pn))
            memcpy(pn, &vch[0], sizeof(pn));
        else
            *this = 0;
    }
    inline unsigned int GetPn(int n)
    {
        return pn[n];
    }
    inline uint128 Get128()
    {
        uint128 cc;
        for(unsigned int i = 0;i<4;i++)
        cc.setPn(this->pn[i],i);

        return cc;
    }

    inline uint128 GetSecond128()
    {
        uint128 cc;
        for(unsigned int i = 4;i<8;i++)
        cc.setPn(this->pn[i],i-4);

        return cc;
    }
};



inline bool operator==(const uint256& a, uint64 b)                           { return (base_uint256)a == b; }
inline bool operator!=(const uint256& a, uint64 b)                           { return (base_uint256)a != b; }
inline const uint256 operator<<(const base_uint256& a, unsigned int shift)   { return uint256(a) <<= shift; }
inline const uint256 operator>>(const base_uint256& a, unsigned int shift)   { return uint256(a) >>= shift; }
inline const uint256 operator<<(const uint256& a, unsigned int shift)        { return uint256(a) <<= shift; }
inline const uint256 operator>>(const uint256& a, unsigned int shift)        { return uint256(a) >>= shift; }
inline const uint256 operator^(const base_uint256& a, const base_uint256& b) { return uint256(a) ^= b; }
inline const uint256 operator&(const base_uint256& a, const base_uint256& b) { return uint256(a) &= b; }
inline const uint256 operator|(const base_uint256& a, const base_uint256& b) { return uint256(a) |= b; }
inline const uint256 operator+(const base_uint256& a, const base_uint256& b) { return uint256(a) += b; }
inline const uint256 operator-(const base_uint256& a, const base_uint256& b) { return uint256(a) -= b; }

inline bool operator<(const base_uint256& a, const uint256& b)          { return (base_uint256)a <  (base_uint256)b; }
inline bool operator<=(const base_uint256& a, const uint256& b)         { return (base_uint256)a <= (base_uint256)b; }
inline bool operator>(const base_uint256& a, const uint256& b)          { return (base_uint256)a >  (base_uint256)b; }
inline bool operator>=(const base_uint256& a, const uint256& b)         { return (base_uint256)a >= (base_uint256)b; }
inline bool operator==(const base_uint256& a, const uint256& b)         { return (base_uint256)a == (base_uint256)b; }
inline bool operator!=(const base_uint256& a, const uint256& b)         { return (base_uint256)a != (base_uint256)b; }
inline const uint256 operator^(const base_uint256& a, const uint256& b) { return (base_uint256)a ^  (base_uint256)b; }
inline const uint256 operator&(const base_uint256& a, const uint256& b) { return (base_uint256)a &  (base_uint256)b; }
inline const uint256 operator|(const base_uint256& a, const uint256& b) { return (base_uint256)a |  (base_uint256)b; }
inline const uint256 operator+(const base_uint256& a, const uint256& b) { return (base_uint256)a +  (base_uint256)b; }
inline const uint256 operator-(const base_uint256& a, const uint256& b) { return (base_uint256)a -  (base_uint256)b; }

inline bool operator<(const uint256& a, const base_uint256& b)          { return (base_uint256)a <  (base_uint256)b; }
inline bool operator<=(const uint256& a, const base_uint256& b)         { return (base_uint256)a <= (base_uint256)b; }
inline bool operator>(const uint256& a, const base_uint256& b)          { return (base_uint256)a >  (base_uint256)b; }
inline bool operator>=(const uint256& a, const base_uint256& b)         { return (base_uint256)a >= (base_uint256)b; }
inline bool operator==(const uint256& a, const base_uint256& b)         { return (base_uint256)a == (base_uint256)b; }
inline bool operator!=(const uint256& a, const base_uint256& b)         { return (base_uint256)a != (base_uint256)b; }
inline const uint256 operator^(const uint256& a, const base_uint256& b) { return (base_uint256)a ^  (base_uint256)b; }
inline const uint256 operator&(const uint256& a, const base_uint256& b) { return (base_uint256)a &  (base_uint256)b; }
inline const uint256 operator|(const uint256& a, const base_uint256& b) { return (base_uint256)a |  (base_uint256)b; }
inline const uint256 operator+(const uint256& a, const base_uint256& b) { return (base_uint256)a +  (base_uint256)b; }
inline const uint256 operator-(const uint256& a, const base_uint256& b) { return (base_uint256)a -  (base_uint256)b; }

inline bool operator<(const uint256& a, const uint256& b)               { return (base_uint256)a <  (base_uint256)b; }
inline bool operator<=(const uint256& a, const uint256& b)              { return (base_uint256)a <= (base_uint256)b; }
inline bool operator>(const uint256& a, const uint256& b)               { return (base_uint256)a >  (base_uint256)b; }
inline bool operator>=(const uint256& a, const uint256& b)              { return (base_uint256)a >= (base_uint256)b; }
inline bool operator==(const uint256& a, const uint256& b)              { return (base_uint256)a == (base_uint256)b; }
inline bool operator!=(const uint256& a, const uint256& b)              { return (base_uint256)a != (base_uint256)b; }
inline const uint256 operator^(const uint256& a, const uint256& b)      { return (base_uint256)a ^  (base_uint256)b; }
inline const uint256 operator&(const uint256& a, const uint256& b)      { return (base_uint256)a &  (base_uint256)b; }
inline const uint256 operator|(const uint256& a, const uint256& b)      { return (base_uint256)a |  (base_uint256)b; }
inline const uint256 operator+(const uint256& a, const uint256& b)      { return (base_uint256)a +  (base_uint256)b; }
inline const uint256 operator-(const uint256& a, const uint256& b)      { return (base_uint256)a -  (base_uint256)b; }








#ifdef TEST_UINT256

inline int Testuint256AdHoc(std::vector<std::string> vArg)
{
    uint256 g(0);


    logPrint("%s\n", g.ToString().c_str());
    g--;  logPrint("g--\n");
    logPrint("%s\n", g.ToString().c_str());
    g--;  logPrint("g--\n");
    logPrint("%s\n", g.ToString().c_str());
    g++;  logPrint("g++\n");
    logPrint("%s\n", g.ToString().c_str());
    g++;  logPrint("g++\n");
    logPrint("%s\n", g.ToString().c_str());
    g++;  logPrint("g++\n");
    logPrint("%s\n", g.ToString().c_str());
    g++;  logPrint("g++\n");
    logPrint("%s\n", g.ToString().c_str());



    uint256 a(7);
    logPrint("a=7\n");
    logPrint("%s\n", a.ToString().c_str());

    uint256 b;
    logPrint("b undefined\n");
    logPrint("%s\n", b.ToString().c_str());
    int c = 3;

    a = c;
    a.pn[3] = 15;
    logPrint("%s\n", a.ToString().c_str());
    uint256 k(c);

    a = 5;
    a.pn[3] = 15;
    logPrint("%s\n", a.ToString().c_str());
    b = 1;
    b <<= 52;

    a |= b;

    a ^= 0x500;

    logPrint("a %s\n", a.ToString().c_str());

    a = a | b | (uint256)0x1000;


    logPrint("a %s\n", a.ToString().c_str());
    logPrint("b %s\n", b.ToString().c_str());

    a = 0xfffffffe;
    a.pn[4] = 9;

    logPrint("%s\n", a.ToString().c_str());
    a++;
    logPrint("%s\n", a.ToString().c_str());
    a++;
    logPrint("%s\n", a.ToString().c_str());
    a++;
    logPrint("%s\n", a.ToString().c_str());
    a++;
    logPrint("%s\n", a.ToString().c_str());

    a--;
    logPrint("%s\n", a.ToString().c_str());
    a--;
    logPrint("%s\n", a.ToString().c_str());
    a--;
    logPrint("%s\n", a.ToString().c_str());
    uint256 d = a--;
    logPrint("%s\n", d.ToString().c_str());
    logPrint("%s\n", a.ToString().c_str());
    a--;
    logPrint("%s\n", a.ToString().c_str());
    a--;
    logPrint("%s\n", a.ToString().c_str());

    d = a;

    logPrint("%s\n", d.ToString().c_str());
    for (int i = uint256::WIDTH-1; i >= 0; i--) logPrint("%08x", d.pn[i]); logPrint("\n");

    uint256 neg = d;
    neg = ~neg;
    logPrint("%s\n", neg.ToString().c_str());


    uint256 e = uint256("0xABCDEF123abcdef12345678909832180000011111111");
    logPrint("\n");
    logPrint("%s\n", e.ToString().c_str());


    logPrint("\n");
    uint256 x1 = uint256("0xABCDEF123abcdef12345678909832180000011111111");
    uint256 x2;
    logPrint("%s\n", x1.ToString().c_str());
    for (int i = 0; i < 270; i += 4)
    {
        x2 = x1 << i;
        logPrint("%s\n", x2.ToString().c_str());
    }

    logPrint("\n");
    logPrint("%s\n", x1.ToString().c_str());
    for (int i = 0; i < 270; i += 4)
    {
        x2 = x1;
        x2 >>= i;
        logPrint("%s\n", x2.ToString().c_str());
    }


    for (int i = 0; i < 100; i++)
    {
        uint256 k = (~uint256(0) >> i);
        logPrint("%s\n", k.ToString().c_str());
    }

    for (int i = 0; i < 100; i++)
    {
        uint256 k = (~uint256(0) << i);
        logPrint("%s\n", k.ToString().c_str());
    }

    return (0);
}

#endif

#endif
