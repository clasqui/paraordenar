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

using namespace ParaordenarCore;

/* 4 typedefs */
/* 5 global variable declarations */

std::string paraordenar_dir;
std::string mainstoragepath;
Storage *mstr;

/* 6 function prototypes */
bool pathExists(const std::string &s);
std::string creaEmmagatzematge();

void sel_command(CLI::App *comm);
void ls_command(CLI::App *comm); 
void crea_command(CLI::App *comm);

void read_global_state(int *, int *);
void set_global_state_app(int);
void set_global_state_vault(int);

object_t objectHierarchyFromOptions(CLI::App *comm, std::vector<std::string> &hierarchy);

void initializeStorage();

int main(int argc, char **argv)
{
    /* 7 command-line parsing */

    CLI::App app{"ParaOrdenar - Eina d'emmagatzematge i gestió de traces Paraver"};
    app.set_version_flag("--version", std::string(PARAORDENAR_VERSION), "Mostra la informació de versió i acaba.");
    app.set_help_all_flag("--help-all", "Mostra l'ajuda completa");

    
    app.add_option("-a", "Aplicació");
    app.add_option("-x", "Caixa");
    app.add_option("-t", "Traça");

    // CLI::App *init = app.add_subcommand("init", "Inicialitza un entorn paraordenar al directori actual");
    // CLI::App *proj = app.add_subcommand("app", "Crea, gestiona o mou-te a una aplicació/projecte");
    // std::string app_name;
    // proj->add_option("app_name", app_name, "Nom del projecte")->required();
    // auto crea_group = proj->add_option_group("crea", "Opcions de creació");
    // std::string description;
    // CLI::Option *crea_flag = crea_group->add_flag("-c,--crea", "Crea un projecte a l'emmagatzematge actual.");
    // crea_group->add_option("description", description, "Descripcio del nou projecte/aplicació");

    CLI::App *crea = app.add_subcommand("crea", "Crea un emmagatzematge nou, aplicació, caixa, o traça")->fallthrough();
    crea->add_option("description", "Descripcio del nou objecte creat. S'ignora si es crea un nou emmagatzematge.");
    crea->add_flag("-s,--set", "Selecciona el nou objecte creat per l'entorn");

    CLI::App *llista = app.add_subcommand("llista", "Llista els elements de l'emmagatzematge, una aplicacio o caixa")
    ->alias("lli")->fallthrough();

    CLI::App *info = app.add_subcommand("informacio", "Obté la informació d'una aplicació, caixa o traça")
    ->alias("inf")->fallthrough();

    CLI::App *mod = app.add_subcommand("modifica", "Modifica una aplicació, caixa o traça")
    ->alias("mod")->fallthrough();

    CLI::App *arx = app.add_subcommand("arxiva", "Arxiva una aplicació")
    ->alias("arx")->fallthrough();

    CLI::App *sel = app.add_subcommand("selecciona", "Sel·lecciona l'objecte per establir com a entorn")
    ->alias("sel")->fallthrough();
    sel->add_flag("-d", "Descarta l'entorn actual.");

    app.require_subcommand(1);  // 1 subcomanda requerida



    try {
        initializeStorage();
    }
    catch(const PROException& e) {
        std::cout << "Error en inicialitzar l'emmagatzematge: " << std::endl;
        std::cout << "\t-> " << e.message << std::endl;
        std::cerr << e.message << '\n';
        exit(1);
    }
    
    

    try {
        mstr = new Storage(mainstoragepath);
    }
    catch(PROException &e) {
        std::cout << "Error en carregar l'emmagatzematge: " << std::endl;
        std::cout << "\t-> " << e.message << std::endl;
        exit(1);
    }

    CLI11_PARSE(app, argc, argv);

    for(auto subcom : app.get_subcommands()) {
        std::string nom = subcom->get_name();
        if(nom == "sel") sel_command(subcom);
        if(nom == "llista") ls_command(subcom);
        if(nom == "crea") crea_command(subcom);
    }

    delete mstr; // El destructor guarda la info al fitxer!

    return 0;
}

/* 8 function declarations */

/**
 * @brief Construeix la jerarquia d'objectes donats per entrada
 *        a través de les opcions. També retorna quin tipus 
 *        d'objecte és el subjecte de l'acció.  
 * 
 * Per fer-ho, examina per orde l'existència de les opcions que
 * es refereixen a objectes. Aquestes son:
 *      -a Aplicació
 *      -x Caixa
 *      -t Traça
 * 
 * @param comm Objecte comanda de la llibreria CLI
 * @param hierarchy vector buit per omplir amb la jerarquia
 * @return object_t Subjecte final de la jerarquia
 */
object_t objectHierarchyFromOptions(CLI::App *comm, std::vector<std::string> & hierarchy) {
    object_t subject = TStorage;
    std::string o;
    if(comm->count("-a")) {
        o = *(comm->get_option("-a")->results().begin());
        hierarchy.push_back(o);
        subject = TApplication;
        if (comm->count("-x")) {
            o = *(comm->get_option("-x")->results().begin());
            hierarchy.push_back(o);
            subject = TVault;
            if(comm->count("-t")) {
                o = *(comm->get_option("-t")->results().begin());
                hierarchy.push_back(o);
                subject = TTrace;
                return subject;
            } else {
                return subject;
            }
        } else {
            return subject;
        }
        
    } else {
        hierarchy.clear();
        return subject;
    }
}

/**
 * @brief Inicialitza emmagatzematge si esta configurat, sino el crea.
 * 
 */
void initializeStorage() {
    std::fstream storageconfig;
    char *homepath = getenv("HOME");
    std::string buff;
    paraordenar_dir = std::string(homepath) + std::string(CONFIG_PATH);
    if (!pathExists(paraordenar_dir))
    {
        boost::filesystem::create_directory(paraordenar_dir);
    }
    buff = paraordenar_dir + "/storage";

    if (!pathExists(buff))
    {
        storageconfig.open(buff, std::ios::out);
        std::cout << "Benvingut a ParaOrdenar! No tens definit cap emmagatzematge." << std::endl;
        std::cout << "Abans de començar a treballar, hauries de definir quin vols que sigui l'emmagatzematge per defecte de les traces.  Tranquil, després el podràs modificar." << std::endl;

        std::string tmp_storagedir = creaEmmagatzematge();
        storageconfig << tmp_storagedir << std::endl;

        storageconfig.close();

        exit(0);
    }
    else
    {
        storageconfig.open(buff, std::ios::in | std::ios::out);
    }

    std::string line;
    while (std::getline(storageconfig, line))
    {
        mainstoragepath = line;
        if (mainstoragepath.length() == 0)
        {
            std::cout << "Emmagatzematge no especificat. Indiqueu-lo a ~/.config/paraordenar/storage." << std::endl;
            exit(0);
        }
        break;
    }

    storageconfig.close();
}

void sel_command(CLI::App *comm) {

    if(comm->count("-d")) {
        // Cas de descartar l'entorn seleccionat
        set_global_state_app(-1);
        set_global_state_vault(-1);
        return;
    }

    std::vector<std::string> objectHierarchy;
    object_t subject = objectHierarchyFromOptions(comm, objectHierarchy);

    switch (subject)
    {
    case TStorage:
        return;
        // Seleccionar un storage no te sentit de moment
    
    case TApplication:
        set_global_state_app(mstr->get_app_id(objectHierarchy.back()));
        break;

    case TVault:

        break;
    
    default:
        break;
    }

    return;
}

void ls_command(CLI::App *comm) {

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

    std::vector<std::string> objectHierarchy;
    object_t subject = objectHierarchyFromOptions(comm, objectHierarchy);


    std::fstream storageconfig;
    std::string tmp_storagedir;
    std::string description;
    CLI::results_t res_desc;
    Project *nova_app;
    int id_inserit;

    switch (subject)
    {
    case TStorage:
        delete mstr;

        
        storageconfig.open(paraordenar_dir + "/storage", std::ios::out);

        tmp_storagedir = creaEmmagatzematge();
        storageconfig << tmp_storagedir << std::endl;

        storageconfig.close();
        break;
        

    case TApplication:
        res_desc = comm->get_option("description")->results();
        description = *(res_desc.begin());
        nova_app = new Project(objectHierarchy.back(), description, mstr->get_path());
        std::cout << "Nou projecte creat a " << nova_app->get_path() << std::endl;

        // Afegim app al storage
        id_inserit = mstr->new_app(nova_app);

        if (comm->count("-s"))
        {
            // Seleccionem app a l'entorn
            set_global_state_app(id_inserit);
        }
        break;

    default:
        break;
    }

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

void set_global_state_vault(int vault_id)
{
    std::fstream vault_state_file(paraordenar_dir + "/vault", std::ios::out | std::ios::binary | std::ios::trunc);
    vault_state_file.write(reinterpret_cast<char *>(&vault_id), sizeof(vault_id));
    vault_state_file.close();
}

/**
 * @brief Gestiona la creacio d'un nou emmagatzematge
 * L'usuari indica per entrada on s'ha de crear l'emmagatzematge
 * 
 * @return std::string cami del nou emmagatzematge
 */
std::string creaEmmagatzematge() {
    std::string path;
    std::cout << "Indica el camí on vols inicialitzar l'emmagatzematge: " << std::endl << "> " ;
    std::cin >> path;

    mstr = new Storage();

    mstr->init_path(path);

    std::cout << "Emmagatzematge buit creat a " << path << std::endl;

    return path;
}

bool pathExists(const std::string &s)
{
    struct stat buffer;
    return (stat (s. c_str(), &buffer) == 0);
}
