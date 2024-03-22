#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define LED_PIN     5
#define LED_COUNT  64
#define BRIGHTNESS 25

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

uint32_t colorSnake;
uint32_t colorPoint;
uint32_t colorNull;
int masterScore;

int nowMove=0;
int tab[8][8];
int head1, head2;
int snakeSize=1;
bool generateapple = true;
bool continueGame = true;

void colorChange(int pixelNumber, uint32_t color);
void colorRollup();
void changeDirection();
void indicateScore(int score);
void snakeStart();
void snakeEnd();
void applegeneration();
void wypisz();
void snakeMaster();
void indicateNumber(int pos, int num);

void setup() {
  /// Initializing LEDs
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif

  strip.begin();           
  strip.show();            
  strip.setBrightness(BRIGHTNESS);
  /// ==========

  /// Game setup
  // Random seed
  randomSeed(analogRead(A0));
  
  // Color preparing
  colorRollup();
  
  // Score setup
  masterScore = 0;
  pinMode(9, OUTPUT);
  noTone(9);
  /// ==========
}

void loop() {
  // Clearing strip
  strip.clear();

  // Game code
  snakeStart();
  wypisz();
  while(nowMove==0)
    changeDirection();
    
  while (continueGame) {
      changeDirection();
      snakeMaster();
      delay(500);
  }
  snakeEnd();
}

/// Funkcje LEDów
// Change specific cell color
void colorChange(int pixelNumber, uint32_t color){
  strip.setPixelColor(pixelNumber, color);
}

// Color setup
void colorRollup(){
  long option = random(4);
  if(option<1){
    colorSnake = strip.Color(0,255,0);
    colorPoint = strip.Color(255,0,0);
  }
  else if(option<2){
    colorSnake = strip.Color(0,195,255);
    colorPoint = strip.Color(255,255,0);
  }
  else{
    colorSnake = strip.Color(255,0,255);
    colorPoint = strip.Color(255,100,0);
  }

  colorNull = strip.Color(0,0,0);
}
/// ==========

/// Funkcje obsługi gry

// Podaj, w którą stronę ma iść wąż
void changeDirection(){
  int x = analogRead(A1);
  int y = analogRead(A2);

  if(x<250)
    nowMove = 1;
  else if(x>750)
    nowMove = 3;
  else if(y>750)
    nowMove = 2;
  else if(y<250)
    nowMove = 4;
}

// Zaktualizuj zmienną score i wypisz na wyświetlaczu
void scoreUpdate(){
  masterScore += 1;
  tone(9, 1000);
  delay(50);
  noTone(9);
}

void snakeStart(){
    for(int i=0; i<8; i++){
         for(int j=0; j<8; j++){
             tab[i][j]=0;
             }
    }

    ::head1 = random(7);
    ::head2 = random(7);
    tab[head1][head2]=snakeSize;
}

void snakeEnd(){
    for(int i=0; i<64; i++){
      colorChange(i, colorNull);  
    }
    indicateNumber(0,masterScore/10);
    indicateNumber(1,masterScore%10);
    strip.show();
    delay(1000000);
}

void applegeneration(){
    int positions1 [64];
    int positions2 [64];
    int k=0;
    for(int i=0; i<8; i++){ // szukanie wolnych miejsc
        for(int j=0; j<8; j++){
            if (tab[i][j] == 0){
                positions1[k]=i;
                positions2[k]=j;
                k++;
            }
        }
    }
    int random_choice = random(0,k);
    tab[positions1[random_choice]][positions2[random_choice]] = -1;
    generateapple = false;
}

void wypisz(){
    strip.clear();
    
    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
            if(tab[i][j] == 0){
                colorChange(i*8+j, colorNull);
            }
            else if (tab[i][j] == -1){
                colorChange(i*8+j, colorPoint);
            }
            else{
                tab[i][j]=tab[i][j]-1;
                colorChange(i*8+j, colorSnake);
            }
        }
    }
    
    strip.show();
}

void snakeMaster(){
    int move = nowMove;

    if(move == 0){
        wypisz();
    }
    else{
        strip.clear();
        if(move == 1){ // W
            head1=head1-1;
        }
        else if(move == 2){ // A
            head2=head2-1;
        }
        else if(move == 3){ // S
            head1=head1+1;
        }
        else if(move == 4){ // D
            head2=head2+1;
        }

        if( ( head1 < 0 || head1 > 7 ) || ( head2 < 0 || head2 > 7 ) ){ // sprawdzanie czy wbil sie w kraniec planszy
            continueGame = false;
        }
        else if (tab[head1][head2] == -1){ // sprawdzanie czy nie natrafil na jablko
            snakeSize++;
            scoreUpdate();
            generateapple = true;
        }
        else if (tab[head1][head2] != 0){ // sprawdzanie czy nie natrafil na siebie
            continueGame = false;
        }
        
        tab[head1][head2]=snakeSize;

        if (generateapple == true){
            applegeneration();  // tworzenie nowego jablka jesli trzeba
        }

        wypisz();
    }
}

/// ==========

/// Funkcje wysyłu informacji na score indicator
// Indicate new score
void indicateNumber(int pos, int num){
  int startPos = 5*pos;

  if(num==0){
    colorChange(8+startPos, strip.Color(255,0,0));
    colorChange(8+startPos+1, strip.Color(255,0,0)); 
    colorChange(8+startPos+2, strip.Color(255,0,0)); 

    colorChange(16+startPos, strip.Color(255,0,0));
    colorChange(16+startPos+2, strip.Color(255,0,0)); 

    colorChange(24+startPos, strip.Color(255,0,0));
    colorChange(24+startPos+2, strip.Color(255,0,0));

    colorChange(32+startPos, strip.Color(255,0,0));
    colorChange(32+startPos+2, strip.Color(255,0,0));

    colorChange(40+startPos, strip.Color(255,0,0));
    colorChange(40+startPos+1, strip.Color(255,0,0)); 
    colorChange(40+startPos+2, strip.Color(255,0,0)); 
  }

  else if(num==1){
    colorChange(8+startPos+2, strip.Color(255,0,0)); 

    colorChange(16+startPos+2, strip.Color(255,0,0)); 

    colorChange(24+startPos+2, strip.Color(255,0,0));

    colorChange(32+startPos+2, strip.Color(255,0,0));

    colorChange(40+startPos+2, strip.Color(255,0,0)); 
  }

  else if(num==2){
    colorChange(8+startPos, strip.Color(255,0,0));
    colorChange(8+startPos+1, strip.Color(255,0,0)); 
    colorChange(8+startPos+2, strip.Color(255,0,0)); 

    colorChange(16+startPos+2, strip.Color(255,0,0)); 

    colorChange(24+startPos, strip.Color(255,0,0));
    colorChange(24+startPos+1, strip.Color(255,0,0)); 
    colorChange(24+startPos+2, strip.Color(255,0,0));

    colorChange(32+startPos, strip.Color(255,0,0));

    colorChange(40+startPos, strip.Color(255,0,0));
    colorChange(40+startPos+1, strip.Color(255,0,0)); 
    colorChange(40+startPos+2, strip.Color(255,0,0)); 
  }

  else if(num==3){
    colorChange(8+startPos, strip.Color(255,0,0));
    colorChange(8+startPos+1, strip.Color(255,0,0)); 
    colorChange(8+startPos+2, strip.Color(255,0,0)); 

    colorChange(16+startPos+2, strip.Color(255,0,0)); 

    colorChange(24+startPos, strip.Color(255,0,0));
    colorChange(24+startPos+1, strip.Color(255,0,0)); 
    colorChange(24+startPos+2, strip.Color(255,0,0));

    colorChange(32+startPos+2, strip.Color(255,0,0));

    colorChange(40+startPos, strip.Color(255,0,0));
    colorChange(40+startPos+1, strip.Color(255,0,0)); 
    colorChange(40+startPos+2, strip.Color(255,0,0)); 
  }

  else if(num==4){
    colorChange(8+startPos, strip.Color(255,0,0));
    colorChange(8+startPos+2, strip.Color(255,0,0)); 

    colorChange(16+startPos, strip.Color(255,0,0)); 
    colorChange(16+startPos+2, strip.Color(255,0,0)); 

    colorChange(24+startPos, strip.Color(255,0,0));
    colorChange(24+startPos+1, strip.Color(255,0,0)); 
    colorChange(24+startPos+2, strip.Color(255,0,0));

    colorChange(32+startPos+2, strip.Color(255,0,0));

    colorChange(40+startPos+2, strip.Color(255,0,0)); 
  }

  else if(num==5){
    colorChange(8+startPos, strip.Color(255,0,0));
    colorChange(8+startPos+1, strip.Color(255,0,0)); 
    colorChange(8+startPos+2, strip.Color(255,0,0)); 

    colorChange(16+startPos, strip.Color(255,0,0));

    colorChange(24+startPos, strip.Color(255,0,0));
    colorChange(24+startPos+1, strip.Color(255,0,0)); 
    colorChange(24+startPos+2, strip.Color(255,0,0));

    colorChange(32+startPos+2, strip.Color(255,0,0));

    colorChange(40+startPos, strip.Color(255,0,0));
    colorChange(40+startPos+1, strip.Color(255,0,0)); 
    colorChange(40+startPos+2, strip.Color(255,0,0)); 
  }

  else if(num==6){
    colorChange(8+startPos, strip.Color(255,0,0));
    colorChange(8+startPos+1, strip.Color(255,0,0)); 
    colorChange(8+startPos+2, strip.Color(255,0,0)); 

    colorChange(16+startPos, strip.Color(255,0,0));

    colorChange(24+startPos, strip.Color(255,0,0));
    colorChange(24+startPos+1, strip.Color(255,0,0)); 
    colorChange(24+startPos+2, strip.Color(255,0,0));

    colorChange(32+startPos, strip.Color(255,0,0));
    colorChange(32+startPos+2, strip.Color(255,0,0));

    colorChange(40+startPos, strip.Color(255,0,0));
    colorChange(40+startPos+1, strip.Color(255,0,0)); 
    colorChange(40+startPos+2, strip.Color(255,0,0)); 
  }

  else if(num==7){
    colorChange(8+startPos, strip.Color(255,0,0));
    colorChange(8+startPos+1, strip.Color(255,0,0)); 
    colorChange(8+startPos+2, strip.Color(255,0,0)); 

    colorChange(16+startPos+2, strip.Color(255,0,0)); 

    colorChange(24+startPos+2, strip.Color(255,0,0));

    colorChange(32+startPos+2, strip.Color(255,0,0));

    colorChange(40+startPos+2, strip.Color(255,0,0)); 
  }

  else if(num==8){
    colorChange(8+startPos, strip.Color(255,0,0));
    colorChange(8+startPos+1, strip.Color(255,0,0)); 
    colorChange(8+startPos+2, strip.Color(255,0,0)); 

    colorChange(16+startPos, strip.Color(255,0,0));
    colorChange(16+startPos+2, strip.Color(255,0,0)); 

    colorChange(24+startPos, strip.Color(255,0,0));
    colorChange(24+startPos+1, strip.Color(255,0,0)); 
    colorChange(24+startPos+2, strip.Color(255,0,0));

    colorChange(32+startPos, strip.Color(255,0,0));
    colorChange(32+startPos+2, strip.Color(255,0,0));

    colorChange(40+startPos, strip.Color(255,0,0));
    colorChange(40+startPos+1, strip.Color(255,0,0)); 
    colorChange(40+startPos+2, strip.Color(255,0,0)); 
  }

  else if(num==9){
    colorChange(8+startPos, strip.Color(255,0,0));
    colorChange(8+startPos+1, strip.Color(255,0,0)); 
    colorChange(8+startPos+2, strip.Color(255,0,0)); 

    colorChange(16+startPos, strip.Color(255,0,0));
    colorChange(16+startPos+2, strip.Color(255,0,0)); 

    colorChange(24+startPos, strip.Color(255,0,0));
    colorChange(24+startPos+1, strip.Color(255,0,0)); 
    colorChange(24+startPos+2, strip.Color(255,0,0));

    colorChange(32+startPos+2, strip.Color(255,0,0));

    colorChange(40+startPos, strip.Color(255,0,0));
    colorChange(40+startPos+1, strip.Color(255,0,0)); 
    colorChange(40+startPos+2, strip.Color(255,0,0)); 
  }
}
