# DX9 Engine SDK

## Overview

This directory contains third-party libraries and SDKs integrated into the DX9 Engine project.

## Libraries Included

### RapidXML v1.17
**Location:** `sdk/rapidxml/`
**License:** Boost Software License
**Description:** Fast XML DOM-style parser for C++

#### Files:
- `rapidxml.hpp` - Main RapidXML header with core parsing functionality
- `rapidxml_utils.hpp` - Utility functions for file loading and memory management
- `rapidxml_iterators.hpp` - STL-style iterators for XML nodes
- `rapidxml_print.hpp` - Functions for printing XML back to text

#### Usage:
```cpp
#include "sdk/rapidxml/rapidxml.hpp"
#include "sdk/rapidxml/rapidxml_utils.hpp"

// Example usage
rapidxml::xml_document<> doc;
rapidxml::file<> xmlFile("config.xml");
doc.parse<0>(xmlFile.data());

auto root = doc.first_node();
```

#### Integration:
RapidXML is a header-only library, so no compilation is required. Simply include the appropriate headers in your source files.

#### Use Cases in DX9 Engine:
- Configuration file parsing (materials, effects, settings)
- Scene/level data loading
- Shader parameter definitions
- Asset pipeline configuration

## CMake Integration

The SDK is automatically included in the project via CMakeLists.txt:
- Headers are added to the include path
- No linking required (header-only library)

## Adding New Libraries

To add a new third-party library to the SDK:

1. Create a subdirectory in `sdk/`
2. Extract or copy library files to the subdirectory
3. Update this README.md with library information
4. Update CMakeLists.txt to include headers/libraries as needed
5. Create usage examples if needed
