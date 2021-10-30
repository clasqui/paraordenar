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
#include "pugixml.hpp"

using namespace std;

class Storage {

    string storage_path;
    fstream storage_file;

    pugi::xml_document doc;

    public:
        /**
            Constructor: Quan ja existeix emmagatzematge
        */
        Storage (string);

        /**
            Constructor: Crear emmagatzematge buit
            Si s'inicialitza amb aquest constructor, cal cridar
            la funcio "init_path" en algun moment
        */
        Storage (); 

        /**
            Destructor: guarda xml al fitxer i tanca.
        */
        ~Storage ();
        
        string get_path();
        void get_apps();
        void init_path(string);

};


