#include "ConfigManager.h"
#include "rapidxml/rapidxml_print.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

// Global instance
ConfigManager g_configManager;

ConfigManager::ConfigManager()
    : m_currentFile("")
{
}

ConfigManager::~ConfigManager()
{
    Clear();
}

bool ConfigManager::LoadConfig(const std::string& configFile)
{
    try
    {
        m_currentFile = configFile;
        m_xmlFile = std::make_unique<rapidxml::file<>>(configFile.c_str());

        m_doc.clear();
        m_doc.parse<0>(m_xmlFile->data());

        // Parse XML into internal structure
        m_config.clear();
        auto root = m_doc.first_node("Configuration");
        if (!root)
        {
            std::cerr << "Invalid config file: missing Configuration root node" << std::endl;
            return false;
        }

        // Parse all sections
        for (auto section = root->first_node(); section; section = section->next_sibling())
        {
            std::string sectionName = section->name();
            ParseXmlNode(section, sectionName);
        }

        std::cout << "Loaded configuration from: " << configFile << std::endl;
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to load config file '" << configFile << "': " << e.what() << std::endl;
        CreateDefaultConfig();
        return false;
    }
}

bool ConfigManager::SaveConfig(const std::string& configFile)
{
    try
    {
        // Create new XML document
        rapidxml::xml_document<> doc;

        // Create root node
        auto root = doc.allocate_node(rapidxml::node_element, "Configuration");
        doc.append_node(root);

        // Create sections and keys
        for (const auto& section : m_config)
        {
            auto sectionNode = doc.allocate_node(rapidxml::node_element,
                                               doc.allocate_string(section.first.c_str()));
            root->append_node(sectionNode);

            for (const auto& keyValue : section.second)
            {
                auto keyNode = doc.allocate_node(rapidxml::node_element,
                                               doc.allocate_string(keyValue.first.c_str()));
                auto valueText = doc.allocate_string(keyValue.second.value.c_str());
                keyNode->value(valueText);

                // Add type attribute
                auto typeAttr = doc.allocate_attribute("type",
                                                     doc.allocate_string(keyValue.second.type.c_str()));
                keyNode->append_attribute(typeAttr);

                sectionNode->append_node(keyNode);
            }
        }

        // Write to file
        std::ofstream file(configFile);
        if (!file.is_open())
        {
            std::cerr << "Failed to open config file for writing: " << configFile << std::endl;
            return false;
        }

        file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        file << doc;
        file.close();

        m_currentFile = configFile;
        std::cout << "Saved configuration to: " << configFile << std::endl;
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to save config file '" << configFile << "': " << e.what() << std::endl;
        return false;
    }
}

void ConfigManager::CreateDefaultConfig()
{
    Clear();

    // Engine settings
    SetString("Engine", "Title", "DX9 Engine - Advanced Texture System");
    SetInt("Engine", "Width", 1024);
    SetInt("Engine", "Height", 768);
    SetBool("Engine", "Fullscreen", false);
    SetBool("Engine", "VSync", true);

    // Graphics settings
    SetString("Graphics", "Adapter", "Primary");
    SetInt("Graphics", "MultiSampleLevel", 4);
    SetBool("Graphics", "EnableAnisotropicFiltering", true);
    SetInt("Graphics", "MaxAnisotropy", 16);

    // Texture settings
    SetString("Textures", "DefaultTextureFilter", "Linear");
    SetInt("Textures", "MaxTextureSize", 2048);
    SetBool("Textures", "GenerateMipmaps", true);
    SetFloat("Textures", "LodBias", 0.0f);

    // Performance settings
    SetInt("Performance", "MaxEffectsPerFrame", 10);
    SetFloat("Performance", "TargetFrameRate", 60.0f);
    SetBool("Performance", "EnableProfiling", false);

    std::cout << "Created default configuration" << std::endl;
}

std::string ConfigManager::GetString(const std::string& section, const std::string& key, const std::string& defaultValue) const
{
    auto sectionIt = m_config.find(section);
    if (sectionIt == m_config.end())
        return defaultValue;

    auto keyIt = sectionIt->second.find(key);
    if (keyIt == sectionIt->second.end())
        return defaultValue;

    return keyIt->second.value;
}

int ConfigManager::GetInt(const std::string& section, const std::string& key, int defaultValue) const
{
    std::string value = GetString(section, key);
    if (value.empty())
        return defaultValue;

    try
    {
        return std::stoi(value);
    }
    catch (...)
    {
        return defaultValue;
    }
}

float ConfigManager::GetFloat(const std::string& section, const std::string& key, float defaultValue) const
{
    std::string value = GetString(section, key);
    if (value.empty())
        return defaultValue;

    try
    {
        return std::stof(value);
    }
    catch (...)
    {
        return defaultValue;
    }
}

bool ConfigManager::GetBool(const std::string& section, const std::string& key, bool defaultValue) const
{
    std::string value = GetString(section, key);
    if (value.empty())
        return defaultValue;

    // Convert to lowercase for comparison
    std::string lowerValue = value;
    std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);

    return (lowerValue == "true" || lowerValue == "1" || lowerValue == "yes");
}

void ConfigManager::SetString(const std::string& section, const std::string& key, const std::string& value)
{
    m_config[section][key] = {value, "string"};
}

void ConfigManager::SetInt(const std::string& section, const std::string& key, int value)
{
    m_config[section][key] = {std::to_string(value), "int"};
}

void ConfigManager::SetFloat(const std::string& section, const std::string& key, float value)
{
    m_config[section][key] = {std::to_string(value), "float"};
}

void ConfigManager::SetBool(const std::string& section, const std::string& key, bool value)
{
    m_config[section][key] = {value ? "true" : "false", "bool"};
}

void ConfigManager::CreateSection(const std::string& sectionName)
{
    if (!SectionExists(sectionName))
    {
        m_config[sectionName] = ConfigSection();
    }
}

bool ConfigManager::SectionExists(const std::string& sectionName) const
{
    return m_config.find(sectionName) != m_config.end();
}

void ConfigManager::Clear()
{
    m_config.clear();
    m_doc.clear();
    m_xmlFile.reset();
    m_currentFile.clear();
}

void ConfigManager::PrintConfig() const
{
    std::cout << "=== Configuration ===" << std::endl;
    for (const auto& section : m_config)
    {
        std::cout << "[" << section.first << "]" << std::endl;
        for (const auto& keyValue : section.second)
        {
            std::cout << "  " << keyValue.first << " = " << keyValue.second.value
                      << " (" << keyValue.second.type << ")" << std::endl;
        }
        std::cout << std::endl;
    }
}

void ConfigManager::ParseXmlNode(rapidxml::xml_node<>* node, const std::string& sectionName)
{
    if (!node)
        return;

    // Parse all child nodes as key-value pairs
    for (auto child = node->first_node(); child; child = child->next_sibling())
    {
        std::string keyName = child->name();
        std::string value = child->value() ? child->value() : "";

        // Get type from attribute
        std::string type = "string";
        auto typeAttr = child->first_attribute("type");
        if (typeAttr)
        {
            type = typeAttr->value();
        }

        m_config[sectionName][keyName] = {value, type};
    }
}
