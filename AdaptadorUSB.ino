
#define MAX_MEMORIA_BOTON 20
#define CANT_LECTURAS_IGUALES 3

int req = 5; //mic REQ line goes to pin 5 through q1 (arduino high pulls request line low)
int dat = 2; //mic Data line goes to pin 2
int clk = 3; //mic Clock line goes to pin 3
int boton = 4; 
int i = 0; int j = 0; int k = 0;

int ultimos_valores_boton[MAX_MEMORIA_BOTON];
int ultimo_valor_firme_boton = 1;
int contador = 0;
byte mydata[14];
long lecturas_instrumento[CANT_LECTURAS_IGUALES];
int signo = 0;
int unidad = 0;
int decimal = 0;

void setup(void){
	Serial.begin(115200);  
	pinMode(req, OUTPUT);
	pinMode(clk, INPUT_PULLUP);
	pinMode(dat, INPUT_PULLUP);
	digitalWrite(req,LOW); // set request at LOW

	// initialize the button pin as a input:
	pinMode(boton, INPUT_PULLUP);
	limpiarMemoriaBoton();
}

void loop(void){
	if (apretoBoton()) {
		Serial.println(leerInstrumento());
	}	
}

char* leerInstrumento(void){
	static char buf[7];
	static char string_retorno[12];
	string_retorno[0] = '\0';
	bool todas_las_mediciones_iguales = false;
	for(i=1;i<CANT_LECTURAS_IGUALES; i++){
		lecturas_instrumento[i] = 1234;
	}
	while(!todas_las_mediciones_iguales){
		for(i=1;i<CANT_LECTURAS_IGUALES; i++){
			lecturas_instrumento[i-1] = lecturas_instrumento[i];
		}
		
		digitalWrite(req, HIGH); // generate set request
		for( i = 0; i < 13; i++ ) {

			k = 0;

			for (j = 0; j < 4; j++) {

				while( digitalRead(clk) == LOW) { } // hold until clock is high

				while( digitalRead(clk) == HIGH) { } // hold until clock is low

				bitWrite(k, j, (digitalRead(dat) & 0x1)); // read data bits, and reverse order )

			}

			// extract data

			mydata[i] = k;
		}
		signo = mydata[4]; 
		decimal = mydata[11];
		unidad = mydata[12];
		
		bool alguno_no_es_numero_valido = false;
		for(int lp=0;lp<6;lp++)	{
			//Serial.print(mydata[lp+5]);
			//Serial.print(", ");	
			if(mydata[lp+5]>9 || mydata[lp+5]<0) alguno_no_es_numero_valido = true;
			buf[lp]=mydata[lp+5]+'0';
		}
                //Serial.println();
                
		buf[6]=0x00;
		lecturas_instrumento[CANT_LECTURAS_IGUALES-1]=atol(buf); //assembled measurement, no decimal place added
		bool alguna_medicion_distinta_a_la_primera = false;
		for(i=0;i<CANT_LECTURAS_IGUALES; i++){
			if(lecturas_instrumento[i] != lecturas_instrumento[0]) alguna_medicion_distinta_a_la_primera = true;
		}
		todas_las_mediciones_iguales = !alguna_medicion_distinta_a_la_primera && !alguno_no_es_numero_valido;
		//debug
//		for(i=0;i<CANT_LECTURAS_IGUALES; i++){
//			Serial.print(lecturas_instrumento[i]);
//			if(i<CANT_LECTURAS_IGUALES-1) Serial.print(", ");
//		}
//		Serial.println("");
		//debug
         delay(10);
	}
	int i_string = 0;
	if(signo == 8) {
		string_retorno[0] = '-';
		i_string++;
	}
	for(int i_buf=0;i_buf<6;i_buf++)	{
		if(buf[i_buf]!='0' || (i_buf+decimal)>4){
			string_retorno[i_string] = buf[i_buf];
			i_string++;		
		}
		if((i_buf+decimal)==5) {
			string_retorno[i_string] = '.';
			i_string++;
		}
	}
	string_retorno[i_string] = ' ';
	i_string++;
	if(unidad==0){
		string_retorno[i_string] = 'm';
		i_string++;
		string_retorno[i_string] = 'm';
	}
	if(unidad==1){
		string_retorno[i_string] = 'i';
		i_string++;
		string_retorno[i_string] = 'n';
	}
	i_string++;
	string_retorno[i_string] = '\0';
	Serial.println(unidad);
	return string_retorno;
}

void limpiarMemoriaBoton(){
    for(i=0;i<MAX_MEMORIA_BOTON; i++){
        ultimos_valores_boton[i] = 1;
    }
}

bool apretoBoton(){
    for(i=1;i<MAX_MEMORIA_BOTON; i++){
        ultimos_valores_boton[i-1] = ultimos_valores_boton[i];
    }
	ultimos_valores_boton[MAX_MEMORIA_BOTON-1] = digitalRead(boton);
	bool todos_cero = true;
	for(i=0;i<MAX_MEMORIA_BOTON; i++){
        if(ultimos_valores_boton[i] == 1) todos_cero = false;
    }
	if(todos_cero){
		if(ultimo_valor_firme_boton == 1){
			ultimo_valor_firme_boton = 0;
			return true;
		}
	}
	else{
		bool todos_uno = true;
		for(i=0;i<MAX_MEMORIA_BOTON; i++){
			if(ultimos_valores_boton[i] == 0) todos_uno = false;
		}
		if(todos_uno){
			if(ultimo_valor_firme_boton == 0){
				ultimo_valor_firme_boton = 1;
			}
		}
	}
	return false;
}











































