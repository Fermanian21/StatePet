#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int LED_VERMELHO = 27;
const int LED_AMARELO = 14;
const int LED_VERDE = 26;

const int BTN_BRINCAR = 25;
const int BTN_ALIMENTAR = 33;
const int BTN_DORMIR = 32;

const int BUZZER = 4;

struct Botao {
  int pino;
  bool ultimaLeitura;
  bool estavel;
  unsigned long tempo;
};

Adafruit_SSD1306 tela(128, 64, &Wire, -1);
bool telaOk = false;

const char* rotuloAtual = "";
unsigned long mensagemAte = 0;

void escreverLinha(const char* txt) {
  if (!telaOk) return;
  tela.fillRect(0, 54, 128, 10, SSD1306_BLACK);
  tela.setTextSize(1);
  tela.setTextColor(SSD1306_WHITE);
  int x = (128 - (int)strlen(txt) * 6) / 2;
  if (x < 0) x = 0;
  tela.setCursor(x, 56);
  tela.print(txt);
  tela.display();
}

void definirRotulo(const char* r) {
  rotuloAtual = r;
  mensagemAte = 0;
  escreverLinha(r);
}

void mostrarMensagemTemporaria(const char* m) {
  escreverLinha(m);
  mensagemAte = millis() + 1500;
}

void atualizarTela() {
  if (mensagemAte != 0 && millis() > mensagemAte) {
    escreverLinha(rotuloAtual);
    mensagemAte = 0;
  }
}

void rostoFeliz() {
  if (!telaOk) return;
  tela.clearDisplay();
  tela.fillCircle(40, 18, 6, SSD1306_WHITE);
  tela.fillCircle(88, 18, 6, SSD1306_WHITE);
  tela.drawCircleHelper(64, 32, 18, 0x0C, SSD1306_WHITE);  // sorriso
  tela.drawCircleHelper(64, 32, 17, 0x0C, SSD1306_WHITE);
}

void rostoFome() {
  if (!telaOk) return;
  tela.clearDisplay();
  tela.fillCircle(40, 20, 4, SSD1306_WHITE);
  tela.fillCircle(88, 20, 4, SSD1306_WHITE);
  tela.drawLine(30, 14, 46, 10, SSD1306_WHITE);
  tela.drawLine(98, 14, 82, 10, SSD1306_WHITE);
  tela.drawCircleHelper(64, 52, 14, 0x03, SSD1306_WHITE);
  tela.drawCircleHelper(64, 52, 13, 0x03, SSD1306_WHITE);
  tela.fillCircle(98, 30, 2, SSD1306_WHITE);
}

void rostoSono() {
  if (!telaOk) return;
  tela.clearDisplay();
  tela.drawLine(30, 22, 50, 22, SSD1306_WHITE);
  tela.drawLine(30, 23, 50, 23, SSD1306_WHITE);
  tela.drawLine(78, 22, 98, 22, SSD1306_WHITE);
  tela.drawLine(78, 23, 98, 23, SSD1306_WHITE);
  tela.drawCircle(64, 40, 5, SSD1306_WHITE);
  tela.setTextColor(SSD1306_WHITE);
  tela.setTextSize(2);
  tela.setCursor(104, 2);
  tela.print("Z");
  tela.setTextSize(1);
  tela.setCursor(118, 22);
  tela.print("z");
}

void bipe(int ms) {
  digitalWrite(BUZZER, HIGH);
  delay(ms);
  digitalWrite(BUZZER, LOW);
}

void bipes(int vezes, int ms) {
  for (int i = 0; i < vezes; i++) {
    bipe(ms);
    if (i < vezes - 1) delay(ms);
  }
}

void mostrarLeds(bool vermelho, bool amarelo, bool verde) {
  digitalWrite(LED_VERMELHO, vermelho ? HIGH : LOW);
  digitalWrite(LED_AMARELO, amarelo ? HIGH : LOW);
  digitalWrite(LED_VERDE, verde ? HIGH : LOW);
}

void recusar(const char* msgTela) {
  mostrarMensagemTemporaria(msgTela);
  bipes(1, 30);
}

class StatePet;

class EstadoStatePet {
public:
  virtual void entrar() = 0;
  virtual void alimentar(StatePet* t) = 0;
  virtual void brincar(StatePet* t) = 0;
  virtual void dormir(StatePet* t) = 0;
  virtual ~EstadoStatePet() {}
};

class StatePet {
private:
  EstadoStatePet* estadoAtual;

public:
  StatePet(EstadoStatePet* estadoInicial) {
    estadoAtual = estadoInicial;
  }

  void iniciar() {
    estadoAtual->entrar();
  }

  void definirEstado(EstadoStatePet* novoEstado) {
    estadoAtual = novoEstado;
    estadoAtual->entrar();
  }

  void alimentar() {
    estadoAtual->alimentar(this);
  }
  void brincar() {
    estadoAtual->brincar(this);
  }
  void dormir() {
    estadoAtual->dormir(this);
  }
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

void EstadoFeliz::entrar() {
  mostrarLeds(true, false, false);
  rostoFeliz();
  definirRotulo("Feliz!");
  bipes(1, 80);
}

void EstadoFeliz::alimentar(StatePet* t) {
  Serial.println("StatePet recusa a comida: ele nao esta com fome.");
  recusar("Sem fome, obrigado");
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
  rostoFome();
  definirRotulo("Com fome");
  bipes(2, 80);
}

void EstadoFome::alimentar(StatePet* t) {
  Serial.println("StatePet comeu e ficou muito feliz!");
  t->definirEstado(&feliz);
}

void EstadoFome::brincar(StatePet* t) {
  Serial.println("StatePet esta com muita fome para brincar e chora!");
  recusar("Com fome... buaa");
}

void EstadoFome::dormir(StatePet* t) {
  Serial.println("StatePet esta com fome demais para conseguir dormir.");
  recusar("Fome demais!");
}

void EstadoSono::entrar() {
  mostrarLeds(false, false, true);
  rostoSono();
  definirRotulo("Dormindo");
  bipes(1, 300);
}

void EstadoSono::alimentar(StatePet* t) {
  Serial.println("StatePet esta dormindo e nao quer comer agora.");
  recusar("Zzz... dormindo");
}

void EstadoSono::brincar(StatePet* t) {
  Serial.println("Shhh! StatePet esta dormindo, nao da para brincar.");
  recusar("Shhh! Dormindo");
}

void EstadoSono::dormir(StatePet* t) {
  Serial.println("StatePet acordou descansado e feliz!");
  t->definirEstado(&feliz);
}

Botao btnBrincar = { BTN_BRINCAR, HIGH, HIGH, 0 };
Botao btnAlimentar = { BTN_ALIMENTAR, HIGH, HIGH, 0 };
Botao btnDormir = { BTN_DORMIR, HIGH, HIGH, 0 };

bool foiPressionado(Botao& b) {
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
  Serial.begin(115200);

  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(LED_AMARELO, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  pinMode(BTN_BRINCAR, INPUT_PULLUP);
  pinMode(BTN_ALIMENTAR, INPUT_PULLUP);
  pinMode(BTN_DORMIR, INPUT_PULLUP);

  Wire.begin();
  telaOk = tela.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  if (!telaOk) {
    Serial.println("OLED nao encontrado (confira SDA/SCL e o endereco 0x3C). Seguindo sem tela.");
  }

  meuPet.iniciar();
  Serial.println("StatePet pronto! Estado inicial: Feliz");
}

void loop() {
  if (foiPressionado(btnBrincar)) meuPet.brincar();
  if (foiPressionado(btnAlimentar)) meuPet.alimentar();
  if (foiPressionado(btnDormir)) meuPet.dormir();

  atualizarTela();
}