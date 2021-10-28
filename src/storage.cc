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

Storage::Storage(string path) {
    // Check if exists
    if(!boost::filesystem::exists(path)) {
        throw ExceptionType::EStorageNotExists;
    }

    this->storage_path = path;
    this->storage_file.open(path+"/.prostorage", ios::in | ios::out);

    // Check if storage file is ok
    string firstline;
    getline(storage_file, firstline);
    if(firstline!="paraordenar_storage_definition") {
        throw ExceptionType::EMalformedStorage;
    }

}
    
string Storage::get_path() {
    return this->storage_path;
}
