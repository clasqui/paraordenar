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
#include "project.hpp"
#include "types.h"

/* 2 defines */
#define CONFIG_PATH "/.config/paraordenar"

/* 3 external declarations */

using namespace Paraordenar;

/* 4 typedefs */
/* 5 global variable declarations */

std::string paraordenar_dir;
Storage *mstr;

/* 6 function prototypes */
bool pathExists(const std::string &s);
std::string creaEmmagatzematge();

void app_command(CLI::App *comm, std::string *app_name);
void ls_command(CLI::App *comm); 
void crea_command(CLI::App *comm);

void read_global_state(int *, int *);
void set_global_state_app(int);

int main(int argc, char **argv) {
/* 7 command-line parsing */

    CLI::App app{"ParaOrdenar - Eina d'emmagatzematge i gestió de traces Paraver"};
    app.set_version_flag("--version", std::string(PARAORDENAR_VERSION), "Mostra la informació de versió i acaba.");
    app.set_help_all_flag("--help-all", "Mostra l'ajuda completa");
    
    // CLI::App *init = app.add_subcommand("init", "Inicialitza un entorn paraordenar al directori actual");
    CLI::App *proj = app.add_subcommand("app", "Crea, gestiona o mou-te a una aplicació/projecte");
    std::string app_name;
    proj->add_option("app_name", app_name, "Nom del projecte")->required();
    auto crea_group = proj->add_option_group("crea", "Opcions de creació");
    std::string description;
    CLI::Option *crea_flag = crea_group->add_flag("-c,--crea", "Crea un projecte a l'emmagatzematge actual.");
    crea_group->add_option("description", description, "Descripcio del nou projecte/aplicació");
    crea_group->add_flag("-s,--set", "Selecciona el nou projecte creat per l'entorn")->needs(crea_flag);

    /* CLI::App *crea = */app.add_subcommand("crea", "Crea un emmagatzematge nou i posa'l com a predeterminat");
    
    /* CLI::App *llista = */app.add_subcommand("llista", "Llista els elements de l'entorn seleccionat");
    app.require_subcommand(1);  // 1 subcomanda requerida


    std::fstream storageconfig;
    char *homepath = getenv("HOME");
    std::string buff;
    paraordenar_dir = std::string(homepath)+std::string(CONFIG_PATH);
    if(!pathExists(paraordenar_dir)) {
        boost::filesystem::create_directory(paraordenar_dir);
    }
    buff = paraordenar_dir + "/storage";

    if(!pathExists(buff)) {
        storageconfig.open(buff, std::ios::out);
        std::cout << "Benvingut a ParaOrdenar! No tens definit cap emmagatzematge." << std::endl;
        std::cout << "Abans de començar a treballar, hauries de definir quin vols que sigui l'emmagatzematge per defecte de les traces.  Tranquil, després el podràs modificar." << std::endl;
        
        std::string tmp_storagedir = creaEmmagatzematge();
        storageconfig << tmp_storagedir << std::endl;

        storageconfig.close();

        exit(0);
    } else {
        storageconfig.open(buff, std::ios::in | std::ios::out);
    }


    std::string mainstoragepath;
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
        std::cout << "Error en carregar l'emmagatzematge: " << std::endl;
        std::cout << "\t-> " << e.message << std::endl;
        exit(1);
    }

    CLI11_PARSE(app, argc, argv);

    for(auto subcom : app.get_subcommands()) {
        std::string nom = subcom->get_name();
        if(nom == "app") {
            app_command(subcom, &app_name);
        }
        if(nom == "llista") ls_command(subcom);
        if(nom == "crea") crea_command(subcom);
    }

    delete mstr; // El destructor guarda la info al fitxer!

    return 0;
}

/* 8 function declarations */

void app_command(CLI::App *comm, std::string *app_name) {

    if(comm->count("-c") or comm->count("--crea")) {
        // Creem un projecte, ignorem la resta
        CLI::results_t res_desc = comm->get_option("description")->results();
        std::string description = *(res_desc.begin());
        Project *nova_app = new Project(*app_name, description, mstr->get_path());
        std::cout << "Nou projecte creat a " << nova_app->get_path() << std::endl;

        // Afegim app al storage
        int num_apps = mstr->new_app(nova_app);

        if(comm->count("-s")) {
            // Seleccionem app a l'entorn
            set_global_state_app(num_apps-1);
        }
    } else {
        set_global_state_app(mstr->get_app_id(*app_name));
    }

    return;
}

void ls_command(CLI::App *comm) {

    int c_app, c_vault;
    read_global_state(&c_app, &c_vault);
    std::cout << "Current app: " << std::to_string(c_app) << std::endl;


    // De moment llistem apps
    std::vector<std::pair<std::string, bool>> apps;
    apps = mstr->get_apps();
    std::cout << "Llista de projectes: (" << apps.size() << " en total)" << std::endl;
    for (auto &&p : apps)
    {
        std::cout << "\t-> " << p.first << std::endl;
    }
    

    return;
}

void crea_command(CLI::App *comm) {
    // Tanquem emmagatzematge actual
    delete mstr;

    std::fstream storageconfig;
    storageconfig.open(paraordenar_dir+"/storage", std::ios::out);

    std::string tmp_storagedir = creaEmmagatzematge();
    storageconfig << tmp_storagedir << std::endl;

    storageconfig.close();

}

void read_global_state(int *app, int *vault) {
    if(boost::filesystem::exists(paraordenar_dir+"/app"))  {
        std::fstream app_state_file(paraordenar_dir+"/app", std::ios::in | std::ios::binary);
        int app_id;
        app_state_file.read(reinterpret_cast<char*>(&app_id), sizeof(app_id));
        *app = app_id;
    } else {
        std::fstream app_state_file(paraordenar_dir+"/app", std::ios::out | std::ios::binary);
        int no_app = -1;
        app_state_file.write(reinterpret_cast<char*>(&no_app), sizeof(no_app));
        app_state_file.close();
        *app = no_app;
    }

    if(boost::filesystem::exists(paraordenar_dir+"/vault"))  {
        std::fstream vault_state_file(paraordenar_dir+"/vault", std::ios::in | std::ios::binary);
        int vault_id;
        vault_state_file.read(reinterpret_cast<char*>(&vault_id), sizeof(vault_id));
        *vault = vault_id;
    } else {
        std::fstream vault_state_file(paraordenar_dir+"/vault", std::ios::out | std::ios::binary);
        int no_vault = -1;
        vault_state_file.write(reinterpret_cast<char*>(&no_vault), sizeof(no_vault));
        vault_state_file.close();
        *vault = no_vault;
    }

    return;
}

void set_global_state_app(int app_id) {
    std::fstream app_state_file(paraordenar_dir+"/app", std::ios::out | std::ios::binary | std::ios::trunc);
    app_state_file.write(reinterpret_cast<char*>(&app_id), sizeof(app_id));
    app_state_file.close();
}

std::string creaEmmagatzematge() {
    std::string path;
    std::cout << "Indica el camí on vols inicialitzar l'emmagatzematge: " << std::endl << "> " ;
    std::cin >> path;

    mstr = new Storage();

    try
    {
        mstr->init_path(path);
    }
    catch(const PROException& e)
    {
        std::cerr << e.message << '\n';
        exit(1);
    }
    

    std::cout << "Emmagatzematge buit creat a " << path << std::endl;

    return path;
}

bool pathExists(const std::string &s)
{
    struct stat buffer;
    return (stat (s. c_str(), &buffer) == 0);
}
