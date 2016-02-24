//
//
//    This software is provided 'as-is', without any express or implied
//    warranty. In no event will the authors be held liable for any
//    damages arising from the use of this software.
//
//    Author:muidea@gmail.com
//
//

#ifndef __MUPROTOCOL_H_2304__
#define __MUPROTOCOL_H_2304__
#include "muprotocol/muserialize.h"

class IMUProtocol
{
public:
    virtual ~IMUProtocol() {};

    virtual bool encode( 
        void* pBuffPtr, 
        UINT32 uBuffSize, 
        UINT32& uRemainSize) const = 0;

    virtual bool decode( 
        const void* pDataPtr, 
        UINT32 uDataSize, 
        UINT32& uRemainSize) = 0;

    virtual UINT32 calcSize() const = 0;
};

#endif // __MUPROTOCOL_H_2304__

