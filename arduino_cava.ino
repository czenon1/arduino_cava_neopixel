#include <Adafruit_NeoPixel.h>

#define PIN            6      // Pin de contrôle des LEDs Neopixel
#define NUMPIXELS      16     // Nombre de LEDs dans l'anneau

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int freqBands[NUMPIXELS];     // Tableau des bandes de fréquence
int chenillardIndex = 0;      // Index pour l'effet chenillard
unsigned long lastUpdate = 0; // Temps de la dernière mise à jour du chenillard
int chenillardSpeed = 100;    // Vitesse initiale du chenillard

void setup() {
  pixels.begin();             // Initialiser l'anneau Neopixel
  Serial.begin(1000000);      // Initialiser la communication série à 1 000 000 bauds
}

void loop() {
  if (Serial.available()) {
    // Lire la ligne série qui contient les valeurs de bandes de fréquence
    String line = Serial.readStringUntil('\n');
    int numBands = min(line.length(), NUMPIXELS);  // Limiter les bandes à la taille de l'anneau
    for (int i = 0; i < numBands; i++) {
      freqBands[i] = (int) line.charAt(i) - '0';   // Convertir chaque caractère en entier
    }
  }

  // Mise à jour de l'effet Chenillard
  unsigned long currentTime = millis();
  if (currentTime - lastUpdate >= chenillardSpeed) {
    lastUpdate = currentTime;
    chenillardIndex = (chenillardIndex + 1) % NUMPIXELS;
  }

  // Calcul de la vitesse du chenillard en fonction des basses fréquences
  // Suppose que les 2 premières bandes représentent les fréquences basses (20-250Hz)
  int lowFreqIntensity = (freqBands[0] + freqBands[1]) / 2;
  chenillardSpeed = map(lowFreqIntensity, 0, 9, 200, 50);  // Plus la fréquence est forte, plus c'est rapide

  // Appliquer les effets sur l'anneau Neopixel
  for (int i = 0; i < NUMPIXELS; i++) {
    int intensity = map(freqBands[i], 0, 9, 0, 255);  // Mapper l'intensité sur une échelle de 0 à 255
    int r = (intensity * (1 + sin((chenillardIndex + i) * PI / 8))) / 2;  // Rouge avec effet chenillard
    int g = (intensity * (1 + sin((chenillardIndex + i + 2) * PI / 8))) / 2;  // Vert avec effet chenillard
    int b = (intensity * (1 + sin((chenillardIndex + i + 4) * PI / 8))) / 2;  // Bleu avec effet chenillard
    pixels.setPixelColor(i, pixels.Color(r, g, b));  // Mettre à jour la couleur de la LED
  }

  pixels.show();  // Afficher les modifications sur l'anneau
}
