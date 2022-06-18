/* project.cc */
/**
 * ParaOrdenar
 * Project Class
 * GNU General Public License v3.0
 * Author: Marc Clascà Ramírez
 *
 **/

 #include "project.hpp"
 #include "types.h"
 #include <boost/filesystem.hpp>
 #include <boost/regex.hpp>

using namespace Paraordenar;

 Project::Project(std::string name, std::string description, std::string storage_path) {

     // Comprovem nom projecte
    static const boost::regex e("^\\w+$");
    if(!boost::regex_match(name, e)) {
        throw PROException(ExceptionType::ENameNotValid, "El nom pel projecte no és vàlid");
    }

    this->name = name;
    this->description = description;

    pugi::xml_node root = doc.append_child("paraordenar");
    pugi::xml_node config_type = root.append_child("param");
    config_type.append_attribute("name") = "tipus";
    config_type.append_attribute("type") = "string";
    config_type.append_attribute("value") = "ProjectDefinition";

    pugi::xml_node config_version = root.append_child("param");
    config_version.append_attribute("name") = "versio";
    config_version.append_attribute("type") = "string";
    config_version.append_attribute("value") = PARAORDENAR_VERSION;

    pugi::xml_node info = root.append_child("info");
    pugi::xml_node node_name = info.append_child("name");
    node_name.append_child(pugi::node_pcdata).set_value(name.c_str());
    pugi::xml_node node_description = info.append_child("description");
    node_description.append_child(pugi::node_pcdata).set_value(description.c_str());


    /*pugi::xml_node vaults_node =*/ root.append_child("vaults");

    if(!boost::filesystem::exists(storage_path)) {
        throw PROException(ExceptionType::ENoPath, "No existeix el path del storage proporcionat");
    }

    this->app_path = storage_path+"/"+name;

    if(!boost::filesystem::exists(this->app_path)) {
        boost::filesystem::create_directory(this->app_path);
    }

    std::fstream tmp_appfile;
    tmp_appfile.open(app_path+"/.proapp", std::ios::out | std::ios::trunc); 
    doc.save(tmp_appfile);
    tmp_appfile.close();

    this->app_file.open(app_path+"/.proapp", std::ios::in);

    pugi::xml_parse_result result = doc.load(app_file);
    if(!result) throw PROException(ExceptionType::EXMLParseError, result.description());
    app_file.close();
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

    this->app_file.open(path+"/.proapp", std::ios::in);
    

    pugi::xml_parse_result result = doc.load(app_file);
    if(!result) throw PROException(ExceptionType::EXMLParseError, result.description());

    this->name = doc.child("paraordenar").child("info").child("name").text().get();
    this->description = doc.child("paraordenar").child("info").child("description").text().get();

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

 const bool Project::get_active() {
    return active;
 }