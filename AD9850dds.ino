//AD9850 シリアル入力モードを使用する
#define FQ_UD (12)
#define W_CLK (13)
#define DATA (14)
#define RESET (15)
#define MASK40 (0xFFFFFFFFFFL)
#define MASK32 (0xFFFFFFFFL)
#define OUTPUTLEN (40)
#define LSB (1)
#define XCLOCK (125000.0) // 125MHz = 125000kHz 
#define POW2_32 (4294967296.0) // 2^32 = 4294967296

//LSB First Serial data output
int lfser_out(unsigned long data) {
  int i, ser_data;

  data = data & MASK40;
  digitalWrite(FQ_UD, LOW);
  digitalWrite(W_CLK, LOW);
  delayMicroseconds(1);
  for (i = 0; i < OUTPUTLEN; i++) {
    digitalWrite(W_CLK, LOW);
    ser_data = data & LSB;
    if (ser_data > 0) {
      digitalWrite(DATA, HIGH);
    } else {
      digitalWrite(DATA, LOW);
    }
    // W_CLK : Low -> High
    digitalWrite(W_CLK, HIGH);
    data = data >> 1;
  }
  digitalWrite(W_CLK, LOW);
  digitalWrite(FQ_UD, HIGH);
  delayMicroseconds(1);
  digitalWrite(FQ_UD, LOW);
  delayMicroseconds(1);
  return 1;
}

void setup() {
  unsigned long Freq = 103079;  // Clock:125MHz  Freq = 125MHz*103079/2^32=2999.9937Hz;

  Serial.begin(115200);
  Serial.println("");
  Serial.println("Start Setup");
  pinMode(FQ_UD, OUTPUT);
  pinMode(W_CLK, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(RESET, OUTPUT);
  // AD9850 Master Reset
  digitalWrite(RESET, HIGH);
  delay(100);
  digitalWrite(RESET, LOW);
  // Reset clear
  digitalWrite(W_CLK, LOW);
  digitalWrite(FQ_UD, LOW);
  digitalWrite(DATA, LOW);
  delayMicroseconds(1);
  digitalWrite(W_CLK, HIGH);
  digitalWrite(W_CLK, LOW);
  digitalWrite(FQ_UD, HIGH);
  delayMicroseconds(1);
  digitalWrite(FQ_UD, LOW);
  delayMicroseconds(1);
  Freq &= MASK32;
  lfser_out(Freq);
  Serial.print("pram: ");
  Serial.print(Freq);
  Serial.print(" Freq: ");
  Serial.print(XCLOCK / POW2_32 * Freq, 4);
  Serial.println("kHz");
}

void loop() {
  unsigned long lfreq;
  if (Serial.available()) {
    lfreq = Serial.parseInt();  //read until timeout
    if (lfreq != 0) {
      lfreq = lfreq & MASK32;
      Serial.println(lfreq);
      lfser_out(lfreq);
      Serial.print("pram: ");
      Serial.print(lfreq);
      Serial.print(" Freq: ");
      Serial.print(XCLOCK / POW2_32  * lfreq, 4);
      Serial.println("kHz");
    }
  }
}
