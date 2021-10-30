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

#include "storage.hpp"
#include "types.h"

using namespace std;

/* 2 defines */
#define CONFIG_PATH "/.config/paraordenar"

/* 3 external declarations */
/* 4 typedefs */
/* 5 global variable declarations */

std::string paraordenar_dir;
Storage *mstr;

/* 6 function prototypes */
bool pathExists(const std::string &s);
string creaEmmagatzematge();

void app_command(CLI::App *comm);
void ls_command(CLI::App *comm); 

int main(int argc, char **argv) {
/* 7 command-line parsing */

    CLI::App app{"ParaOrdenar - Eina d'emmagatzematge i gestió de traces Paraver"};
    
    // CLI::App *init = app.add_subcommand("init", "Inicialitza un entorn paraordenar al directori actual");
    CLI::App *proj = app.add_subcommand("app", "Crea, gestiona o mou-te a una aplicació");
    CLI::App *crea = app.add_subcommand("crea", "Crea un element");
    CLI::App *llista = app.add_subcommand("llista", "Llista els elements de l'entorn actual");
    app.require_subcommand(1);  // 1 subcomanda requerida


    std::fstream storageconfig;
    char *homepath = getenv("HOME");
    string buff;
    paraordenar_dir = string(homepath)+string(CONFIG_PATH);
    if(!pathExists(paraordenar_dir)) {
        boost::filesystem::create_directory(paraordenar_dir);
    }
    buff = paraordenar_dir + "/storage";
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


    string mainstoragepath;
    std::string line;
    while (std::getline(storageconfig, line))
    {
       mainstoragepath = line; 
       if(mainstoragepath.length() == 0) {
           std::cout << "Emmagatzematge no especificat. Indiqueu-lo a ~/.config/paraordenar/storage." << std::endl;
           exit(0);
       } 
       break;
    }

    storageconfig.close();

    try {
        mstr = new Storage(mainstoragepath);
    } catch(PROException &e) {
        cout << "Error en carregar l'emmagatzematge: " << endl;
        cout << "  -> " << e.message << endl;
        exit(1);
    }

    CLI11_PARSE(app, argc, argv);

    for(auto subcom : app.get_subcommands()) {
        string nom = subcom->get_name();
        if(nom == "app") app_command(subcom);
        if(nom == "llista") ls_command(subcom);
    }

    delete mstr;

    return 0;
}

/* 8 function declarations */

void app_command(CLI::App *comm) {
    cout << comm->get_name();

    return;
}

void ls_command(CLI::App *comm) {

    return;
}

void read_global_state(int &app, int &vault) {
    if(boost::filesystem::exists(paraordenar_dir+"/app"))  {
        fstream app_state_file(paraordenar_dir+"/app", ios::in);
        
    }
}

string creaEmmagatzematge() {
    string path;
    cout << "Indica el camí on vols inicialitzar l'emmagatzematge: " << endl << "> " ;
    cin >> path;

    mstr = new Storage();
    mstr->init_path(path);

    cout << "Emmagatzematge buit creat a " << path << endl;

    return path;
}

bool pathExists(const std::string &s)
{
    struct stat buffer;
    return (stat (s. c_str(), &buffer) == 0);
}
