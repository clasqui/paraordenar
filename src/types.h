#pragma once
#define PARAORDENAR_VERSION "v0.1"

enum struct ExceptionType { 
    ENoPath, 
    EStorageNotExists, 
    EMalformedStorage,
    EXMLParseError,
    EMemoryError
};

struct PROException {
    ExceptionType e_t;
    string message;

    PROException(ExceptionType e_t, string message) : e_t(e_t), message(message) {};
    ~PROException() {};
};