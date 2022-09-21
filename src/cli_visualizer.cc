/**
 * @file cli_visualizer.cc
 * @author Marc Clasca Ramirez
 * @brief Conté la implementació de la classe de visualització per la CLI
 * @version 0.1
 * @date 2022-07-07
 * 
 */

#include "cli_visualizer.hpp"

CLI_Visualizer::CLI_Visualizer(bool pretty_print)
{
    this->pr = pretty_print;
}

CLI_Visualizer::~CLI_Visualizer()
{
}

void CLI_Visualizer::print_info_storage(Storage *strg) {
    if(pr) {
        fmt::print("El teu emmagatzematge es troba a {}\n", fmt::styled(strg->get_path(), fmt::emphasis::italic));
        fmt::print("Número de projectes: {}\n", strg->get_apps().size());
    } else {
        fmt::print("{}\n", strg->get_path());
        fmt::print("{}\n", strg->get_apps().size());
    }
}

void CLI_Visualizer::print_info_application(Project *p) {

    if(pr) {
        fmt::print("\t┌{0:-^{1}}┐\n"
                "\t│{2: ^{1}}│\n"
                "\t│{3: ^{1}}│\n"
                "\t└{0:-^{1}}┘\n",
                    "", 30, fmt::styled(p->get_name(), fmt::emphasis::bold), p->get_active() ? "Actiu" : "Arxivat");
        fmt::print("Descripció:   {}\n"
                "Llenguatge:   {}\n"
                "Data d'inici: {:%m-%Y}\n",
                    p->get_description(), p->get_language(), boost::gregorian::to_tm(p->get_inici()));
        if(!p->get_active()) {
            fmt::print("Data fi:      {:%m-%Y}\n", boost::gregorian::to_tm(p->get_final()));
        }
    } else {
        fmt::print("{0}\n"
                   "{1}\n"
                   "{2}\n"
                   "{3}\n"
                   "{4:%m-%Y}\n"
                   "{5:%m-%Y}\n",
        p->get_name(), p->get_active(), p->get_description(), p->get_language(), 
        boost::gregorian::to_tm(p->get_inici()), boost::gregorian::to_tm(p->get_final()));
    }
}

void CLI_Visualizer::print_info_vault(Vault *x) {
    if(pr) {
        fmt::print(fmt::emphasis::bold, "{}\n", x->get_name());
        std::cout << "Descripció: " << x->get_description() << std::endl;
        if(!x->get_conclusions().empty()) {
            std::cout << "Conclusions: " << x->get_conclusions() << std::endl;
        }
    } else {
        std::cout << x->get_name() << std::endl;
        std::cout << x->get_description() << std::endl;
        std::cout << x->get_conclusions() << std::endl;
    }
}

void CLI_Visualizer::print_info_trace(Trace *t) {
    if(pr) {
        fmt::print("\t┌{0:-^{1}}┐\n"
                "\t│{2: ^{1}}│\n"
                "\t│{3: ^{1}}│\n"
                "\t│ #Threads: {4: <8}  #Ranks: {5: <11}│\n"
                "\t└{0:-^{1}}┘\n",
                "", 40, fmt::styled(t->get_name(), 
                fmt::emphasis::bold), 
                fmt::styled(Experiment::ExperimentTypeNames[t->get_type()], fmt::fg(fmt::color::sky_blue)),
                t->get_omp_threads(), t->get_mpi_tasks());
        fmt::print("\nDescrpició: {}\n"
                    "Arxiu de Log: {}\n", 
                    t->get_description(),
                    t->get_logfile_name());
    } else {
        fmt::print("{0}\n"
                   "{1}\n"
                   "{2}\n"
                   "{3}\n"
                   "{4}\n",
        t->get_name(), Experiment::ExperimentTypeNames[t->get_type()], 
        t->get_base_path().string(), 
        t->get_description(),
        t->get_logfile_name());
    }
}


void CLI_Visualizer::list_applications(Storage *s) {
    if(pr) {
        std::cout << std::endl;
        fmt::print(fmt::emphasis::bold, "{:^40}\n", "Llista de projectes");
        fmt::print("{:^30}|{:^9}\n", "Nom", "Estat");
        fmt::print("{:-^40}\n", "");
        for (auto &&p : s->get_apps())
        {
            if(p.second) {
                fmt::print(" {:<29}| {:<8}\n", p.first, fmt::styled("Actiu", fmt::fg(fmt::color::light_green)));
            } else {
                fmt::print(" {:<29}| {:<8}\n", p.first, fmt::styled("Arxivat", fmt::fg(fmt::color::red)));
            }
        }

    } else {
        for (auto &&p : s->get_apps())
        {
            fmt::print("{}:{}\n", p.first, p.second);
        }
    }
}

void CLI_Visualizer::list_vaults(Project *p) {
    if(pr) {
        std::cout << std::endl;
        fmt::print(fmt::emphasis::bold, "{:^30}\n", "Llista de caixes");
        fmt::print("{:^30}\n", "Nom");
        fmt::print("{:-^30}\n", "");
        for (auto &&p : p->get_vaults())
        {
            fmt::print("{:^30}\n", p);
        }
    } else {
        for (auto &&p : p->get_vaults())
        {
            fmt::print("{}\n", p);
        }
    }
}

void CLI_Visualizer::list_experiments(Vault *x) {
    Experiment *experiment;
    if(pr) {
        std::cout << std::endl;
        fmt::print(fmt::emphasis::bold, "{:^60}\n", "Llista d'experiments");
        fmt::print("{:^32}|{:^10}|{:^16}\n", "Nom", "Tipus", "Data");
        fmt::print("{:-^60}\n", "");
        for (auto &&tw : x->get_experiments())
        {
            experiment = x->open_experiment(tw.first);
            fmt::print(" {:<31}| {:<9}| {:%d-%m-%Y}\n", tw.first, 
                Experiment::ExperimentTypeNames[tw.second], 
                boost::gregorian::to_tm(experiment->get_date()));
            delete experiment;
        }
        std::cout << std::endl;
    } else {
        for (auto &&tw : x->get_experiments())
        {
            fmt::print("{}:{}\n", tw.first, tw.second);
        }
    }
}

void CLI_Visualizer::list_prvs(Trace *t) {
        if(pr) {
        std::cout << std::endl;
        fmt::print(fmt::emphasis::bold, "{:^74}\n", "Llista de traces");
        fmt::print("{:^32}|{:^21}|{:^10}|{:^8}\n", "Arxiu", "Etiqueta", "#Threads", "#Ranks");
        fmt::print("{:-^74}\n", "");
        for (auto &&prv : t->get_list_prv())
        {
            fmt::print(" {:<31}| {:<20}| {:<9}|{:<7}\n", prv.first, 
                prv.second.label, 
                prv.second.n_threads,
                prv.second.n_ranks);
        }
        std::cout << std::endl;
    } else {
        for (auto &&prv : t->get_list_prv())
        {
            fmt::print("{}:{}:{}:{}\n", prv.first, prv.second.label, prv.second.n_threads, prv.second.n_ranks);
        }
    }
}


void CLI_Visualizer::err_no_existeix(std::string que, std::string s) {
    if(pr)
        fmt::print(std::cerr, "{} {} {} no existeix.\n", fmt::styled("!!", fmt::fg(fmt::color::red) | fmt::emphasis::bold), que, fmt::styled(s, fmt::emphasis::italic));
    else
        fmt::print(std::cerr, "!! {} {} no existeix.\n", que, s);
}

void CLI_Visualizer::error_generic(std::string missatge) {
    if(pr)
        fmt::print(std::cerr, "{} {}\n", fmt::styled("!!", fmt::fg(fmt::color::red) | fmt::emphasis::bold), missatge);
    else
        fmt::print(std::cerr, "!! {}\n", missatge);
}