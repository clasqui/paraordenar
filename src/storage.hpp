#pragma once

/* storage.hpp */
/**
 * ParaOrdenar
 * Storage Class
 * GNU General Public License v3.0
 * Author: Marc Clascà Ramírez
 *
 **/

#include <iostream>
#include <fstream>
#include <string>
#include <boost/regex.hpp>

using namespace std;

class Storage {

    string storage_path;
    fstream storage_file;

    public:
        Storage (string);
        string get_path();
        void get_apps();

};


