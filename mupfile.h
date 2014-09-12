#ifndef __MUP_FILE_H_2048__
#define __MUP_FILE_H_2048__
#include <string>

/*
 * filePath: 文件路径
 * fileName: 文件名
 * deep: 文件深度
 * 文件的完整路径是：文件路径+文件名
 **/
class CMUPFile
{
public:
    CMUPFile()
    {
    }

    CMUPFile( 
        std::string const& workPath,
        std::string const& fileName, 
        int deep )
        : _workPath(workPath)
        , _fileName(fileName)
        , _deep(deep)
    {
    }

    ~CMUPFile()
    {
    }

    CMUPFile& operator=( 
        CMUPFile const& right)
    {
        if (this != &right)
        {
            _workPath = right._workPath;
            _fileName = right._fileName;
            _deep = right._deep;
        }

        return *this;
    }

    void setWorkPath( 
        std::string const& workPath)
    {
        _workPath = workPath;
    }

    std::string const& getWorkPath() const
    {
        return _workPath;
    }

    void setFileName( 
        std::string const& fileName)
    {
        _fileName = fileName;
    }

    std::string const& getFileName() const
    {
        return _fileName;
    }

    void setDeep( 
        int deep )
    {
        _deep = deep;
    }

    int getDeep() const
    {
        return _deep;
    }

protected:
    std::string _workPath;
    std::string _fileName;
    int _deep;
};

#endif // __MUP_FILE_H_2048__

