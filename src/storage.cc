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
        throw PROException(ExceptionType::EStorageNotExists, "No existeix el path");
    }

    this->storage_path = path;
    
    this->storage_file.open(path+"/.prostorage", ios::in | ios::out);
    
    /* CODI PER TREBALLAR AMB FITXERS TAL CUAL

    // Check if storage file is ok
    string firstline;
    getline(storage_file, firstline);
    if(firstline!="paraordenar_storage_definition") {
        throw PROException(ExceptionType::EMalformedStorage, "Malformed Storage");
    }
    */

    result = doc.load(storage_file);
    if(!result) throw PROException(ExceptionType::EXMLParseError, result.description());

}

Storage::~Storage() {
    storage_file.close();
}
    
string Storage::get_path() {
    return this->storage_path;
}
