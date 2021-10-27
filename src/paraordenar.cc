/* paraordenar.cc */
/* 0 copyright/licensing */
/**
 * ParaOrdenar - Eina d'emmagatzematge i gestió de traces Paraver.
 * GNU General Public License v3.0
 * Author: Marc Clasca Ramirez
 *
 **/

/* 1 includes */
#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <CLI11.hpp>
#include <boost/filesystem.hpp>

using namespace std;

/* 2 defines */
#define CONFIG_PATH "%s/.config/paraordenar"

/* 3 external declarations */
/* 4 typedefs */
/* 5 global variable declarations */

std::string storagepath;

/* 6 function prototypes */
bool pathExists(const std::string &s);
string creaEmmagatzematge();

int main(int argc, char **argv) {
/* 7 command-line parsing */

    CLI::App app{"ParaOrdenar - Eina d'emmagatzematge i gestió de traces Paraver"};
    
    // CLI::App *init = app.add_subcommand("init", "Inicialitza un entorn paraordenar al directori actual");
    CLI::App *proj = app.add_subcommand("app", "Crea, gestiona o mou-te a una aplicació");
    CLI::App *crea = app.add_subcommand("crea", "Crea un element");
    CLI::App *llista = app.add_subcommand("llista", "Llista els elements de l'entorn actual");
    app.require_subcommand(1);  // 1 subcomanda requerida

    CLI11_PARSE(app, argc, argv);

    std::fstream storageconfig;
    char *homepath = getenv("HOME");
    char buff[136], paraordenar_dir[128];
    sprintf(paraordenar_dir, CONFIG_PATH , homepath);
    if(!pathExists(paraordenar_dir)) {
        boost::filesystem::create_directory(paraordenar_dir);
    }
    sprintf(buff, "%s/storage", paraordenar_dir);


    if(!pathExists(buff)) {
        storageconfig.open(buff, ios::out);
        cout << "Benvingut a ParaOrdenar! No tens definit cap emmagatzematge." << endl;
        cout << "Abans de començar a treballar, hauries de definir quin vols que sigui l'emmagatzematge per defecte de les traces.  Tranquil, després el podràs modificar." << endl;
        
        string tmp_storagedir = creaEmmagatzematge();
        storageconfig << tmp_storagedir << endl;

        storageconfig.close();

        exit(0);
    } else {
        storageconfig.open(buff, ios::in | ios::out);
    }


    std::string line;
    while (std::getline(storageconfig, line))
    {
       storagepath = line; 
       if(storagepath.length() == 0) {
           std::cout << "Emmagatzematge no especificat. Indiqueu-lo a ~/.paraordenar/storage." << std::endl;
       } else {
           std::cout << storagepath << endl;;
       }
    }

    storageconfig.close();


    return 0;
}

/* 8 function declarations */

string creaEmmagatzematge() {
    string path;
    cout << "Indica el camí on vols inicialitzar l'emmagatzematge: " << endl << "> " ;
    cin >> path;
    fstream tmp_storagefile;
    string path_storage = path;
    path.append("/.prostorage");
    tmp_storagefile.open(path, ios::out); 
    tmp_storagefile << "paraordenar_storage_definition" << endl;
    tmp_storagefile.close();
    cout << "Emmagatzematge buit creat a " << path_storage << endl;

    return path_storage;
}

bool pathExists(const std::string &s)
{
    struct stat buffer;
    return (stat (s. c_str(), &buffer) == 0);
}
