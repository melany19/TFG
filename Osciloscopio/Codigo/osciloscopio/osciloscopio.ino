#include <Adafruit_SPITFT.h>
#include <SPFD5408_Adafruit_GFX.h> // Core graphics library
#include <SPFD5408_Adafruit_TFTLCD.h> // Hardware-specific library
#include <SPFD5408_TouchScreen.h>

#define YP A2 // must be an analog pin, use "An" notation!
#define XM A3 // must be an analog pin, use "An" notation!
#define YM 8 // can be a digital pin
#define XP 9 // can be a digital pin

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // you can also just connect RESET to the arduino RESET pin. *if so just //comment line out

#define YP A3 // must be an analog pin, use "An" notation! A1 for shield
#define XM A2 // must be an analog pin, use "An" notation! A2 for shield
#define YM 9 // can be a digital pin-----uno=9 mega=23 7 for shield
#define XP 8 // can be a digital pin-----uno=8 mega=22 6 for shield
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940
float V1 = {0.00};
float volt1;
// Color definitions - in 5:6:5
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF
//NEM KELL MAJF
#define MINPRESSURE 10
#define MAXPRESSURE 1000
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
#define txtLINE0 0
#define txtLINE1 16
#define txtLINE2 30
#define txtLINE3 46

int high_time;
int low_time;
float time_period;
float frequency;
const int LCD_WIDTH = 320;
const int LCD_HEIGHT = 240;
const int SAMPLES = 270;
const int DOTS_DIV = 30;
const int ad_ch0 = 4; // Analog 4 pin for channel 0
const int ad_ch1 = 5; // Analog 5 pin for channel 1
const unsigned long VREF[] = {150, 300, 750, 1500, 3000};
const int MILLIVOL_per_dot[] = {33, 17, 6, 3, 2}; // mV/dot
const int MODE_ON = 0;
const int MODE_INV = 1;
const int MODE_OFF = 2;
const char *Modes[] = {"NORM", "INV", "OFF"};
const int TRIG_AUTO = 0;
const int TRIG_NORM = 1;
const int TRIG_SCAN = 2;
const int TRIG_ONE = 3;
const char *TRIG_Modes[] = {"Auto", "Norm", "Scan", "One"};
const int TRIG_E_UP = 0;
const int TRIG_E_DN = 1;
#define RATE_MIN 0
#define RATE_MAX 13
const char *Rates[] = {"F1-1", "F1-2 ", "F2 ", "5ms", "10ms", "20ms", "50ms", "0.1s", "0.2s", "0.5s", "1s", "2s", "5s", "10s"};
#define RANGE_MIN 0
#define RANGE_MAX 4
const char *Ranges[] = {" 1V ", "0.5V", "0.2V", "0.1V", "50mV"};
unsigned long startMillis;
byte data[4][SAMPLES];
byte sample=0;

#define BGCOLOR BLACK
#define GRIDCOLOR WHITE
#define CH1COLOR BLUE
#define CH2COLOR YELLOW
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF
#define VRF 5

byte range0 = RANGE_MIN, ch0_mode = MODE_OFF; // CH0
short ch0_off = 204;
byte range1 = RANGE_MIN, ch1_mode = MODE_ON; // CH1
short ch1_off = 204;
byte rate = 1; // sampling rate
byte trig_mode = TRIG_AUTO, trig_lv = 30, trig_edge = TRIG_E_UP, trig_ch = 1; // trigger settings
byte Start = 1; // Start sampling
byte menu = 0; // Default menu

void setup(){
Serial.begin(9600);
tft.reset();

uint16_t identifier = tft.readID();
if(identifier == 0x9325) {
Serial.println(F("ILI9325 LCD driver"));
} else if(identifier == 0x9327) {
Serial.println(F("ILI9327 LCD driver"));
} else if(identifier == 0x9328) {
Serial.println(F("ILI9328 LCD driver"));
} else if(identifier == 0x7575) {
Serial.println(F("HX8347G LCD driver"));
} else if(identifier == 0x9341) {
Serial.println(F("ILI9341 LCD driver"));
} else if(identifier == 0x8357) {
Serial.println(F("HX8357D LCD driver"));
} else if(identifier == 0x0154) {
Serial.println(F("S6D0154 LCD driver"));
} else {
Serial.print(F("Ismeretlen meghajto ic: "));
Serial.println(identifier, HEX);
return;
}

tft.begin(identifier);
tft.setRotation(1);
tft.fillScreen(BLACK);
tft.setTextColor(BLUE);
tft.setTextSize(2);
tft.setCursor(75, 100);
tft.print("OSCILOSCOPIO");
tft.setTextColor(WHITE);
tft.setCursor(130, 125);
tft.print("TFG");
tft.setTextColor(WHITE);
tft.setTextSize(1);
tft.setCursor(200, 220);
tft.print("Melany Acuna Vega");
delay(10000);
tft.fillScreen(BGCOLOR);
Serial.begin(9600);
DrawGrid();
DrawText();
}

void SendData() {
Serial.print(Rates[rate]);
Serial.println("/div (30 samples)");
for (int i=0; i<SAMPLES; i ++) {
Serial.print(data[sample + 0][i]*MILLIVOL_per_dot[range0]);
Serial.print(" ");
Serial.println(data[sample + 1][i]*MILLIVOL_per_dot[range1]);
}
}

void DrawGrid() {
for (int x=0; x<=SAMPLES; x += 2) { // Horizontal Line
for (int y=0; y<=LCD_HEIGHT; y += DOTS_DIV) {
tft.drawPixel(x, y, GRIDCOLOR);

}
if (LCD_HEIGHT == 240)
tft.drawPixel(x, LCD_HEIGHT-1, GRIDCOLOR);
}
for (int x=0; x<=SAMPLES; x += DOTS_DIV ) { // Vertical Line
for (int y=0; y<=LCD_HEIGHT; y += 2) {
tft.drawPixel(x, y, GRIDCOLOR);

}
}
}

void DrawText() {
tft.setTextColor(WHITE);
tft.setTextSize(1);
tft.setCursor(SAMPLES+3, 20);
tft.print(Ranges[range1]);
tft.println("/DIV");
tft.setCursor(SAMPLES+3, 30);
tft.print(Rates[rate]);
tft.println("/DIV");
tft.setCursor(SAMPLES+3, 40);
tft.println(TRIG_Modes[trig_mode]);
tft.setCursor(SAMPLES+3, 50);
tft.println(trig_edge == TRIG_E_UP ? "UP" : "DN");
tft.setCursor(SAMPLES+3, 60);
tft.println(Modes[ch1_mode]);

#if 0
GLCD.FillRect(101,txtLINE0,28,64, WHITE);

switch (menu) {
case 0:
GLCD.GotoXY(SAMPLES + 1,txtLINE0);
GLCD.Puts(Ranges[range0]);
GLCD.GotoXY(SAMPLES + 1,txtLINE1);
GLCD.Puts(Ranges[range1]);
GLCD.GotoXY(SAMPLES + 1,txtLINE2);
GLCD.Puts(Rates[rate]);
GLCD.GotoXY(SAMPLES + 1,txtLINE3);
GLCD.Puts(TRIG_Modes[trig_mode]);
break;
case 1:
GLCD.GotoXY(SAMPLES + 1,txtLINE0);
GLCD.Puts("OF1");
GLCD.GotoXY(SAMPLES + 1,txtLINE1);
GLCD.Puts("OF2");
GLCD.GotoXY(SAMPLES + 1,txtLINE2);
GLCD.Puts("Tlv");
GLCD.GotoXY(SAMPLES + 1,txtLINE3);
GLCD.Puts(trig_edge == TRIG_E_UP ? "UP" : "DN");
break;
case 2:
GLCD.GotoXY(SAMPLES + 1,txtLINE0);
GLCD.Puts(Modes[ch0_mode]);
GLCD.GotoXY(SAMPLES + 1,txtLINE1);
GLCD.Puts(Modes[ch1_mode]);
GLCD.GotoXY(SAMPLES + 1,txtLINE2);
GLCD.Puts(trig_ch == 0 ? "T:1" : "T:2");
break;
}
#endif
}

void DrawGrid(int x) {
if ((x % 2) == 0)
for (int y=0; y<=LCD_HEIGHT; y += DOTS_DIV)
tft.drawPixel(x, y, GRIDCOLOR);
if ((x % DOTS_DIV) == 0)
for (int y=0; y<=LCD_HEIGHT; y += 2)
tft.drawPixel(x, y, GRIDCOLOR);
}

void ClearAndDrawGraph() {
int clear = 0;

if (sample == 0)
clear = 2;
#if 0
for (int x=0; x<SAMPLES; x++) {
GLCD.SetDot(x, LCD_HEIGHT-data[clear+0][x], WHITE);
GLCD.SetDot(x, LCD_HEIGHT-data[clear+1][x], WHITE);
GLCD.SetDot(x, LCD_HEIGHT-data[sample+0][x], BLACK);
GLCD.SetDot(x, LCD_HEIGHT-data[sample+1][x], BLACK);
}
#else
for (int x=0; x<(SAMPLES-1); x++) {
tft.drawLine(x, LCD_HEIGHT-data[clear+0][x], x+1, LCD_HEIGHT-data[clear+0][x+1], BGCOLOR);
tft.drawLine(x, LCD_HEIGHT-data[clear+1][x], x+1, LCD_HEIGHT-data[clear+1][x+1], BGCOLOR);
if (ch0_mode != MODE_OFF)
tft.drawLine(x, LCD_HEIGHT-data[sample+0][x], x+1, LCD_HEIGHT-data[sample+0][x+1], CH1COLOR);
if (ch1_mode != MODE_OFF)
tft.drawLine(x, LCD_HEIGHT-data[sample+1][x], x+1, LCD_HEIGHT-data[sample+1][x+1], CH2COLOR);

}
#endif
}

void ClearAndDrawDot(int i) {
int clear = 0;

if (i <= 1)
return;
if (sample == 0)
clear = 2;
#if 0
for (int x=0; x<SAMPLES; x++) {
GLCD.SetDot(x, LCD_HEIGHT-data[clear+0][x], WHITE);
GLCD.SetDot(x, LCD_HEIGHT-data[clear+1][x], WHITE);
GLCD.SetDot(x, LCD_HEIGHT-data[sample+0][x], BLACK);
GLCD.SetDot(x, LCD_HEIGHT-data[sample+1][x], BLACK);
}
#else
tft.drawLine(i-1, LCD_HEIGHT-data[clear+0][i-1], i, LCD_HEIGHT-data[clear+0][i], BGCOLOR);
tft.drawLine(i-1, LCD_HEIGHT-data[clear+1][i-1], i, LCD_HEIGHT-data[clear+1][i], BGCOLOR);
if (ch0_mode != MODE_OFF)
tft.drawLine(i-1, LCD_HEIGHT-data[sample+0][i-1], i, LCD_HEIGHT-data[sample+0][i], CH1COLOR);
if (ch1_mode != MODE_OFF)
tft.drawLine(i-1, LCD_HEIGHT-data[sample+1][i-1], i, LCD_HEIGHT-data[sample+1][i], CH2COLOR);
#endif
DrawGrid(i);
}

void DrawGraph() {
for (int x=0; x<SAMPLES; x++) {
tft.drawPixel(x, LCD_HEIGHT-data[sample+0][x], CH1COLOR);
tft.drawPixel(x, LCD_HEIGHT-data[sample+1][x], CH2COLOR);
}
}

void ClearGraph() {
int clear = 0;

if (sample == 0)
clear = 2;
for (int x=0; x<SAMPLES; x++) {
tft.drawPixel(x, LCD_HEIGHT-data[clear+0][x], BGCOLOR);
tft.drawPixel(x, LCD_HEIGHT-data[clear+1][x], BGCOLOR);
}
}

inline unsigned long adRead(byte ch, byte mode, int off)
{
unsigned long a = analogRead(ch);
a = ((a+off)*VREF[ch == ad_ch0 ? range0 : range1]+512) >> 10;
a = a>=(LCD_HEIGHT+1) ? LCD_HEIGHT : a;
if (mode == MODE_INV)
return LCD_HEIGHT - a;
return a;
}


void loop() {
if (trig_mode != TRIG_SCAN) {
unsigned long st = millis();
byte oad;
if (trig_ch == 0)
oad = adRead(ad_ch0, ch0_mode, ch0_off);
else
oad = adRead(ad_ch1, ch1_mode, ch1_off);
for (;;) {
byte ad;
if (trig_ch == 0)
ad = adRead(ad_ch0, ch0_mode, ch0_off);
else
ad = adRead(ad_ch1, ch1_mode, ch1_off);

if (trig_edge == TRIG_E_UP) {
if (ad >= trig_lv && ad > oad)
break;
} else {
if (ad <= trig_lv && ad < oad)
break;
}
oad = ad;


if (trig_mode == TRIG_SCAN)
break;
if (trig_mode == TRIG_AUTO && (millis() - st) > 100)
break;
}
}


if (rate <= 5 && Start) {
if (sample == 0)
sample = 2;
else
sample = 0;

if (rate == 0) {
unsigned long st = millis();
for (int i=0; i<SAMPLES; i ++) {
data[sample+0][i] = adRead(ad_ch0, ch0_mode, ch0_off);
}
for (int i=0; i<SAMPLES; i ++)
data[sample+1][i] = 0;
} else if (rate == 1) {
unsigned long st = millis();
for (int i=0; i<SAMPLES; i ++) {
data[sample+1][i] = adRead(ad_ch1, ch1_mode, ch1_off);
}
for (int i=0; i<SAMPLES; i ++)
data[sample+0][i] = 0;
} else if (rate == 2) {
unsigned long st = millis();
for (int i=0; i<SAMPLES; i ++) {
data[sample+0][i] = adRead(ad_ch0, ch0_mode, ch0_off);
data[sample+1][i] = adRead(ad_ch1, ch1_mode, ch1_off);
}

} else if (rate >= 3 && rate <= 5) {
const unsigned long r_[] = {5000/DOTS_DIV, 10000/DOTS_DIV, 20000/DOTS_DIV};
unsigned long st0 = millis();
unsigned long st = micros();
unsigned long r = r_[rate - 3];
for (int i=0; i<SAMPLES; i ++) {
while((st - micros())<r) ;
st += r;
data[sample+0][i] = adRead(ad_ch0, ch0_mode, ch0_off);
data[sample+1][i] = adRead(ad_ch1, ch1_mode, ch1_off);
}

}
ClearAndDrawGraph();

DrawGrid();
DrawText();
} else if (Start) {

if (sample == 0) {
for (int i=0; i<SAMPLES; i ++) {
data[2][i] = data[0][i];
data[3][i] = data[1][i];
}
} else {
for (int i=0; i<SAMPLES; i ++) {
data[0][i] = data[2][i];
data[1][i] = data[3][i];
}
}

const unsigned long r_[] = {50000/DOTS_DIV, 100000/DOTS_DIV, 200000/DOTS_DIV,
500000/DOTS_DIV, 1000000/DOTS_DIV, 2000000/DOTS_DIV,
5000000/DOTS_DIV, 10000000/DOTS_DIV};
unsigned long st0 = millis();
unsigned long st = micros();
for (int i=0; i<SAMPLES; i ++) {
while((st - micros())<r_[rate-6]) {

if (rate<6)
break;
}
if (rate<6) {
tft.fillScreen(BGCOLOR);
break;
}
st += r_[rate-6];
if (st - micros()>r_[rate-6])
st = micros();
if (!Start) {
i --;
continue;
}
data[sample+0][i] = adRead(ad_ch0, ch0_mode, ch0_off);
data[sample+1][i] = adRead(ad_ch1, ch1_mode, ch1_off);
ClearAndDrawDot(i);
}
// Serial.println(millis()-st0);
DrawGrid();
DrawText();
} else {

}
if (trig_mode == TRIG_ONE)
Start = 0;

TSPoint p = ts.getPoint();

//pinMode(XP, OUTPUT);
pinMode(XM, OUTPUT);
pinMode(YP, OUTPUT);
//pinMode(YM, OUTPUT);


p.x = map(p.x, TS_MINX, TS_MAXX, 0, 240);
p.y = map(p.y, TS_MINY, TS_MAXY, 0, 320);

// s/div buttin
tft.fillRect(275, 75, 40, 20, RED);
tft.setTextSize(1);
tft.setTextColor(WHITE);
tft.setCursor(280, 80);
tft.println("S/DIV");
if (p.y > 270 && p.y < 305 && p.x > 65 && p.x < 85) {

tft.fillRect(275, 75, 40, 20, GREEN);
tft.setTextSize(1);
tft.setTextColor(WHITE);
tft.setCursor(280, 80);
tft.println("S/DIV");
tft.fillRect(275, 75, 40, 20, RED);
tft.setTextColor(GREEN);
tft.setCursor(280, 80);
tft.println("S/DIV");
tft.fillScreen(BLACK);
if (rate < RATE_MAX)
{
rate ++;
}
else if (rate = RATE_MAX)
{
rate = RATE_MIN;

}
}
//s/div end

//v/div button

tft.fillRect(275, 100, 40, 20, RED);
tft.setTextSize(1);
tft.setTextColor(WHITE);
tft.setCursor(280, 105);
tft.println("V/DIV");
if (p.y > 272 && p.y < 305 && p.x > 95 && p.x < 115) {


tft.fillRect(275, 100, 40, 20, GREEN);
tft.setTextSize(1);
tft.setTextColor(WHITE);
tft.setCursor(280, 105);
tft.println("V/DIV");
tft.fillRect(275, 100, 40, 20, RED);
tft.setTextColor(GREEN);
tft.setCursor(280, 105);
tft.println("V/DIV");
tft.fillScreen(BLACK);
if (range1 < RANGE_MAX)
{
range1 ++;
}
else if (range1 = RANGE_MAX)
{
range1 = RANGE_MIN;

}
//v/div end

//TRIG BUTTON
}
}