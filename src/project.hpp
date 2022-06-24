#pragma once

/* project.hpp */
/**
 * ParaOrdenar
 * Project Class
 * GNU General Public License v3.0
 * Author: Marc Clascà Ramírez
 *
 **/

#include <iostream>
#include <fstream>
#include <string>
#include <boost/regex.hpp>
#include "pugixml.hpp"

namespace ParaordenarCore {

class Project {
    std::string name;
    std::string description;
    bool active;

    pugi::xml_document doc;
    std::fstream app_file;
    std::string app_path;

    public:
    Project(std::string);
    Project(std::string, std::string, std::string);

    const std::string get_path();
    const std::string get_name();
    const std::string get_description();
    const bool        get_active();

    
};

}