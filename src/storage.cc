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

Storage::Storage(string path) {
    // Check if exists
    if(!boost::filesystem::exists(path)) {
        throw PROException(ExceptionType::ENoPath, "No existeix el path");
    }

    this->storage_path = path;
    
    if(!boost::filesystem::exists(path+"/.prostorage")) {
        throw PROException(ExceptionType::EStorageNotExists, "No existeix un emmagatzematge inicialitzat al directori definit.");
    }

    this->storage_file.open(path+"/.prostorage", ios::in);
    
    /* CODI PER TREBALLAR AMB FITXERS TAL CUAL

    // Check if storage file is ok
    string firstline;
    getline(storage_file, firstline);
    if(firstline!="paraordenar_storage_definition") {
        throw PROException(ExceptionType::EMalformedStorage, "Malformed Storage");
    }
    */

    pugi::xml_parse_result result = doc.load(storage_file);
    if(!result) throw PROException(ExceptionType::EXMLParseError, result.description());

    storage_file.close();

}

Storage::Storage() {
    storage_path = "";
    // storage_file = NULL;

    pugi::xml_node root = doc.append_child("paraordenar");
    pugi::xml_node config_type = root.append_child("param");
    config_type.append_attribute("name") = "tipus";
    config_type.append_attribute("type") = "string";
    config_type.append_attribute("value") = "StorageDefinition";

    pugi::xml_node config_version = root.append_child("param");
    config_version.append_attribute("name") = "versio";
    config_version.append_attribute("type") = "string";
    config_version.append_attribute("value") = PARAORDENAR_VERSION;

    /*pugi::xml_node apps_node =*/ root.append_child("apps");
    
}

Storage::~Storage() {
    storage_file.open(storage_path+"/.prostorage", ios::out | ios::trunc); 
    doc.save(storage_file);

    storage_file.close();
}
    
string Storage::get_path() {
    return this->storage_path;
}

void Storage::init_path(string path) {
    if(!boost::filesystem::exists(path)) {
        throw PROException(ExceptionType::ENoPath, "No existeix el path");
    }
    this->storage_path = path;

    fstream tmp_storagefile;
    tmp_storagefile.open(path+"/.prostorage", ios::out | ios::trunc); 
    doc.save(tmp_storagefile);
    tmp_storagefile.close();

    this->storage_file.open(path+"/.prostorage", ios::in);

    pugi::xml_parse_result result = doc.load(storage_file);
    if(!result) throw PROException(ExceptionType::EXMLParseError, result.description());
    storage_file.close();

}

int Storage::new_app(Project * app) {
    pugi::xml_node app_node = doc.child("paraordenar").child("apps").append_child("app");
    app_node.append_attribute("active") = "true";
    app_node.append_child(pugi::node_pcdata).set_value(app->get_name().c_str());

    return distance(doc.child("paraordenar").child("apps").children().begin(), 
                    doc.child("paraordenar").child("apps").children().end());
}

int Storage::get_app_id(string *app_name) {
    int i = 0;
    pugi::xml_node app_node;
    for (app_node = doc.child("paraordenar").child("apps").first_child(); app_node; app_node = app_node.next_sibling())
    {
        if(app_node.text().get() == *app_name) {
            break;
        }
        ++i;
    }
    return app_node ? i : -1;
}

void Storage::get_apps(vector<Project*> *apps) {
    pugi::xml_node app_node;
    for (app_node = doc.child("paraordenar").child("apps").first_child(); app_node; app_node = app_node.next_sibling())
    {
        Project *app = new Project(storage_path+"/"+app_node.text().get());
        (*apps).push_back(app);
    }
}