/**
 * @file types.h
 * @author Marc Clascà Ramírez
 * @brief Declaració de tipus varis de l'espai de noms
 * @version 0.1
 * @date 2022-07-02
 * 
 */

#pragma once

#define SUBJECT_SWITCH switch (s) \
                        { \
                        case TStorage:

#define SUBJECT_SWITCH_APP    break; \
                                \
                            case TApplication:

#define SUBJECT_SWITCH_VAULT    break; \
                                \
                            case TVault:

#define SUBJECT_SWITCH_TRACE    break; \
                                \
                            case TTrace:

#define SUBJECT_SWITCH_FIN   break; \
                            default: \
                             break; \
                            }

    namespace ParaordenarCore
    {

        /**
         * @brief Enum que declara diferents tipus d'excepcions
         *          que gestiona la llibreria
         *
         */
        enum struct ExceptionType
        {
            ENoPath,           /*!< No existeix el camí */
            EStorageNotExists, /*!< L'emmagatzematge indicat no existeix */
            EMalformedStorage, /*!< L'emmagatzematge obert no correspon al format de paraordenar */
            EXMLParseError,    /*!< Hi ha un problema parsejant XML dels fitxers d'especificació de l'emmagatzematge */
            EMemoryError,      /*!< Hi ha hagut un problema de memòria */
            ENameNotValid,     /*!< El nom especificat no és vàlid */
            EAlreadyExists     /*!< Ja existeix un emmagatzematge en el camí indicat */
        };

        /**
         * @brief Representa una excepció dins
         *  el sistema de paraordenar.
         *
         * En comptes de ser una classe, és un struct
         * per simplicitat.
         */
        struct PROException
        {
            ExceptionType e_t;   /*!< El tipus d'excpeció ExceptionType */
            std::string message; /*!< Missatge que acompanya l'excpeció */

            PROException(ExceptionType e_t, std::string message) : e_t(e_t), message(message){};
            ~PROException(){};
        };

        /**
         * @brief Enum que declara els diferents tipus
         *          de recursos que existeixen al sistema
         *          ParaordenarCore.
         *
         * El seu valor en enter representa també la jerarquia
         * d'emmagatzematge dins el sistema. Per tant permet
         * comparar diferents valors de object_t per saber
         * l'ordre en el que es troben.
         *
         */
        typedef enum
        {
            TStorage,     /*!< Representa el nivell Storage */
            TApplication, /*!< Representa el nivell Project */
            TVault,       /*!< Representa el nivell Vault */
            TTrace        /*!< Representa el nivell Trace */
        } object_t;
    }