const int LED_VERDE      = 27;
const int LED_AMARELO    = 26;
const int LED_AZUL       = 25;

const int BTN_BRINCAR    = 14;
const int BTN_ALIMENTAR  = 32;
const int BTN_DORMIR     = 33;

struct Botao {
    int pino;
    bool ultimaLeitura;
    bool estavel;
    unsigned long tempo;
};

class StatePet;

class EstadoStatePet {
public:
    virtual void entrar() = 0;
    virtual void alimentar(StatePet * t) = 0;
    virtual void brincar(StatePet * t) = 0;
    virtual void dormir(StatePet * t) = 0;
    virtual ~EstadoStatePet() {}
};

class StatePet {
private:
    EstadoStatePet * estadoAtual;

public:
    StatePet(EstadoStatePet * estadoInicial) {
        estadoAtual = estadoInicial;
    }

    void iniciar() {
        estadoAtual->entrar();
    }

    void definirEstado(EstadoStatePet * novoEstado) {
        estadoAtual = novoEstado;
        estadoAtual->entrar();
    }

    void alimentar() { estadoAtual->alimentar(this); }
    void brincar()   { estadoAtual->brincar(this); }
    void dormir()    { estadoAtual->dormir(this); }
};

class EstadoFeliz : public EstadoStatePet {
public:
    void entrar() override;
    void alimentar(StatePet* t) override;
    void brincar(StatePet* t) override;
    void dormir(StatePet* t) override;
};

class EstadoFome : public EstadoStatePet {
public:
    void entrar() override;
    void alimentar(StatePet* t) override;
    void brincar(StatePet* t) override;
    void dormir(StatePet* t) override;
};

class EstadoSono : public EstadoStatePet {
public:
    void entrar() override;
    void alimentar(StatePet* t) override;
    void brincar(StatePet* t) override;
    void dormir(StatePet* t) override;
};

EstadoFeliz feliz;
EstadoFome fome;
EstadoSono sono;

void mostrarLeds(bool verde, bool amarelo, bool azul) {
    digitalWrite(LED_VERDE, verde ? HIGH : LOW);
    digitalWrite(LED_AMARELO, amarelo ? HIGH : LOW);
    digitalWrite(LED_AZUL, azul ? HIGH : LOW);
}

void EstadoFeliz::entrar() {
    mostrarLeds(true, false, false);
}

void EstadoFeliz::alimentar(StatePet* t) {
    Serial.println("StatePet recusa a comida: ele nao esta com fome.");
}

void EstadoFeliz::brincar(StatePet* t) {
    Serial.println("Voces brincam muito! Mas agora ele ficou com fome.");
    t->definirEstado(&fome);
}

void EstadoFeliz::dormir(StatePet* t) {
    Serial.println("StatePet bocejou e foi dormir. Zzz...");
    t->definirEstado(&sono);
}

void EstadoFome::entrar() {
    mostrarLeds(false, true, false);
}

void EstadoFome::alimentar(StatePet* t) {
    Serial.println("StatePet comeu e ficou muito feliz!");
    t->definirEstado(&feliz);
}

void EstadoFome::brincar(StatePet* t) {
    Serial.println("StatePet esta com muita fome para brincar e chora!");
}

void EstadoFome::dormir(StatePet* t) {
    Serial.println("StatePet esta com fome demais para conseguir dormir.");
}

void EstadoSono::entrar() {
    mostrarLeds(false, false, true);
}

void EstadoSono::alimentar(StatePet* t) {
    Serial.println("StatePet esta dormindo e nao quer comer agora.");
}

void EstadoSono::brincar(StatePet* t) {
    Serial.println("Shhh! StatePet esta dormindo, nao da para brincar.");
}

void EstadoSono::dormir(StatePet* t) {
    Serial.println("StatePet acordou descansado e feliz!");
    t->definirEstado(&feliz);
}

Botao btnBrincar   = { BTN_BRINCAR,   HIGH, HIGH, 0 };
Botao btnAlimentar = { BTN_ALIMENTAR, HIGH, HIGH, 0 };
Botao btnDormir    = { BTN_DORMIR,    HIGH, HIGH, 0 };

bool foiPressionado(Botao & b) {
    bool leitura = digitalRead(b.pino);

    if (leitura != b.ultimaLeitura) {
        b.tempo = millis();
        b.ultimaLeitura = leitura;
    }

    if (millis() - b.tempo > 50 && leitura != b.estavel) {
        b.estavel = leitura;
        if (b.estavel == LOW) {
            return true;
        }
    }
    return false;
}

StatePet meuPet(&feliz);

void setup() {
    Serial.begin(9600);

    pinMode(LED_VERDE, OUTPUT);
    pinMode(LED_AMARELO, OUTPUT);
    pinMode(LED_AZUL, OUTPUT);

    pinMode(BTN_BRINCAR, INPUT_PULLUP);
    pinMode(BTN_ALIMENTAR, INPUT_PULLUP);
    pinMode(BTN_DORMIR, INPUT_PULLUP);

    meuPet.iniciar();
    Serial.println("StatePet pronto! Estado inicial: Feliz");
}

void loop() {
    if (foiPressionado(btnBrincar))   meuPet.brincar();
    if (foiPressionado(btnAlimentar)) meuPet.alimentar();
    if (foiPressionado(btnDormir))    meuPet.dormir();
}

