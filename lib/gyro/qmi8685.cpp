
#include "qmi8685.h"

#include <QMI8658.h>
QMI8658 imu;
#define IMU_QMI8658_ADDR      0x6B
// Motion detection variables
float accelThreshold = 2.0;  // m/s² threshold for motion detection
bool motionDetected = false;
unsigned long lastMotionTime = 0;

// Calibration variables
float accelOffsetX = 0, accelOffsetY = 0, accelOffsetZ = 0;
float gyroOffsetX = 0, gyroOffsetY = 0, gyroOffsetZ = 0;
bool calibrated = false;

// Filter variables (simple low-pass filter)
float filteredAccelX = 0, filteredAccelY = 0, filteredAccelZ = 0;
float filteredGyroX = 0, filteredGyroY = 0, filteredGyroZ = 0;
const float alpha = 0.1; // Filter coefficient (0-1, lower = more filtering)

ges_data_t ges_data_8685;

void performCalibration() {
    Serial.println(" Starting calibration...");
    Serial.println("Place the sensor on a flat, stable surface.");
    Serial.println("Calibration will start in 3 seconds...");
    
    // for (int i = 3; i > 0; i--) {
    //     Serial.print(i);
    //     Serial.println("...");
    //     delay(1000);
    // }
    
    Serial.println("📈 Collecting calibration data...");
    
    const int numSamples = 1000;
    float accelSumX = 0, accelSumY = 0, accelSumZ = 0;
    float gyroSumX = 0, gyroSumY = 0, gyroSumZ = 0;
    int validSamples = 0;
    
    for (int i = 0; i < numSamples; i++) {
        QMI8658_Data data;
        if (imu.readSensorData(data)) {
            accelSumX += data.accelX;
            accelSumY += data.accelY;
            accelSumZ += data.accelZ;
            
            gyroSumX += data.gyroX;
            gyroSumY += data.gyroY;
            gyroSumZ += data.gyroZ;
            
            validSamples++;
        }
        
        if (i % 100 == 0) {
            Serial.print(".");
        }
        
        delay(10);
    }
    
    if (validSamples > 0) {
        // Calculate offsets
        accelOffsetX = accelSumX / validSamples;
        accelOffsetY = accelSumY / validSamples;
        accelOffsetZ = (accelSumZ / validSamples) - 9.81; // Remove gravity
        
        gyroOffsetX = gyroSumX / validSamples;
        gyroOffsetY = gyroSumY / validSamples;
        gyroOffsetZ = gyroSumZ / validSamples;
        
        calibrated = true;
        
        Serial.println("\n✅ Calibration complete!");
        Serial.print("Accel offsets: ");
        Serial.print(accelOffsetX, 3); Serial.print(", ");
        Serial.print(accelOffsetY, 3); Serial.print(", ");
        Serial.println(accelOffsetZ, 3);
        
        Serial.print("Gyro offsets: ");
        Serial.print(gyroOffsetX, 3); Serial.print(", ");
        Serial.print(gyroOffsetY, 3); Serial.print(", ");
        Serial.println(gyroOffsetZ, 3);
        
        Serial.println("💡 Type 'cal' to recalibrate anytime.");
    } else {
        Serial.println("\n Calibration failed! No valid samples collected.");
    }
}


void calculateOrientation(float ax, float ay, float az, 
                         float gx, float gy, float gz,
                         float &roll, float &pitch, float &yaw) {
    roll = atan2(ay, sqrt(ax * ax + az * az)) * 180.0 / M_PI;

    // roll = atan2(ax, az) * 180.0 / M_PI;
    pitch = atan2(-ax, sqrt(ay * ay + az * az)) * 180.0 / M_PI;
    pitch=-pitch;


    float roll_180;
    if (az < 0) { // Z轴向下（设备翻转超过90°）
        // 翻转后：180° - 基础角  或  -180° - 基础角（根据ax符号）
        roll_180 = (ax > 0) ? (180.0 - roll) : (-180.0 - roll);
    } else { // Z轴向上（正常范围，±90°）
        roll_180 = roll;
    }
    // 3. 角度归一化（防止超出±180，比如190°→-170°）
    roll_180 = fmod(roll_180 + 180.0, 360.0) - 180.0;
    roll = roll_180;

    // int16_t roll_af = roll_180;
    // if(roll_180<0){
    //     roll_af = -(roll_af+roll);
    // }
    // if(roll>0){
    //     roll_af =(180-roll);
    // }
    // roll = roll_af;


    
    if(CALI_PITCH_90 == 1){
        float pitch_180;
        if (az < 0) { // Z轴向下（设备俯仰翻转超过90°）
            // 翻转后：180° - 基础角 或 -180° - 基础角（根据ay符号判断方向）
            pitch_180 = (ay > 0) ? (180.0 - pitch) : (-180.0 - pitch);
        } else { // Z轴向上（正常范围，±90°）
            pitch_180 = pitch;
        }
        // 3. 角度归一化（强制限制在-180° ~ +180°，避免跳变）
        pitch_180 = fmod(pitch_180 + 180.0, 360.0) - 180.0;
        pitch = pitch_180;
        if(pitch>0){
            pitch = 180-pitch; 
        }
        if(pitch < 0){
            pitch = -(pitch+180);
        }
        float p;
        if(pitch>0){
            p = pitch -90;
        }
        if(pitch<0){
            p = -(pitch+90);
        }
        pitch = p;
        

    }
    


    
    // Simple yaw integration from gyroscope (not accurate for long term)
    static float yawIntegrated = 0;
    static unsigned long lastTime = 0;
    
    unsigned long currentTime = millis();
    if (lastTime > 0) {
        float dt = (currentTime - lastTime) / 1000.0; // Convert to seconds
        yawIntegrated += gz * dt;
    }
    lastTime = currentTime;
    
    yaw = yawIntegrated;
    
    // Keep yaw in range [-180, 180]
    while (yaw > 180) yaw -= 360;
    while (yaw < -180) yaw += 360;

    if(yaw<0){
        yaw = 360-abs(yaw);
    }
    
}

void checkMotion(float accelMag, float gyroMag) {
    // Check if current acceleration deviates significantly from gravity
    float accelDeviation = abs(accelMag - 9.81);
    
    if (accelDeviation > accelThreshold || gyroMag > 10.0) {
        if (!motionDetected) {
            Serial.println("\n🚶 Motion detected!");
        }
        motionDetected = true;
        lastMotionTime = millis();
    } else {
        // No motion for 2 seconds
        if (motionDetected && (millis() - lastMotionTime > 2000)) {
            Serial.println("🛑 Motion stopped.");
            motionDetected = false;
        }
    }
}

void qmi8685_init(){

    bool success = imu.begin(15,7, IMU_QMI8658_ADDR);
    
    if (!success) {
        Serial.println("❌ Failed to initialize QMI8658!");
        Serial.println("Please check:");
        // while (1) {
        //     Serial.println("⏳ Retrying in 5 seconds...");
        //     delay(5000);
        // }
    }

    imu.setAccelRange(QMI8658_ACCEL_RANGE_8G);
    imu.setAccelODR(QMI8658_ACCEL_ODR_8000HZ);
    imu.setGyroRange(QMI8658_GYRO_RANGE_512DPS);
    imu.setGyroODR(QMI8658_GYRO_ODR_8000HZ);
    imu.setAccelUnit_mg(true);      // Use mg (like your screen: ACC_X = -965.82)
    imu.setGyroUnit_dps(true);      // Use dps (degrees per second)
    imu.setDisplayPrecision(2);     // 6 decimal places (like your screen)

    imu.enableSensors(QMI8658_ENABLE_ACCEL | QMI8658_ENABLE_GYRO);
    // performCalibration();

    delay(1000);
}

ges_data_t receive_parse_qmi8685(){

    QMI8658_Data data;

    // gfx->fillScreen(BLACK);
    // gfx->setCursor(100, 100);
    // gfx->setTextSize(4);

    if (imu.readSensorData(data)) {
        // Apply calibration offsets
        float accelX = data.accelX - accelOffsetX;
        float accelY = data.accelY - accelOffsetY;
        float accelZ = data.accelZ - accelOffsetZ;
        
        float gyroX = data.gyroX - gyroOffsetX;
        float gyroY = data.gyroY - gyroOffsetY;
        float gyroZ = data.gyroZ - gyroOffsetZ;
        
        // Apply low-pass filter
        filteredAccelX = alpha * accelX + (1 - alpha) * filteredAccelX;
        filteredAccelY = alpha * accelY + (1 - alpha) * filteredAccelY;
        filteredAccelZ = alpha * accelZ + (1 - alpha) * filteredAccelZ;
        
        filteredGyroX = alpha * gyroX + (1 - alpha) * filteredGyroX;
        filteredGyroY = alpha * gyroY + (1 - alpha) * filteredGyroY;
        filteredGyroZ = alpha * gyroZ + (1 - alpha) * filteredGyroZ;
        
        // Calculate orientation (roll, pitch, yaw)
        float roll, pitch, yaw;
        calculateOrientation(filteredAccelX, filteredAccelY, filteredAccelZ,
                           filteredGyroX, filteredGyroY, filteredGyroZ,
                           roll, pitch, yaw);
        
        // Motion detection
        float accelMagnitude = sqrt(accelX*accelX + accelY*accelY + accelZ*accelZ);
        float gyroMagnitude = sqrt(gyroX*gyroX + gyroY*gyroY + gyroZ*gyroZ);
        
        checkMotion(accelMagnitude, gyroMagnitude);
        
 
        Serial.print(motionDetected ? "YES" : "NO");

        ges_data_8685.roll = roll;
        ges_data_8685.pitch = pitch;
        ges_data_8685.yaw = yaw;
        // Serial.print(roll, 1);
        // Serial.print(pitch, 1);
        // Serial.print(yaw, 1);


        // gfx->setCursor(100, 10);
        // gfx->fillRect(100, 0, 200, 50, BLACK);
        // gfx->println(String(roll) );

        // gfx->setCursor(100, 100);
        // gfx->fillRect(100, 90, 200, 50, BLACK);
        // gfx->println(String(pitch));

        // gfx->setCursor(100, 200);
        // gfx->fillRect(100, 190, 200, 50, BLACK);
        // gfx->println(String(yaw));
        return ges_data_8685;
    }
    return ges_data_8685;
}