#ifndef __MU_CMDLINE_PARSER_H_2324__
#define __MU_CMDLINE_PARSER_H_2324__
#include <string>
#include <list>
#include <map>
#include <set>

class CMUCmdLineParser;

class CMUOptionValueSet
{
public:
    CMUOptionValueSet();

    virtual ~CMUOptionValueSet();

    bool getStringOption( 
        std::string const& name, 
        std::string& value ) const;

    bool getStringOptions( 
        std::string const& name, 
        std::list<std::string>& valueList ) const;

    bool getIntOption( 
        std::string const& name, 
        int& value ) const;

    bool getIntOptions( 
        std::string const& name, 
        std::list<int>& valueList ) const;

    void reset( );

protected:
    void appendOption( 
        std::string const& name, 
        std::string const& value );

private:
    friend class CMUCmdLineParser;

    std::multimap<std::string,std::string> _options;
};

class CMUCmdLineParser
{
public:
    CMUCmdLineParser(void);

    virtual ~CMUCmdLineParser(void);

    enum
    {
        OptionOptional = 0x00,
        OptionRequiresValue = 0x01,
        OptionRequired = 0x02
    };

    bool defineOption( 
        std::string const& name, 
        std::string const& description, 
        int attribute = OptionOptional );

    bool parser( 
        int argc, 
        const char* argv[] );

    std::string usageDescription( 
        std::string const& applicationName ) const;

    bool getStringOption( 
        std::string const& name, 
        std::string& value ) const;

    bool getStringOptions( 
        std::string const& name, 
        std::list<std::string>& valueList ) const;

    bool getIntOption( 
        std::string const& name, 
        int& value ) const;

    bool getIntOptions( 
        std::string const& name, 
        std::list<int>& valueList ) const;

    const CMUOptionValueSet& getOptionSet( ) const;

    void reset( );

protected:
    std::string trim( 
        std::string const& content, 
        char flag ) const;

    bool isKey( 
        std::string const& content ) const;

protected:
    typedef std::map<std::string, std::string> String2StringMap;
    typedef std::map<std::string, int> String2IntMap;
    typedef std::set<std::string> StringSet;

    StringSet _optionSet;
    String2StringMap _option2Description;
    String2IntMap _option2Attribute;
    CMUOptionValueSet _optionValueSet;
};

#endif // __MU_CMDLINE_PARSER_H_2324__

