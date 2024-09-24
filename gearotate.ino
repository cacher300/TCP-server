#include "ClearCore.h"
#include "EthernetTcpServerManual.h"


#define sprocket ConnectorM0
#define gear ConnectorM1
#define rod ConnectorM2

#define gear_sensor ConnectorDI8
#define sprocket_sensor ConnectorDI6
#define baudRate 9600
#define SerialPort ConnectorUsb

#define rodVelocityLimit 500 
#define rodAccelerationLimit 100000

#define gearVelocityLimit 3000 
#define gearAccelerationLimit 100000

#define sprocketVelocityLimit 3000 
#define sprocketAccelerationLimit 100000

bool inputState;
bool gear_homed = false;
bool sprocket_homed = false;
bool inputStateGear = false;  
bool inputStateSprocket = false;  

void home();
void ProcessClientMessage(char *message);

void setup() {
    InitServer(); 
    MotorMgr.MotorInputClocking(MotorManager::CLOCK_RATE_LOW);
    MotorMgr.MotorModeSet(MotorManager::MOTOR_ALL, Connector::CPM_MODE_STEP_AND_DIR);
    rod.VelMax(rodVelocityLimit);
    rod.AccelMax(rodAccelerationLimit);
    gear.VelMax(gearVelocityLimit);
    gear.AccelMax(gearAccelerationLimit);
    sprocket.VelMax(sprocketVelocityLimit);
    sprocket.AccelMax(sprocketAccelerationLimit);
    
    SerialPort.Mode(Connector::USB_CDC);
    SerialPort.Speed(baudRate);
    uint32_t timeout = 6000;
    uint32_t startTime = Milliseconds();
    SerialPort.PortOpen();
    while (!SerialPort && Milliseconds() - startTime < timeout) {
        continue;
    }
    SerialPort.SendLine("Serial port opened.");
    sprocket.EnableRequest(true);
    gear.EnableRequest(true);
    rod.EnableRequest(true);

    SerialPort.SendLine("Motor enabled.");
    gear_sensor.Mode(Connector::INPUT_DIGITAL);
    sprocket_sensor.Mode(Connector::INPUT_DIGITAL);
    SerialPort.SendLine("Input pins set to digital input.");
    home();
    Delay_ms(20000);  

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
    if (strcmp(message, "GEAR") == 0) {
     
      gear.Move(8000);
      Delay_ms(3000);  
      SerialPort.SendLine("Done");
    }

    if (strcmp(message, "ROD") == 0) {
     
      rod.Move(1700);
      Delay_ms(3000);  
      SerialPort.SendLine("Done");
    }

    if (strcmp(message, "SPR") == 0) {
     
      sprocket.Move(8000);
      Delay_ms(3000);  
      SerialPort.SendLine("Done");
    }

    else {
        SerialPort.SendLine("Unknown command.");
    }
}

void home() {
    while (!gear_homed || !sprocket_homed) {
        if (!gear_homed) {
            inputStateGear = gear_sensor.State();
            SerialPort.SendLine("Reading gear sensor state.");
            gear.Move(6400);  
            if (inputStateGear) {
                SerialPort.SendLine("Gear Homed");
                gear.MoveStopAbrupt();
                Delay_ms(200);  
                gear_homed = true;
            }
        }

        if (!sprocket_homed) {
            inputStateSprocket = sprocket_sensor.State();
            SerialPort.SendLine("Reading sprocket sensor state.");
            sprocket.Move(6400);  
            if (inputStateSprocket) {
                SerialPort.SendLine("Sprocket Homed");
                sprocket.MoveStopAbrupt();
                Delay_ms(200);  
                sprocket_homed = true;
            }
        }

        if (!inputStateGear && !inputStateSprocket) {
            SerialPort.SendLine("No trigger detected.");
        }

        Delay_ms(100); 
    }
}
