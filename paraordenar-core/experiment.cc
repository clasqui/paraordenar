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
    static const boost::regex e("^[\\w-]+$");
    if(!boost::regex_match(name, e)) {
        throw PROException(ExceptionType::ENameNotValid, "El nom per l'experiment no és vàlid. Només pot contenir lletres, números i guions.");
    }

    this->name = name;
    this->type = type;
    this->description = "";
    this->date = boost::gregorian::date(boost::gregorian::day_clock::local_day());
    
    this->base_path = vaultPath;
    this->complete_path = vaultPath + "/" + "." + name + ".proexp";

}

 Experiment::Experiment(std::string path) {
         // Check if exists
    if(!boost::filesystem::exists(path)) {
        throw PROException(ExceptionType::EStorageNotExists, "No existeix un experiment de Paraordenar al directori definit.");
    }

    this->base_path = path;
    base_path.remove_filename();

    this->complete_path = path;

}

void Experiment::save() {
    this->writeExperimentDefinition();
}


const std::filesystem::path Experiment::get_path() {
    return complete_path;
}

const std::filesystem::path Experiment::get_base_path() {
    return base_path;
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

const ExperimentType Experiment::get_type() {
    return type;
}

const boost::gregorian::date Experiment::get_date() {
    return date;
}



void Experiment::set_description(std::string desc) {
    this->description = desc;
}

void Experiment::set_logfile_name(std::string n) {
    this->log_file = n;
}

std::string Experiment::ExperimentTypeNames[2] = {"Traceig", "Temps"};
std::string Experiment::ResourceTypeNames[4] = {"PRV", "CSV", "Registre", "CFG Paraver"};