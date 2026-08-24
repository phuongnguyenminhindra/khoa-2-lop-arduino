// DAY 28 FINAL - FIX LOI - KHOA 2 LOP DOI DUOC CA MAT KHAU
#include <EEPROM.h>

#define LED_DO 2
#define LED_XANH 3
#define BTN_A 4
#define BTN_B 5
#define CHAN_DK_IC 6
#define BTN_C 7
#define BIEN_TRO A0

#define ADDR_MIN 0
#define ADDR_MAX 10
#define ADDR_MK 20

int MIN_BI_MAT, MAX_BI_MAT;
int matKhau[3] = {0, 1, 2}; // 0=A,1=B,2=C

unsigned long lastBlink = 0;
bool ledState = false;
int buoc = 0;
unsigned long thoiGianBuoc = 0;

int soLanSai = 0;
unsigned long thoiGianKhoa = 0;
bool dangBiKhoa = false;

void setup() {
  pinMode(LED_DO, OUTPUT);
  pinMode(LED_XANH, OUTPUT);
  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(BTN_B, INPUT_PULLUP);
  pinMode(BTN_C, INPUT_PULLUP);
  pinMode(CHAN_DK_IC, OUTPUT);
  digitalWrite(CHAN_DK_IC, HIGH);
  Serial.begin(9600);

  EEPROM.get(ADDR_MIN, MIN_BI_MAT);
  EEPROM.get(ADDR_MAX, MAX_BI_MAT);
  EEPROM.get(ADDR_MK, matKhau);

  if (MIN_BI_MAT < 0 || MIN_BI_MAT > 1023 || MAX_BI_MAT < 0 || MAX_BI_MAT > 1023) {
    MIN_BI_MAT = 400; MAX_BI_MAT = 600;
    EEPROM.put(ADDR_MIN, MIN_BI_MAT);
    EEPROM.put(ADDR_MAX, MAX_BI_MAT);
  }
  if (matKhau[0] > 2 || matKhau[1] > 2 || matKhau[2] > 2 || matKhau[0]==matKhau[1] || matKhau[1]==matKhau[2] || matkhau[0]==matkhau[2]) {
    matKhau[0]=0; matKhau[1]=1; matKhau[2]=2;
    EEPROM.put(ADDR_MK, matKhau);
  }

  Serial.print("Vung: "); Serial.print(MIN_BI_MAT); Serial.print("-"); Serial.print(MAX_BI_MAT);
  Serial.print(" | MK: ");
  Serial.print(char('A'+matKhau[0])); Serial.print("->");// ví dụ 65+0=65 là A
  Serial.print(char('A'+matKhau[1])); Serial.print("->");// 65+1=66 là B
  Serial.println(char('A'+matKhau[2]));// 65+2=67 là C
}

void loop() {
  // 1. KHOA CUNG 30S
  if (dangBiKhoa) {
    if (millis() - thoiGianKhoa >= 30000) {
      dangBiKhoa = false; soLanSai = 0;
      Serial.println("Het khoa - Nhap lai");
      digitalWrite(LED_DO, LOW);
      digitalWrite(CHAN_DK_IC, HIGH);
    } else {
      if (millis() - lastBlink > 100) {
        lastBlink = millis(); ledState =!ledState;
        digitalWrite(LED_DO, ledState);
        digitalWrite(CHAN_DK_IC,!ledState);
      }
      return;
    }
  }

  int tro = analogRead(BIEN_TRO);
  bool lop1 = (tro >= MIN_BI_MAT && tro <= MAX_BI_MAT);

  // 2. LOP 1 - BIEN TRO
  if (!lop1) {
    buoc = 0;
    if (millis() - lastBlink > 200) {
      lastBlink = millis(); ledState =!ledState;
      digitalWrite(LED_DO, ledState);
    }
    digitalWrite(LED_XANH, LOW);
    return;
  }

  // Lop 1 dung -> Xanh nhap nhay cham
  digitalWrite(LED_DO, LOW);
  if (millis() - lastBlink > 800) {
    lastBlink = millis(); ledState =!ledState;
    digitalWrite(LED_XANH, ledState);
  }

  // 3. GIU NUT DOI CHE DO (Phai giu truoc khi tinh buoc)
  if (kiemTraGiu(BTN_A, 3000)) { vaoDoiVung(); return; }
  if (kiemTraGiu(BTN_C, 3000)) { vaoDoiMatKhau(); return; }
  if (kiemTraGiu(BTN_B, 5000)) {
    MIN_BI_MAT = 400; MAX_BI_MAT = 600;
    matKhau[0]=0; matKhau[1]=1; matKhau[2]=2;
    EEPROM.put(ADDR_MIN, MIN_BI_MAT);
    EEPROM.put(ADDR_MAX, MAX_BI_MAT);
    EEPROM.put(ADDR_MK, matKhau);
    Serial.println("RESET VE MAC DINH 400-600 + A->B->C");
    baoSai(); delay(200); baoSai();
    return;
  }

  // 4. KIEM TRA HET GIO
  if (buoc > 0 && millis() - thoiGianBuoc > 2000) {
    Serial.println("Het 2s - Sai");
    xuLySai();
    return;
  }

  // 5. DOC NUT BAM
  int nutBam = docNut(); // -1=khong bam, 0=A,1=B,2=C, 99=bam nhieu nut
  if (nutBam == 99) { xuLySai(); choNhaNut(); return; }
  if (nutBam == -1) return; // Khong bam gi

  // Co bam 1 nut
  if (nutBam == matKhau[buoc]) {
    Serial.print("Dung buoc "); Serial.print(buoc+1); Serial.print(": "); Serial.println(char('A'+nutBam));
    buoc++;
    thoiGianBuoc = millis();
    choNhaNut();
    if (buoc == 3) {
      Serial.println("=== MO KHOA THANH CONG ===");
      soLanSai = 0;
      digitalWrite(LED_XANH, HIGH);
      digitalWrite(CHAN_DK_IC, LOW); delay(1000); digitalWrite(CHAN_DK_IC, HIGH);
      digitalWrite(LED_XANH, LOW);
      buoc = 0;
    }
  } else {
    Serial.print("Sai - Bam "); Serial.print(char('A'+nutBam));
    Serial.print(" nhung can "); Serial.println(char('A'+matKhau[buoc]));
    xuLySai();
    choNhaNut();
  }
}

// Ham doc 1 nut duy nhat
int docNut() {
  bool A =!digitalRead(BTN_A);
  bool B =!digitalRead(BTN_B);
  bool C =!digitalRead(BTN_C);
  int dem = A+B+C;// đếm xem đang bấm mấy nút cùng lúc
  if (dem == 0) return -1;
  if (dem > 1) return 99;// nếu bấm cả 3 nút thì sẽ báo là không được
  if (A) return 0;
  if (B) return 1;
  return 2;// đây là trường hợp còn lại khi bấm C thì trả về 3
}

void choNhaNut() {
  while (!digitalRead(BTN_A) ||!digitalRead(BTN_B) ||!digitalRead(BTN_C)) delay(10);
  delay(50); // chong doi phim
}

bool kiemTraGiu(int chan, int ms) {
  if (digitalRead(chan) == LOW) {
    unsigned long batDau = millis();
    while (digitalRead(chan) == LOW) {
      if (millis() - batDau >= ms) {
        choNhaNut();
        return true;
      }
    }
  }
  return false;// để đảm bao đang ấn mà nhả chưa đủ thời gian hoặc nhả luôn thì trả về false
}

void xuLySai() {
  buoc = 0;
  soLanSai++;
  Serial.print("SAI lan "); Serial.println(soLanSai);
  baoSai();
  if (soLanSai >= 3) {
    dangBiKhoa = true;
    thoiGianKhoa = millis();
    Serial.println("SAI 3 LAN - KHOA 30S");
  }
}

void vaoDoiVung() {
  Serial.println(">>> DOI VUNG - Xoay bien tro, A=Luu B=Huy");
  digitalWrite(CHAN_DK_IC, LOW); delay(300); digitalWrite(CHAN_DK_IC, HIGH); delay(100);
  digitalWrite(CHAN_DK_IC, LOW); delay(300); digitalWrite(CHAN_DK_IC, HIGH);
  while (true) {
    int tam = analogRead(BIEN_TRO);
    Serial.print("Tam: "); Serial.println(tam);
    digitalWrite(LED_XANH, millis() % 200 < 100);
    if (!digitalRead(BTN_A)) { // Luu
      MIN_BI_MAT = tam - 50; MAX_BI_MAT = tam + 50;
      if (MIN_BI_MAT < 0) MIN_BI_MAT = 0;
      if (MAX_BI_MAT > 1023) MAX_BI_MAT = 1023;
      EEPROM.put(ADDR_MIN, MIN_BI_MAT);
      EEPROM.put(ADDR_MAX, MAX_BI_MAT);
      Serial.print("DA LUU VUNG: "); Serial.print(MIN_BI_MAT); Serial.print("-"); Serial.println(MAX_BI_MAT);
      digitalWrite(LED_XANH, HIGH);
      digitalWrite(CHAN_DK_IC, LOW); delay(150); digitalWrite(CHAN_DK_IC, HIGH); delay(80);
      digitalWrite(CHAN_DK_IC, LOW); delay(150); digitalWrite(CHAN_DK_IC, HIGH);
      choNhaNut(); break;
    }
    if (!digitalRead(BTN_B)) { Serial.println("HUY DOI VUNG"); choNhaNut(); break; }
    delay(100);
  }
  digitalWrite(LED_XANH, LOW);
  buoc = 0;
}

void vaoDoiMatKhau() {
  Serial.println(">>> DOI MAT KHAU - Bam 3 nut theo thu tu moi (khong trung nhau)");
  digitalWrite(CHAN_DK_IC, LOW); delay(200); digitalWrite(CHAN_DK_IC, HIGH); delay(100);
  digitalWrite(CHAN_DK_IC, LOW); delay(200); digitalWrite(CHAN_DK_IC, HIGH);
  int mkMoi[3]; int dem = 0;
  while (dem < 3) {
    int n = docNut();
    if (n >= 0 && n <= 2) {
      mkMoi[dem] = n;
      Serial.print(dem+1); Serial.print(": "); Serial.println(char('A'+n));
      dem++;
      digitalWrite(LED_XANH, HIGH); delay(200); digitalWrite(LED_XANH, LOW);
      choNhaNut();
    }
    delay(20);
  }
  if (mkMoi[0]==mkMoi[1] || mkMoi[1]==mkMoi[2] || mkMoi[0]==mkMoi[2]) {
    Serial.println("LOI: Trung nut - Huy");
    baoSai(); return;
  }
  matKhau[0]=mkMoi[0]; matKhau[1]=mkMoi[1]; matKhau[2]=mkMoi[2];
  EEPROM.put(ADDR_MK, matKhau);
  Serial.print("DA LUU MK MOI: ");
  Serial.print(char('A'+matKhau[0])); Serial.print("->");
  Serial.print(char('A'+matKhau[1])); Serial.print("->");
  Serial.println(char('A'+matKhau[2]));
  digitalWrite(LED_XANH, HIGH); delay(1000); digitalWrite(LED_XANH, LOW);
  buoc = 0;// reset bước về 0 để nhập lại từ đầu
}

void baoSai() {
  digitalWrite(LED_DO, HIGH);
  digitalWrite(CHAN_DK_IC, LOW); delay(150); digitalWrite(CHAN_DK_IC, HIGH); delay(80);
  digitalWrite(CHAN_DK_IC, LOW); delay(150); digitalWrite(CHAN_DK_IC, HIGH);
  digitalWrite(LED_DO, LOW);
}
