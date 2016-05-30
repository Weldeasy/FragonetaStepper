#include "Servo.h"
#include "FragonetaStepper.h"
#include "PimpamScara.h"
#define _USE_MATH_DEFINES
#include <math.h>
//#define PI 3.14159265 si no lo pillara
PimpamScara::PimpamScara(FragonetaStepper &motor1, 
	FragonetaStepper &motor2, 
	FragonetaStepper &motor3,
	float L1, float L2, int pinservo1, int pinservo2, int botManual) {
	_estado = 0;
	boolean calibrado = false;
	float motor1Fin;
	_L1 = L1;
	_L2 = L2;

	_botManual =  botManual;

	_motor1 = motor1;
	_motor2 = motor2;
	_motor3 = motor3;
	_pinservo1 = pinservo1;
	_pinservo2 = pinservo2;

	aux = false;
	float _x=0;
	float _y=0;
	float _z=0; //coordenada X que llega por serial
	punto_enviado = false;
	paso = 1;

	_count_reproduccion = 0;
	_count_reproducidas = 0;
	esperando_punto = false;

	//pinMode(ledPin, OUTPUT); 
}

void PimpamScara::newData(String data) {
	//data = "x:120;y:200;z:300;otro:254;";
	//Serial.println(data);
	char buffer[10];
	int index = data.indexOf(';');
	int lastIndex = 0;
	int count_temp;
	while(index >= 0) {

		String value = data.substring(lastIndex, index);

		char v1c = value[0];
		String valor = data.substring(lastIndex+1, index);
		//Serial.print(valor);
		switch(v1c) {
			case 'x':			
			valor.toCharArray(buffer, 10);
			_x = atof(buffer);
			break;
			case 'y':
			//_y = valor.toFloat();
			valor.toCharArray(buffer, 10);
			_y = atof(buffer);
			break;
			case 'z':
			//_z = valor.toFloat();
			valor.toCharArray(buffer, 10);
			_z = atof(buffer);
			break;
			case 'c':
			valor.toCharArray(buffer, 10);
			count_temp = atoi(buffer);
			break;
			case 'h':
				//notese que si primero ve la hpos se movera sin que las variables sean asignadas por tanto deve pasarse al final del string
				if(valor == "pos") {
					if(_estado == 0) {
						moverAbs(_x, _y, _z);
					}
				}

				if(valor == "reset") {
					reset();
				}

				if(valor == "setup") {
					webSetup();
				}

				if(valor == "calibrado") {
					paso = 1;
					calibrado = false;
				}

				if(valor == "getpunto") {
					enviar_punto();
				}

				if(valor == "reproducir") {
					if(_count_reproduccion == 0 && _count_reproducidas == 0) {
						_estado = 2;
						_count_reproduccion = count_temp;
					}
				}

				if(valor == "sumar_paso") {
					moverAbs(_x,_y,_z);
					_count_reproducidas++;
					esperando_punto = false;
				}

				if(valor == "poweroff") {
					
					_motor1.enableOutputs();
					_motor2.enableOutputs();
					_motor3.enableOutputs();
				}

				if(valor == "poweron") {
					_motor1.disableOutputs();
					_motor2.disableOutputs();
					_motor3.disableOutputs();
				}
			break;
		}

		lastIndex = index+1;
		index = data.indexOf(';', lastIndex+1);

	} 

}


void PimpamScara::enviar_punto() {
	Serial.print("{\"header\":\"punto\"");
	Serial.print(",\"x\":\"");
	Serial.print(_x);
	Serial.print("\"");
	Serial.print(",\"y\":\"");
	Serial.print(_y);
	Serial.print("\"");
	Serial.print(",\"z\":\"");
	Serial.print(_z);
	Serial.print("\"");
	Serial.print(",\"angpinza\":\"");
	Serial.print(_servo1.read());
	Serial.print("\"");
	Serial.print(",\"angmuneca\":\"");
	Serial.print(_servo2.read());
	Serial.print("\"");
	Serial.println("}");
}

void PimpamScara::webSetup() {
	Serial.print("{\"header\":\"setup\"");
	Serial.print(",\"l1\":\"");
	Serial.print(_L1);
	Serial.print("\"");
	Serial.print(",\"l2\":\"");
	Serial.print(_L2);
	Serial.print("\"");
	Serial.print(",\"l1a\":\"");
	Serial.print(_motor1.currentDeg());
	Serial.print("\"");
	Serial.print(",\"l2a\":\"");
	Serial.print(_motor2.currentDeg());
	Serial.print("\"");
	Serial.print(",\"z\":\"");
	Serial.print(_motor3.currentMM());
	Serial.print("\"");
	Serial.println("}");		
}


//crear metodo "tomar decision"
//este seria el unico que se deveria llamar en el sketch
void PimpamScara::choser(){
//si esta calibrado operara sino se calibrara.

	if(calibrado==true){
//con los estados y que decida que hacer.
//int botManualState = digitalRead(_botManual);
	//Serial.println(botManualState);

	//int prev_botManualState=LOW;
	switch(_estado){
//STOP
		case 0:



		
		/*if (botManualState == HIGH && prev_botManualState== LOW) {
			Serial.println("Cambiando a manual...");
			delay(1000);
			_estado = 3;
			prev_botManualState=HIGH;
		}else{
			prev_botManualState=LOW;
		}*/

			break;
//ESPERA
			case 1  :
		//MOVIENDOSE BIEN
			if( _motor1.distanceToGo()==0 && _motor2.distanceToGo()==0 && _motor3.distanceToGo()==0){
				webSetup();
				_estado = 0;
			}

			break;
		//REPRODUCIR
			case 2: 
			if( _motor1.distanceToGo()==0 && _motor2.distanceToGo()==0 && _motor3.distanceToGo()==0){
				if(!esperando_punto) {
					if(_count_reproduccion > 0 && _count_reproducidas < _count_reproduccion) {
						//siguiente plas
						pedir_siguiente_punto();
						esperando_punto = true;
					} else if(_count_reproduccion > 0 && _count_reproducidas >= _count_reproduccion) {
						_estado = 0;
						_count_reproduccion = 0;
						_count_reproducidas = 0;
						emite("acabar_reproduccion", "", "");
					}
					webSetup();
				} else {
					//movimiento completado

				}
			}
			break;
			case 3: 

			movManual();

			break;
			case 4:
		//RESET
			if( _motor1.distanceToGo()==0 && _motor2.distanceToGo()==0 && _motor3.distanceToGo()==0){
				emite("robot_reset", "", "");
				_estado = 0;
			}


			break;
		}
	}else{
		calibrar();
	}
}

void PimpamScara::pedir_siguiente_punto() {

	Serial.print("{\"header\":\"siguiente_punto\"");
	Serial.print(",\"offset\":\"");
	Serial.print(_count_reproducidas);
	Serial.print("\"");
	Serial.println("}");

}



void PimpamScara::motorSetup() {
	_motor1.setMaxSpeed(2000.0);
	_motor1.setAcceleration(2000.0);
	_motor1.setSpeed(2000);

	_motor2.setMaxSpeed(1000.0);
	_motor2.setAcceleration(1000.0);
	_motor2.setSpeed(1000);

	_motor3.setMaxSpeed(200.0);
	_motor3.setAcceleration(200.0);
	_motor3.setSpeed(200);

//_motor1.setCurrentDeg(90);
//_motor2.setCurrentDeg(90);
	pinMode(_botManual, INPUT);

	_servo1.attach(_pinservo1);

//_x = _L1+_L2;

			//	_motor1.moveDeg(10);

}


//calibra el robot al iniciar usando pasos.
	void PimpamScara::calibrar(){
	//Serial.print(_motor1.currentDeg());
	//Serial.print(" ");
	//Serial.println(_motor2.currentDeg());
		if(calibrado==false){

//como e sun loop devo controlar en que paso esta yo mismo
//paso 0 mover a la izquierda hasta que toque fianl
//cuando toca el final seteo a 0 la posicion y sumo paso.
	//	Serial.print(_motor1.distanceToGo());
	//	Serial.print(" - ");
	//	Serial.println(_motor2.distanceToGo());
			int paso_aux = 0;
		/*Serial.print(paso);
		Serial.print(" ");
		Serial.print(_motor1.currentPosition());
		Serial.print(" ");
		Serial.print(_motor2.currentPosition());
		Serial.println(" ");*/
		switch(paso){
			case 1  :
			
			if(_motor1.distanceToGo() == 0 && _motor2.distanceToGo() == 0) {
				_motor1.moveDeg(360);
				_motor2.moveDeg(-360);
			} else { 
//if(_motor1.checkEnd()==1){
				if(_motor1.checkEnd()){
					_motor1.stop();
//si no va probar  
					_motor1.setCurrentPosition(0);

				}

				if(_motor2.checkEnd()){
					_motor2.stop();
//si no va probar  
					_motor2.setCurrentPosition(0);

				}

				if(_motor1.targetPosition() == 0 && _motor2.targetPosition() == 0) {
					paso++;
				}
			}

			break; 
			case 2  :
			
			if( _motor1.distanceToGo() == 0 && _motor2.distanceToGo() == 0 && _motor1.targetPosition() == 0 && _motor2.targetPosition() == 0) {
				_motor1.moveToDeg(_motor1._gradosFin/2);
				_motor2.moveToDeg((_motor2._gradosFin/2)-4);
				_motor3.moveMM(-300);

			}else{
//if(_motor1.checkEnd()==2){
				//Serial.println(_motor1._gradosFin/2);
				if(_motor1.currentDeg()<=_motor1._gradosFin/2*_motor1._direccion){
					//Serial.println("He llegado al medio (motor1)");
//guardo ahi la pasoicion del paso donde esta el final de carrera en grados
					_motor1.stop();
					
				}

				if(_motor2.currentDeg()>=_motor2._gradosFin/2){
//guardo ahi la pasoicion del paso donde esta el final de carrera en grados
					//Serial.println("He llegado al medio (motor2)");
					_motor2.stop();
					
				}

				if(_motor3.checkEnd()){
//guardo ahi la pasoicion del paso donde esta el final de carrera en grados
					//Serial.println("He llegado al medio (motor2)");
					_motor3.setCurrentPosition(0);
					_motor3.stop();
					_motor3.moveMM(_motor3._gradosFin);
					
				}

				if(_motor1.distanceToGo() == 0 && _motor2.distanceToGo() == 0 && _motor3.distanceToGo() == 0) {
					paso++;
				}
			}
			break;
			case 3:
			if(_motor3.distanceToGo() == 0) {
				paso++;
			}

			break;
//el otro motor
			case 4:

//voy al punto intermedio
			//_motor2.moveToDeg(motor2Fin/2 );
//me aseguro k ha acabado
			//if(_motor2.distanceToGo()==0){
			_angbrazo_inicial = _motor1.currentDeg();
			_angcodo_inicial = _motor2.currentDeg();
//paso = paso +1 ;		

			reset();
			paso++;
			//}
			break;

			case 5:
			if(_motor1.distanceToGo() == 0 && _motor2.distanceToGo() == 0) {
				emite("robot_reset", "", "");
				calibrado = true;
				paso++;
			}
			break;
//default : 
		}
	}

}

void PimpamScara::emite(String name, String key, String valor) {
	Serial.print("{\"header\":\"");
	Serial.print(name);
	Serial.print("\"");
	if(key!="" && valor!="") {
		Serial.print(",\"");
		Serial.print(key);
		Serial.print("\":");
		Serial.print("\"");
		Serial.print(valor);
		Serial.print("\"");
	}
	Serial.println("}");	
}

//metodo que devuelva el estado "averiguar estado en el que esta"
int PimpamScara::testStatus(){
	//boton del modo manual
				/*int buttonState = 0;
				buttonState = digitalRead(botManual);

//si el estado no es manual.
				if(_estado != 2){
		//si alguno de los motroes se esta moviendo : estado MOVIENDOSE.
		//aqui dice que no tiene esta funcion el frago sin embargo uso otras del accel y no se queja ahora mismo no lo veo.
					if( _motor1.distanceToGo()==0 || _motor2.distanceToGo()==0 || _motor3.distanceToGo()==0){
						_estado = 1;
		}//aqui deveria comprobar que el servidor no le manda ordenes
		//si no se le mandan ordenes ni se mueve y se pulsa el boton el estado sera manual
	}*/

}

// enviar datos al servidor por serial
	void PimpamScara::envServer(){
//grados actuales de los motores etc.
	}

	void PimpamScara::movManual(){
//lo tipico que lea el yoystic y que pase la lectura a
// la funcion de movimiento y si se pulsa el boton se acave el modomanual

//Habria que pasar los pines al constructor pero no me hace gracia la idea mejor predefinirlos k sino el constructor esun monstruo
//Serial.println("manual");

		int joyPin1 = A0;
		int joyPin2 = A1;
		int valor1 = 0;
		int valor2 = 0;
		float movX = 0;
		float movY = 0; 
		valor1 = analogRead(joyPin1);
		valor2 = analogRead(joyPin2);

//asumo que superior a 500 es  sumar
		if(valor1 > 550 ){
			//la X actual + lo que quiero moverla
			movX = calcJoySum(valor1-512); 
	 	//movX = 
		}
		else if(valor1 < 490) {
			//lo sumo porque le llega un negativo
			movX = calcJoySum(valor1-512); 
		}
		if(valor2 > 550 ){
			movY =  calcJoySum(valor2-512); 
		}else if(valor2 < 490){
		//lo sumo porque le llega un negativo
			movY =  calcJoySum(valor2-512);
		}
		
	//ahora comprovara que ningun motor se mueve ! para que no calcule la cinematica cada vez que lo pulses sin ser necesaria.

		if( _motor1.distanceToGo()==0 && _motor2.distanceToGo()==0 && _motor3.distanceToGo()==0){
			int botManualState = digitalRead(_botManual);

			if (botManualState == HIGH) {
				//Serial.println("Parando movimiento manual");
				delay(1000);
				_estado = 0;

			} 

			if(movX!=0 || movY!=0){
					//ahora si que se movera usando la posicion absoluta

	//_motor1.moveDeg(90);

				//moverAbs(movX, movY);

	/*Serial.print("X");
	Serial.println(movX);*/


//	Serial.print("_xxx");
//	Serial.println(_x);

	//Serial.println("Y");
//	Serial.println(movY);
//	_x = movX;
//	_y = movY;
			}
		}
	}



	float PimpamScara::calcJoySum(float joyVal){
		return joyVal/512*5;
	}


// funcion que lea de serial

// mover el robot entero
	void PimpamScara::moverAbs(float x,float y, float z){

//solo he traducido a c++ falta rematar la logica del palo que cuando se hay aacavado el calibrado asignar X e Y supongo
	/*X = X - robotx;
	 Y = Y - roboty;*/
/*	Serial.print("L1: ");
	Serial.println(_L1);

	Serial.print("L2: ");
	Serial.println(_L2);

	Serial.print("x: ");
	Serial.println(x);

	Serial.print("y: ");
	Serial.println(y);*/
	if( _motor1.distanceToGo()==0 && _motor2.distanceToGo()==0 && _motor3.distanceToGo()==0){


		x =  x;
		y =  y;

		/*Serial.print("x y");
		Serial.print(x);
		Serial.print(" ");
		Serial.println(y);*/


		float hipotenusa = sqrt( pow(x, 2) + pow(y, 2));
		/*Serial.print("hipotenusa: ");
		Serial.println(hipotenusa);*/
		float alfa = atan2(y,x);
	/*	Serial.print("alfa: ");
		Serial.println(alfa);*/


		float beta = pow(_L1, 2) - pow(_L2, 2) + pow(hipotenusa, 2);
		/*Serial.print("beta1: ");
		Serial.println(beta);*/
		float calcul = 2*_L1*hipotenusa;
	/*Serial.print("calcul: ");
		Serial.println(calcul);*/


		beta = beta / calcul;

		//beta = beta / ( 2 * L1 * hipotenusa);
	/*	Serial.print("beta2: ");
		Serial.println(beta);*/
		beta = acos(beta);

		//Serial.print("beta3: ");
		//Serial.println(beta);

		float angbrazo = ((float)alfa+(float)beta)*(180/M_PI)-145;//Alfa+beta son radianes
	//	Serial.print(_x);
	//	Serial.print(" ");
	//	Serial.println(_y);
		float gamma = acos( ( (float)pow(_L1, 2) + (float)pow(_L2, 2) - (float)pow(hipotenusa, 2) ) / ( 2*_L1*_L2 ) );

		float angcodo = (gamma*(180/M_PI))-180;//gamma son radiane

		angcodo = -angcodo;
		angbrazo = -angbrazo;

		
	//asumiendo que _motor1 sea el brazo

		/*if(angbrazo > 0 && angbrazo > 180) {
			angbrazo = 360 - angbrazo;
		}

		if(angcodo > 0 && angcodo > 180) {
			angcodo = 360 - angcodo;
		}*/


			float angbrazo_actual = _motor1.currentDeg();
			float angcodo_actual = _motor2.currentDeg();
	/*	Serial.print("angbrazo actual: ");
		Serial.println(angbrazo_actual);
		Serial.print("angcodo actual: ");
		Serial.println(angcodo_actual);
		Serial.print("angbrazo TOTAL: ");
		Serial.println(angbrazo);
		Serial.print("angcodo TOTAL: ");
		Serial.println(angcodo);*/

		/*Serial.print(angbrazo-234.98);
		Serial.print(" ");
		Serial.println(angcodo);
		Serial.print("Medio:");
		Serial.println(_motor1._gradosFin/2);*/
		float angbrazo_fin = (angbrazo - angbrazo_actual) - 234.98;
		float angcodo_fin = (angcodo - angcodo_actual) + ((_motor2._gradosFin/2)-4);

		if(x >=0 && y>=0) {
			angbrazo_fin = angbrazo_fin;
			angcodo_fin = angcodo_fin;
			emite("asf", "valor", "u got it bro");
		} else {
			

			float angbrazo_futuro = _motor1.currentDeg() + angbrazo_fin;
			//if(angbrazo_futuro < _motor)

			if(angbrazo_futuro < _motor1._gradosFin) {
				angbrazo_fin = angbrazo_fin+(90);
				angcodo_fin = (-angcodo)-(angcodo_actual-((_motor2._gradosFin/2)-4));
				//Serial.println((angbrazo_fin + 360)-180);

			} else if(angbrazo_futuro > 0) {
				//Serial.print("currentDeg brazo");
				//Serial.println(_motor1.currentDeg());
				//Serial.print("angbrazo_futuro");
				//Serial.println(angbrazo_futuro);


				angbrazo_fin = angbrazo_fin;
				angcodo_fin = angcodo_fin;

				//Serial.println("CAMBIAME");


			} else {
				angbrazo_fin = angbrazo_fin;
				angcodo_fin = angcodo_fin;
			}

			
		}

		//angbrazo = angbrazo_fin + angbrazo;
		//angcodo = angcodo_fin + angcodo;
		
		/*Serial.print("angbrazo currenDeg: ");
		Serial.println(_motor1.currentDeg());
		Serial.print("angbrazo TOTAL: ");
		Serial.println(angbrazo);*/

		//angbrazo = angbrazo - angbrazo_actual-234.98;
		//angcodo = angcodo - angcodo_actual+116;


		//Serial.println("MOVIENDO:");
		//Serial.print(angbrazo_fin);
		//Serial.print("  -  ");
		//Serial.println(angcodo_fin);

		//Serial.print("ang brazo:");
		//Serial.println(_motor1.currentDeg());
		//Serial.print("ang codo:");
		//Serial.println(_motor2.currentDeg());





		if(angbrazo != angbrazo || angcodo != angcodo) {
			//Serial.println("Fuera del plano");
		} else {
			_x = x;
			_y = y;
			_motor1.moveDeg(angbrazo_fin);
			_motor2.moveDeg(angcodo_fin);
			

			float mm_absolutos = -(_motor3.currentMM()-_z);

			_motor3.moveMM(mm_absolutos);
			if(_estado != 2) {
				_estado = 1;
			}


		}
	}

}

void PimpamScara::reset(){

	//Serial.println("reset");
	_motor1.moveToDeg(_angbrazo_inicial);
	_motor2.moveToDeg(_angcodo_inicial);
	_estado = 4;
}


void PimpamScara::run(){
	/*Serial.print(_motor1.distanceToGo());
	Serial.print(" ");
	Serial.println(_motor2.distanceToGo());*/
	choser();


	if(!calibrado) {
		_motor1.run();
		_motor2.run();
		_motor3.run();
	} else {
		int fin1 = _motor1.checkEnd();
		int pos1 = _motor1.currentDeg();
		int tpos1 = _motor1.targetPosition();
		int motor1_direccion = _motor1.getDirection();

		if(fin1) {
			
			if(motor1_direccion < 0.0) {
				_motor1.run();
			} else {
				_motor1.setCurrentPosition(_motor1.currentPosition());
			}
		} else {
			if(pos1 > _motor1._gradosFin || (motor1_direccion > 0.0 && tpos1 > _motor1.currentPosition())) {
				_motor1.run();
			} else {
				_motor1.setCurrentPosition(_motor1.currentPosition());
			}
		}
		

		/*
		Serial.print(" - ");
		Serial.print(tpos1);
		Serial.print(" - ");
		Serial.println(pos1);*/

		int fin2 = _motor2.checkEnd();
		int pos2 = _motor2.currentDeg();
		int tpos2 = _motor2.targetPosition();
		int motor2_direccion = _motor2.getDirection();


		if(fin2) {

			if(motor2_direccion > 0.0) {
				_motor2.run();
			} else {
				_motor2.setCurrentPosition(_motor2.currentPosition());
			}
		} else {
			if(pos2 < _motor2._gradosFin || (motor2_direccion < 0.0 && tpos2 > _motor2.currentPosition())) {
				_motor2.run();
			} else {
				_motor2.setCurrentPosition(_motor2.currentPosition());
			}
		}


		int fin3 = _motor3.checkEnd();
		int pos3 = _motor3.currentMM();
		int tpos3 = _motor3.targetPosition();
		int motor3_direccion = _motor3.getDirection();


		if(fin3) {

			if(motor3_direccion > 0.0) {
				_motor3.run();
			} else {
				_motor3.setCurrentPosition(_motor3.currentPosition());
			}
		} else {
			if(pos3 < _motor3._gradosFin || motor3_direccion < 0.0) {
				_motor3.run();
			} else {
				_motor3.setCurrentPosition(_motor3.currentPosition());
			}
		}


	}

}