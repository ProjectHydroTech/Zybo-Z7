# Zybo Z7-20 walkthrough

This notebook will provide a simple walkthrough on how to start up your own indoor vertical farming project.

Head to the Misc folder for some extra information.

This example will be using the Zybo Z7-20 along with the Pmod ESP32 (Port JA), Pmod ALS (Port JB Top row), Pmod HYGRO (Port JC Top row), Pmod AQS (Port JD Bottom row) and
Pmod AD1 (Port JD Top row).

You will be required to go through Step 1-3 to be able to generate your source file.

Required hardware for demo:

1x Zybo Z7-20

1x Pmod ESP32

1x Pmod ALS

1x Pmod HYGRO

1x Pmod AQS

1x Pmod AD1

1x 5v power supply

1x MicroUSB cable

![WhatsApp Image 2022-02-14 at 1 00 00 PM](https://user-images.githubusercontent.com/92703672/153803215-c0f0dd82-449e-4855-acdc-ffa27a36e241.jpeg)

## Steps

<b>Step 1.</b> Install all the necessary softwares https://github.com/ProjectHydroTech/Zybo-Z7-20/tree/main/Installation

<b>Step 2.</b> Go through the Vivado notebook https://github.com/ProjectHydroTech/Zybo-Z7-20/tree/main/Vivado

<b>Step 3.</b> Go through the Vitis IDE notebook https://github.com/ProjectHydroTech/Zybo-Z7-20/tree/main/Vitis

<b>Step 4.</b> Once you have your source file ready, go ahead and build project.

![image](https://user-images.githubusercontent.com/92703672/153803776-37d66faa-030d-4827-a276-ce1c624b4dec.png)

<b>Step 5.</b> This should appear in the console after bulding.

![image](https://user-images.githubusercontent.com/92703672/153804278-53ae8941-d967-4020-9b20-4e6fad41600b.png)

<b>Step 6.</b> Plug in 5v power supply to the Zybo Z7-20, then set the jumper for power mode to WALL and jumper for the boot up as JTAG. (Shown in image above)

<b>Step 7.</b> Turn ON the board and connect the microUSB from the Zybo Z7-20 to your laptop.

<b>Step 8.</b> Next you are ready to boot up the Zybo Z7-20 with your code.

![image](https://user-images.githubusercontent.com/92703672/153804886-ba0be7ce-c48e-425c-b79c-767c869a5434.png)

<b>Step 9.</b> If the boot up was successful, the green DONE LED will light up.

![WhatsApp Image 2022-02-14 at 1 24 22 PM](https://user-images.githubusercontent.com/92703672/153805274-9a60eb0b-ea18-4490-b594-734cf2b1dd18.jpeg)

<b>Step 10.</b> To see the serial interface, open up Tera Term and select serial.

![image](https://user-images.githubusercontent.com/92703672/153805375-d0e7cb95-30c4-4a26-8150-11246ac81537.png)

<b>Step 11.</b> Navigate to Setup > Serial Port...

![Screenshot 2022-02-14 132825](https://user-images.githubusercontent.com/92703672/153805527-2b015923-036a-4d5f-82ec-2a6b2b5d1796.png)

<b>Step 12.</b> Change the baud rate to 115200 which is the standard baud rate for the Zybo Z7-20 and click on New Setting.

![image](https://user-images.githubusercontent.com/92703672/153805593-fc393337-0f45-49b2-bc1a-c5a022fd6504.png)

<b>Step 13.</b> Your serial terminal should look like this if boot up was successful.

![image](https://user-images.githubusercontent.com/92703672/153805848-4c32262b-6ce9-44fb-8a67-6508d6eb3d1e.png)

<b>Step 14.</b> Finished!!!

![Screenshot 2022-02-04 123401](https://user-images.githubusercontent.com/92703672/152496084-fa89e1fa-c3e7-428c-97dc-c77c50f59f6f.png)
