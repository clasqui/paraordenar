/* project.cc */
/**
 * ParaOrdenar
 * Project Class
 * GNU General Public License v3.0
 * Author: Marc Clascà Ramírez
 *
 **/

/**
 * @file project.cc
 * @author Marc Clascà Ramírez
 * @brief Fitxer de la implementació de la classe Project
 * @version 0.1
 * @date 2022-07-02
 * 
 */

 #include "project.hpp"
 #include "types.h"
 #include "ParaordenarConfig.h"
 #include <boost/filesystem.hpp>
 #include <boost/regex.hpp>

using namespace ParaordenarCore;

 Project::Project(std::string name, std::string description, std::string storagePath) {

     // Comprovem nom projecte
    static const boost::regex e("^\\w+$");
    if(!boost::regex_match(name, e)) {
        throw PROException(ExceptionType::ENameNotValid, "El nom pel projecte no és vàlid");
    }

    this->name = name;
    this->description = description;
    this->language = "";

    this->d_inici = boost::gregorian::date(boost::gregorian::day_clock::local_day());
    this->d_fin = boost::gregorian::date(1970, 1, 1);

    this->app_path = storagePath +"/"+name;

    if(!boost::filesystem::exists(this->app_path)) {
        boost::filesystem::create_directory(this->app_path);
    }

    writeProjectDefinition();

 }

 Project::Project(std::string path) {
         // Check if exists
    if(!boost::filesystem::exists(path)) {
        throw PROException(ExceptionType::ENoPath, "No existeix el path");
    }

    this->app_path = path;
    
    if(!boost::filesystem::exists(path+"/.proapp")) {
        throw PROException(ExceptionType::EStorageNotExists, "No existeix un projecte de ParaOrdenar al directori definit.");
    }

    parseProjectDefinition();

 }

void Project::save() {
    writeProjectDefinition();
}

void Project::writeProjectDefinition() {
    pugi::xml_document doc;
    pugi::xml_node root = doc.append_child("paraordenar");
    pugi::xml_node config_type = root.append_child("param");
    config_type.append_attribute("name") = "tipus";
    config_type.append_attribute("type") = "string";
    config_type.append_attribute("value") = "ProjectDefinition";

    pugi::xml_node config_version = root.append_child("param");
    config_version.append_attribute("name") = "versio";
    config_version.append_attribute("type") = "string";
    config_version.append_attribute("value") = Paraordenar_VERSION_MAJOR+"."+Paraordenar_VERSION_MINOR;

    pugi::xml_node info = root.append_child("info");
    
    pugi::xml_node node_name = info.append_child("name");
    node_name.append_child(pugi::node_pcdata).set_value(name.c_str());
    pugi::xml_node node_description = info.append_child("description");
    node_description.append_child(pugi::node_pcdata).set_value(description.c_str());
    pugi::xml_node node_inici = info.append_child("dataInici");
    node_inici.append_child(pugi::node_pcdata).set_value(boost::gregorian::to_iso_string(d_inici).c_str());
    pugi::xml_node node_final = info.append_child("dataFinal");
    node_final.append_child(pugi::node_pcdata).set_value(boost::gregorian::to_iso_string(d_fin).c_str());
    pugi::xml_node node_lanugage = info.append_child("language");
    node_lanugage.append_child(pugi::node_pcdata).set_value(language.c_str());
    pugi::xml_node node_active = info.append_child("active");
    node_active.append_child(pugi::node_pcdata).set_value(std::to_string(active).c_str());

    pugi::xml_node vaults_node = root.append_child("vaults");

    for (auto it = vaults.begin(); it != vaults.end(); it++)
    {
        std::string name = *it;

        pugi::xml_node vault_node = vaults_node.append_child("vault");
        vault_node.append_child(pugi::node_pcdata).set_value(name.c_str());
    }

    app_file.open(app_path + "/.proapp", std::ios::out | std::ios::trunc);
    doc.save(app_file);
    app_file.close();

    this->app_file.open(app_path + "/.proapp", std::ios::in);
}

void Project::parseProjectDefinition() {
    this->app_file.open(this->app_path + "/.proapp", std::ios::in);

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load(app_file);
    if (!result)
        throw PROException(ExceptionType::EXMLParseError, result.description());

    pugi::xml_node root = doc.child("paraordenar");
    pugi::xml_node info = root.child("info");

    this->name = info.child("name").text().get();
    this->description = info.child("description").text().get();
    
    this->d_inici = info.child("dataInici") ? 
        boost::gregorian::from_undelimited_string(info.child("dataInici").text().get()) : 
        boost::gregorian::date(1970, 1, 1);

    this->d_fin = info.child("dataFinal") ? 
        boost::gregorian::from_undelimited_string(info.child("dataFinal").text().get()) : 
        boost::gregorian::date(1970, 1, 1);

    this->language = info.child("language").text().get();
    this->active = info.child("active").text().as_bool(true);

    for (pugi::xml_node param : root.children("param"))
    {
        params.emplace(param.attribute("name").as_string(), param.attribute("value").as_string());
    }

    if (params.find("tipus") == params.end() || params.find("tipus")->second.compare("ProjectDefinition") != 0)
    {
        throw PROException(ExceptionType::EMalformedStorage, "No es troba el parametre identificador d'emmagatzematge");
    }

    for (pugi::xml_node vault_node : root.child("vaults"))
    {
        vaults.push_back(vault_node.text().get());
    }

    app_file.close();
}

const std::string Project::get_name() {
    return name;
}

const std::string Project::get_path() {
    return app_path;
}

const std::string Project::get_description() {
    return description;
}

const std::string Project::get_language() {
    return language;
}

const bool Project::get_active() {
    return active;
}

const boost::gregorian::date Project::get_inici() {
    return d_inici;
}

const boost::gregorian::date Project::get_final()
{
    return d_fin;
}

void Project::set_language(std::string lang) {
    this->language = lang;
}

void Project::set_description(std::string desc) {
    this->description = desc;
}

void Project::set_dInici(boost::gregorian::date d) {
    this->d_inici = d;
}
void Project::set_dFinal(boost::gregorian::date d) {
    this->d_fin = d;
}

void Project::set_active(bool active) {
    this->active = active;
}