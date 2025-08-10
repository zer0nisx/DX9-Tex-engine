#pragma once

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include <string>
#include <unordered_map>
#include <memory>

class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager();

    // Configuration loading
    bool LoadConfig(const std::string& configFile);
    bool SaveConfig(const std::string& configFile);
    void CreateDefaultConfig();

    // Value retrieval
    std::string GetString(const std::string& section, const std::string& key, const std::string& defaultValue = "") const;
    int GetInt(const std::string& section, const std::string& key, int defaultValue = 0) const;
    float GetFloat(const std::string& section, const std::string& key, float defaultValue = 0.0f) const;
    bool GetBool(const std::string& section, const std::string& key, bool defaultValue = false) const;

    // Value setting
    void SetString(const std::string& section, const std::string& key, const std::string& value);
    void SetInt(const std::string& section, const std::string& key, int value);
    void SetFloat(const std::string& section, const std::string& key, float value);
    void SetBool(const std::string& section, const std::string& key, bool value);

    // Configuration sections
    void CreateSection(const std::string& sectionName);
    bool SectionExists(const std::string& sectionName) const;

    // Utility
    void Clear();
    void PrintConfig() const;

private:
    struct ConfigValue {
        std::string value;
        std::string type; // "string", "int", "float", "bool"
    };

    using ConfigSection = std::unordered_map<std::string, ConfigValue>;
    using ConfigData = std::unordered_map<std::string, ConfigSection>;

    ConfigData m_config;
    std::string m_currentFile;
    mutable rapidxml::xml_document<> m_doc;
    mutable std::unique_ptr<rapidxml::file<>> m_xmlFile;

    // Helper methods
    std::string GetSectionKey(const std::string& section, const std::string& key) const;
    rapidxml::xml_node<>* FindOrCreateSection(rapidxml::xml_node<>* root, const std::string& sectionName);
    rapidxml::xml_node<>* FindOrCreateKey(rapidxml::xml_node<>* section, const std::string& keyName);
    void ParseXmlNode(rapidxml::xml_node<>* node, const std::string& sectionName = "");
};

// Singleton access
extern ConfigManager g_configManager;
