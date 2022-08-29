#pragma once

/* project.hpp */
/**
 * ParaOrdenar
 * Project Class
 * GNU General Public License v3.0
 * Author: Marc Clascà Ramírez
 *
 **/

/**
 * @file project.hpp
 * @author Marc Clascà Ramírez
 * @brief Fitxer de la declaració de la classe Project
 * @version 0.1
 * @date 2022-07-02
 * 
 */

#include <iostream>
#include <fstream>
#include <string>
#include <boost/regex.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include "pugixml.hpp"
#include "vault.hpp"

namespace ParaordenarCore {


    /**
     * @brief Un projecte representa el nivell d'ordenació bàsic
     *  en el sistema de Paraordenar.
     *
     * Dins de qualsevol emmagatzematge trobem projectes. Cada projecte
     * engloba una aplicació o altre tipus de software, sobre el que
     * s'esta fent un treball de performance analysis amb uns objectius.
     *
     * Un projecte té un nom, una descripció, una data d'inici i possible
     * data de finalització, i un llenguatge de programació. Un projecte pot
     * estar actiu o inactiu. Quan està inactiu, el directori del projecte
     * es comprimeix.
     *
     * Dins d'un projecte, les traces s'ordenen per caixes.
     *
     */
    class Project
    {
        std::string name;
        std::string description;
        boost::gregorian::date d_inici;
        boost::gregorian::date d_fin;
        std::string language;
        bool active;

        std::set<std::string> vaults;

        std::fstream app_file;
        std::string app_path;
        std::map<std::string, std::string> params;

        /**
         * @name Persistència
         * Funcions per escriure i parsejar el
         * contingut xml corresponent al sistema
         * de fitxers.
         */
        ///@{
        void parseProjectDefinition();
        void writeProjectDefinition();
        ///@}

    public:
        /**
         * @brief Construeix un nou projecte a partir
         *          d'un projecte existent al sistema.
         *
         * @param path path del projecte.
         */
        Project(std::string path);

        /**
         * @brief Construeix un nou projecte amb les dades
         *        proporcionades, i l'inclou a l'emmagatzematge indicat
         *
         * @param name Nom del nou projecte
         * @param description Descripcio del nou projecte
         * @param storagePath Cami de l'emmagatzematge on s'incloura
         */
        Project(std::string name, std::string description, std::string storagePath);


        /**
         * @brief Crea una nova caixa dins el projecte
         * Crea un nou objecte de caixa, i afegeix l'identificador
         * al vector de caixes del projecte.
         * @param name Nom de la nova caixa
         * @param description Descripció de la caixa
         * @return Punter a la nova caixa
         */
        Vault* new_vault(std::string name, std::string description);

        /**
         * @brief Carrega l'objecte d'una caixa
         * Obté les dades d'una caixa d'aquest projecte i
         * crea un objecte i el carrega en memòria.
         * 
         * @param key Nom de la caixa
         * @return Punter a l'objecte de la caixa
         */
        Vault* open_vault(std::string key);


        /**
         * @brief Get the list of vaults
         * Obtains a copy of the set containing the 
         * string keys of the vaults of this project.
         * 
         * @return copy of the list of vaults
         */
        std::set<std::string> get_vaults();
        

        /**
         * @name Getters and Setters
         * Funcions per obtenir i posar els valors
         * als membres de la classe Project.
         */
        ///@{
        const std::string get_path();
        const std::string get_name();
        const std::string get_description();
        const std::string get_language();
        const bool get_active();
        const boost::gregorian::date get_inici();
        const boost::gregorian::date get_final();

        void set_description(std::string desc);
        void set_active(bool active);
        void set_language(std::string lang);
        void set_dInici(boost::gregorian::date d);
        void set_dFinal(boost::gregorian::date d);
        ///@}
    

        /**
         * @brief Guarda la informació del projecte en disc
         * @todo La implementació actual és molt simple, només 
         * escriu en disc. Cal millorar la implementació per 
         * diferenciar entre projectes arxivats o actius.
         */
        void save();
};

}