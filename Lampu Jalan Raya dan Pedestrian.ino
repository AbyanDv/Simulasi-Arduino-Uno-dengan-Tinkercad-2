
const int PIN_TOMBOL = 3;

const int jalanMerah[]    = {4, 8};
const int jalanKuning[]   = {5, 9};
const int jalanHijau[]    = {6, 10};

const int penjalanHijau[] = {7, 13};
const int penjalanMerah[] = {11, 12};


volatile bool tombolDitekan  = false;
volatile bool tombolDilepas  = false;
bool sedangSequence          = false;


void ISR_tombol() {
  if (digitalRead(PIN_TOMBOL) == LOW) {
    // Tekan: hanya proses jika belum sequence
    if (!sedangSequence) {
      tombolDitekan = true;
      tombolDilepas = false;
    }
  } else {
    // Lepas: selalu proses (perlu untuk keluar while loop)
    if (tombolDitekan) tombolDilepas = true;
  }
}

void setJalan(int merah, int kuning, int hijau) {
  for (int i = 0; i < 2; i++) {
    digitalWrite(jalanMerah[i],  merah);
    digitalWrite(jalanKuning[i], kuning);
    digitalWrite(jalanHijau[i],  hijau);
  }
}

void setPenjalan(bool hijau) {
  for (int i = 0; i < 2; i++) {
    digitalWrite(penjalanMerah[i], hijau ? LOW  : HIGH);
    digitalWrite(penjalanHijau[i], hijau ? HIGH : LOW);
  }
}

void stateDefault() {
  setJalan(LOW, LOW, HIGH);  // jalan raya: HIJAU
  setPenjalan(false);         // pejalan kaki: MERAH
}

void setup() {
  for (int i = 0; i < 2; i++) {
    pinMode(jalanMerah[i],    OUTPUT);
    pinMode(jalanKuning[i],   OUTPUT);
    pinMode(jalanHijau[i],    OUTPUT);
    pinMode(penjalanHijau[i], OUTPUT);
    pinMode(penjalanMerah[i], OUTPUT);
  }

  pinMode(PIN_TOMBOL, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_TOMBOL), ISR_tombol, CHANGE);

  stateDefault();
}

void loop() {
  if (!tombolDitekan) return;

  sedangSequence = true;

  // 1. Kuning 3 detik (fixed, tidak ditahan)
  setJalan(LOW, HIGH, LOW);
  setPenjalan(false);
  delay(3000);

  // 2. Jalan MERAH + Pejalan HIJAU, tunggu tombol dilepas dulu
  setJalan(HIGH, LOW, LOW);
  setPenjalan(true);

  // Tunggu sampai tombol dilepas, BARU mulai 7 detik
  while (!tombolDilepas) { /* tunggu */ }
  tombolDitekan = false;
  tombolDilepas = false;
  delay(7000);

  // 3. Kuning 3 detik sebelum kembali hijau
  setJalan(LOW, HIGH, LOW);
  setPenjalan(false);
  delay(3000);

  // 4. Default
  stateDefault();
  sedangSequence = false;
}