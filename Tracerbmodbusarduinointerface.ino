// CONNECT THE RS485 MODULE RX->RX, TX->TX.
// Disconnect when uploading code.
#include <SimpleTimer.h>
#include <ModbusMaster.h>
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

int timerTask1, timerTask2;
float battBhargeCurrent, bvoltage, ctemp, btemp, bremaining, lpower, lcurrent, pvvoltage, pvcurrent, pvpower;
float stats_today_pv_volt_min, stats_today_pv_volt_max;
uint8_t result;

// To add later
//uint8_t result, time1, time2, time3, date1, date2, date3, dateDay, dateMonth, dateYear, timeHour, timeMinute, timeSecond;
//char buf[10];
//String dtString;

// this is to check if we can write since rs485 is half duplex
bool rs485DataReceived = true;

ModbusMaster node;
SimpleTimer timer;

// tracer requires no handshaking
void preTransmission() {}
void postTransmission() {}

// a list of the regisities to query in order
typedef void (*RegistryList[])();
RegistryList Registries = {
  AddressRegistry_3100,
  AddressRegistry_311A,
  AddressRegistry_3300,
};
// keep log of where we are
uint8_t currentRegistryNumber = 0;
// function to switch to next registry
void nextRegistryNumber() {
  currentRegistryNumber = (currentRegistryNumber + 1) % ARRAY_SIZE( Registries);
}

void setup()
{
  Serial.begin(115200);
  // Modbus slave ID 1
  node.begin(1, Serial);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);


  timerTask1 = timer.setInterval(1000, doRegistryNumber);
  timerTask2 = timer.setInterval(1000, nextRegistryNumber);
}

// --------------------------------------------------------------------------------



void doRegistryNumber() {
  Registries[currentRegistryNumber]();
}

void AddressRegistry_3100() {
  result = node.readInputRegisters(0x3100, 7);
  if (result == node.ku8MBSuccess)
  {
    ctemp = node.getResponseBuffer(0x11) / 100.0f;
     {
       Serial.print("Controller Temp: ");
      Serial.println(ctemp);
     
    }
    bvoltage = node.getResponseBuffer(0x04) / 100.0f;
     {
      Serial.print("Battery Voltage: ");
      Serial.println(bvoltage);

    }
    lpower = ((long)node.getResponseBuffer(0x0F) << 16 | node.getResponseBuffer(0x0E)) / 100.0f;
     {
      Serial.print("Load Power: ");
      Serial.println(lpower);

    }
    lcurrent = (long)node.getResponseBuffer(0x0D) / 100.0f;
     {
      Serial.print("Load Current: ");
      Serial.println(lcurrent);

    }
    pvvoltage = (long)node.getResponseBuffer(0x00) / 100.0f;
     {
      Serial.print("PV Voltage: ");
      Serial.println(pvvoltage);

    }
    pvcurrent = (long)node.getResponseBuffer(0x01) / 100.0f;
     {
      Serial.print("PV Current: ");
      Serial.println(pvcurrent);

    }
    pvpower = ((long)node.getResponseBuffer(0x03) << 16 | node.getResponseBuffer(0x02)) / 100.0f;
     {
      Serial.print("PV Power: ");
      Serial.println(pvpower);
    }
    battBhargeCurrent = (long)node.getResponseBuffer(0x05) / 100.0f;
     {
      Serial.print("Battery Charge Current: ");
      Serial.println(battBhargeCurrent);
      Serial.println();
    }
  } else {
    rs485DataReceived = false;
  }
}

void AddressRegistry_311A() {
  result = node.readInputRegisters(0x311A, 2);
  if (result == node.ku8MBSuccess)
  {
    bremaining = node.getResponseBuffer(0x00) / 1.0f;
     {
      Serial.print("Battery Remaining %: ");
      Serial.println(bremaining);

    }
    btemp = node.getResponseBuffer(0x01) / 100.0f;
     {
      Serial.print("Battery Temperature: ");
      Serial.println(btemp);
      
    }
  } else {
    rs485DataReceived = false;
  }
}

void AddressRegistry_3300() {
  result = node.readInputRegisters(0x3300, 2);
  if (result == node.ku8MBSuccess)
  {

    
    stats_today_pv_volt_max = node.getResponseBuffer(0x00) / 100.0f;
     {
      Serial.print("Stats Today PV Voltage MAX: ");
      Serial.println(stats_today_pv_volt_max);
    }

   
  } else {
    rs485DataReceived = false;
  }
}

void loop()
{
 
  timer.run();
}
