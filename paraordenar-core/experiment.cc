/**
 * @file experiment.cc
 * @author Marc Clascà Ramírez
 * @brief Conté la implementació de la classe Experiment
 * @version 0.1
 * @date 2022-09-01
 * 
 */

/* experiment.cc */
/**
 * ParaOrdenar
 * Experiment Class
 * GNU General Public License v3.0
 * Author: Marc Clascà Ramírez
 *
 **/

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#include "ParaordenarConfig.h"
#include "experiment.hpp"

using namespace ParaordenarCore;

Experiment::Experiment(std::string name, ExperimentType type, std::string vaultPath) {

     // Comprovem nom experiment
    static const boost::regex e("^\\w+$");
    if(!boost::regex_match(name, e)) {
        throw PROException(ExceptionType::ENameNotValid, "El nom per l'experiment no és vàlid");
    }

    this->name = name;
    this->type = type;
    this->description = "";

    this-> complete_path = vaultPath + "/" + "." + name + ".proexp";


    //writeExperimentDefinition();

}

 Experiment::Experiment(std::string path) {
         // Check if exists
    if(!boost::filesystem::exists(path)) {
        throw PROException(ExceptionType::EStorageNotExists, "No existeix un experiment de Paraordenar al directori definit.");
    }

    //parseExperimentDefinition();

}


const std::string Experiment::get_path() {
    return complete_path;
}
const std::string Experiment::get_name() {
    return name;
}
const std::string Experiment::get_description() {
    return description;
}

const std::string Experiment::get_logfile_name() {
    return log_file;
}


void Experiment::set_description(std::string desc) {
    this->description = desc;
}

void Experiment::set_logfile_name(std::string n) {
    this->log_file = n;
}