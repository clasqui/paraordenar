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