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
#define LongueurNomStation      16 // 16x2 LCD screen

#define BoutonLigne              7
#define BoutonGauche             6
#define BoutonDroite             9
#define BoutonValider            8

#define TempsChangement          3

#define OneChangeLimit          20
#define TwoChangesLimit         50

typedef struct {
  char nom[LongueurNomStation+1]; // wanted number +1 for '\0' terminaison
  byte i2c;
  byte led; // note: 0 = on, 1 = off
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
  Wire.write(0b11111111);
  Wire.endTransmission();
}

byte readI2C(byte i2c_addr) {
  Wire.requestFrom((int)i2c_addr, 1);
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
  for (byte id=0b0100000; id<=0b0100111; id++) { // FIXME: 0b0100110
    Wire.beginTransmission(id);
    Wire.write(0b11111111);
    Wire.endTransmission();
  }
  //PCF8574AP
  for (byte id=0b0111000; id<=0b0111111; id++) { // FIXME: 0b0111110
    Wire.beginTransmission(id);
    Wire.write(0b11111111);
    Wire.endTransmission();
  }
}

void displayLCD(int i, int j, char attribute[7]) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ligne ");
    lcd.print(i+1);
    lcd.setCursor(8, 0); // Keep same indent regardless of the line number
    lcd.print("(");
    lcd.print(attribute);
    lcd.print(")");
    lcd.setCursor(0, 1);
    lcd.print(pgm_read_string(station[pgm_read_byte(&(ligne[i][j]))].nom));
}

byte getStation(char attribute[7]) {
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
      if (pgm_read_byte(&(ligne[i+1][0])) == (byte)-1) { // line 2 guard
        i++;
      }
      if ((i+1) < NombreLignes) { // array bound check
        i++;
        j = 0; // reset station count
      } else {
        i = 0;
        j = 0; // reset station count
      }
      displayLCD(i,j,attribute);
      delay(80);
    } else if (digitalRead(BoutonGauche) == HIGH) {
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
    } else if (digitalRead(BoutonDroite) == HIGH) {
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

void findLines(char lines[5], byte station) {
  byte cpt = 0;
  for (int i=0; i<NombreLignes; i++) {
    for (int j=0; j<NombreStationsParLigne; j++) {
      if (pgm_read_byte(&(ligne[i][j])) == station) {
        lines[cpt] = i;
        cpt++;
        if (cpt > 5) {
            lcd.setCursor(0, 0);
            lcd.print("W: cpt.ovfl fFL");
            delay(5000);
            lcd.clear();
        }
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
          if (cpt > 20) {
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

void twoChanges(char ret[50][3], char d[5], char a[5]) {
  int cpt = 0;
  for (int i=0; i<5; i++) {
    for (int j=0; j<5; j++) {
      if (d[i] != -1 && a[j] != -1) {
        for (int k=0; k<NombreLignes; k++) {
          if (pgm_read_byte(&(change[d[i]][k])) == 1 && pgm_read_byte(&(change[a[j]][k])) == 1) {
            ret[cpt][0] = d[i];
            ret[cpt][1] = k;
            ret[cpt][2] = a[j];
            cpt++;
            if (cpt > 50) {
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
  lcd.print("W: st.notfnd fRS");
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
  byte arrival = getStation("Arriv.");
  /* Break if both stations are the same. */
  if (departure == arrival) {
    return;
  }
  char d_lines[5] = { -1, -1, -1, -1, -1 };
  findLines(d_lines, departure);
  char a_lines[5] = { -1, -1, -1, -1, -1 };
  findLines(a_lines, arrival);

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
    char ret[OneChangeLimit][2] = { {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1} };
    oneChange(ret, d_lines, a_lines);

    if (ret[0][0] != -1) { // w/a to pass else
      byte d_station;
      byte changement0, c0_station_d, c0_station_a;
      byte a_station;
      byte compteur1=0, compteur2=0;
      byte temps[OneChangeLimit][2] = {{0, 0}}; // TODO: check if this function is correctly initialized

      for (int r=0; r<OneChangeLimit; r++) {
        /* On compte en partant du départ et en allant à l'arrivée. */
        if (ret[r][0] != -1) {
          d_station = rangStation(ret[r][0], departure); // D. to
          a_station = rangStation(ret[r][1], arrival);   // A.
          for (int i=1; i<NombreStationsParLigne; i++) {
            if ((d_station-i) >= 0) {
              if (crossesLine(ret[r][1],pgm_read_byte(&(ligne[ret[r][0]][d_station-i])))) {
                c0_station_d = d_station - i;
                compteur1 = i + TempsChangement; // On compte le temps du départ au changement + temps changement
                break;
              }
            }
            if ((d_station+i) < NombreStationsParLigne && pgm_read_byte(&(ligne[ret[r][0]][d_station+i])) != (byte)-1) {
              if (crossesLine(ret[r][1],pgm_read_byte(&(ligne[ret[r][0]][d_station+i])))) {
                c0_station_d = d_station + i;
                compteur1 = i + TempsChangement; // idem
                break;
              }
            }
          }
          changement0 = pgm_read_byte(&(ligne[ret[r][0]][c0_station_d]));
          c0_station_a = rangStation(ret[r][1], changement0);
          compteur1 += abs(c0_station_a - a_station); // On compte le temps du changement à l'arrivée
        }
        temps[r][1] = changement0;
        
        /* On compte en sens inverse. Le chemin le plus court est dans l'un des deux trajets. */
        if (ret[r][0] != -1) {
          a_station = rangStation(ret[r][0], departure); // A. to
          d_station = rangStation(ret[r][1], arrival);   // D.
          for (int i=1; i<NombreStationsParLigne; i++) {
            if ((d_station-i) >= 0) {
              if (crossesLine(ret[r][0],pgm_read_byte(&(ligne[ret[r][1]][d_station-i])))) {
                c0_station_d = d_station - i;
                compteur2 = i + TempsChangement;
                break;
              }
            }
            if ((d_station+i) < NombreStationsParLigne && pgm_read_byte(&(ligne[ret[r][1]][d_station+i])) != (byte)-1) {
              if (crossesLine(ret[r][0],pgm_read_byte(&(ligne[ret[r][1]][d_station+i])))) {
                c0_station_d = d_station + i;
                compteur2 = i + TempsChangement;
                break;
              }
            }
          }
          
          changement0 = pgm_read_byte(&(ligne[ret[r][1]][c0_station_d]));
          c0_station_a = rangStation(ret[r][0], changement0);
          compteur2 += abs(c0_station_a - a_station);
        }
        temps[r][0] = min(compteur1, compteur2);
        if (temps[r][0] == compteur2) {
           temps[r][1] = changement0;
        }
      }
      /* Find best path between all possibilities. */
      byte tmp = 0;
      for (int i=1; i<OneChangeLimit; i++) {
        if (temps[i][0] < temps[tmp][0] && temps[i][0] > 0) {
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
      /* Check if there's two changes. */
      // initialize the array to -2 instead of -1 (avr bootloader interprets lots of -1 in memory as corrupt)
      char ret2[TwoChangesLimit][3] = { {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2}, {-2,-2,-2} };
      twoChanges(ret2, d_lines, a_lines);
      byte temps[TwoChangesLimit][3] = {{0, 0, 0}};

      if (ret2[0][0] != -2) { // w/a to pass else
        byte d_station;
        byte changement0, c0_station_d, c0_station_a;
        byte changement1, c1_station_d, c1_station_a;
        byte a_station;
        byte compteur1=0, compteur2=0;
        for (int r=0; r<TwoChangesLimit; r++) {
          /* On compte en partant du départ et en allant à l'arrivée en passant par la ligne intermédiaire. */
          if (ret2[r][0] != -2) {
            d_station = rangStation(ret2[r][0], departure); // D. to
            a_station = rangStation(ret2[r][2], arrival);   // A.
            for (int i=1; i<NombreStationsParLigne; i++) {
              if ((d_station-i) >= 0) {
                if (crossesLine(ret2[r][1],pgm_read_byte(&(ligne[ret2[r][0]][d_station-i])))) {
                  c0_station_d = d_station - i;
                  compteur1 = i + TempsChangement; // On compte le temps du départ au changement0
                  break;
                }
              }
              if ((d_station+i) < NombreStationsParLigne && pgm_read_byte(&(ligne[ret2[r][0]][d_station+i])) != (byte)-1) {
                if (crossesLine(ret2[r][1],pgm_read_byte(&(ligne[ret2[r][0]][d_station+i])))) {
                  c0_station_d = d_station + i;
                  compteur1 = i + TempsChangement; // idem
                  break;
                }
              }
            }
            changement0 = pgm_read_byte(&(ligne[ret2[r][0]][c0_station_d]));
            c0_station_a = rangStation(ret2[r][1], changement0);

            d_station = c0_station_a;
            for (int i=1; i<NombreStationsParLigne; i++) {
              if ((d_station-i) >= 0) {
                if (crossesLine(ret2[r][2],pgm_read_byte(&(ligne[ret2[r][1]][d_station-i])))) {
                  c1_station_d = d_station - i;
                  compteur1 = i + TempsChangement; // On compte le temps du changement0 au changement1
                  break;
                }
              }
              if ((d_station+i) < NombreStationsParLigne && pgm_read_byte(&(ligne[ret2[r][1]][d_station+i])) != (byte)-1) {
                if (crossesLine(ret2[r][2],pgm_read_byte(&(ligne[ret2[r][1]][d_station+i])))) {
                  c1_station_d = d_station + i;
                  compteur1 = i + TempsChangement; // idem
                  break;
                }
              }
            }
            changement1 = pgm_read_byte(&(ligne[ret2[r][1]][c1_station_d]));
            c1_station_a = rangStation(ret2[r][2], changement1);
            compteur1 += abs(c1_station_a - a_station); // on compte le temps du changement1 à l'arrivée
          }
          temps[r][1] = changement0;
          temps[r][2] = changement1;

          /* On compte en sens inverse. Le chemin le plus court est dans l'un des deux trajets. */
          if (ret2[r][0] != -2) {
            a_station = rangStation(ret2[r][0], departure); // A. to
            d_station = rangStation(ret2[r][2], arrival);   // D.
            for (int i=1; i<NombreStationsParLigne; i++) {
              if ((d_station-i) >= 0) {
                if (crossesLine(ret2[r][1],pgm_read_byte(&(ligne[ret2[r][2]][d_station-i])))) {
                  c1_station_d = d_station - i;
                  compteur2 = i + TempsChangement; // On compte le temps du changement0 au changement1
                  break;
                }
              }
              if ((d_station+i) < NombreStationsParLigne && pgm_read_byte(&(ligne[ret2[r][2]][d_station+i])) != (byte)-1) {
                if (crossesLine(ret2[r][1],pgm_read_byte(&(ligne[ret2[r][2]][d_station+i])))) {
                  c1_station_d = d_station + i;
                  compteur2 = i + TempsChangement; // idem
                  break;
                }
              }
            }
            changement1 = pgm_read_byte(&(ligne[ret2[r][2]][c1_station_d]));
            c1_station_a = rangStation(ret2[r][1], changement1);

            d_station = c1_station_a;
            for (int i=1; i<NombreStationsParLigne; i++) {
              if ((d_station-i) >= 0) {
                if (crossesLine(ret2[r][0],pgm_read_byte(&(ligne[ret2[r][1]][d_station-i])))) {
                  c0_station_d = d_station - i;
                  compteur2 = i + TempsChangement; // On compte le temps du départ au changement0
                  break;
                }
              }
              if ((d_station+i) < NombreStationsParLigne && pgm_read_byte(&(ligne[ret2[r][1]][d_station+i])) != (byte)-1) {
                if (crossesLine(ret2[r][0],pgm_read_byte(&(ligne[ret2[r][1]][d_station+i])))) {
                  c0_station_d = d_station + i;
                  compteur2 = i + TempsChangement; // idem
                  break;
                }
              }
            }

            changement0 = pgm_read_byte(&(ligne[ret2[r][1]][c0_station_d]));
            c0_station_a = rangStation(ret2[r][0], changement0);
            compteur2 += abs(c0_station_a - a_station); // on compte le temps du changement1 à l'arrivée
          }
          temps[r][0] = min(compteur1, compteur2);
          if (temps[r][0] == compteur2) {
            temps[r][1] = changement0;
            temps[r][2] = changement1;
          }
        }
        byte tmp = 0;
        for (int i=1; i<TwoChangesLimit; i++) {
          if (temps[i][0] < temps[tmp][0] && temps[i][0] > 0) {
            tmp = i;
          }
        }
        /* We have the best path. Display it. */
        byte i = 0; // Departure to changement0
        while (pgm_read_byte(&(ligne[ret2[tmp][0]][i])) != temps[tmp][1] && pgm_read_byte(&(ligne[ret2[tmp][0]][i])) != departure) {
          i++;
        }
        do {
          turnOnLED(pgm_read_byte(&(station[pgm_read_byte(&(ligne[ret2[tmp][0]][i]))].i2c)), pgm_read_byte(&(station[pgm_read_byte(&(ligne[ret2[tmp][0]][i]))].led)));
          i++;
        } while (pgm_read_byte(&(ligne[ret2[tmp][0]][i])) != temps[tmp][1] && pgm_read_byte(&(ligne[ret2[tmp][0]][i])) != departure); // don't light up change station the first time
        turnOnLED(pgm_read_byte(&(station[pgm_read_byte(&(ligne[ret2[tmp][0]][i]))].i2c)), pgm_read_byte(&(station[pgm_read_byte(&(ligne[ret2[tmp][0]][i]))].led)));

        i = 0; // changement0 to changement1
        while (pgm_read_byte(&(ligne[ret2[tmp][1]][i])) != temps[tmp][1] && pgm_read_byte(&(ligne[ret2[tmp][1]][i])) != temps[tmp][2]) {
          i++;
        }
        do {
          turnOnLED(pgm_read_byte(&(station[pgm_read_byte(&(ligne[ret2[tmp][1]][i]))].i2c)), pgm_read_byte(&(station[pgm_read_byte(&(ligne[ret2[tmp][1]][i]))].led)));
          i++;
        } while (pgm_read_byte(&(ligne[ret2[tmp][1]][i])) != temps[tmp][1] && pgm_read_byte(&(ligne[ret2[tmp][1]][i])) != temps[tmp][2]);
        turnOnLED(pgm_read_byte(&(station[pgm_read_byte(&(ligne[ret2[tmp][1]][i]))].i2c)), pgm_read_byte(&(station[pgm_read_byte(&(ligne[ret2[tmp][1]][i]))].led)));

        i = 0; // changement1 to arrival
        while (pgm_read_byte(&(ligne[ret2[tmp][2]][i])) != arrival && pgm_read_byte(&(ligne[ret2[tmp][2]][i])) != temps[tmp][2]) {
          i++;
        }
        do {
          turnOnLED(pgm_read_byte(&(station[pgm_read_byte(&(ligne[ret2[tmp][2]][i]))].i2c)), pgm_read_byte(&(station[pgm_read_byte(&(ligne[ret2[tmp][2]][i]))].led)));
          i++;
        } while (pgm_read_byte(&(ligne[ret2[tmp][2]][i])) != arrival && pgm_read_byte(&(ligne[ret2[tmp][2]][i])) != temps[tmp][2]);
        turnOnLED(pgm_read_byte(&(station[pgm_read_byte(&(ligne[ret2[tmp][2]][i]))].i2c)), pgm_read_byte(&(station[pgm_read_byte(&(ligne[ret2[tmp][2]][i]))].led)));
      } else {
        lcd.setCursor(0, 0);
        lcd.print("W: 3chg+ unsupp.");
        delay(5000);
        lcd.clear();
        exit(-1);
      }
    }
  }

  lcd.setCursor(0, 0);
  lcd.print("Calcul termine.");
  delay(2000);
  while (digitalRead(BoutonValider) != HIGH) {}
  lcd.clear();
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
