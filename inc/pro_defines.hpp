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

#define SUBJECT_SWITCH_WALLTIME    break; \
                                \
                            case TWalltime:

#define SUBJECT_SWITCH_FIN   break; \
                            default: \
                             break; \
                            }


#define LOAD_PROJECT_MACRO(p) p = mstr->open_app(oh[1]); \
        if(p == nullptr) { \
            cli->err_no_existeix("El projecte", oh[1]); \
            exit(1); \
        }
        

#define LOAD_VAULT_MACRO(x) x = p->open_vault(oh[2]); \
        if(x == nullptr) { \
            cli->err_no_existeix("La caixa", oh[2]); \
            exit(1); \
        }


#define LOAD_TRACE_MACRO(t) t = (Trace *)x->open_experiment(oh[3]); \
        if(t == nullptr) { \
            cli->err_no_existeix("La traça", oh[3]); \
            exit(1); \
        }
