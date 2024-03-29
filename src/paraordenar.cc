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
#include <sys/wait.h>
#include <cerrno>

//Libraries
#include "CLI11.hpp"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>

// project includes
#include "storage.hpp"
#include "project.hpp"
#include "trace.hpp"
#include "types.h"
#include "ParaordenarConfig.h"
#include "cli_visualizer.hpp"

/* 2 defines */
#define CONFIG_PATH "/.config/paraordenar"
#include "pro_defines.hpp"

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
void add_command(CLI::App *comm, object_t s, std::vector<std::string> oh);
void obr_command(CLI::App *comm, object_t s, std::vector<std::string> oh);
void rem_command(CLI::App *comm, object_t s, std::vector<std::string> oh);
void sin_command(CLI::App *comm, object_t s, std::vector<std::string> oh);
void auto_command(CLI::App *comm);

void read_global_state(std::string *, std::string *);
void set_global_state_app(std::string);
void set_global_state_vault(std::string);

object_t objectHierarchyFromOptions(CLI::App *comm, std::vector<std::string> &hierarchy);

void initializeStorage();

int main(int argc, char **argv)
{
    /* 7 command-line parsing */

    CLI::App app{"ParaOrdenar - Eina d'emmagatzematge i gestió de traces Paraver"};
    app.set_version_flag("--version", "pro v" Paraordenar_VERSION_MAJOR "." Paraordenar_VERSION_MINOR, "Mostra la informació de versió i acaba.");
    app.set_help_all_flag("--help-all", "Mostra l'ajuda completa");

    bool raw_output{false};
    app.add_flag("-r,--raw", raw_output, "Imprimeix els resultats sense decorar el text");

    
    app.add_option("-a", "Aplicació");
    app.add_option("-x", "Caixa");
    app.add_option("-t", "Experiment de Traceig");
    app.add_option("-w", "Experiment de Walltime");

    // Modul base
    CLI::App *crea = app.add_subcommand("crea", "Crea un emmagatzematge nou, aplicació, caixa, o traça")
    ->alias("cre")->fallthrough();
    crea->add_option("description", "Descripcio del nou objecte creat. S'ignora si es crea un nou emmagatzematge.");
    bool set_env_on_create{false};
    crea->add_flag("-s,--set", set_env_on_create, "Selecciona el nou objecte creat per l'entorn");

    CLI::App *llista = app.add_subcommand("llista", "Llista els elements de l'emmagatzematge, una aplicacio o caixa")
    ->alias("lli")->fallthrough();

    CLI::App *info = app.add_subcommand("informacio", "Obté la informació d'una aplicació, caixa o traça")
    ->alias("inf")->fallthrough();

    CLI::App *mod = app.add_subcommand("modifica", "Modifica una aplicació, caixa o traça")
    ->alias("mod")->fallthrough();
    mod->add_option("clau", "Clau del paràmetre a modificar")->required();
    mod->add_option("valor", "Nou valor pel paràmetre")->required();

    CLI::App *eli = app.add_subcommand("elimina", "Elimina una aplicació, caixa o traça")
    ->alias("eli")->fallthrough();
    bool remove_files_fs_object{false};
    eli->add_flag("-f,--fitxers", remove_files_fs_object, "Elimina dels arxius del sistema de fitxers també");

    // Modul treball amb traces
    CLI::App *add = app.add_subcommand("afegeix", "Afegeix un recurs a un experiment")
    ->alias("add")->fallthrough();
    add->add_option("arxiu", "Nom del recurs a afegir")->required();
    add->add_option("etiqueta", "Etiqueta del recurs")->required();
    add->add_option("n_threads", "Número de threads si varia de l'experiment base");
    add->add_option("n_ranks", "Número de MPI ranks si varia de l'experiment base");

    CLI::App *rem = app.add_subcommand("treu", "Treu un recurs a un experiment")
    ->alias("rem")->fallthrough();
    rem->add_option("arxiu", "Nom del recurs a treure")->required();
    bool remove_files_fs_resource{false};
    rem->add_flag("-f,--fitxers", remove_files_fs_resource, "Elimina dels arxius del sistema de fitxers també");

    CLI::App *obr = app.add_subcommand("obre", "Obre les traces d'un experiment amb Paraver")
    ->alias("obr")->fallthrough();
    obr->add_option("etiqueta", "Etiqueta de la traça que vols obrir");

    // Modul gestio repositori
    CLI::App *arx = app.add_subcommand("arxiva", "Arxiva una aplicació")
    ->alias("arx")->fallthrough();

    CLI::App *sync = app.add_subcommand("sincronitza", "Sincronitza un emmagatzematge, projecte o caixa amb un repositori remot")
    ->alias("sin")->fallthrough();

    // Estat
    CLI::App *sel = app.add_subcommand("selecciona", "Sel·lecciona l'objecte per establir com a entorn")
    ->alias("sel")->fallthrough();
    bool discard_current_env{false};
    sel->add_flag("-d", discard_current_env, "Descarta l'entorn actual.");


    // Internals
    CLI::App *_autocomplete = app.add_subcommand("_completion");
    _autocomplete->add_option("ncurs");
    // _autocomplete->add_option("params"); Obtain by cin

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
        
        if(nom == "_completion") auto_command(subcom);
        if(nom == "selecciona") sel_command(subcom, subject, objectHierarchy);
        if(nom == "llista") ls_command(subcom, subject, objectHierarchy);
        if(nom == "crea") crea_command(subcom, subject, objectHierarchy);
        if(nom == "informacio") inf_command(subcom, subject, objectHierarchy);
        if(nom == "modifica") mod_command(subcom, subject, objectHierarchy);
        if(nom == "afegeix") add_command(subcom, subject, objectHierarchy);
        if(nom == "obre") obr_command(subcom, subject, objectHierarchy);
        if(nom == "treu") rem_command(subcom, subject, objectHierarchy);
        //if(nom == "sincronitza") sin_command(subcom, subject, objectHierarchy);
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
    bool flag_ignore_state = false;

    // First push back of empty value, representing storage.
    hierarchy.push_back("");
    std::string state_app, state_vault;
    read_global_state(&state_app, &state_vault);

    if(comm->count("-a") || !state_app.empty()) {
        if(comm->count("-a")) {
            flag_ignore_state = true;
            o = *(comm->get_option("-a")->results().begin());
            hierarchy.push_back(o);
        } else {
            hierarchy.push_back(state_app);
        }
        subject = TApplication;

        if (comm->count("-x") || (!state_vault.empty() && !flag_ignore_state)) {
            if(comm->count("-x")) {
                o = *(comm->get_option("-x")->results().begin());
                hierarchy.push_back(o);
            } else {
                hierarchy.push_back(state_vault);
            }
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
        //hierarchy.clear();
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
        set_global_state_app("");
        set_global_state_vault("");
        return;
    }

    std::string state_app, state_vault;

    SUBJECT_SWITCH
        // Mostrem el que tenim seleccionat
        read_global_state(&state_app, &state_vault);
        cli->print_current_state(state_app, state_vault);
    
    SUBJECT_SWITCH_APP
        set_global_state_app(oh[s]);

    SUBJECT_SWITCH_VAULT
        set_global_state_app(oh[s-1]);
        set_global_state_vault(oh[s]);

    SUBJECT_SWITCH_FIN

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
    Trace *t;

    
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
    if(t == nullptr) {
        cli->err_no_existeix("La traça", oh[s]);
        exit(1);
    }
    cli->list_prvs(t);

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

        try
        {
            p = mstr->new_app(oh.back(), description);
        }
        catch(const PROException& e)
        {
            cli->error_generic(e.message);
            exit(1);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            exit(1);
        }
        std::cout << "Nou projecte creat a " << p->get_path() << std::endl;


        // if (comm->count("-s"))
        // {
        //     // Seleccionem app a l'entorn
        //     id_inserit = mstr->get_app_id(p->get_name());
        //     set_global_state_app(id_inserit);
        // }

    SUBJECT_SWITCH_VAULT
        try {       
            p = mstr->open_app(oh[1]);
            if(p == nullptr) {
                cli->err_no_existeix("El projecte", oh[1]);
                exit(1);
            }
            x = p->new_vault(oh.back(), "");
            p->save();
        }
        catch(const PROException& e)
        {
            cli->error_generic(e.message);
            exit(1);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            exit(1);
        }

        std::cout << "Nova caixa creada a " << x->get_path() << std::endl;
        

        // if (comm->count("-s"))
        // {
        //     // Seleccionem caixa a l'entorn
        //     id_inserit = p->get_vault_id(x->get_name());
        //     set_global_state_vault(id_inserit);
        // }

    SUBJECT_SWITCH_TRACE
        try
        {
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
        }
        catch(const PROException& e)
        {
            cli->error_generic(e.message);
            exit(1);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            exit(1);
        }

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
    std::string clau, valor;
    Project *p;
    Vault *x;
    Trace *t;

    if(comm->get_option("clau")->count()) {
        clau = *(comm->get_option("clau")->results().begin());
    }
    if(comm->get_option("valor")->count()) {
        valor = *(comm->get_option("valor")->results().begin());
    }

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
        p = mstr->open_app(oh[1]);
        if(p == nullptr) {
            std::cerr << "El projecte no existeix!" << std::endl;
            exit(1);
        }

        if(clau == "descripcio") {
            p->set_description(valor);
        } else if(clau == "llenguatge") {
            p->set_language(valor);
        }

        p->save();


    SUBJECT_SWITCH_VAULT
        p = mstr->open_app(oh[1]);
        if(p == nullptr) {
            std::cerr << "El projecte no existeix!" << std::endl;
            exit(1);
        }
        x = p->open_vault(oh[2]);
        if(x == nullptr) {
            cli->err_no_existeix("La caixa", oh[2]);
            exit(1);
        }

        if(clau == "descripcio") {
            x->set_description(valor);
        } else if(clau == "conclusions") {
            x->set_conclusions(valor);
        }
        x->save();

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
        if(t == nullptr) {
            cli->err_no_existeix("La traça", oh[s]);
            exit(1);
        }

        // Check which parameters we can modify

        if(clau == "log") {
            if(!std::filesystem::exists(valor)) {
                cli->err_no_existeix("L'arxiu de registre", valor);
                exit(1);
            }
            t->set_logfile_name(valor);
        } else if(clau == "descripcio") {
            t->set_description(valor);
        } else if(clau == "threads") {
            t->set_omp_threads(std::stoi(valor));
        } else if(clau == "ranks") {
            t->set_mpi_tasks(std::stoi(valor));
        }
        t->save();



    SUBJECT_SWITCH_FIN

}

/**
 * @brief Funció per gestionar la comanda _afegeix_
 *
 * @param comm Punter a l'objecte amb la subcomanda
 * @param s subjecte sobre el que es crida l'accio
 * @param oh jerarquia d'objectes
 */
void add_command(CLI::App *comm, object_t s, std::vector<std::string> oh) {

    Project *p;
    Vault *x;
    Trace *t;

    std::string arxiu, etiqueta;
    int n_threads = 0, n_ranks = 0;

    if(s != TTrace && s != TWalltime) {
        cli->error_generic("La comanda 'afegeix' només es pot utilitzar sobre un experiment.");
        exit(1);
    }

    SUBJECT_SWITCH

    SUBJECT_SWITCH_APP

    SUBJECT_SWITCH_VAULT

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
        if(t == nullptr) {
            cli->err_no_existeix("La traça", oh[s]);
            exit(1);
        }

        // Collect info from parameters
        if(comm->get_option("arxiu")->count()) {
            arxiu = *(comm->get_option("arxiu")->results().begin());
        }
        if(comm->get_option("etiqueta")->count()) {
            etiqueta = *(comm->get_option("etiqueta")->results().begin());
        }
        if(comm->get_option("n_threads")->count()) {
            n_threads = comm->get_option("n_threads")->as<int>();
        }
        if(comm->get_option("n_ranks")->count()) {
            n_ranks = comm->get_option("n_ranks")->as<int>();
        }

        if(!std::filesystem::exists(t->get_base_path() / arxiu)) {
            cli->err_no_existeix("L'arxiu PRV", arxiu);
            exit(1);
        }

        t->add_prv(arxiu, etiqueta, n_threads, n_ranks);
        t->save();


    SUBJECT_SWITCH_FIN

}

/**
 * @brief Funció per gestionar la comanda _treu_
 *
 * @param comm Punter a l'objecte amb la subcomanda
 * @param s subjecte sobre el que es crida l'accio
 * @param oh jerarquia d'objectes
 */
void rem_command(CLI::App *comm, object_t s, std::vector<std::string> oh) {

    Project *p;
    Vault *x;
    Trace *t;

    std::string arxiu, etiqueta;
    int n_threads = 0, n_ranks = 0;
    prv_dict_t copy_dict;
    prv_dict_t::iterator titer;

    if(s != TTrace && s != TWalltime) {
        cli->error_generic("La comanda 'afegeix' només es pot utilitzar sobre un experiment.");
        exit(1);
    }

    SUBJECT_SWITCH

    SUBJECT_SWITCH_APP

    SUBJECT_SWITCH_VAULT

    SUBJECT_SWITCH_TRACE

        LOAD_PROJECT_MACRO(p)
        LOAD_VAULT_MACRO(x)
        LOAD_TRACE_MACRO(t)

        // Collect info from parameters
        if(comm->get_option("arxiu")->count()) {
            arxiu = *(comm->get_option("arxiu")->results().begin());
        }

        copy_dict = t->get_list_prv();
        titer = copy_dict.find(arxiu);
        if(titer == copy_dict.end()) {
            cli->err_no_existeix("El recurs PRV", arxiu);
            exit(1);
        }

        t->rem_prv(arxiu, comm->count("-f"));
        t->save();

        if(comm->count("-f")) {
            if(!std::filesystem::exists(t->get_base_path() / arxiu)) {
                cli->err_no_existeix("L'arxiu PRV", arxiu);
                exit(1);
            }
        }



    SUBJECT_SWITCH_FIN

}


/**
 * @brief Funció per gestionar la comanda _obre_
 *
 * @param comm Punter a l'objecte amb la subcomanda
 * @param s subjecte sobre el que es crida l'accio
 * @param oh jerarquia d'objectes
 */
void obr_command(CLI::App *comm, object_t s, std::vector<std::string> oh) {

    Project *p;
    Vault *x;
    Trace *t;

    int pid, status, i;
    prv_dict_t dict_copy;
    std::vector<std::string> prv_paths;
    const char **argv;
    std::string etiqueta;

    const char* paraver_name = "wxparaver";

    if(s != TTrace) {
        cli->error_generic("La comanda 'afegeix' només es pot utilitzar sobre un experiment de Tracing.");
        exit(1);
    }

    SUBJECT_SWITCH

    SUBJECT_SWITCH_APP

    SUBJECT_SWITCH_VAULT

    SUBJECT_SWITCH_TRACE
        
        LOAD_PROJECT_MACRO(p)
        LOAD_VAULT_MACRO(x)
        LOAD_TRACE_MACRO(t)

        dict_copy = t->get_list_prv();

        i = 1;
        if(comm->get_option("etiqueta")->count()) {
            argv = new const char* [3];
            etiqueta = *(comm->get_option("etiqueta")->results().begin());

            // Trobar el prv de letiqueta
            for (auto const& el : dict_copy) {
                if(el.second.label == etiqueta) {
                    prv_paths.push_back(t->get_base_path() / std::filesystem::path(el.first));
                    argv[i] = prv_paths[i-1].c_str();
                    i++;
                    break;
                }
            }
        } else {
            argv = new const char* [dict_copy.size()+2];
            for (auto const& el : dict_copy) {
                prv_paths.push_back(t->get_base_path() / std::filesystem::path(el.first));
                argv[i] = prv_paths[i-1].c_str();
                i++;
            }
        }



        argv[0] = paraver_name;
        argv[i] = NULL;
        
        // Crida Paraver
        if(pid = fork()) {
            waitpid(pid, &status, 0);
            if(status == -1) {
                cli->error_generic("Alguna cosa ha anat malament en executar Paraver:");
                cli->error_generic(std::strerror(errno));
            }
        } else {
            try
            {
                execvp(paraver_name, (char **)argv);
                /* exec does not return unless the program couldn't be started. 
                when the child process stops, the waitpid() above will return.
                */
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
            
            
        }

    SUBJECT_SWITCH_FIN

}


/**
 * @brief Funció per gestionar la comanda _informacio_
 * 
 * @param comm Punter a l'objecte amb la subcomanda
 * @param s subjecte sobre el que es crida l'accio
 * @param oh jerarquia d'objectes
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


void auto_command(CLI::App *comm) {
    Project * p;
    Vault *x;
    boost::gregorian::date di;
    Trace *t;
    int ncursor = comm->get_option("ncurs")->as<int>();
    //std::string parameters = *(comm->get_option("params")->results().begin());
    std::string parameters;
    object_t s;
    std::vector<std::string> params_list, oh;
    oh.push_back("");

    std::getline(std::cin, parameters);
    // Parsing manual del que ens arriba: tots els parametres sense lexecutable
    boost::split(params_list, parameters, boost::is_any_of(" "));
    for (size_t i = 0; i < params_list.size(); i++)
    {
        std::string opt = params_list[i];
        if (opt == "-a")
        {
            if(ncursor-1 == i+1) {
                // Jerarquia que estem suggerint
                s = TApplication;
                break;
            } else {
                // aquesta la guardem
                oh.push_back(params_list[i+1]);
            }
            i++;
        }
        if (opt == "-x")
        {
            if(ncursor-1 == i+1) {
                // Jerarquia que estem suggerint
                s = TVault;
                break;
            } else {
                // aquesta la guardem
                oh.push_back(params_list[i+1]);
            }
            i++;
        }
        if (opt == "-t")
        {
            if(ncursor-1 == i+1) {
                // Jerarquia que estem suggerint
                s = TTrace;
                break;
            } else {
                // aquesta la guardem
                oh.push_back(params_list[i+1]);
            }
            i++;
        }
        s = TStorage;
    }
    
    SUBJECT_SWITCH
    // Aquest cas es dona quan no hi ha cap situacio en la que sigui un recurs. Per tant podriem suggerir tant comandes com flags.

    SUBJECT_SWITCH_APP
        cli->list_applications(mstr);
    SUBJECT_SWITCH_VAULT
        LOAD_PROJECT_MACRO(p)
        cli->list_vaults(p);
    SUBJECT_SWITCH_TRACE
        LOAD_VAULT_MACRO(x)
        cli->list_experiments(x);
    SUBJECT_SWITCH_FIN


}

//@}

void read_global_state(std::string *app, std::string *vault) {
    if(boost::filesystem::exists(paraordenar_dir+"/app"))  {
        std::fstream app_state_file(paraordenar_dir+"/app", std::ios::in);
        std::string app_id;
        std::getline(app_state_file, app_id);
        *app = app_id;
    } else {
        std::fstream app_state_file(paraordenar_dir+"/app", std::ios::out);
        std::string no_app = "";
        app_state_file.write(no_app.c_str(), no_app.size());
        app_state_file.close();
        *app = no_app;
    }

    if(boost::filesystem::exists(paraordenar_dir+"/vault"))  {
        std::fstream vault_state_file(paraordenar_dir+"/vault", std::ios::in);
        std::string vault_id;
        std::getline(vault_state_file, vault_id);
        *vault = vault_id;
    } else {
        std::fstream vault_state_file(paraordenar_dir+"/vault", std::ios::out);
        std::string no_vault = "";
        vault_state_file.write(no_vault.c_str(), no_vault.size());
        vault_state_file.close();
        *vault = no_vault;
    }

    return;
}

void set_global_state_app(std::string app_id) {
    std::fstream app_state_file(paraordenar_dir+"/app", std::ios::out | std::ios::trunc);
    app_state_file.write(app_id.c_str(), app_id.size());
    app_state_file.close();
}

void set_global_state_vault(std::string vault_id)
{
    std::fstream vault_state_file(paraordenar_dir + "/vault", std::ios::out | std::ios::trunc);
    vault_state_file.write(vault_id.c_str(), vault_id.size());
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
