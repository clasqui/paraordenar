#pragma once

/* vault.hpp */
/**
 * ParaOrdenar
 * Vault Class
 * GNU General Public License v3.0
 * Author: Marc Clascà Ramírez
 *
 **/

/**
 * @file vault.hpp
 * @author Marc Clascà Ramírez
 * @brief Fitxer de la declaració de la classe Vault
 * @version 0.1
 * @date 2022-07-02
 * 
 */

#include <iostream>
#include <fstream>
#include <string>
#include <boost/regex.hpp>

#include "pugixml.hpp"
#include "experiment.hpp"

namespace ParaordenarCore {


    /**
     * @brief Una caixa representa un conjunt d'experiments, relacionats
     * entre ells, dins d'un projecte.
     * 
     * TODO: Escriure descripcio
     *
     * Dins d'una caixa, trobem les traces i els experiments.
     *
     */
    class Vault
    {
        std::string name;
        std::string description;
        std::string conclusions;

        std::map<std::string, ExperimentType> experiments;

        std::fstream vault_file;
        std::string vault_path;
        std::map<std::string, std::string> params;

        /**
         * @name Persistència
         * Funcions per escriure i parsejar el
         * contingut xml corresponent al sistema
         * de fitxers.
         */
        ///@{
        void parseVaultDefinition();
        void writeVaultDefinition();
        ///@}

    public:
        /**
         * @brief Construeix un nou objecte caixa a partir
         *          d'una caixa existent al sistema.
         *
         * @param path path de la caixa.
         */
        Vault(std::string path);

        /**
         * @brief Construeix un nou objecte caixa amb les dades
         *        proporcionades, i l'inclou a l'emmagatzematge indicat
         *
         * @param name Nom de la nova caixa
         * @param description Descripcio de la nova caixa
         * @param storagePath Cami de l'emmagatzematge on s'incloura
         */
        Vault(std::string name, std::string description, std::string applicationPath);

        

        /**
         * @name Getters and Setters
         * Funcions per obtenir i posar els valors
         * als membres de la classe Project.
         */
        ///@{
        const std::string get_path();
        const std::string get_name();
        const std::string get_description();
        const std::string get_conclusions();

        void set_description(std::string desc);
        void set_conclusions(std::string conclusions);
        ///@}
    
        /**
         * @brief Crea un nou experiment dins la caixa
         * Crea un nou objecte d'experiment, i afegeix 
         * l'identificador al vector d'experiments de 
         * la caixa.
         * 
         * @param name Nom del nou experiment
         * @param description Tipus d'experiment
         * @return Punter al nou objecte d'experiment
         */
        Experiment* new_experiment(std::string name, ExperimentType t);

        /**
         * @brief Carrega l'objecte d'un experiment
         * Obté les dades d'un experiment d'aquesta 
         * caixa i crea un objecte i el carrega 
         * en memòria.
         * 
         * @param key Nom de l'experiment
         * @return Punter a l'objecte de l'experiment
         */
        Experiment* open_experiment(std::string key);

        /**
         * @brief Obté la llista d'experiments
         * Fa una còpia del map d'experiments a 
         * un nou map, i el retorna.
         */
        std::map<std::string, ExperimentType> get_experiments();

        /**
         * @brief Guarda la informació de la caixa en disc
         * @todo La implementació actual és molt simple, només 
         * escriu en disc.
         */
        void save();
};

}