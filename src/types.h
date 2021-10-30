
enum struct ExceptionType { 
    ENoStorage, 
    EStorageNotExists, 
    EMalformedStorage 
};

struct PROException {
    ExceptionType e_t;
    string message;

    PROException(ExceptionType e_t, string message) : e_t(e_t), message(message) {};
    ~PROException() {};
};