//
//
//    This software is provided 'as-is', without any express or implied
//    warranty. In no event will the authors be held liable for any
//    damages arising from the use of this software.
//
//    Author:muidea@gmail.com
//
//

#ifndef __MUSERIALIZE_H_2242__
#define __MUSERIALIZE_H_2242__

#include <string>
#include <list>
#include <vector>
#include <memory.h>
#include <assert.h>

#if defined(_MSC_VER)
#include <winsock2.h>
#include <windows.h>

typedef __int64 INT64;
typedef unsigned __int64 UINT64;

#elif defined(__GNUC__)
typedef char INT8;
typedef unsigned char UINT8;
typedef short INT16;
typedef unsigned short UINT16;
typedef int INT32;
typedef unsigned int UINT32;
typedef unsigned int UINT;
typedef unsigned long ULONG;
typedef float FLOAT;
typedef double DOUBLE;

#if defined( __LP64__ )
typedef long INT64;
typedef unsigned long UINT64;
#else // defined( __LP64__ )
typedef long long INT64;
typedef unsigned long long UINT64;
#endif // __LP64__

#else
#error Unknown compiler! 
#endif

namespace MUPProtocol
{
	// 1:little endian
	// 0:big endian
	inline int checkCPU()
	{
		union
		{
			INT16 i16;
			INT8 i8;
		}test;

		test.i16 = 1;
		return (test.i8 == 1);
	}

	inline  void swapEndian(void* pBytePtr, UINT32 nByteCount)
	{
		if (!checkCPU()) {
			return ;
		}

		UINT8 tmp,*ptr = (UINT8 *)pBytePtr;
		for (UINT32 i = 0; i<nByteCount/2; i++) {
			tmp = ptr[i];
			ptr[i] = ptr[nByteCount-1-i];
			ptr[nByteCount-1-i] = tmp;
		}
	}

    template<class TypeName>
    inline UINT32 getSize( 
        TypeName const& value )
    {
        return sizeof(value);
    }

    template<class TypeName>
    inline bool encode( 
        TypeName const& value, 
        void* pBuffPtr, 
        UINT32 uBuffSize, 
        UINT32& uRemainSize )
    {
        assert(pBuffPtr);

        UINT32 uTotalSize = getSize(value);
#if _DEBUG
        if (uBuffSize < uTotalSize) {
            return false;
        }
#endif
        *(TypeName*)pBuffPtr = value;
		swapEndian(pBuffPtr, uTotalSize);
        uRemainSize = uBuffSize - uTotalSize;
        return true;
    }

    template<class TypeName>
    inline bool decode( 
        const void* pDataPtr, 
        UINT32 uDataSize, 
        TypeName& value, 
        UINT32& uRemainSize )
    {
        assert(pDataPtr);

        UINT32 uTotalSize = getSize(value);
#if _DEBUG
        if (uDataSize < uTotalSize) {
            return false;
        }
#endif
		value = *(TypeName*)pDataPtr;
		swapEndian(&value, uTotalSize);
        uRemainSize = uDataSize - uTotalSize;
        return true;
    }

	inline UINT32 getSize( 
		std::string const& value )
	{
		return value.size() + sizeof(UINT32);
	}

	inline bool encode( 
		std::string const& value, 
		void* pBuffPtr, 
		UINT32 uBuffSize, 
		UINT32& uRemainSize )
	{
		assert(pBuffPtr);

		UINT32 uStringSize = getSize(value);
#if _DEBUG
		if (uBuffSize < uStringSize) {
			return false;
		}
#endif
		if (!encode(uStringSize,pBuffPtr,uBuffSize,uRemainSize)) {
			return false;
		}

		::memcpy((char*)pBuffPtr + uBuffSize - uRemainSize, value.c_str(), value.size());
		uRemainSize -= value.size();
		return true;
	}

	inline bool decode( 
		const void* pDataPtr, 
		UINT32 uDataSize, 
		std::string& value, 
		UINT32& uRemainSize )
	{
		assert(pDataPtr);

		UINT32 uStringSize = 0;
		if (!decode(pDataPtr, uDataSize, uStringSize, uRemainSize)) {
			return false;
		}

#if _DEBUG
		if (uDataSize < uStringSize) {
			return false;
		}
#endif
		value.assign((const char*)pDataPtr + uDataSize - uRemainSize, uStringSize - sizeof(UINT32));
		uRemainSize = uDataSize - uStringSize;

		return true;
	}

    template<class TypeName>
    inline UINT32 getSize( 
        std::list<TypeName> const& value )
    {
        UINT32 uRet = 0;

        std::list<TypeName>::const_iterator iIter = value.begin();
        for ( ; iIter != value.end(); ++iIter )
        {
            uRet += getSize(*iIter);
        }

        return uRet  + sizeof(UINT32);
    }

    template<class TypeName>
    inline bool encode( 
        std::list<TypeName> const& value, 
        void* pBuffPtr, 
        UINT32 uBuffSize, 
        UINT32& uRemainSize )
    {
        assert(pBuffPtr);

        UINT32 uDataSize = getSize(value);
#if _DEBUG
        if (uBuffSize < uDataSize) {
            return false;
        }
#endif
        UINT32 uItemNum = value.size();
        if (!encode(uItemNum,pBuffPtr,uBuffSize,uRemainSize)) {
            return false;
        }

        std::list<TypeName>::const_iterator iIter = value.begin();
        for ( ; iIter != value.end(); ++iIter ) 
        {
            if (!encode(*iIter, (char*)pBuffPtr + uBuffSize - uRemainSize, uRemainSize, uRemainSize)) {
                return false;
            }
        }

        return true;
    }

    template<class TypeName>
    inline bool decode( 
        const void* pDataPtr, 
        UINT32 uDataSize, 
        std::list<TypeName>& value, 
        UINT32& uRemainSize )
    {
        assert(pDataPtr);

        UINT32 uItemNum = 0;
        if (!decode(pDataPtr, uDataSize, uItemNum, uRemainSize))
        {
            return false;
        }

        for ( UINT32 ii = 0; ii < uItemNum; ++ii )
        {
            TypeName typeName;
            if (!decode((const char*)pDataPtr + uDataSize - uRemainSize, uRemainSize, typeName, uRemainSize))
            {
                return false;
            }

            value.push_back(typeName);
        }

        return true;
    }

    template<class TypeName>
    inline UINT32 getSize( 
        std::vector<TypeName> const& value )
    {
        UINT32 uRet = 0;

        std::vector<TypeName>::const_iterator iIter = value.begin();
        for ( ; iIter != value.end(); ++iIter )
        {
            uRet += getSize(*iIter);
        }

        return uRet  + sizeof(UINT32);
    }

    template<class TypeName>
    inline bool encode( 
        std::vector<TypeName> const& value, 
        void* pBuffPtr, 
        UINT32 uBuffSize, 
        UINT32& uRemainSize )
    {
        assert(pBuffPtr);

        UINT32 uDataSize = getSize(value);
#if _DEBUG
        if (uBuffSize < uDataSize) {
            return false;
        }
#endif
        UINT32 uItemNum = value.size();
        if (!encode(uItemNum,pBuffPtr,uBuffSize,uRemainSize)) {
            return false;
        }

        std::vector<TypeName>::const_iterator iIter = value.begin();
        for ( ; iIter != value.end(); ++iIter ) 
        {
            if (!encode(*iIter, (char*)pBuffPtr + uBuffSize - uRemainSize, uRemainSize, uRemainSize)) {
                return false;
            }
        }

        return true;
    }

    template<class TypeName>
    inline bool decode( 
        const void* pDataPtr, 
        UINT32 uDataSize, 
        std::vector<TypeName>& value, 
        UINT32& uRemainSize )
    {
        assert(pDataPtr);

        UINT32 uItemNum = 0;
        if (!decode(pDataPtr, uDataSize, uItemNum, uRemainSize))
        {
            return false;
        }

        value.reserve(uItemNum);
        for ( UINT32 ii = 0; ii < uItemNum; ++ii )
        {
            TypeName typeName;
            if (!decode((const char*)pDataPtr + uDataSize - uRemainSize, uRemainSize, typeName, uRemainSize))
            {
                return false;
            }

            value.push_back(typeName);
        }

        return true;
    }

    class CMUNull
    {
    public:
        CMUNull()
        {
        }

        ~CMUNull()
        {
        }

        bool encode( 
            void* pBuffPtr, 
            UINT32 uBuffSize, 
            UINT32& uRemainSize )
        {
            return true;
        }

        bool decode( 
            const void* pDataPtr, 
            UINT32 uDataSize, 
            UINT32& uRemainSize )
        {
            return true;
        }

        UINT32 calcSize()
        {
            return 0;
        }
    };
}

#endif // __MUSERIALIZE_H_2242__

