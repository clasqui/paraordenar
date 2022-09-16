/* paraordenar.cc */
/* 0 copyright/licensing */
/**
 * ParaOrdenar - Eina d'emmagatzematge i gestió de traces Paraver.
 * GNU General Public License v3.0
 * Author: Marc Clasca Ramirez
 *
 **/

/**
 * @file paraordenar.cc
 * @author Marc Clascà Ramírez
 * @brief Fitxer d'entrada del programa paraordenar.
 * @version 0.1
 * @date 2022-07-02
 * 
 */

/* 1 includes */
#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>

//Libraries
#include "CLI11.hpp"
#include <boost/filesystem.hpp>

#include "storage.hpp"
#include "project.hpp"
#include "trace.hpp"
#include "types.h"
#include "ParaordenarConfig.h"
#include "cli_visualizer.hpp"

/* 2 defines */
#define CONFIG_PATH "/.config/paraordenar"

/* 3 external declarations */
using namespace ParaordenarCore;

/* 4 typedefs */

/* 5 global variable declarations */
std::string paraordenar_dir; /*!< Camí al directori de configuració de l'aplicació */
std::string mainstoragepath; /*!< Camí de l'emmagatzematge principal */
Storage *mstr;               /*!< Punter a l'objecte global d'emmagatzemarge principal */
CLI_Visualizer *cli;

/* 6 function prototypes */
bool pathExists(const std::string &s);
std::string creaEmmagatzematge();

void sel_command(CLI::App *comm, object_t s, std::vector<std::string> oh);
void ls_command(CLI::App *comm, object_t s, std::vector<std::string> oh);
void crea_command(CLI::App *comm, object_t s, std::vector<std::string> oh);
void inf_command(CLI::App *comm, object_t s, std::vector<std::string> oh);
void mod_command(CLI::App *comm, object_t s, std::vector<std::string> oh);

void read_global_state(int *, int *);
void set_global_state_app(int);
void set_global_state_vault(int);

object_t objectHierarchyFromOptions(CLI::App *comm, std::vector<std::string> &hierarchy);

void initializeStorage();

int main(int argc, char **argv)
{
    /* 7 command-line parsing */

    CLI::App app{"ParaOrdenar - Eina d'emmagatzematge i gestió de traces Paraver"};
    app.set_version_flag("--version", std::string(Paraordenar_VERSION_MAJOR+"."+Paraordenar_VERSION_MINOR), "Mostra la informació de versió i acaba.");
    app.set_help_all_flag("--help-all", "Mostra l'ajuda completa");

    app.add_flag("-r,--raw", "Imprimeix els resultats sense decorar el text");

    
    app.add_option("-a", "Aplicació");
    app.add_option("-x", "Caixa");
    app.add_option("-t", "Experiment de Traceig");
    app.add_option("-w", "Experiment de Walltime");

    CLI::App *crea = app.add_subcommand("crea", "Crea un emmagatzematge nou, aplicació, caixa, o traça")
    ->alias("cre")->fallthrough();
    crea->add_option("description", "Descripcio del nou objecte creat. S'ignora si es crea un nou emmagatzematge.");
    crea->add_flag("-s,--set", "Selecciona el nou objecte creat per l'entorn");

    CLI::App *llista = app.add_subcommand("llista", "Llista els elements de l'emmagatzematge, una aplicacio o caixa")
    ->alias("lli")->fallthrough();

    CLI::App *info = app.add_subcommand("informacio", "Obté la informació d'una aplicació, caixa o traça")
    ->alias("inf")->fallthrough();

    CLI::App *mod = app.add_subcommand("modifica", "Modifica una aplicació, caixa o traça")
    ->alias("mod")->fallthrough();
    mod->add_option("clau", "Clau del paràmetre a modificar")->required();
    mod->add_option("valor", "Nou valor pel paràmetre")->required();

    CLI::App *arx = app.add_subcommand("arxiva", "Arxiva una aplicació")
    ->alias("arx")->fallthrough();

    CLI::App *sync = app.add_subcommand("sincronitza", "Sincronitza un emmagatzematge, projecte o caixa amb un repositori remot")
    ->alias("sin")->fallthrough();

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

    std::vector<std::string> objectHierarchy;
    object_t subject = objectHierarchyFromOptions(&app, objectHierarchy);
    cli = new CLI_Visualizer(!app.count("-r"));

    for(auto subcom : app.get_subcommands()) {
        std::string nom = subcom->get_name();
        if(nom == "selecciona") sel_command(subcom, subject, objectHierarchy);
        if(nom == "llista") ls_command(subcom, subject, objectHierarchy);
        if(nom == "crea") crea_command(subcom, subject, objectHierarchy);
        if(nom == "informacio") inf_command(subcom, subject, objectHierarchy);
        if(nom == "modifica") mod_command(subcom, subject, objectHierarchy);
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
 *      - -a Aplicació
 *      - -x Caixa
 *      - -t Traça
 * 
 * @param comm Objecte comanda de la llibreria CLI
 * @param hierarchy vector buit per omplir amb la jerarquia
 * @return object_t Subjecte final de la jerarquia
 */
object_t objectHierarchyFromOptions(CLI::App *comm, std::vector<std::string> & hierarchy) {
    object_t subject = TStorage;
    std::string o;
    // First push back of empty value, representing storage.
    hierarchy.push_back("");

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
            } else if(comm->count("-w")) {
                o = *(comm->get_option("-w")->results().begin());
                hierarchy.push_back("");  // We leave a blank space in hierarchy to match with type integer value
                hierarchy.push_back(o);
                subject = TWalltime;
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


/**
 * @name Command Functions
 * 
 */
//@{

/**
 * @brief Funció per gestionar la comanda _selecciona_
 * 
 * @param comm Punter a l'objecte amb la subcomanda
 */
void sel_command(CLI::App *comm, object_t s, std::vector<std::string> oh) {

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

/**
 * @brief Funció per gestionar la comanda _llista_
 *
 * @param comm Punter a l'objecte amb la subcomanda
 */
void ls_command(CLI::App *comm, object_t s, std::vector<std::string> oh) {

    Project *p;
    Vault *x;
    
    SUBJECT_SWITCH

    cli->list_applications(mstr);

    SUBJECT_SWITCH_APP

     p = mstr->open_app(oh[1]);
    if(p == nullptr) {
        cli->err_no_existeix("El projecte", oh[1]);
        exit(1);
    }
    cli->list_vaults(p);

    SUBJECT_SWITCH_VAULT
    p = mstr->open_app(oh[1]);
    if(p == nullptr) {
        cli->err_no_existeix("El projecte", oh[1]);
        exit(1);
    }
    x = p->open_vault(oh[2]);
    if(x == nullptr) {
        cli->err_no_existeix("La caixa", oh[2]);
        exit(1);
    }
    cli->list_experiments(x);
    
    SUBJECT_SWITCH_FIN

    return;
}

/**
 * @brief Funció per gestionar la comanda _crea_
 *
 * @param comm Punter a l'objecte amb la subcomanda
 */
void crea_command(CLI::App *comm, object_t s, std::vector<std::string> oh) {

    std::fstream storageconfig;
    std::string tmp_storagedir;
    std::string description = "";
    CLI::results_t res_desc;
    Project *p;
    Vault *x;
    Trace *t;
    int id_inserit;

    SUBJECT_SWITCH

        delete mstr;
        
        storageconfig.open(paraordenar_dir + "/storage", std::ios::out);

        tmp_storagedir = creaEmmagatzematge();
        storageconfig << tmp_storagedir << std::endl;

        storageconfig.close();

    SUBJECT_SWITCH_APP

        if(comm->get_option("description")->count()) {
            res_desc = comm->get_option("description")->results();
            description = *(res_desc.begin());
        }

        p = mstr->new_app(oh.back(), description);
        std::cout << "Nou projecte creat a " << p->get_path() << std::endl;


        // if (comm->count("-s"))
        // {
        //     // Seleccionem app a l'entorn
        //     id_inserit = mstr->get_app_id(p->get_name());
        //     set_global_state_app(id_inserit);
        // }

    SUBJECT_SWITCH_VAULT
        p = mstr->open_app(oh[1]);
        if(p == nullptr) {
            std::cerr << "El projecte no existeix!" << std::endl;
            exit(1);
        }

        x = p->new_vault(oh.back(), "");
        std::cout << "Nova caixa creada a " << x->get_path() << std::endl;
        p->save();

        // if (comm->count("-s"))
        // {
        //     // Seleccionem caixa a l'entorn
        //     id_inserit = p->get_vault_id(x->get_name());
        //     set_global_state_vault(id_inserit);
        // }

    SUBJECT_SWITCH_TRACE
        p = mstr->open_app(oh[1]);
        if(p == nullptr) {
            cli->err_no_existeix("El projecte", oh[1]);
            exit(1);
        }
        x = p->open_vault(oh[2]);
        if(x == nullptr) {
            cli->err_no_existeix("La caixa", oh[2]);
            exit(1);
        }
        t = (Trace *)x->new_experiment(oh[s], ExperimentType::Tracing);
        x->save();
        fmt::print("Nova traça creada a {} amb el nom {}\n", x->get_path(), t->get_name());


    SUBJECT_SWITCH_FIN

}

/**
 * @brief Funció per gestionar la comanda _modifica_
 *
 * @param comm Punter a l'objecte amb la subcomanda
 */
void mod_command(CLI::App *comm, object_t s, std::vector<std::string> oh) {

    std::fstream storageconfig;
    std::string tmp_storagedir;
    std::string description = "";
    CLI::results_t res_desc;
    Project *p;
    Vault *x;
    Trace *t;
    int id_inserit;

    SUBJECT_SWITCH

        delete mstr;
        
        storageconfig.open(paraordenar_dir + "/storage", std::ios::out);

        std::cout << "Indica el camí de l'emmagatzematge que vols obrir: " << std::endl << "> " ;
        std::cin >> tmp_storagedir;

        try {
        mstr = new Storage(tmp_storagedir);
        }
        catch(PROException &e) {
            std::cout << "Error en carregar l'emmagatzematge: " << std::endl;
            std::cout << "\t-> " << e.message << std::endl;
            exit(1);
        }
        mainstoragepath = tmp_storagedir;
        storageconfig << tmp_storagedir << std::endl;

        storageconfig.close();

    SUBJECT_SWITCH_APP



    SUBJECT_SWITCH_VAULT
        p = mstr->open_app(oh[1]);
        if(p == nullptr) {
            std::cerr << "El projecte no existeix!" << std::endl;
            exit(1);
        }


    SUBJECT_SWITCH_TRACE
        p = mstr->open_app(oh[1]);
        if(p == nullptr) {
            cli->err_no_existeix("El projecte", oh[1]);
            exit(1);
        }
        x = p->open_vault(oh[2]);
        if(x == nullptr) {
            cli->err_no_existeix("La caixa", oh[2]);
            exit(1);
        }

        t = (Trace *)x->open_experiment(oh[s]);





    SUBJECT_SWITCH_FIN

}


/**
 * @brief Funció per gestionar la comanda _informacio_
 * 
 * @param comm Punter a l'objecte amb la subcomanda
 * @param s subjecte sobre el que es crida l'accio
 * @param oh jerarquia d'objectes
 * @param r indica si cal mostrar la informacio sense format
 */
void inf_command(CLI::App *comm, object_t s, std::vector<std::string> oh) {
    Project * p;
    Vault *x;
    boost::gregorian::date di;
    Trace *t;
    
    SUBJECT_SWITCH
    cli->print_info_storage(mstr);

    SUBJECT_SWITCH_APP
    p = mstr->open_app(oh.back());
    if(p == nullptr) {
        cli->err_no_existeix("El projecte", oh[s]);
        exit(1);
    }
    cli->print_info_application(p);

    SUBJECT_SWITCH_VAULT
    p = mstr->open_app(oh[1]);
    if(p == nullptr) {
        cli->err_no_existeix("El projecte", oh[1]);
        exit(1);
    }

    x = p->open_vault(oh[s]);
    if(x == nullptr) {
        cli->err_no_existeix("La caixa", oh[s]);
        exit(1);
    }
    cli->print_info_vault(x);

    SUBJECT_SWITCH_TRACE
        p = mstr->open_app(oh[1]);
    if(p == nullptr) {
        cli->err_no_existeix("El projecte", oh[1]);
        exit(1);
    }

    x = p->open_vault(oh[2]);
    if(x == nullptr) {
        cli->err_no_existeix("La caixa", oh[2]);
        exit(1);
    }

    try
    {
        t = (Trace *)x->open_experiment(oh[s]);
    }
    catch(const PROException& e)
    {
        std::cerr << e.message << '\n';
        exit(1);
    }
    
    if(t == nullptr) {
        cli->err_no_existeix("La traça", oh[s]);
        exit(1);
    }
    cli->print_info_trace(t);



    SUBJECT_SWITCH_FIN
}


//@}

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
