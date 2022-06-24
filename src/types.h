#pragma once
#define PARAORDENAR_VERSION "v0.1"

namespace ParaordenarCore {

enum struct ExceptionType { 
    ENoPath, 
    EStorageNotExists, 
    EMalformedStorage,
    EXMLParseError,
    EMemoryError,
    ENameNotValid,
    EAlreadyExists
};

struct PROException {
    ExceptionType e_t;
    std::string message;

    PROException(ExceptionType e_t, std::string message) : e_t(e_t), message(message) {};
    ~PROException() {};
};

typedef enum {
    TStorage,
    TApplication,
    TVault,
    TTrace
} object_t;

}