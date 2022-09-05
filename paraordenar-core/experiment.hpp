/**
 * @file experiment.hpp
 * @author Marc Clascà Ramírez
 * @brief Conté la definició de la classe Experiment
 * @version 0.1
 * @date 2022-09-01
 * 
 */

/* experiment.hpp */
/**
 * ParaOrdenar
 * Experiment Class
 * GNU General Public License v3.0
 * Author: Marc Clascà Ramírez
 *
 **/
#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <map>


#include "types.h"

namespace ParaordenarCore {

    typedef enum {
        Tracing,
        Walltime
    } ExperimentType;

    typedef enum {
        PrvFile,
        CsvFile,
        LogFile
    } ResourceType;


    /**
     * @brief La classe Experiment representa el resultat d'un experiment
     * 
     * TODO: Escriure descripció
     * 
     * Hi ha diferents tipus d'experiments, que poden contenir
     * un conjunt de resultats/arxius.
     * 
     */
    class Experiment
    {
    protected:

        ExperimentType type;
        std::string name;
        std::string description;

        std::string log_file;

        std::fstream experiment_file;
        std::string complete_path;
        std::map<std::string, std::string> params;

        /**
         * @name Persistència
         * Funcions per escriure i parsejar el
         * contingut xml corresponent al sistema
         * de fitxers.
         */
        ///@{
        virtual void parseExperimentDefinition() = 0;
        virtual void writeExperimentDefinition() = 0;
        ///@}

    public:
        Experiment(std::string name, ExperimentType type, std::string vaultPath);
        Experiment(std::string path);

        /**
         * @name Getters and Setters
         * Funcions per obtenir i posar els valors
         * als membres de la classe Project.
         */
        ///@{
        const std::string    get_path();
        const std::string    get_name();
        const std::string    get_description();
        const std::string    get_logfile_name();
        const ExperimentType get_type();

        void set_description(std::string desc);
        void set_logfile_name(std::string n);
        ///@}

        /**
         * @brief Guarda la informació de l'experiment en disc
         * La implementació depen de la subclasse.
         */
        virtual void save() = 0;

    };

}

