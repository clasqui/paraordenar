/**
 * @file trace.cc
 * @author Marc Clascà Ramírez
 * @brief Conté la implementació de la subclasse Trace
 * @version 0.1
 * @date 2022-09-01
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "trace.hpp"

#include "pugixml.hpp"
#include "ParaordenarConfig.h"

using namespace ParaordenarCore;

Trace::Trace(std::string name, std::string vaultPath) : 
    Experiment::Experiment(name, ExperimentType::Tracing, vaultPath) {
        // Aqui posem els valors especifics de la subclasse trace

    this->mpi_tasks = 1;
    this->omp_threads = 1;

    this->writeExperimentDefinition();
}



Trace::Trace(std::string path) :
    Experiment::Experiment(path) {

    parseExperimentDefinition();
}

void Trace::writeExperimentDefinition() {
    pugi::xml_document doc;
    pugi::xml_node root = doc.append_child("paraordenar");
    pugi::xml_node config_type = root.append_child("param");
    config_type.append_attribute("name") = "tipus";
    config_type.append_attribute("type") = "string";
    config_type.append_attribute("value") = "ExperimentDefinition";

    pugi::xml_node config_version = root.append_child("param");
    config_version.append_attribute("name") = "versio";
    config_version.append_attribute("type") = "string";
    config_version.append_attribute("value") = Paraordenar_VERSION_MAJOR+"."+Paraordenar_VERSION_MINOR;

    pugi::xml_node info = root.append_child("info");
    
    pugi::xml_node node_name = info.append_child("name");
    node_name.append_child(pugi::node_pcdata).set_value(name.c_str());
    pugi::xml_node node_description = info.append_child("description");
    node_description.append_child(pugi::node_pcdata).set_value(description.c_str());
    pugi::xml_node node_type = info.append_child("type");
    node_type.append_child(pugi::node_pcdata).set_value(std::to_string(type).c_str());
    pugi::xml_node node_date = info.append_child("date");
    node_date.append_child(pugi::node_pcdata).set_value(boost::gregorian::to_iso_string(date).c_str());

    pugi::xml_node node_threads = info.append_child("threads");
    node_threads.append_child(pugi::node_pcdata).set_value(std::to_string(omp_threads).c_str());
    pugi::xml_node node_tasks = info.append_child("tasks");
    node_tasks.append_child(pugi::node_pcdata).set_value(std::to_string(mpi_tasks).c_str());

    pugi::xml_node resources = root.append_child("resources");

    for (auto it = prv_resources.begin(); it != prv_resources.end(); it++)
    {
        std::string name = it->first;

        pugi::xml_node prv_node = resources.append_child("res");
        prv_node.append_child(pugi::node_pcdata).set_value(name.c_str());
        prv_node.append_attribute("type") = ResourceType::PrvFile;
        prv_node.append_attribute("label") = it->second.label.c_str();
        prv_node.append_attribute("n_threads") = it->second.n_threads;
        prv_node.append_attribute("n_ranks") = it->second.n_ranks;
    }

    pugi::xml_node log_node = resources.append_child("res");
    log_node.append_child(pugi::node_pcdata).set_value(this->log_file.c_str());
    log_node.append_attribute("type") = ResourceType::LogFile;

    experiment_file.open(complete_path, std::ios::out | std::ios::trunc);
    doc.save(experiment_file);
    experiment_file.close();

    this->experiment_file.open(complete_path, std::ios::in);
}

void Trace::parseExperimentDefinition() {
    this->experiment_file.open(this->complete_path, std::ios::in);

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load(experiment_file);
    if (!result)
        throw PROException(ExceptionType::EXMLParseError, result.description());

    pugi::xml_node root = doc.child("paraordenar");
    pugi::xml_node info = root.child("info");

    this->name = info.child("name").text().get();
    this->description = info.child("description").text().get();

    this->type = ExperimentType(info.child("type").text().as_int());
    if(this->type != ExperimentType::Tracing) {
        throw PROException(ExceptionType::EMalformedStorage, "L'experiment no es una traça");
    }

    this->date = info.child("date") ? 
        boost::gregorian::from_undelimited_string(info.child("date").text().get()) : 
        boost::gregorian::date(1970, 1, 1);
    
    this->omp_threads = info.child("threads").text().as_int();
    this->mpi_tasks = info.child("tasks").text().as_int();


    for (pugi::xml_node param : root.children("param"))
    {
        params.emplace(param.attribute("name").as_string(), param.attribute("value").as_string());
    }

    if (params.find("tipus") == params.end() || params.find("tipus")->second.compare("ExperimentDefinition") != 0)
    {
        throw PROException(ExceptionType::EMalformedStorage, "No es troba el parametre identificador d'experiment");
    }

    for (pugi::xml_node res_node : root.child("resources"))
    {
        ResourceType t = ResourceType(res_node.attribute("type").as_int());
        if(t == ResourceType::PrvFile) {
            prv_metadata meta = {
                res_node.attribute("label").as_string(),
                res_node.attribute("n_threads").as_int(), 
                res_node.attribute("n_ranks").as_int()
            };
            std::pair<std::string, struct prv_metadata> entry = std::make_pair(res_node.text().get(), meta);
            prv_resources.insert(entry);
        } else if (t == ResourceType::LogFile) {
            this->log_file = res_node.text().get();
        }
    }

    experiment_file.close();
}

void Trace::add_prv(std::string key, std::string label, int nthr, int nmpi) {
    prv_metadata meta = {
        label,
        nthr, 
        nmpi
    };
    std::pair<std::string, struct prv_metadata> entry = std::make_pair(key, meta);
    prv_resources.insert(entry);
}

const prv_dict_t Trace::get_list_prv() {
    return prv_resources;
}


void Trace::save() {
    this->writeExperimentDefinition();
}

const int Trace::get_omp_threads() {
    return omp_threads;
}
const int Trace::get_mpi_tasks() {
    return mpi_tasks;
}

void Trace::set_omp_threads(int t) {
    this->omp_threads = t;
}
void Trace::set_mpi_tasks(int t) {
    this->mpi_tasks = t;
}