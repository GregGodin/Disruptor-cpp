#include "stdafx.h"
#include "TypeInfo.h"

#include <regex>
#include <vector>

#if defined(__GNUC__)
# include <cxxabi.h>
#endif /* __GNUC__ */


namespace Disruptor
{

    TypeInfo::TypeInfo(const std::type_info& typeInfo)
        : m_typeInfo(&typeInfo)
        , m_fullyQualifiedName(dotNetify(demangleTypeName(m_typeInfo->name())))
        , m_name(unqualifyName(m_fullyQualifiedName))
    {
    }

    const std::type_info& TypeInfo::intrinsicTypeInfo() const
    {
        return *m_typeInfo;
    }

    const std::string& TypeInfo::fullyQualifiedName() const
    {
        return m_fullyQualifiedName;
    }

    const std::string& TypeInfo::name() const
    {
        return m_name;
    }

    bool TypeInfo::operator==(const TypeInfo& rhs) const
    {
        return intrinsicTypeInfo() == rhs.intrinsicTypeInfo();
    }

    std::string TypeInfo::dotNetify(std::string typeName)
    {
        auto pos = typeName.find("::");

        while (pos != std::string::npos)
        {
            typeName.replace(pos, 2, ".");
            pos = typeName.find("::", pos + 1);
        }

        return typeName;
    }

    std::string TypeInfo::unqualifyName(std::string fullyQualifiedName)
    {
        if (fullyQualifiedName.empty())
            return std::string();

        std::vector< std::string > nameParts;
        auto pos = fullyQualifiedName.find('.');
        while (pos != std::string::npos)
        {
            nameParts.push_back(fullyQualifiedName.substr(0, pos));
            fullyQualifiedName = fullyQualifiedName.substr(pos + 1);
            pos = fullyQualifiedName.find('.');
        }
        nameParts.push_back(fullyQualifiedName);

        if (nameParts.empty())
            return std::string();

        return nameParts[nameParts.size() - 1];
    }

    std::string TypeInfo::demangleTypeName(std::string typeName)
    {
#if defined(__GNUC__)
            int status;

            auto demangledName = abi::__cxa_demangle(typeName.c_str(), 0, 0, &status);
            if (demangledName == nullptr)
                return typeName;

            std::string result = demangledName;
            free(demangledName);
            return result;
#else
        typeName = std::regex_replace(typeName, std::regex("(const\\s+|\\s+const)"), std::string());
        typeName = std::regex_replace(typeName, std::regex("(volatile\\s+|\\s+volatile)"), std::string());
        typeName = std::regex_replace(typeName, std::regex("(static\\s+|\\s+static)"), std::string());
        typeName = std::regex_replace(typeName, std::regex("(class\\s+|\\s+class)"), std::string());
        typeName = std::regex_replace(typeName, std::regex("(struct\\s+|\\s+struct)"), std::string());
        return typeName;
#endif /* defined(__GNUC__) */
    }

} // namespace Disruptor
