#include "FragonetaStepper.h"

FragonetaStepper::FragonetaStepper(int direccion, uint8_t interface, uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4, int spr, int finA, float gradosFin) :
AccelStepper(interface, pin1, pin2, pin3, pin4) {
	
	//Steps Per Rotation del motor
	_spr = spr;
	_finA = finA;
	pinMode(_finA, INPUT_PULLUP);
	_direccion = direccion;
	_gradosFin = gradosFin;

	//variable para el stado del boton utilizable para ambos. ¿se podra usar en la clase?
}

int FragonetaStepper::test() {

}

//Devuelve los grados absolutos del motor 0-359 
float FragonetaStepper::currentDeg()
{
	
		return  (float)360/_spr*currentPosition()*_direccion;
	
}

float FragonetaStepper::currentMM()
{
	
		return  (float)(currentPosition()/_spr)*_direccion;
	
}

void FragonetaStepper::setCurrentDeg(float deg = 0)
{
	
		setCurrentPosition(_spr / 360 * deg);

}

//Mueve el motor X grados a izquiera o derecha respecto a la posicion actual.
void FragonetaStepper::moveDeg(float grados)
{	
	//22/05  
	// Si no se pulsa un boton se mueve sin problemas, sino comprobara
	// cual de ellos para saber si el movimiento es posible
		long pasos = grados/360*_spr;
		AccelStepper::move(pasos*_direccion);	
		
}

void FragonetaStepper::moveMM(float mm) {
	long pasos = mm*_spr;
	AccelStepper::move(pasos*_direccion);
}

//Mueve el motor a X grados a izquiera o derecha respecto posicion inicial.
void FragonetaStepper::moveToDeg(float degrees){
	moveDeg(absDeg(degrees-currentDeg()));
} 

float FragonetaStepper::absDeg(float deg){
	if(deg >= 360){
		deg = deg - 360;
		FragonetaStepper::absDeg(deg);
	}else if(deg < 0 && deg <= -360){
		deg = deg + 360;			
		FragonetaStepper::absDeg(deg);
	}else{
		return deg;
	}
}

boolean FragonetaStepper::run() {
	//int final = checkEnd();
	//Serial.println(final);

	//if(!final || currentDeg() < _gradosFin) {
	AccelStepper::run();
	//} 

	
	
	//}
}

float FragonetaStepper::getDirection() {
	return AccelStepper::speed()*_direccion;
}


//22/05
//metodo para saber si se esta pulsando un final de carrera y cual
//devuelve Int 0 si ninguno 1si es el izquierdoA o 2 si es el derechoB
int FragonetaStepper::checkEnd(){
	int var = 0;
	int buttonState = 0;
	//Serial.println("finA = ");
	//Serial.println(_finA);
	buttonState = digitalRead(_finA);
	//Serial.println(buttonState);
	//Serial.println(buttonState);

	if (buttonState == LOW) {
    //pulsado A
		var = 1;
		//guarda la posicion del final en grados
		//posFinA == currentDeg();
	}
	//Serial.println("_finB = ");
	//Serial.println(_finB);
	//buttonState = digitalRead(_finB);
	//Serial.println(buttonState);
	
	//si ninguno se cumple var sera 0
	//Serial.println(var);
	return var;
}