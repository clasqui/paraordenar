/**
 * @file trace.hpp
 * @author Marc Clascà
 * @brief Conté la definició de la subclasse Trace
 * @version 0.1
 * @date 2022-09-01
 * 
 * @copyright Copyright (c) 2022
 * 
 */

/* trace.hpp */
/**
 * ParaOrdenar
 * Experiment Class
 * GNU General Public License v3.0
 * Author: Marc Clascà Ramírez
 *
 **/

#pragma once

#include <set>

#include "experiment.hpp"
#include "types.h"

namespace ParaordenarCore {

    /**
     * @brief La classe Trace es una subclasse de Experiment que representa una traça
     * 
     * TODO: descripcio
     * 
     */
    class Trace : Experiment
    {
    private:
        int omp_threads;
        int mpi_tasks;

        std::set<std::string> prv_resources;

    protected:
        /**
         * @name Persistència
         * Funcions per escriure i parsejar el
         * contingut xml corresponent al sistema
         * de fitxers.
         */
        ///@{
        void parseExperimentDefinition();
        void writeExperimentDefinition();
        ///@}


    public:
        Trace(std::string name, std::string vaultPath);
        Trace(std::string path);


        /**
         * @name Getters and Setters
         * Funcions per obtenir i posar els valors
         * als membres de la classe Project.
         */
        ///@{
            const int get_omp_threads();
            const int get_mpi_tasks();

            void set_omp_threads(int t);
            void set_mpi_tasks(int t);
        ///@}

        /**
         * @brief Guarda la informació de l'experiment en disc
         * La implementació ara mateix només crida la funció 
         * d'escriure en disc l'arxiu XML.
         */
        void save();

    };

}

