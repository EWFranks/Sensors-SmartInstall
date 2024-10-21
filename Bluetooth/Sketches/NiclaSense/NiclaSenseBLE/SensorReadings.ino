


void publishTemperature() {

  String temperatureString = String(temperature.value());

  char out[32];
  temperatureString.toCharArray(out, sizeof(out));
  Serial.println(out);

  temperatureCharacteristic.writeValue( out );

}

void publishHumidity() {
  
  uint8_t humidityValue = humidity.value() + 0.5f;  //since we are truncating the float type to a uint8_t, we want to round it
  String humidityString = String(humidityValue);

  char out[32];
  humidityString.toCharArray(out, sizeof(out));
  Serial.println(out);

  humidityCharacteristic.writeValue( out );

}

void publishPressure() {
    
    String pressureString = String(pressure.value());
  
    char out[32];
    pressureString.toCharArray(out, sizeof(out));
    Serial.println(out);
  
    pressureCharacteristic.writeValue( out );
  
}

// String readBsec(BLEDevice central, BLECharacteristic characteristic){
//   float airQuality = float(bsec.iaq());
//   return(String(airQuality));
// }

void publishBsec() {
    
    String bsecString = String(bsec.iaq());
    
    char out[32];
    bsecString.toCharArray(out, sizeof(out));
    Serial.println(out);
    
    bsecCharacteristic.writeValue( out );
    
}

// String readCo2(BLEDevice central, BLECharacteristic characteristic){
//   uint32_t co2 = bsec.co2_eq();
//   return(String(co2));
// }

void publishCo2() {
      
      String co2String = String(bsec.co2_eq());
      
      char out[32];
      co2String.toCharArray(out, sizeof(out));
      Serial.println(out);
      
      co2Characteristic.writeValue( out );
      
}

// String readGas(BLEDevice central, BLECharacteristic characteristic){
//   unsigned int g = gas.value();
//   return(String(g));
// }

void publishGas() {
        
  String gasString = String(gas.value());

  char out[32];
  gasString.toCharArray(out, sizeof(out));
  Serial.println(out);

  gasCharacteristic.writeValue( out );
        
}


void publishGyroscope() {
  
  String gyroscopeString = "";

  gyroscopeString += String(gyroscope.x()) + "_";
  gyroscopeString += String(gyroscope.y()) + "_";
  gyroscopeString += String(gyroscope.z());

  char out[32];
  gyroscopeString.toCharArray(out, sizeof(out));
  Serial.println(out);

  gyroscopeCharacteristic.writeValue( out );

}


void publishAccelerometer() {
    
    String accelerometerString = "";
  
    accelerometerString += String(accelerometer.x()) + "_";
    accelerometerString += String(accelerometer.y()) + "_";
    accelerometerString += String(accelerometer.z());
  
    char out[32];
    accelerometerString.toCharArray(out, sizeof(out));
    Serial.println(out);

    accelerometerCharacteristic.writeValue( out );

}


void publishQuaternion() {
  
  String quaternionString = "";

  quaternionString += String(quaternion.x()) + "_";
  quaternionString += String(quaternion.y()) + "_";
  quaternionString += String(quaternion.z()) + "_";
  quaternionString += String(quaternion.w());

  char out[32];
  quaternionString.toCharArray(out, sizeof(out));
  Serial.println(out);

  quaternionCharacteristic.writeValue( out );

}