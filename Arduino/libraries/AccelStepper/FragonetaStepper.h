#include <AccelStepper.h>
class FragonetaStepper : public AccelStepper {
public:
  FragonetaStepper(int direccion = 1,
  				   uint8_t interface = AccelStepper::FULL4WIRE, 
				   uint8_t pin1 = 2, 
				   uint8_t pin2 = 3, 
				   uint8_t pin3 = 4, 
				   uint8_t pin4 = 5,
				   int spr = 4096,
				   int finA = 0,
				   float gradosFin = 10);     
	void setCurrentDeg(float deg);
	void moveDeg(float grado);
	void moveToDeg(float degrees);
	float currentDeg();
	float absDeg(float deg);
	int checkEnd();
	boolean run();
	int test();
	float _gradosFin;
	float targetDeg();
	int _direccion;
	int _spr;
	float getDirection();
	void moveMM(float mm);
	float currentMM();
 private:
	
	int _finA;
	
	float posFinA;
	float posFinB;

};
