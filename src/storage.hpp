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
#include <map>
#include <vector>
#include <boost/regex.hpp>
#include "pugixml.hpp"
#include "project.hpp"

namespace ParaordenarCore {

class Storage {

    std::string storage_path;
    std::fstream storage_file;
    std::map<std::string, std::string> params;

    std::map<std::string, bool> apps;


    public:
        /**
            Constructor: Quan ja existeix emmagatzematge
        */
        Storage (std::string);

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
        
        std::string get_path();
        void init_path(std::string);

        /**
         * @brief 
         * 
         * @param p Projecte a afegir a l'emmagatzematge
         * @return int 
         */
        int new_app(Project* p);

        /**
         * @brief Obte l'id del projecte passat per parametre
         * 
         * @param key Nom del projecte
         * @return int ID numeric 
         */
        int get_app_id(std::string key);

        /**
         * @brief Get the apps object
         * 
         */
        std::vector<std::pair<std::string, bool>> get_apps();

        //// PERSISTENCE ////
        void writeStorageConfig();
        void parseStorageConfig();

};

}
