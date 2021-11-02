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
    string path;

    string description;

    public:
    Project::Project(string);
    Project::Project(string, string);

    
}