// change the file.h over here with your own model from edge impulse. Open the arduino file from edgeimpulse, copy the .h name file and paste it here
#include <REPLACE WITH YOUR EDGE IMPLUSE MODEL.h>
#include <eloquent_esp32cam.h>
#include <eloquent_esp32cam/edgeimpulse/fomo.h>
#include <FS.h>
#include <SPI.h>
#include <eloquent_esp32cam/extra/esp32/fs/sdmmc.h>


    
using eloq::camera;
using eloq::ei::fomo;



void setup() {
    delay(3000);
    Serial.begin(115200);
    Serial.println("__EDGE IMPULSE FOMO EVENT DRIVEN_");

    // Camera settings from the first code
    camera.pinout.aithinker();
    camera.brownout.disable();
    camera.resolution.hvga();
    camera.quality.high();

  // you can configure each pin of SDMMC (if needed)
  // (delete these lines if you are not sure)
    sdmmc.pinout.clk(01);
    sdmmc.pinout.cmd(02);
    sdmmc.pinout.d0(03);
    sdmmc.pinout.d1(04);
    sdmmc.pinout.d2(05);
    sdmmc.pinout.d3(06);

    // Initialize camera
    while (!camera.begin().isOk()) {
        Serial.println(camera.exception.toString());
    }
    Serial.println("Camera OK");

    // Initialize SD card using the eloquent_esp32cam SDMMC library
    while (!sdmmc.begin().isOk()) {
        Serial.println(sdmmc.exception.toString());
    }
    Serial.println("SD card OK");

    // Run function when no object is detected
    fomo.daemon.whenYouDontSeeAnything([]() {
        Serial.println("No Object Detected");
    });

    // Run function when you see any object
    fomo.daemon.whenYouSeeAny([](int i, bbox_t bbox) {
        Serial.printf(
            "#%d) Found %s at (x = %d, y = %d) (size %d x %d). Proba is %.2f\n",
            i + 1,
            bbox.label,
            bbox.x,
            bbox.y,
            bbox.width,
            bbox.height,
            bbox.proba
        );

        // Save the captured image to SD card with an incremental filename
        static unsigned int fileCount = 0;
        String filename = "/picture_" + String(fileCount++) + ".jpg";
        if (sdmmc.save(camera.frame).to("","jpg").isOk()) {
            Serial.print("File written to ");
            Serial.println(filename);
        } else {
            Serial.println(sdmmc.session.exception.toString());
        }
    });

    // Start daemon
    fomo.daemon.start();
}

void loop() {
    // Empty loop
}
