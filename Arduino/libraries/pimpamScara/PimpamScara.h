//#include <../AccelStepper/FragonetaStepper.h>
class PimpamScara {
public:
	PimpamScara( FragonetaStepper &motor1, FragonetaStepper &motor2, FragonetaStepper &motor3, float L1, float L2, int pinservo1, int pinservo2, int botManual);
	int _estado;
	FragonetaStepper _motor1;
	FragonetaStepper _motor2;
	FragonetaStepper _motor3;
	Servo _servo1;
	Servo _servo2;

	void calibrar();
	void choser();
	void envServer();
	int testStatus();
	void movManual();
	void moverAbs(float x,float y, float z);
	float calcJoySum(float joyVal);
	void run();
	void motorSetup();
	void newData(String data);
	String* StringToArray(String data);
	void emite(String name, String key, String valor);
	int _pinservo1;
	int _pinservo2;

private:
	float _L1;
	float _L2;

	float motor2Fin;
	float motor1Fin;
	int paso;
	int _botManual;
	boolean calibrado;
	float _x, _y, _z;
	float _angcodo_inicial;
	float _angbrazo_inicial;
	boolean aux;
	void webSetup();
	void reset();
	void enviar_punto();
	boolean punto_enviado;
	int _count_reproduccion = 0;
	int _count_reproducidas = 0;
	boolean esperando_punto;
	void pedir_siguiente_punto();


};
