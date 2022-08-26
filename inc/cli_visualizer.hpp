/**
 * @file cli_visualizer.hpp
 * @author Marc Clasca Ramirez
 * @brief Conté la definició de la classe de visualització per la CLI
 * @version 0.1
 * @date 2022-07-07
 * 
 */

#include <fmt/core.h>
#include <fmt/chrono.h>
#include <fmt/color.h>
#include <string>

#include "storage.hpp"
#include "project.hpp"
#include <boost/date_time/gregorian/conversion.hpp>

using namespace ParaordenarCore;

/**
 * @brief Funcions de visualització de la informació per terminal.
 * 
 */
class CLI_Visualizer
{
private:
    bool pr; /*!< indica si per aquest objecte s'imprimira amb format i descripcio o sense */
public:

    /**
     * @brief Construeix un nou objecte de visualitzacio per terminal
     * 
     * @param pretty_print Indica si la informació es mostra amb format i descripcio (true) o sense (true)
     */
    CLI_Visualizer(bool pretty_print);
    ~CLI_Visualizer();

    void print_info_application(Project *p);
};