#ifndef __MUP_DEFINE_STREAM_H_2307__
#define __MUP_DEFINE_STREAM_H_2307__
#include <fstream>
#include "mupconfig.h"

class CMUPDefineStream 
{
public:
    CMUPDefineStream( 
        std::string const& outputDir )
        : _outputDir(outputDir)
    {
        if (!CMUPCommon::isExist(_outputDir))
        {
            CMUPCommon::makeDir(_outputDir);
        }
    }

    ~CMUPDefineStream()
    {
    }

    bool open( 
        std::string name)
    {
        std::string defineFileName(_outputDir);
        defineFileName += name;
        defineFileName += ".cpp";
        _fstream.open(defineFileName.c_str(), std::ios_base::trunc);
        bool ret = _fstream.is_open();
        if (!ret)
        {
            return ret;
        }

        _fstream << COPYRIGHT_DECLARE;

        perpareInclude( name );

        return ret;
    }

    void close()
    {
        _fstream.close();
    }


    CMUPDefineStream& operator<<( 
        std::string const& right)
    {
        _fstream << right.c_str();
        return *this;
    }

    CMUPDefineStream& operator<<(
        time_t right)
    {
        char buff[64] ={0};
        sprintf_s(buff,"%I64d", right);
        return *this << buff;
    }

protected:
    void perpareInclude( 
        std::string name)
    {
        *this << "#include \"" << name << ".h\"\n";
    }

protected:
    std::ofstream _fstream;
    std::string _outputDir;
};

#endif // __MUP_DEFINE_STREAM_H_2307__
