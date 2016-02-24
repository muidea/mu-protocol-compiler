//
//
//    This software is provided 'as-is', without any express or implied
//    warranty. In no event will the authors be held liable for any
//    damages arising from the use of this software.
//
//    Author:muidea@gmail.com
//
//

#ifndef __MU_PROTOCOL_HELPER_H_1621__
#define __MU_PROTOCOL_HELPER_H_1621__
#include "muprotocol/muprotocol.h"

namespace MUPProtocol
{
    template<class T>
    class CMUBlobTailHelper
        : public T
    {
    public:
        CMUBlobTailHelper()
            : _cpBlobPtr(nullptr)
            , _uBlobSize(0)
        {
        }

        CMUBlobTailHelper( 
            T& head )
            : T(head)
            , _cpBlobPtr(nullptr)
            , _uBlobSize(0)
        {
        }

        CMUBlobTailHelper( 
            T& head, 
            const void* pDataPtr, 
            UINT32 uDataSize )
            : T(head)
            , _cpBlobPtr(pDataPtr)
            , _uBlobSize(uDataSize)
        {
            assert(_cpBlobPtr);
        }

        ~CMUBlobTailHelper()
        {
        }

        virtual bool encode( 
            void* pBuffPtr, 
            UINT32 uBuffSize, 
            UINT32& uRemainSize ) const
        {
            assert(pBuffPtr);

            UINT32 uTotalSize = CMUBlobTailHelper<T>::calcSize();
            if (uBuffSize < uTotalSize) {
                return false;
            }

            if (!T::encode(pBuffPtr,uBuffSize,uRemainSize)) {
                return false;
            }

            if (NULL != _cpBlobPtr) {
                ::memcpy((char*)pBuffPtr + uBuffSize - uRemainSize, _cpBlobPtr, _uBlobSize);
                uRemainSize -= _uBlobSize;
            }

            return true;
        }

        virtual bool decode( 
            const void* pDataPtr, 
            UINT32 uDataSize, 
            UINT32& uRemainSize )
        {
            assert(pDataPtr);

            if (!T::decode(pDataPtr, uDataSize, uRemainSize)) {
                return false;
            }

            _cpBlobPtr = (const char*)pDataPtr + uDataSize - uRemainSize;
            _uBlobSize = uRemainSize;
            uRemainSize = 0;

            return true;
        }

        virtual UINT32 calcSize() const
        {
            return T::calcSize() + _uBlobSize;
        }

        const void* getBlobData( 
            UINT32& uDataSize ) const
        {
            uDataSize = _uBlobSize;

            return _cpBlobPtr;
        }

        void setBlobData( 
            const void* pDataPtr, 
            UINT32 uDataSize )
        {
            assert(pDataPtr);

            _cpBlobPtr = pDataPtr;
            _uBlobSize = uDataSize;
        }

    protected:
        const void* _cpBlobPtr;
        UINT32 _uBlobSize;
    };

    template<class Head,class Data>
    class CMUPackHelper
    {
    public:
        CMUPackHelper( 
            Head const& head, 
            Data const& data )
            : _head(head)
            , _data(data)
        {
        }

        ~CMUPackHelper()
        {
        }

        bool encode( 
            void* pBuffPtr, 
            UINT32 uBuffSize, 
            UINT32& uRemainSize ) const
        {
            UINT32 uTotalSize = CMUPackHelper<Head,Data>::calcSize();
            if (uBuffSize < uTotalSize) {
                return false;
            }

            if (!_head.encode(pBuffPtr,uBuffSize,uRemainSize)) {
                return false;
            }

            if (!_data.encode((char*)pBuffPtr+uBuffSize-uRemainSize,uRemainSize,uRemainSize)) {
                return false;
            }

            return true;
        }

        UINT32 calcSize() const
        {
            return _head.calcSize() + _data.calcSize();
        }

    protected:
        Head const& _head;
        Data const& _data;
    };
}

#endif // __MU_PROTOCOL_HELPER_H_1621__
