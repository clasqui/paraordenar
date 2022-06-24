/* storage.cc */
/**
 * ParaOrdenar
 * Storage Class
 * GNU General Public License v3.0
 * Author: Marc Clascà Ramírez
 *
 **/

#include "storage.hpp" 
#include <boost/filesystem.hpp>
#include "types.h"
#include <string>
#include "pugixml.hpp"

using namespace ParaordenarCore;


Storage::Storage(std::string path) {
    // Check if exists
    if(!boost::filesystem::exists(path)) {
        throw PROException(ExceptionType::ENoPath, "No existeix el path");
    }

    this->storage_path = path;
    
    if(!boost::filesystem::exists(path+"/.prostorage")) {
        throw PROException(ExceptionType::EStorageNotExists, "No existeix un emmagatzematge inicialitzat al directori definit.");
    }

    this->storage_file.open(path+"/.prostorage", std::ios::in);

    parseStorageConfig();

}

Storage::Storage() {
    storage_path = "";
    // storage_file = NULL;
    
}

Storage::~Storage() {
    writeStorageConfig();
}
    
std::string Storage::get_path() {
    return this->storage_path;
}

void Storage::init_path(std::string path) {
    if(!boost::filesystem::exists(path)) {
        throw PROException(ExceptionType::ENoPath, "No existeix el path");
    }

    if(boost::filesystem::exists(boost::filesystem::path(path).append(".prostorage"))) {
        throw PROException(ExceptionType::EAlreadyExists, "Ja hi ha un emmagatzematge inicialitzat en aquest directori.");
    }

    this->storage_path = path;

    
    writeStorageConfig();

    this->storage_file.open(path+"/.prostorage", std::ios::in);

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load(storage_file);
    if(!result) throw PROException(ExceptionType::EXMLParseError, result.description());
    storage_file.close();

}

int Storage::new_app(Project * p) {

    auto it = apps.insert({p->get_name(), p->get_active()});
    return std::distance(apps.begin(), it.first);
}

int Storage::get_app_id(std::string key) {
    auto el = apps.find(key);
    if(el == apps.end()) {
        return -1;
    }
    else {
        return std::distance(apps.begin(), el);
    }
}

std::vector<std::pair<std::string, bool>> Storage::get_apps() {
    std::vector < std::pair<std::string, bool>> v;

    for (auto it = apps.begin(); it != apps.end(); it++)
    {
        std::string key = it->first;
        bool active = it->second;
        v.push_back(make_pair(key, active));
    }

    return v;
}

void Storage::writeStorageConfig()
{
    pugi::xml_document doc;
    pugi::xml_node root = doc.append_child("paraordenar");
    pugi::xml_node config_type = root.append_child("param");
    config_type.append_attribute("name") = "tipus";
    config_type.append_attribute("type") = "string";
    config_type.append_attribute("value") = "StorageDefinition";

    pugi::xml_node config_version = root.append_child("param");
    config_version.append_attribute("name") = "versio";
    config_version.append_attribute("type") = "string";
    config_version.append_attribute("value") = PARAORDENAR_VERSION;

    pugi::xml_node apps_node = root.append_child("apps");

    for (auto it = apps.begin(); it != apps.end(); it++)
    {
        std::string key = it->first;
        bool active = it->second;
        pugi::xml_node app_node = apps_node.append_child("app");
        app_node.append_attribute("active") = active ? "true" : "false";
        app_node.append_child(pugi::node_pcdata).set_value(key.c_str());

    }

    storage_file.open(storage_path + "/.prostorage", std::ios::out | std::ios::trunc);
    doc.save(storage_file);

    storage_file.close();
}

void Storage::parseStorageConfig()
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load(storage_file);
    if (!result)
        throw PROException(ExceptionType::EXMLParseError, result.description());

    pugi::xml_node root = doc.child("paraordenar");
    for(pugi::xml_node param: root.children("param")) {
        params.emplace(param.attribute("name").as_string(), param.attribute("value").as_string());
    }

    if(params.find("tipus") == params.end() || params.find("tipus")->second.compare("StorageDefinition") != 0) {
        throw PROException(ExceptionType::EMalformedStorage, "No es troba el parametre identificador d'emmagatzematge");
    }

    for (pugi::xml_node app_node : root.child("apps"))
    {
        apps.emplace(std::make_pair(app_node.text().get(), app_node.attribute("active").as_bool()));
    }

    storage_file.close();
}
