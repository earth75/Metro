/*
 * Copyright © 2013, Adrien Tétar. All Rights Reserved.
 * Licensed under the GNU GPL License, Version 2.
 * available at: http://www.gnu.org/licenses/gpl-2.0.html
 */
#include <avr/pgmspace.h>
#include <LiquidCrystal.h>
#include <Wire.h>

#define NombreStations          86
#define NombreLignes            14
#define NombreStationsParLigne  15
#define LongueurNomStation      16

#define BoutonLigne              7
#define BoutonGauche             6
#define BoutonDroite             9
#define BoutonValider            8

#define TempsChangement          3

typedef struct {
  char nom[LongueurNomStation+1]; // wanted number +1 for '\0' terminaison
  unsigned char i2c;
  unsigned char led; // note: 0 = on, 1 = off
} Tstation;

const Tstation station[NombreStations] PROGMEM = {
  {"Champs Elysees  ", 0b0100001, 0b11101111}, /* 0 */
  {"Concorde        ", 0b0100001, 0b10111111},
  {"Tuileries       ", 0b0100110, 0b11101111},
  {"Palais Royal    ", 0b0100110, 0b10111111},
  {"Louvre Rivoli   ", 0b0100010, 0b11101111},
  {"Chatelet        ", 0b0100100, 0b10111111},
  {"Hotel de Ville  ", 0b0100100, 0b11101111},
  {"St-Paul         ", 0b0111001, 0b11011111},
  {"Bastille        ", 0b0111010, 0b11110111},
  {"Ledru-Rollin    ", 0b0111001, 0b11110111},
  {"Faidherbe       ", 0b0111001, 0b11111011}, /* 10 */
  {"Saint-Lazare    ", 0b0100001, 0b11111110},
  {"Havre Caumartin ", 0b0100110, 0b11111110},
  {"Opera           ", 0b0100010, 0b11110111},
  {"Quatre Septembre", 0b0100010, 0b11111101},
  {"Bourse          ", 0b0100110, 0b11011111},
  {"Sentier         ", 0b0100100, 0b11110111},
  {"R. Sebastopol   ", 0b0100100, 0b11111101},
  {"Arts et Metiers ", 0b0100100, 0b01111111},
  {"Temple          ", 0b0100000, 0b10111111},
  {"Republique      ", 0b0100000, 0b11110111}, /* 20 */
  {"S. Saint-Denis  ", 0b0100000, 0b11011111},
  {"Etienne Marcel  ", 0b0100100, 0b11111011},
  {"Les Halles      ", 0b0100010, 0b10111111},
  {"Cite            ", 0b0100110, 0b01111111},
  {"St-Michel       ", 0b0111100, 0b11111110},
  {"Odeon           ", 0b0111000, 0b11111101},
  {"St-Germain      ", 0b0100101, 0b10111111},
  {"Saint-Sulpice   ", 0b0111100, 0b10111111},
  {"St-Placide      ", 0b0111100, 0b01111111},
  {"Montparnasse    ", 0b0111100, 0b11101111}, /* 30 */
  {"Vavin           ", 0b0111010, 0b01111111},
  {"Raspail         ", 0b0111100, 0b11111011},
  {"Denfert         ", 0b0111110, 0b11101111},
  {"J. Bonsergent   ", 0b0100000, 0b11111011},
  {"Oberkampf       ", 0b0100000, 0b01111111},
  {"Richard Lenoir  ", 0b0111001, 0b11111110},
  {"Breguet Sabin   ", 0b0111001, 0b11111101},
  {"Quai de la Rapee", 0b0111010, 0b11111101},
  {"Gare Austerlitz ", 0b0111001, 0b11101111},
  {"Saint Marcel    ", 0b0111001, 0b10111111}, /* 40 */
  {"Campo Formio    ", 0b0111100, 0b11011111},
  {"Pasteur         ", 0b0111000, 0b11011111},
  {"Edgar Quinet    ", 0b0111010, 0b10111111},
  {"Le Peletier     ", 0b0100110, 0b11110111},
  {"La Fayette      ", 0b0100110, 0b11111011},
  {"Pyramides       ", 0b0100001, 0b01111111},
  {"Pont Neuf       ", 0b0100010, 0b11011111},
  {"Pont Marie      ", 0b0111110, 0b11111101},
  {"Sully Morland   ", 0b0111110, 0b11111110},
  {"Jussieu         ", 0b0111010, 0b11111011}, /* 50 */
  {"Place Monge     ", 0b0111100, 0b11110111},
  {"C. Daubenton    ", 0b0111110, 0b11111011},
  {"Les Gobelins    ", 0b0111110, 0b10111111},
  {"Invalides       ", 0b0100101, 0b11110111},
  {"Madeleine       ", 0b0100001, 0b11011111},
  {"Richelieu Drouot", 0b0100110, 0b11111101},
  {"Gds Boulevards  ", 0b0100010, 0b11111011},
  {"Bonne Nouvelle  ", 0b0100100, 0b11111110},
  {"F. du Calvaire  ", 0b0100000, 0b11101111},
  {"St-S. Froissart ", 0b0100010, 0b01111111}, /* 60 */
  {"Chemin Vert     ", 0b0111001, 0b01111111},
  {"Saint Philippe  ", 0b0100001, 0b11110111},
  {"Miromesnil      ", 0b0100001, 0b11111011},
  {"Saint-Augustin  ", 0b0100001, 0b11111101},
  {"Duroc           ", 0b0111000, 0b11110111},
  {"Vaneau          ", 0b0111000, 0b01111111},
  {"Sevres Babylone ", 0b0111000, 0b11111011},
  {"Mabillon        ", 0b0100101, 0b01111111},
  {"La Sorbonne     ", 0b0111100, 0b11111101},
  {"Maubert         ", 0b0111110, 0b11011111}, /* 70 */
  {"Cardinal Lemoine", 0b0111110, 0b01111111},
  {"Rambuteau       ", 0b0100100, 0b11011111},
  {"Goncourt        ", 0b0100000, 0b11111101},
  {"Belleville      ", 0b0100000, 0b11111110},
  {"Assemblee       ", 0b0100101, 0b11111011},
  {"Solferino       ", 0b0100101, 0b11111110},
  {"Rue du Bac      ", 0b0100101, 0b11101111},
  {"Rennes          ", 0b0111010, 0b11101111},
  {"N-D. Des-Champs ", 0b0111010, 0b11011111},
  {"Falguiere       ", 0b0111000, 0b10111111}, /* 80 */
  {"Volontaires     ", 0b0111000, 0b11101111},
  {"Varenne         ", 0b0100101, 0b11111101},
  {"St-Fr. Xavier   ", 0b0100101, 0b11011111},
  {"Gaite           ", 0b0111000, 0b11111110},
  {"Gare de Lyon    ", 0b0111110, 0b11110111}
};

const char ligne[NombreLignes][NombreStationsParLigne] PROGMEM = {
  {  0,  1,  2,  3,  4,  5,  6,  7,  8, 85, -1, -1, -1, -1, -1 }, /*  1 */
  { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, /*  F */
  { 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, -1, -1, -1, -1, -1 }, /*  3 */
  { 21, 17, 22, 23,  5, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33 }, /*  4 */
  { 34, 20, 35, 36, 37,  8, 38, 39, 40, 41, -1, -1, -1, -1, -1 }, /*  5 */
  { 42, 30, 43, 32, 33, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, /*  6 */
  { 44, 45, 13, 46,  3, 47,  5, 48, 49, 50, 51, 52, 53, -1, -1 }, /*  7 */
  { 54,  1, 55, 13, 56, 57, 58, 21, 20, 59, 60, 61,  8,  9, 10 }, /*  8 */
  { 62, 63, 64, 12, 45, 56, 57, 58, 21, 20, 35, -1, -1, -1, -1 }, /*  9 */
  { 65, 66, 67, 68, 26, 69, 70, 71, 50, 39, -1, -1, -1, -1, -1 }, /* 10 */
  {  5,  6, 72, 18, 20, 73, 74, -1, -1, -1, -1, -1, -1, -1, -1 }, /* 11 */
  { 11, 55,  1, 75, 76, 77, 67, 78, 79, 30, 80, 42, 81, -1, -1 }, /* 12 */
  { 11, 63,  0, 54, 82, 83, 65, 30, 84, -1, -1, -1, -1, -1, -1 }, /* 13 */
  { 11, 55, 46,  5, 85, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }  /* 14 */
};

const boolean change[NombreLignes][NombreLignes] PROGMEM = {
  { 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1 },
  { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* fake */
  { 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1 },
  { 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
  { 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0 },
  { 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0 },
  { 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1 },
  { 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1 },
  { 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0 },
  { 0, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0 },
  { 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1 },
  { 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1 },
  { 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1 },
  { 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1 }
};

char* pgm_read_string(char *s);

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void shutdownI2C(int i, int j) {
  Wire.beginTransmission(pgm_read_byte(&(station[pgm_read_byte(&(ligne[i][j]))].i2c)));
  Wire.write(B11111111);
  Wire.endTransmission();
}

byte readI2C(byte addr) {
  Wire.requestFrom((int)addr, 1);
  return Wire.read();
}

void switchLED(byte i2c_addr, byte led_addr) {
  byte buf = readI2C(i2c_addr);
// Do a XOR on the NOT of LED addr
// Identifiers are inverted
// Note: 0: ON, 1: OFF
  buf ^= ~led_addr;
  Wire.beginTransmission(i2c_addr);
  Wire.write(buf);
  Wire.endTransmission();
}

void turnOnLED(byte i2c_addr, byte led_addr) {
  byte buf = readI2C(i2c_addr);
// Do an AND on the LED addr
// Identifiers are inverted
// Note: 0: ON, 1: OFF
  buf &= led_addr;
  Wire.beginTransmission(i2c_addr);
  Wire.write(buf);
  Wire.endTransmission();
}

void resetI2C() {
  // FIXME: I2C identifiers are inverted
  // PCF8574P
  byte id = 0b0100000;
  while (id <= 0b0100111) { // FIXME: 0b0100110
    Wire.beginTransmission(id);
    Wire.write(B11111111);
    Wire.endTransmission();
    id++;
  }
  //PCF8574AP
  id = 0b0111000;
  while (id <= 0b0111111) { // FIXME: 0b0111110
    Wire.beginTransmission(id);
    Wire.write(B11111111);
    Wire.endTransmission();
    id++;
  }
}

void displayLCD(int i, int j, char attribute[6]) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ligne ");
    lcd.print(i+1);
    lcd.setCursor(8, 0); // Keep same indent no matter if the line number has 1/2 number(s)
    lcd.print("(");
    lcd.print(attribute);
    lcd.print(")");
    lcd.setCursor(0, 1);
    lcd.print(pgm_read_string(station[pgm_read_byte(&(ligne[i][j]))].nom));
}

byte getStation(char attribute[6]) {
  int i=0, j=0;
  displayLCD(i,j,attribute);
  delay(80);
  do {
    Wire.beginTransmission(pgm_read_byte(&(station[pgm_read_byte(&(ligne[i][j]))].i2c)));
    Wire.write(pgm_read_byte(&(station[pgm_read_byte(&(ligne[i][j]))].led)));
    Wire.endTransmission();
    // delay avoids sending same instruction multiple times when you click on a button
    if (digitalRead(BoutonLigne) == HIGH) {
      shutdownI2C(i,j);
      if (pgm_read_byte(&(ligne[i+1][0])) == (byte)-1) {
        i++;
      }
      if ((i+1) < NombreLignes) { // check if we're within ArrayBounds
        i++;
        j = 0; // reset station count
      } else {
        i = 0;
        j = 0; // reset station count
      }
      displayLCD(i,j,attribute);
      delay(80);
    }
    else if (digitalRead(BoutonGauche) == HIGH) {
      shutdownI2C(i,j);
      if ((j-1) >= 0) {
        j--;
      } else {
        j = NombreStationsParLigne - 1;
        while (pgm_read_byte(&(ligne[i][j])) == (byte)-1) { // -1 check
          j--;
        }
      }
      displayLCD(i,j,attribute);
      delay(80);
    }
    else if (digitalRead(BoutonDroite) == HIGH) {
      shutdownI2C(i,j);
      if ((j+1) < NombreStationsParLigne && pgm_read_byte(&(ligne[i][j+1])) != (byte)-1) { // TODO: second check valid if out of bound?
        j++;
      } else {
        j = 0;
      }
      displayLCD(i,j,attribute);
      delay(80);
    }
  } while (digitalRead(BoutonValider) != HIGH);

  shutdownI2C(i,j);
  lcd.clear();
  return pgm_read_byte(&(ligne[i][j]));
}

/* ------ */

void findLines(char* ret, byte d) {
  byte cpt = 0;
  for (int i=0; i<NombreLignes; i++) {
    for (int j=0; j<NombreStationsParLigne; j++) {
      if (pgm_read_byte(&(ligne[i][j])) == d) {
        ret[cpt] = i;
        cpt++;
      }
    }
  }
}

char sameLine(char d[5], char a[5]) {
  for (int i=0; i<5; i++) {
    for (int j=0; j<5; j++) {
      if (d[i] != -1 && a[j] != -1) {
        if (d[i] == a[j]) {
          return d[i]; // Pick the common line
        }
      }
    }
  }
  return -1;
}

boolean crossesLine(byte line, byte target) {
  for (int j=0; j<NombreStationsParLigne; j++) {
    if (pgm_read_byte(&(ligne[line][j])) == target) {
      return 1;
    }
  }
  return 0;
}

void oneChange(char ret[20][2], char d[5], char a[5]) {
  int cpt = 0;
  for (int i=0; i<5; i++) {
    for (int j=0; j<5; j++) {
      if (d[i] != -1 && a[j] != -1) {
        if (pgm_read_byte(&(change[d[i]][a[j]])) == 1) {
          ret[cpt][0] = d[i];
          ret[cpt][1] = a[j];
          cpt++;
          if (cpt >= 20) {
            lcd.setCursor(0, 0);
            lcd.print("W: cpt.ovfl fOC");
            delay(5000);
            lcd.clear();
          }
        }
      }
    }
  }
}

char twoChanges(char ret[50][2], char d[5], char a[5]) {
  int cpt = 0;
  for (int i=0; i<5; i++) {
    for (int j=0; j<5; j++) {
      if (d[i] != -1 && a[j] != -1) {
        for (int k=0; k<5; k++) {
          if (pgm_read_byte(&(change[d[i]][k])) == 1 && pgm_read_byte(&(change[a[j]][k])) == 1) {
            ret[cpt][0] = d[i];
            ret[cpt][1] = k;
            ret[cpt][2] = a[j];
            cpt++;
            if (cpt >= 20) {
              lcd.setCursor(0, 0);
              lcd.print("W: cpt.ovfl fTC");
              delay(5000);
              lcd.clear();
            }
          }
        }
      }
    }
  }
}

byte rangStation(byte line, byte target) {
  for (int i=0; i<NombreStationsParLigne; i++) {
    if (pgm_read_byte(&(ligne[line][i])) == target) {
      return i;
    }
  }
  lcd.setCursor(0, 0);
  lcd.print("W: st. ntfnd fRS");
  delay(5000);
  lcd.clear();
  exit(-1);
  return -1; 
}

/* --------- */

void setup() {
  pinMode(BoutonLigne, INPUT);
  pinMode(BoutonGauche, INPUT);
  pinMode(BoutonDroite, INPUT);
  pinMode(BoutonValider, INPUT);
  Wire.begin();
  lcd.begin(16, 2);
}

void loop() {
  resetI2C(); // previous lighted LEDs may still be ON
  byte departure = getStation("Depart");
  char d_lines[5] = { -1, -1, -1, -1, -1 };
  findLines(d_lines, departure);
  byte arrival = getStation("Arriv.");
  char a_lines[5] = { -1, -1, -1, -1, -1 };
  findLines(a_lines, arrival);
  
  /* Check if departure and arrival stations are the same. */
  if (departure == arrival) {
    return;
  }
  /* Check if both stations are on the same line. */
  char buf = sameLine(d_lines, a_lines);
  if (buf != -1) {
    byte i = 0;
    while ((pgm_read_byte(&(ligne[buf][i])) != arrival) && (pgm_read_byte(&(ligne[buf][i])) != departure)) {
      i++;
    }
    do {
      turnOnLED(pgm_read_byte(&(station[pgm_read_byte(&(ligne[buf][i]))].i2c)), pgm_read_byte(&(station[pgm_read_byte(&(ligne[buf][i]))].led)));
      i++;
    } while (pgm_read_byte(&(ligne[buf][i])) != arrival && pgm_read_byte(&(ligne[buf][i])) != departure);
    turnOnLED(pgm_read_byte(&(station[pgm_read_byte(&(ligne[buf][i]))].i2c)), pgm_read_byte(&(station[pgm_read_byte(&(ligne[buf][i]))].led)));
  } else {
    /* Check if there's one change. */
    char ret[20][2] = { {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1} };
    oneChange(ret, d_lines, a_lines);
    byte temps[20][2] = {{0, 0}};
    
    if (ret[0][0] != -1) { // w/a to pass else
      byte d_station;
      byte a_station;
      byte changement0;
      byte c0_station_d;
      byte c0_station_a;
      byte compteur1 = 0, compteur2 = 0;
      for (int r=0; r<20; r++) {
        /* On compte en partant du départ et en allant à l'arrivée. */
        if (ret[r][0] != -1) {
          d_station = rangStation(ret[r][0], departure);
          a_station = rangStation(ret[r][1], arrival);
          for (int i=1; i<NombreStationsParLigne; i++) {
            if ((d_station-i) >= 0) {
              if (crossesLine(ret[r][1],pgm_read_byte(&(ligne[ret[r][0]][d_station-i])))) {
                c0_station_d = d_station - i;
                compteur1 = i + TempsChangement;
                break;
              }
            }
            if ((d_station+i) < NombreStationsParLigne && pgm_read_byte(&(ligne[ret[r][0]][d_station+i])) != (byte)-1) {
              if (crossesLine(ret[r][1],pgm_read_byte(&(ligne[ret[r][0]][d_station+i])))) {
                c0_station_d = d_station + i;
                compteur1 = i + TempsChangement;
                break;
              }
            }
          }
          changement0 = pgm_read_byte(&(ligne[ret[r][0]][c0_station_d]));
          c0_station_a = rangStation(ret[r][1], changement0); // FIXME Concorde - Edgar Quinet
          compteur1 += abs(c0_station_a - a_station);
        }
        /* On compte en sens inverse. Le chemin le plus court est dans l'un des deux trajets. */
        if (ret[r][0] != -1) {
          a_station = rangStation(ret[r][0], departure);
          d_station = rangStation(ret[r][1], arrival);
          for (int i=1; i<NombreStationsParLigne; i++) {
            if ((d_station-i) >= 0) {
              if (crossesLine(ret[r][1],pgm_read_byte(&(ligne[ret[r][0]][d_station-i])))) {
                c0_station_d = d_station - i;
                compteur2 = i + TempsChangement;
                break;
              }
            }
            if ((d_station+i) < NombreStationsParLigne && pgm_read_byte(&(ligne[ret[r][0]][d_station+i])) != (byte)-1) {
              if (crossesLine(ret[r][1],pgm_read_byte(&(ligne[ret[r][0]][d_station+i])))) {
                c0_station_d = d_station + i;
                compteur2 = i + TempsChangement;
                break;
              }
            }
          }
          
          changement0 = pgm_read_byte(&(ligne[ret[r][0]][c0_station_d]));
          c0_station_a = rangStation(ret[r][1], changement0);
          compteur2 += abs(c0_station_a - a_station);
        }
        temps[r][0] = max(compteur1, compteur2);
        temps[r][1] = changement0;
      }
      byte tmp = 0;
      for (int i=1; i<20; i++) {
        if (temps[i][0] < temps[tmp][0]) {
          tmp = i;
        }
      }
      /* We have the best path. Display it. */
      byte i = 0;
      while (pgm_read_byte(&(ligne[ret[tmp][0]][i])) != temps[tmp][1] && pgm_read_byte(&(ligne[ret[tmp][0]][i])) != departure) {
        i++;
      }
      do {
        turnOnLED(pgm_read_byte(&(station[pgm_read_byte(&(ligne[ret[tmp][0]][i]))].i2c)), pgm_read_byte(&(station[pgm_read_byte(&(ligne[ret[tmp][0]][i]))].led)));
        i++;
      } while (pgm_read_byte(&(ligne[ret[tmp][0]][i])) != temps[tmp][1] && pgm_read_byte(&(ligne[ret[tmp][0]][i])) != departure); // don't light up change station the first time
      turnOnLED(pgm_read_byte(&(station[pgm_read_byte(&(ligne[ret[tmp][0]][i]))].i2c)), pgm_read_byte(&(station[pgm_read_byte(&(ligne[ret[tmp][0]][i]))].led)));

      i = 0;
      while (pgm_read_byte(&(ligne[ret[tmp][1]][i])) != temps[tmp][1] && pgm_read_byte(&(ligne[ret[tmp][1]][i])) != arrival) {
        i++;
      }
      do {
        turnOnLED(pgm_read_byte(&(station[pgm_read_byte(&(ligne[ret[tmp][1]][i]))].i2c)), pgm_read_byte(&(station[pgm_read_byte(&(ligne[ret[tmp][1]][i]))].led)));
        i++;
      } while (pgm_read_byte(&(ligne[ret[tmp][1]][i])) != temps[tmp][1] && pgm_read_byte(&(ligne[ret[tmp][1]][i])) != arrival);
      turnOnLED(pgm_read_byte(&(station[pgm_read_byte(&(ligne[ret[tmp][1]][i]))].i2c)), pgm_read_byte(&(station[pgm_read_byte(&(ligne[ret[tmp][1]][i]))].led)));
    } else {
      lcd.setCursor(0, 0);
      lcd.print("W: 2chg. unsupp.");
      delay(5000);
      lcd.clear();
    }
  }
  
  lcd.setCursor(0, 0);
  lcd.print("Calcul termine.");
  delay(2000);
  while (digitalRead(BoutonValider) != HIGH) {}
}

char* pgm_read_string(const char *s) {
  char s2[LongueurNomStation+1];
  char *p;
  char c;
  int i=0;
  p=s2;
  do
  {
    c = pgm_read_byte(s);
    *p = c;
    p++;
    s++;
  } while (c != '\0');
  return s2;
}