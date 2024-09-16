#include "ClearCore.h"
#include "EthernetTcpServerManual.h"


#define motor ConnectorM0
#define inputPin ConnectorDI6
#define JakainputPin ConnectorDI8
#define baudRate 9600
#define SerialPort ConnectorUsb

#define velocityLimit 5000 
#define accelerationLimit 100000


bool inputState;
bool homed = false;
int triggerCount = 0;


void MoveContinuous();
void ProcessClientMessage(char *message);

void setup() {
    InitServer(); 
    MotorMgr.MotorInputClocking(MotorManager::CLOCK_RATE_LOW);
    MotorMgr.MotorModeSet(MotorManager::MOTOR_ALL, Connector::CPM_MODE_STEP_AND_DIR);
    motor.VelMax(velocityLimit);
    motor.AccelMax(accelerationLimit);
    
    SerialPort.Mode(Connector::USB_CDC);
    SerialPort.Speed(baudRate);
    uint32_t timeout = 6000;
    uint32_t startTime = Milliseconds();
    SerialPort.PortOpen();
    while (!SerialPort && Milliseconds() - startTime < timeout) {
        continue;
    }
    SerialPort.SendLine("Serial port opened.");
    motor.EnableRequest(true);
    SerialPort.SendLine("Motor enabled.");
    inputPin.Mode(Connector::INPUT_DIGITAL);
    SerialPort.SendLine("Input pin set to digital input.");
}

void loop() {
    ManageClients(); 

    char messageBuffer[MAX_PACKET_LENGTH];
    if (GetClientMessage(messageBuffer, MAX_PACKET_LENGTH)) {
        ConnectorUsb.SendLine("Received message: ");
        ConnectorUsb.SendLine(messageBuffer);
        ProcessClientMessage(messageBuffer);
    }
}


void ProcessClientMessage(char *message) {
    if (strcmp(message, "1") == 0) {
     
      motor.Move(-8400);
      Delay_ms(3000);  
      MoveContinuous();
      SerialPort.SendLine("Done");
      while(true){
        Delay_ms(3000);  
        motor.Move(-15900);
        Delay_ms(5000);  
        motor.Move(15900);
      }
    }

    else {
        SerialPort.SendLine("Unknown command.");
    }
}

void MoveContinuous() {
    while (!homed) {
        inputState = inputPin.State();
        SerialPort.SendLine("Reading sensor state.");
        motor.Move(6400);  

        if (inputState) {
    
            triggerCount++;
            SerialPort.SendLine("YO Induction sensor triggered.");
            
            motor.MoveStopAbrupt();
            SerialPort.SendLine("Motor stopped.");

    
            Delay_ms(200);  
            homed = true;
        } else {
            SerialPort.SendLine("No trigger detected.");
        }

        Delay_ms(100); 
    }
}
