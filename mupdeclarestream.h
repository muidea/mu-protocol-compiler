#ifndef __MUP_DCLARE_STREAM_H_2324__
#define __MUP_DCLARE_STREAM_H_2324__
#include <fstream>
#include <algorithm>
#include <time.h>
#include "mupconfig.h"
#include "mupcommon.h"

class CMUPDeclareStream 
{
public:
    CMUPDeclareStream( 
        std::string const& outputDir )
        : _outputDir(outputDir)
    {
        if (!CMUPCommon::isExist(_outputDir))
        {
            CMUPCommon::makeDir(_outputDir);
        }
    }

    ~CMUPDeclareStream()
    {
    }

    bool open( 
        std::string name)
    {
        std::string declareFileName(_outputDir);
        declareFileName += name;
        declareFileName += ".h";
        _fstream.open(declareFileName.c_str(), std::ios_base::trunc);
        bool ret = _fstream.is_open();
        if (!ret)
        {
            return ret;
        }

        _fstream << COPYRIGHT_DECLARE;

        perpareDecalreBegin(name);

        return ret;
    }

    void close()
    {
        perpareDecalreEnd();

        _fstream.close();
    }

    CMUPDeclareStream& operator<<( 
        std::string const& right)
    {
        _fstream << right.c_str();

        return *this;
    }

    CMUPDeclareStream& operator<<( 
        time_t right)
    {
        char buff[64] ={0};
        sprintf_s(buff,"%I64d", right);
        return *this << buff;
    }

protected:
    void perpareDecalreBegin( 
        std::string name)
    {
        time_t curTime = 0;
        time(&curTime);
        std::transform(name.begin(), name.end(), name.begin(), CMUPReplaceTag('\\','_'));
        std::transform(name.begin(), name.end(), name.begin(), CMUPReplaceTag('/','_'));
        std::transform(name.begin(), name.end(), name.begin(), CMUPReplaceTag('.','_'));
        std::transform(name.begin(), name.end(), name.begin(), toupper);
        *this << "#ifndef __" << name << "_H_" << curTime << "__\n";
        *this << "#define __" << name << "_H_" << curTime << "__\n";
        *this << "#include \"muprotocol/muprotocol.h\"\n";
        *this << "#include \"muprotocol/muserialize.h\"\n";

    }

    void perpareDecalreEnd()
    {
        *this << "#endif\n";
    }

protected:
    std::ofstream _fstream;
    std::string _outputDir;
};

#endif // __MUP_DCLARE_STREAM_H_2324__

