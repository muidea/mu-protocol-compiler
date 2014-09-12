#include "mupcommon.h"
#include "mupcompiler_parser.h"
#include "mupconsolehelper.h"
#include <io.h>
#include <algorithm>
#include <assert.h>

#pragma warning(disable:4996)

void CMUPCommon::traceInfo( 
    std::string const& type, 
    std::string const& fileName,
    unsigned int lineNo, 
    std::string const& info )
{
    std::cout << "\n" << fileName << "(" << lineNo << "): " << std::endl;

    std::cout << "[";
    changeColor();
    std::cout << type;
    restoreColor();
    std::cout << "] " << info << std::endl;
}

bool CMUPCommon::isIdentifier( 
    std::string const& str )
{
    if (str.empty())
    {
        return false;
    }

    return 0 != ::isalpha(str[0]);
}

bool CMUPCommon::isExist(
    std::string fullPath )
{
    std::transform(fullPath.begin(),fullPath.end(),fullPath.begin(), CMUPReplaceTag('/','\\'));

    // 00 Existence only
    // 02 Write-only
    // 04 Read-only
    // 06 Read and write
    return 0 ==::_access(fullPath.c_str(), 00);
}

bool CMUPCommon::makeDir( 
    std::string path )
{
    std::transform(path.begin(),path.end(),path.begin(), CMUPReplaceTag('/','\\'));

    std::string::size_type offset = path.find_first_of('\\');
    while(offset != std::string::npos)
    {
        std::string subPath = path.substr(0, offset);
        if (!isExist(subPath))
        {
            if(0 !=::_mkdir(subPath.c_str()))
            {
                return false;
            }
        }

        offset = path.find_first_of('\\', offset+1);
    }

    return 0 ==::_mkdir(path.c_str());
}


bool CMUPCommon::deleteDir( 
    std::string path )
{
    std::transform(path.begin(),path.end(),path.begin(), CMUPReplaceTag('/','\\'));

    if (!isExist(path))
    {
        return true;
    }

    return (0 == ::_rmdir(path.c_str()));
}

std::string CMUPCommon::getCurrentWorkPath()
{
    char buff[1024] = {0};
    char* cpPtr = ::_getcwd(buff,1024);
    if (NULL == cpPtr)
    {
        return "";
    }

    std::string ret(cpPtr);

    if ('\\' != ret.back())
    {
        return ret + '\\';
    }

    return ret;
}

bool CMUPCommon::changeCurrentWorkpath( 
    std::string path )
{
    std::transform(path.begin(),path.end(),path.begin(), CMUPReplaceTag('/','\\'));

    return 0 == _chdir(path.c_str());
}

std::string CMUPCommon::spliteFileName( 
    std::string fullPath )
{
    std::transform(fullPath.begin(),fullPath.end(),fullPath.begin(), CMUPReplaceTag('/','\\'));

    std::string::size_type offset = fullPath.find_last_of('\\');
    if (offset == std::string::npos)
    {
        return fullPath;
    }

    return fullPath.substr(offset+1);
}

std::string CMUPCommon::spliteFilePath( 
    std::string fullPath )
{
    std::transform(fullPath.begin(),fullPath.end(),fullPath.begin(), CMUPReplaceTag('/','\\'));

    std::string::size_type offset = fullPath.find_last_of('\\');
    if (offset == std::string::npos)
    {
        return "";
    }

    return fullPath.substr(0, offset + 1);
}

std::string CMUPCommon::spliteName( 
    std::string fileName )
{
    std::string::size_type offset = fileName.find_last_of('.');
    if (offset == std::string::npos)
    {
        return fileName;
    }

    return fileName.substr(0, offset);
}

std::string CMUPCommon::splitExtension( 
    std::string fileName )
{
    std::string::size_type offset = fileName.find_last_of('.');
    if (offset == std::string::npos)
    {
        return "";
    }

    return fileName.substr(offset+1);

}

std::string CMUPCommon::upperFirstTag( 
    std::string str )
{
    std::string value = str;
    if (!value.empty())
    {
        value[0] = ::toupper(value[0]);
    }

    return value;
}


bool CMUPCommon::isAbsolutePath( 
    std::string path )
{
    if (path.size() < 2)
    {
        return false;
    }

    return (::isalpha(path[0]) && (':' == path[1]));
}


std::string CMUPCommon::verifyPath( 
    std::string path )
{
    path = relativePath2absolutePath(path);

    if (path[path.size() -1] != '\\')
    {
        return path + "\\";
    }

    return path;
}

std::string CMUPCommon::relativePath2absolutePath( 
    std::string path )
{
    assert(!path.empty());
    std::transform(path.begin(), path.end(), path.begin(), CMUPReplaceTag('/','\\'));
    std::string currentPath = getCurrentWorkPath();
    std::string fullPath;
    // ensure absolutePath ect "e:/xxx"
    if (isAbsolutePath(path))
    {
        fullPath = path;
    }
    else 
    {
        fullPath = currentPath + path;
    }

    std::list<std::string> pathList;
    std::string::size_type pos = 0;
    std::string::size_type next = fullPath.find_first_of('\\', pos + 1);
    assert(!fullPath.empty());

    do
    {
        std::string sub = fullPath.substr(pos, next - pos);
        if (".." == sub)
        {
            if (!pathList.empty())
            {
                pathList.pop_back();
            }
        }
        else if ("." == sub)
        {
            // nothing to do
        }
        else
        {
            pathList.push_back(sub);
        }

        pos = next + 1;
        next = fullPath.find_first_of('\\', pos);
        if (next == std::string::npos)
        {
            break;
        }
    } while(pos != std::string::npos);

    if (pos != fullPath.size())
    {
        pathList.push_back(fullPath.substr(pos));
    }

    if (pathList.empty())
    {
        return "";
    }

    std::list<std::string>::const_iterator iter = pathList.begin();
    path = *iter;
    for ( ++iter; iter != pathList.end(); ++iter )
    {
        path += ("\\" + *iter);
    }

    return path;
}

