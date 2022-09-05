/* vault.cc */
/**
 * ParaOrdenar
 * Vault Class
 * GNU General Public License v3.0
 * Author: Marc Clascà Ramírez
 *
 **/

/**
 * @file vault.cc
 * @author Marc Clascà Ramírez
 * @brief Fitxer de la implementació de la classe Vault
 * @version 0.1
 * @date 2022-07-02
 * 
 */

 #include "vault.hpp"
 #include "types.h"
 #include "ParaordenarConfig.h"
 #include <boost/filesystem.hpp>
 #include <boost/regex.hpp>

 #include "trace.hpp"

using namespace ParaordenarCore;

 Vault::Vault(std::string name, std::string description, std::string applicationPath) {

     // Comprovem nom vault
    static const boost::regex e("^\\w+$");
    if(!boost::regex_match(name, e)) {
        throw PROException(ExceptionType::ENameNotValid, "El nom per la caixa no és vàlid");
    }

    this->name = name;
    this->description = description;
    this->conclusions = "";

    this->vault_path = applicationPath +"/"+name;

    if(!boost::filesystem::exists(this->vault_path)) {
        boost::filesystem::create_directory(this->vault_path);
    }

    writeVaultDefinition();

 }

 Vault::Vault(std::string path) {
         // Check if exists
    if(!boost::filesystem::exists(path)) {
        throw PROException(ExceptionType::ENoPath, "No existeix el path");
    }

    this->vault_path = path;
    
    if(!boost::filesystem::exists(path+"/.provault")) {
        throw PROException(ExceptionType::EStorageNotExists, "No existeix una caixa de Paraordenar al directori definit.");
    }

    parseVaultDefinition();

 }

Experiment* Vault::new_experiment(std::string name, ExperimentType t) {
    Experiment *tw;
    if(t == ExperimentType::Tracing) {
        tw = new Trace(name, this->vault_path);
    }

    experiments.insert(std::make_pair(tw->get_name(), tw->get_type()));
}

Experiment* Vault::open_experiment(std::string key) {
    auto el = experiments.find(key);
    if (el == experiments.end())
    {
        return nullptr;
    }
    else
    {
        if(el->second == ExperimentType::Tracing) {
            return new Trace(this->vault_path + "/." + key + ".proexp");
        } else {
            return nullptr;
        }
    }
}

std::map<std::string, ExperimentType> Vault::get_experiments() {
    std::map<std::string, ExperimentType> m;

    for (auto it = this->experiments.begin(); it != this->experiments.end(); it++)
    {
        m.insert(std::make_pair(it->first, it->second));
    }
    return m;
}

void Vault::save() {
    writeVaultDefinition();
}

void Vault::writeVaultDefinition() {
    pugi::xml_document doc;
    pugi::xml_node root = doc.append_child("paraordenar");
    pugi::xml_node config_type = root.append_child("param");
    config_type.append_attribute("name") = "tipus";
    config_type.append_attribute("type") = "string";
    config_type.append_attribute("value") = "VaultDefinition";

    pugi::xml_node config_version = root.append_child("param");
    config_version.append_attribute("name") = "versio";
    config_version.append_attribute("type") = "string";
    config_version.append_attribute("value") = Paraordenar_VERSION_MAJOR+"."+Paraordenar_VERSION_MINOR;

    pugi::xml_node info = root.append_child("info");
    
    pugi::xml_node node_name = info.append_child("name");
    node_name.append_child(pugi::node_pcdata).set_value(name.c_str());
    pugi::xml_node node_description = info.append_child("description");
    node_description.append_child(pugi::node_pcdata).set_value(description.c_str());
    pugi::xml_node node_lanugage = info.append_child("conclusions");
    node_lanugage.append_child(pugi::node_pcdata).set_value(conclusions.c_str());

    pugi::xml_node experiments_node = root.append_child("experiments");

    for (auto it = experiments.begin(); it != experiments.end(); it++)
    {
        std::string name = it->first;

        pugi::xml_node exp_node = experiments_node.append_child("experiment");
        exp_node.append_child(pugi::node_pcdata).set_value(name.c_str());
        exp_node.append_attribute("type") = it->second;
    }

    vault_file.open(vault_path + "/.provault", std::ios::out | std::ios::trunc);
    doc.save(vault_file);
    vault_file.close();

    this->vault_file.open(vault_path + "/.provault", std::ios::in);
}

void Vault::parseVaultDefinition() {
    this->vault_file.open(this->vault_path + "/.provault", std::ios::in);

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load(vault_file);
    if (!result)
        throw PROException(ExceptionType::EXMLParseError, result.description());

    pugi::xml_node root = doc.child("paraordenar");
    pugi::xml_node info = root.child("info");

    this->name = info.child("name").text().get();
    this->description = info.child("description").text().get();

    this->conclusions = info.child("conclusions").text().get();

    for (pugi::xml_node param : root.children("param"))
    {
        params.emplace(param.attribute("name").as_string(), param.attribute("value").as_string());
    }

    if (params.find("tipus") == params.end() || params.find("tipus")->second.compare("VaultDefinition") != 0)
    {
        throw PROException(ExceptionType::EMalformedStorage, "No es troba el parametre identificador de caixa");
    }

    for (pugi::xml_node exp_node : root.child("experiments"))
    {
        experiments.insert(std::make_pair(exp_node.text().get(), ExperimentType(exp_node.attribute("type").as_int())));
    }

    vault_file.close();
}

const std::string Vault::get_name() {
    return name;
}

const std::string Vault::get_path() {
    return vault_path;
}

const std::string Vault::get_description() {
    return description;
}

const std::string Vault::get_conclusions() {
    return conclusions;
}

void Vault::set_conclusions(std::string conclusions) {
    this->conclusions = conclusions;
}

void Vault::set_description(std::string desc) {
    this->description = desc;
}