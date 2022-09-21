#pragma once

/* storage.hpp */
/**
 * ParaOrdenar
 * Storage Class
 * GNU General Public License v3.0
 **/

/**
 * @file storage.hpp
 * @author Marc Clascà Ramírez
 * @brief Fitxer de la declaració de la classe Storage
 * @version 0.1
 * @date 2022-07-02
 *
 */

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <boost/regex.hpp>
#include "pugixml.hpp"
#include "project.hpp"

/**
 * @brief El namespace ParaordenarCore inclou
 * el codi base per a la llibreria principal de 
 * Paraordenar.  Conté les classes i mètodes 
 * que serveixen com a kernel pel funcionament de
 * frontends que treballin amb paraodenar.
 * 
 */
namespace ParaordenarCore {


/**
 * @brief Representa el nivell més alt en la jerarquia 
 *          d'emmagatzematge del sistema paraordenar.
 * 
 * Un emmagatzematge és el punt bàsic d'entrada per 
 * l'aplicació: a partir de l'emmagatzematge es 
 * construeix l'arbre de recursos.  Per això, és 
 * imprescindible que l'usuari configuri un emmagatzematge
 * paraordenar al sistema.
 * 
 */
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
        
        /**
         * @brief Obte el cami de l'emmagatzematge al sistema de fitxers.
         * 
         * @return El cami en format string
         */
        std::string get_path();

        /**
         * @brief Inicialitza l'emmagatzematge de paraordenar
         *          al cami indicat del sistema de fitxers.
         * 
         * @param path Cami on inicialitzar
         */
        void init_path(std::filesystem::path path);

        /**
         * @brief Crea un nou projecte dins l'emmagatzematge
         * 
         * @param name Nom del nou projecte 
         * @param description Descripcio del nou projecte
         * @return Punter al nou projecte
         */
        Project* new_app(std::string name, std::string description);

        /**
         * @brief Retorna l'objecte pel projecte indicat dins l'emmagatzematge.
         * 
         * @param key Nom del projecte
         * @return Punter la projecte
         */
        Project* open_app(std::string key);

        /**
         * @brief Obte l'id del projecte passat per parametre
         * 
         * @param key Nom del projecte
         * @return int ID numeric 
         */
        int get_app_id(std::string key);

        /**
         * @brief Get the list of apps
         * 
         */
        std::vector<std::pair<std::string, bool>> get_apps();

        //// PERSISTENCE ////
        void writeStorageConfig();
        void parseStorageConfig();

};

}
