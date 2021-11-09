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

using namespace std;

class Project {
    string name;
    string description;

    pugi::xml_document doc;
    fstream app_file;
    string app_path;

    public:
    Project(string);
    Project(string, string, string);

    const string get_path();
    const string get_name();
    const string get_description();

    
};