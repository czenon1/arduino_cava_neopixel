#include <Adafruit_NeoPixel.h>

#define PIN            6      // Pin de contrôle des LEDs Neopixel
#define NUMPIXELS      12     // Nombre de LEDs dans l'anneau
#define VIBRATION_PIN  7      // Pin de contrôle du moteur de vibration

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
float smoothBands[NUMPIXELS]; // Tableau pour stocker les bandes de fréquence lissées
int freqBands[NUMPIXELS];     // Tableau pour stocker les bandes de fréquence
unsigned long lastUpdate = 0; // Temps de la dernière mise à jour des LEDs
unsigned long startupDuration = 250;  // Durée de l'animation de démarrage (3 secondes)

// Définir les couleurs de début et de fin
int startColor[3] = {255, 50, 0}; // Couleur de début (orange profond)
int endColor[3] = {255, 0, 25}; // Couleur de fin (blanc)

float propagationFactor = 0.5; // Facteur de propagation de la vague

void setup() {
  pinMode(VIBRATION_PIN, OUTPUT);   // Configurer la broche du moteur de vibration
  pixels.setBrightness(40);          // Réglez la luminosité globale à 40 sur 255
  pixels.begin();                    // Initialiser les LEDs Neopixel
  Serial.begin(1000000);            // Communication série pour CAVA
  
  startupAnimation();                // Lancer l'animation de démarrage
}

void loop() {
  if (Serial.available()) {
    // Lire la ligne série qui contient les valeurs de bandes de fréquence
    String line = Serial.readStringUntil('\n');
    Serial.println("Received: " + line);  // Afficher la ligne reçue pour débogage

    // Séparer la ligne en valeurs individuelles
    int index = 0;
    while (line.indexOf(';') != -1 && index < NUMPIXELS) {
      String value = line.substring(0, line.indexOf(';'));
      freqBands[index++] = value.toFloat(); // Convertir chaque valeur en float (fréquence normalisée)
      line = line.substring(line.indexOf(';') + 1);
    }

    // Vérifier que nous avons bien reçu toutes les bandes
    if (index == NUMPIXELS) {
      // Appliquer un lissage pour obtenir un effet vague (influence des voisines)
      for (int i = 0; i < NUMPIXELS; i++) {
        float leftNeighbor = freqBands[(i - 1 + NUMPIXELS) % NUMPIXELS]; // Voisine de gauche
        float rightNeighbor = freqBands[(i + 1) % NUMPIXELS]; // Voisine de droite
        smoothBands[i] = (freqBands[i] + leftNeighbor * propagationFactor + rightNeighbor * propagationFactor) / (1 + 2 * propagationFactor); // Pondération
      }

      // Appliquer l'effet de fondu entre les deux couleurs
      for (int i = 0; i < NUMPIXELS; i++) {
        int intensity = map(smoothBands[i], 0, 2, 0, 60);  // Mapper l'intensité lissée sur une échelle de 0 à 255

        // Calculer la couleur basée sur l'intensité
        int r = map(intensity, 0, 255, endColor[0], startColor[0]);
        int g = map(intensity, 0, 255, endColor[1], startColor[1]);
        int b = map(intensity, 0, 255, endColor[2], startColor[2]);

        pixels.setPixelColor(i, pixels.Color(r, g, b));  // Mettre à jour la couleur de la LED
      }

      pixels.show();  // Afficher les modifications sur l'anneau
    } else {
      Serial.println("Error: Incomplete data."); // Afficher une erreur si toutes les données ne sont pas reçues
    }
  }
}

void startupAnimation() {
  int totalSteps = NUMPIXELS * 3;  // Nombre total de pas pour 3 tours complets
  unsigned long stepDuration = 500 / totalSteps; // Durée totale de 2 secondes divisée par le nombre de pas

  // Transition pour chaque LED
  for (int step = 0; step < totalSteps; step++) {
    int ledIndex = step % NUMPIXELS;

    // Calculer l'intensité croissante en fonction du nombre total de pas
    int intensity = map(step, 0, totalSteps, 0, 255);  // Intensité croissante

    // Créer une couleur allant du blanc pâle à l'orange profond
    int r = map(intensity, 0, 255, endColor[0], startColor[0]); // Rouge
    int g = map(intensity, 0, 255, endColor[1], startColor[1]); // Vert
    int b = map(intensity, 0, 255, endColor[2], startColor[2]); // Bleu

    // Mettre à jour la couleur de la LED
    pixels.setPixelColor(ledIndex, pixels.Color(r, g, b));
    pixels.show();

    // Vibration pour chaque LED
    triggerVibration(70);          // Courte vibration
    delay(3);            // Délai entre les mises à jour
  }

  // Assurer que toutes les LEDs finissent sur l'orange profond
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(startColor[0], startColor[1], startColor[2])); // Couleur orange profond
  }
  pixels.show(); // Mettre à jour les LEDs pour afficher l'orange profond
}

void triggerVibration(int duration) {
  digitalWrite(VIBRATION_PIN, HIGH);
  delay(duration);
  digitalWrite(VIBRATION_PIN, LOW);
}
