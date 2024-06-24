#include <stdio.h>
#include <stdlib.h>

typedef struct State {
    int id;
    int isFinal;
    struct State* transitions[256];
} State;

typedef struct Automato {
    State* star_state;
    State* states[256];
    int num_states;
} Automato;

State* criar_Estado(int id, int isFinal) {
    State* state = (State*)malloc(sizeof(State));
    state->id = id;
    state->isFinal = isFinal;
    for (int i = 0; i < 256; i++) {
        state->transitions[i] = NULL;
    }
    return state;
}

void add_Transicao(State* from, State* to, char symbol) {
    from->transitions[(int)symbol] = to;
}

void free_Automato(Automato* automato) {
    for (int i = 0; i < automato->num_states; i++) {
        free(automato->states[i]);
    }
    free(automato);
}

Automato* read_Automato(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("ERROR! -> Nao foi possivel abrir arquivo de entrada.\n");
        return NULL;
    }

    Automato* automato = (Automato*)malloc(sizeof(Automato));
    automato->num_states = 0;
    automato->star_state = NULL;

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        int isInitial = 0, isFinal = 0;
        int fromStateId, toStateId;
        char symbol;

        if (line[0] == '-' && line[1] == '>') {
            isInitial = 1;
            sscanf(line + 2, "q%d,%c,q%d", &fromStateId, &symbol, &toStateId);
        } else if (line[0] == '*') {
            isFinal = 1;
            sscanf(line + 1, "q%d,%c,q%d", &fromStateId, &symbol, &toStateId);
        } else {
            sscanf(line, "q%d,%c,q%d", &fromStateId, &symbol, &toStateId);
        }

        State* fromState = NULL;
        State* toState = NULL;

        for (int i = 0; i < automato->num_states; i++) {
            if (automato->states[i]->id == fromStateId) {
                fromState = automato->states[i];
            }
            if (automato->states[i]->id == toStateId) {
                toState = automato->states[i];
            }
        }

        if (fromState == NULL) {
            fromState = criar_Estado(fromStateId, 0);
            automato->states[automato->num_states++] = fromState;
        }
        if (toState == NULL) {
            toState = criar_Estado(toStateId, isFinal);
            automato->states[automato->num_states++] = toState;
        }

        add_Transicao(fromState, toState, symbol);

        if (isInitial) {
            automato->star_state = fromState;
        }
    }

    fclose(file);
    return automato;
}

int process_String(Automato* automato, const char* string) {
    State* currentState = automato->star_state;
    for (int i = 0; string[i] != '\0'; i++) {
        char symbol = string[i];
        currentState = currentState->transitions[(int)symbol];
        if (currentState == NULL) {
            return 0;
        }
    }
    return currentState->isFinal;
}

int isLowerCaseOrDigit(char c) {
    return (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9');
}

int validacao_Email(const char* email) {
    int atFound = 0;
    int dotFound = 0;

    while (*email != '\0') {
        if (*email == '@') {
            if (atFound) {
                return 0;
            }
            atFound = 1;
        } else if (*email == '.') {
            if (!atFound) {
                return 0;
            }
            dotFound = 1;
        } else if (!isLowerCaseOrDigit(*email)) {
            return 0;
        }
        email++;
    }

    return atFound && dotFound;
}

void write_Output(const char* email, int result, FILE* outputFile) {
    fprintf(outputFile, "%s,%s\n", email, result ? "yes" : "no");
}

int main() {
    Automato* automato = read_Automato("henrique_santos_interdisciplinar_afd_email.csv");
    if (automato == NULL) {
        printf("ERROR -> Nao foi possivel ler arquivo de entrada.\n");
        return 1;
    }

    FILE* inputFile = fopen("henrique_santos_interdisciplinar_afd_email.input", "r");
    if (inputFile == NULL) {
        printf("ERROR! -> Nao foi possivel abrir arquivo de entrada.\n");
        free_Automato(automato);
        return 1;
    }

    FILE* outputFile = fopen("henrique_santos_interdisciplinar_afd_email.output", "w");
    if (outputFile == NULL) {
        printf("ERROR -> Nao foi possivel abrir arquivo de saida.\n");
        fclose(inputFile);
        free_Automato(automato);
        return 1;
    }

    char line[256];
    while (fgets(line, sizeof(line), inputFile)) {
        int i = 0;
        while (line[i] != '\0' && line[i] != '\n') {
            i++;
        }
        line[i] = '\0';

        int is_Valido = validacao_Email(line);

        int result = is_Valido ? process_String(automato, line) : 0;
        write_Output(line, result, outputFile);
    }

    fclose(inputFile);
    fclose(outputFile);

    free_Automato(automato);

    return 0;
}
