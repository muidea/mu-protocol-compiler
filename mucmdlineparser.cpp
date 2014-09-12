#include "mucmdlineparser.h"
#include <sstream>

CMUOptionValueSet::CMUOptionValueSet()
{
}

CMUOptionValueSet::~CMUOptionValueSet()
{
}

bool CMUOptionValueSet::getStringOption( 
    std::string const& name, 
    std::string& value ) const
{
    std::multimap<std::string,std::string>::const_iterator lIter = _options.lower_bound(name);
    std::multimap<std::string,std::string>::const_iterator uIter = _options.upper_bound(name);
    if (lIter == uIter)
    {
        return false;
    }

    value = lIter->second;
    return true;
}

bool CMUOptionValueSet::getStringOptions( 
    std::string const& name,
    std::list<std::string>& valueList ) const
{
    valueList.clear();
    std::multimap<std::string,std::string>::const_iterator lIter = _options.lower_bound(name);
    std::multimap<std::string,std::string>::const_iterator uIter = _options.upper_bound(name);
    for(; lIter != uIter; ++lIter)
    {
        valueList.push_back(lIter->second);
    }

    return !valueList.empty();
}

bool CMUOptionValueSet::getIntOption( 
    std::string const& name, 
    int& value ) const
{
    std::multimap<std::string,std::string>::const_iterator lIter = _options.lower_bound(name);
    std::multimap<std::string,std::string>::const_iterator uIter = _options.upper_bound(name);
    if (lIter == uIter)
    {
        return false;
    }

    value = ::atoi(lIter->second.c_str());

    return true;
}

bool CMUOptionValueSet::getIntOptions( 
    std::string const& name, 
    std::list<int>& valueList ) const
{
    valueList.clear();
    std::multimap<std::string,std::string>::const_iterator lIter = _options.lower_bound(name);
    std::multimap<std::string,std::string>::const_iterator uIter = _options.upper_bound(name);
    for(; lIter != uIter; ++lIter)
    {
        valueList.push_back(::atoi(lIter->second.c_str()));
    }

    return !valueList.empty();
}

void CMUOptionValueSet::appendOption( 
    std::string const& name, 
    std::string const& value )
{
    _options.insert(std::pair<std::string,std::string>(name, value));
}

void CMUOptionValueSet::reset()
{
    _options.clear();
}

CMUCmdLineParser::CMUCmdLineParser(void)
{
}

CMUCmdLineParser::~CMUCmdLineParser(void)
{
}


bool CMUCmdLineParser::defineOption( 
    std::string const& name, 
    std::string const& description, 
    int attribute /* = OptionOptional */ )
{
    StringSet::iterator nIter = _optionSet.find(name);
    if (nIter != _optionSet.end())
    {
        return false;
    }

    _optionSet.insert(name);
    _option2Description[name] = description;
    _option2Attribute[name] = attribute;

    return true;
}

bool CMUCmdLineParser::parser( 
    int argc, 
    const char* argv[] )
{
    bool returnValue = true;

    _optionValueSet.reset();

    for ( int ii =0; ii < argc; ++ii)
    {
        if (!isKey(argv[ii]))
        {
            continue;
        }

        std::string key(argv[ii++] + 1);
        std::string content;
        do 
        {
            if ((ii < argc) 
                && !isKey(argv[ii]))
            {
                content = argv[ii];
                break;
            }

            String2IntMap::iterator iIter = _option2Attribute.find(key);
            if (iIter != _option2Attribute.end())
            {
                if (OptionRequiresValue == iIter->second)
                {
                    returnValue = false;
                    break;
                }
            }

            content = "1";
        } while (0);

        if (key.empty() || content.empty())
        {
            continue;
        }

        _optionSet.erase(key);

        _optionValueSet.appendOption(key, content);
    }

    if (_optionSet.empty())
    {
        return returnValue;
    }

    StringSet::const_iterator cIter = _optionSet.begin();
    for ( ; cIter != _optionSet.end(); ++cIter )
    {
        if (OptionOptional != _option2Attribute[*cIter])
        {
            returnValue = false;
        }
    }

    return returnValue;
}

std::string CMUCmdLineParser::usageDescription( 
    std::string const& applicationName ) const
{
    std::stringstream ss;

    if (!_option2Description.empty())
    {
        ss << "\nUsage: " << applicationName << " [<Options>...]" << std::endl;

        ss << "<Options>" << std::endl;
        String2StringMap::const_iterator sIter = _option2Description.begin();
        for ( ; sIter != _option2Description.end(); ++sIter )
        {
            ss << "    /" << sIter->first << ": " << sIter->second << std::endl;
        }
    }

    return ss.str();
}

bool CMUCmdLineParser::getStringOption( 
    std::string const& name, 
    std::string& value ) const
{
    return _optionValueSet.getStringOption(name, value);
}

bool CMUCmdLineParser::getStringOptions( 
    std::string const& name, 
    std::list<std::string>& valueList ) const
{
    return _optionValueSet.getStringOptions(name, valueList);
}

bool CMUCmdLineParser::getIntOption( 
    std::string const& name, 
    int& value ) const
{
    return _optionValueSet.getIntOption(name, value);
}

bool CMUCmdLineParser::getIntOptions( 
    std::string const& name, 
    std::list<int>& valueList ) const
{
    return _optionValueSet.getIntOptions(name, valueList);
}

const CMUOptionValueSet& CMUCmdLineParser::getOptionSet() const
{
    return _optionValueSet;
}

std::string CMUCmdLineParser::trim( 
    std::string const& content, 
    char flag ) const
{
    if (content.empty())
    {
        return "";
    }

    std::string::size_type begin = content.find_first_not_of(flag);
    begin = std::string::npos == begin ? 0 : begin;
    std::string::size_type end = content.find_last_not_of(flag);
    end = std::string::npos == end ? content.size() - 1 : end;
    return content.substr(begin, end - begin + 1);
}

bool CMUCmdLineParser::isKey( 
    std::string const& content ) const
{
    if (content.empty())
    {
        return false;
    }

    return '/' == content[0];
}

void CMUCmdLineParser::reset()
{
    _optionSet.clear();
    _option2Description.clear();
    _option2Attribute.clear();

    _optionValueSet.reset();
}

