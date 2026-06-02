#include <TFT_ST7735.h>
#include <SPI.h>
volatile int32_t  vitesse = 9999, Nv_vitesse= 1500;
volatile int8_t selection = 0, multiplicateur =0;
volatile bool flag_multi = 0;
TFT_ST7735 tft = TFT_ST7735();

void setup() {
              Init_Ecran(); 
              attachInterrupt(digitalPinToInterrupt(2), BP, CHANGE);
              attachInterrupt(digitalPinToInterrupt(3), ISR_bouton, RISING);
              Affiche_sel();
              Affiche_vit() ;
 }

void loop() {
             drawBattery(5);  
             delay(1000);
             drawLightning(true); // affiche éclair
             delay(700);

             drawLightning(false); // efface éclair
             delay(700);
}

void BP() {
           if (digitalRead(2) ){ selection = (selection+1)%3; Affiche_sel(); }
           else { Nv_vitesse = multiplicateur>=0? vitesse *(multiplicateur + 1) : vitesse/(1-multiplicateur);
                  if (Nv_vitesse<60) Nv_vitesse = 60;
                  if (Nv_vitesse>9999)  Nv_vitesse= 9999;
                  flag_multi = 0; Affiche_multi(0.5); multiplicateur =0; Affiche_vit();
           }
}

void ISR_bouton() { if (!digitalRead(2)) {Nv_vitesse = vitesse - (digitalRead (4)*2-1)*pow (10,selection);
                                          if (Nv_vitesse<60) Nv_vitesse = 60;
                                          if (Nv_vitesse>9999)  Nv_vitesse= 9999;
                                          Affiche_vit();
                    }
                    if (digitalRead(2)) { if (!flag_multi) {flag_multi =1;Affiche_multi(0.5);}
                                          else Affiche_multi(digitalRead (4));
                    }
}


void Init_Ecran() { 
                  tft.init();  tft.setRotation(3);  tft.fillScreen(TFT_WHITE);
                  tft.setTextColor(TFT_BLUE); tft.setTextSize(2);
                  tft.setCursor(123, 63); tft.print("tr/"); 
                  tft.setCursor(123, 82); tft.print("min");
}

void Affiche_sel() {
                   tft.setTextFont(1); tft.setTextSize(4); tft.setTextColor(TFT_RED);
                   tft.setCursor(90-28*selection, 68);  tft.print("_");
                   tft.setTextColor(TFT_WHITE);  tft.setCursor(90-28* ((selection +2)%3), 68);  tft.print("_");
}

void Affiche_multi(float i) {
                     tft.setTextFont(1); tft.setTextSize(2); tft.setTextColor(TFT_WHITE);
                     tft.setCursor(13, 29); if (multiplicateur>= 0) tft.print("x"); else tft.print("/");
                     tft.setCursor(28, 29); tft.print(abs(multiplicateur)+1);

                     if (flag_multi) { multiplicateur  = multiplicateur -(i*2-1) ; 
                                       if ( multiplicateur  <=-5) multiplicateur =-4;
                                       if ( multiplicateur  >=5) multiplicateur =4;
                                                                    
                                       tft.setCursor(13, 29); tft.setTextSize(2); tft.setTextColor(TFT_RED);
                                       if (multiplicateur>= 0) tft.print("x"); else tft.print("/");
                                       tft.setCursor(28, 29); tft.print(abs(multiplicateur)+1);
                     }
}

void Affiche_vit() {
                  uint8_t vit[4]; uint8_t Nv_vit[4];
                  vit[0]=vitesse/1000; vit[1]=(vitesse/100)%10; vit[2]=(vitesse/10)%10; vit[3]=vitesse%10;
                  Nv_vit[0]=Nv_vitesse/1000; Nv_vit[1]=(Nv_vitesse/100)%10; Nv_vit[2]=(Nv_vitesse/10)%10; Nv_vit[3]=Nv_vitesse%10;
                  for (int i = 0; i < 4;i++) { if (vit[i] != Nv_vit[i]) {tft.setTextFont(4); tft.setTextSize(2); tft.setTextColor(TFT_WHITE);
                                                                         tft.setCursor(2+28*i, 50);tft.print(vit[i]);
                                                                         if (Nv_vitesse>=1000 || Nv_vitesse>=100 && i==1 || i>=2) {tft.setTextColor(TFT_BLUE);
                                                                                                                           tft.setCursor(2+28*i, 50);
                                                                                                                           tft.print(Nv_vit[i]);}
                                               }
                  vitesse = Nv_vitesse;
                  }
}

void drawBattery(int level) {
                             int8_t x = 118,  y = 30, w = 28, h = 12, barWidth = 3,spacing = 2;
                             tft.fillRect(x - 2, y - 2, w + 6, h + 4, TFT_WHITE);  // Efface ancienne
                             tft.drawRect(x, y, w, h, TFT_BLACK);  // Contour
                             tft.fillRect(x + w, y + 3, 3, 4, TFT_BLACK); // Tête batterie
                             for (int i = 0; i < level; i++) {int barX = x + 2 + i * (barWidth + spacing);
                                                              int barY = y + 2;
                                                              if (level == 1)  tft.fillRect(barX, barY, barWidth, h - 4, TFT_YELLOW);
                                                              else tft.fillRect(barX, barY, barWidth, h - 4, TFT_PURPLE);
                             }
}

void drawLightning(bool charging) {

  int x = 155;   // position horizontale
  int y = 36;    // position verticale

  // Efface ancienne icône
 tft.fillTriangle(x, y, x, y-6, x-3,   y,TFT_WHITE);
     tft.fillTriangle(x-1, y-1, x-1, y+5, x+2,   y-1,TFT_WHITE);
  if (charging) {
    // Partie haute de l'éclair
    tft.fillTriangle(x, y, x, y-6, x-3,   y,TFT_RED);
    
    // Partie basse de l'éclair
    tft.fillTriangle(x-1, y-1, x-1, y+5, x+2,   y-1,TFT_RED);
  }
}