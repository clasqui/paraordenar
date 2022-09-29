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

#include <map>

#include "experiment.hpp"
#include "types.h"

namespace ParaordenarCore {

    struct prv_metadata {
        std::string label;
        int n_threads;
        int n_ranks;
    };

    typedef std::map<std::string, struct prv_metadata> prv_dict_t;

    /**
     * @brief La classe Trace es una subclasse de Experiment que representa una traça
     * 
     * TODO: descripcio
     * 
     */
    class Trace : public Experiment
    {
    private:
        int omp_threads;
        int mpi_tasks;

        //std::set<std::string> prv_resources;
        prv_dict_t prv_resources;

    protected:
        /**
         * @name Persistència
         * Funcions per escriure i parsejar el
         * contingut xml corresponent al sistema
         * de fitxers.
         */
        ///@{
        virtual void parseExperimentDefinition();
        virtual void writeExperimentDefinition();
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
         * @brief Afegeix un recurs PRV a la llista de l'experiment de traceig
         * 
         * @param key nom del recurs (nom de l'arxiu)
         * @param label etiqueta del prv
         * @param nthr numero de threads (valor per defecte si=0)
         * @param nmpi numero de mpi ranks (valor per defecte si=0)
         */
        void add_prv(std::string key, std::string label, int nthr, int nmpi);

        /**
         * @brief Treu un recurs PRV de la llista de l'experiment de traceig
         * 
         * @param key nom del recurs (nom de l'arxiu)
         * @param fs Elimina o no els arxius dels sistema de fitxers
         * 
         * @return 0 si s'ha eliminat correctament, 1 si no existia, 2 si s'ha eliminat de la llista pero no del sistema de fitxers
         * 
         * @todo Implementa eliminar fitxers
         */
        int rem_prv(std::string key, bool fs);

        /**
         * @brief Obté la llista de recursos PRV
         * 
         * @return prv_dict_t 
         */
        const prv_dict_t get_list_prv();

        /**
         * @brief Guarda la informació de l'experiment en disc
         * La implementació ara mateix només crida la funció 
         * d'escriure en disc l'arxiu XML.
         */
        virtual void save();

    };

}

