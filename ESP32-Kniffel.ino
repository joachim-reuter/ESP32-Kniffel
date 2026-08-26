/*   Kniffel V 3.0 

user_setup:
#define ILI9488_DRIVER

#define TFT_WIDTH  480
#define TFT_HEIGHT 320

#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   15
#define TFT_DC   2
#define TFT_RST  4

#define TOUCH_CS 21

#define SPI_FREQUENCY       40000000
#define SPI_TOUCH_FREQUENCY 2500000

#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF
#define SMOOTH_FONT

!!! Wichtig :  Espressif esp32 lib 2.10.14 !!!

*/
#include <TFT_eSPI.h>
#include <SPI.h>

// Touch-Kalibrierung
uint16_t calData[5] = { 375, 3394, 487, 3192, 1 };

// Spielvariablen
#define NUM_DICE 5
int dice[NUM_DICE];
bool keep[NUM_DICE];
int rollCount = 0;

// Würfel 3D Farben
#define DICE_LIGHT  0xFFFF   // weiß
#define DICE_DARK   0xC618   // dunkelgrau
#define DICE_EDGE   0x8410   // mittelgrau

// TFT & Touch
TFT_eSPI tft = TFT_eSPI();

// Würfel Position
int diceX[5] = {40, 130, 220, 310, 400};
int diceY = 120;
#define DICE_SIZE 70

// Buttons
#define BTN_X 160
#define BTN_Y 230
#define BTN_W 160
#define BTN_H 60

#define EXIT_X 20
#define EXIT_Y 230
#define EXIT_W 120
#define EXIT_H 60

// Buzzer
#define BUZZER_PIN 13
#define BUZZER_CHANNEL 0
#define BUZZER_RES 8

// Textzeile für Ergebnis
#define RESULT_Y 80

// --- Funktionen --- //

// Würfel 3D zeichnen
void drawDice3D(int x, int y, int size, uint16_t bodyColor) {
  tft.fillRoundRect(x, y, size, size, 10, bodyColor);
  tft.drawFastHLine(x + 4, y + 3, size - 8, DICE_LIGHT);
  tft.drawFastVLine(x + 3, y + 4, size - 8, DICE_LIGHT);
  tft.drawFastHLine(x + 4, y + size - 3, size - 8, DICE_DARK);
  tft.drawFastVLine(x + size - 3, y + 4, size - 8, DICE_DARK);
  tft.drawRoundRect(x, y, size, size, 10, DICE_EDGE);
}

// Würfelaugen zeichnen
void drawPips(int x, int y, int size, int value) {
  int r = size / 10;          
  int o = size / 4;           
  int c = size / 2;           

  auto pip = [&](int px, int py) {
    tft.fillCircle(x + px, y + py, r, TFT_BLACK);
  };

  switch (value) {
    case 1: pip(c, c); break;
    case 2: pip(o, o); pip(size - o, size - o); break;
    case 3: pip(o, o); pip(c, c); pip(size - o, size - o); break;
    case 4: pip(o, o); pip(size - o, o); pip(o, size - o); pip(size - o, size - o); break;
    case 5: pip(o, o); pip(size - o, o); pip(c, c); pip(o, size - o); pip(size - o, size - o); break;
    case 6: pip(o, o); pip(o, c); pip(o, size - o); pip(size - o, o); pip(size - o, c); pip(size - o, size - o); break;
  }
}

// Alle Würfel zeichnen
void drawDice() {
  for (int i = 0; i < NUM_DICE; i++) {
    uint16_t bodyColor = keep[i] ? 0x9FE0 : TFT_WHITE;
    drawDice3D(diceX[i], diceY, DICE_SIZE, bodyColor);
    if (dice[i] > 0) drawPips(diceX[i], diceY, DICE_SIZE, dice[i]);
  }
}

// Würfelanimation mit Sound
void animateRoll() {
  const int frames = 12;
  const int delayMs = 35;

  for (int f = 0; f < frames; f++) {
    for (int i = 0; i < NUM_DICE; i++) if (!keep[i]) dice[i] = random(1,7);
    drawDice();

    // Würfelklackern
    int freq = random(800, 1500);
    ledcWriteTone(BUZZER_CHANNEL, freq);
    delay(delayMs);
    ledcWriteTone(BUZZER_CHANNEL, 0);
    delay(10);
  }
  ledcWriteTone(BUZZER_CHANNEL, 0);
}

// Würfeln
void rollDice() {
  animateRoll();
  for (int i = 0; i < NUM_DICE; i++) if (!keep[i]) dice[i] = random(1,7);
  rollCount++;
  redraw();
}

// Buttons zeichnen
void drawButton() {
  tft.fillRoundRect(BTN_X, BTN_Y, BTN_W, BTN_H, 10, TFT_BLUE);
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(2);
  tft.drawString(rollCount<3?"START":"RESET", BTN_X + BTN_W/2, BTN_Y + BTN_H/2);
}

void drawExitButton() {
  if (rollCount==0 || rollCount>=3) return;
  tft.fillRoundRect(EXIT_X, EXIT_Y, EXIT_W, EXIT_H, 10, TFT_RED);
  tft.setTextColor(TFT_WHITE, TFT_RED);
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(2);
  tft.drawString("FERTIG", EXIT_X + EXIT_W/2, EXIT_Y + EXIT_H/2);
}

void drawResultText() {
  if (rollCount == 0) return;

  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(2);
  tft.setTextColor(TFT_YELLOW, TFT_DARKGREY);

  String result = evaluateDice();
  tft.drawString(result, TFT_WIDTH / 2, RESULT_Y);
}



// Touch auswerten
void handleTouch(uint16_t x, uint16_t y) {
  for (int i=0;i<NUM_DICE;i++) {
    if (x>diceX[i] && x<diceX[i]+DICE_SIZE && y>diceY && y<diceY+DICE_SIZE && rollCount>0) {
      keep[i]=!keep[i]; drawDice(); return;
    }
  }

  if (x>BTN_X && x<BTN_X+BTN_W && y>BTN_Y && y<BTN_Y+BTN_H) {
    if (rollCount<3) rollDice(); else resetGame();
    redraw();
  }

  if (rollCount>0 && rollCount<3 && x>EXIT_X && x<EXIT_X+EXIT_W && y>EXIT_Y && y<EXIT_Y+EXIT_H) {
    delay(500);
    resetGame();
    redraw();
    return;
  }
}

String evaluateDice() {
  bool present[7] = {false}; // 1..6
  int count[7] = {0};

  for (int i = 0; i < NUM_DICE; i++) {
    present[dice[i]] = true;
    count[dice[i]]++;
  }

  // Pasch-Erkennung
  bool three=false, four=false, five=false;
  int pairs=0;

  for (int i=1;i<=6;i++) {
    if (count[i]==5) five=true;
    if (count[i]==4) four=true;
    if (count[i]==3) three=true;
    if (count[i]==2) pairs++;
  }

  // 🔴 Kniffel
  if (five) return "KNIFFEL!";

  // 🟢 Kleine Straße: 1-2-3-4-5
  if (present[1] && present[2] && present[3] &&
      present[4] && present[5])
    return "KLEINE STRASSE";

  // 🔵 Große Straße: 2-3-4-5-6
  if (present[2] && present[3] && present[4] &&
      present[5] && present[6])
    return "GROSSE STRASSE";

  // 🟡 Full House
  if (three && pairs==1) return "FULL HOUSE";

  if (four) return "VIER GLEICHE";
  if (three) return "DREI GLEICHE";
  if (pairs==2) return "ZWEI PAARE";
  if (pairs==1) return "EIN PAAR";

  return "KEINE KOMBINATION";
}






// Reset Spiel
void resetGame() {
  rollCount=0;
  for (int i=0;i<NUM_DICE;i++){ keep[i]=false; dice[i]=0; }
}

// Bildschirm neu zeichnen
void redraw() {
  tft.fillScreen(TFT_DARKGREY);

  tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  tft.setTextSize(2);
  tft.setCursor(10,10);
  tft.printf("Wurf: %d / 3", rollCount);

  drawResultText();
  drawDice();
  drawButton();
  drawExitButton();
}

// --- Setup & Loop --- //
void setup() {
  randomSeed(esp_random());
  tft.init();
  tft.setRotation(3);
  tft.setTouch(calData);
  tft.fillScreen(TFT_DARKGREY);

  // Buzzer konfigurieren
  ledcSetup(BUZZER_CHANNEL, 1000, BUZZER_RES);
  ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);

  resetGame();
  redraw();
}

void loop() {
  uint16_t x, y;
  if (tft.getTouch(&x,&y)) {
    handleTouch(x,y);
    delay(200);
  }
}
