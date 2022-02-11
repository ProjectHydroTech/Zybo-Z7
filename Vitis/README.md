# Vitis IDE walkthrough

The set of instructions below provides a simple startup walkthrough of the Vitis IDE software, to be able to start programming and customizing your very own code.
This notebook will be using Xilinx Vitis IDE 2019.2 as an example.

Please note to go through the Installation Guide before reading this notebook.
(https://github.com/ProjectHydroTech/Zybo-Z7/blob/main/Installation%20Guide.md)

![image](https://user-images.githubusercontent.com/92703672/153341220-35883863-b092-4f38-bf83-f824806a4169.png)

## Steps

<b>Step 1.</b> Search Xilinx Vitis IDE in your search bar and run it.

![Screenshot 2022-02-10 133322](https://user-images.githubusercontent.com/92703672/153344313-1e99c713-7c49-4c20-ad79-bfaf90df610e.png)

<b>Step 2.</b> This is the Start Up screen for new users in Vitis IDE. Click on Create Application project under PROJECT.

![Screenshot 2022-02-10 133915](https://user-images.githubusercontent.com/92703672/153344616-95f52349-c1b9-4776-96de-31fe16c0d174.png)

<b>Step 3.</b> You should see this after clicking create Application project.

![Screenshot 2022-02-10 134115](https://user-images.githubusercontent.com/92703672/153344820-2af71bab-3522-428c-8358-e6d976bdf812.png)

<b>Step 4.</b> Fill in your project name and System project should be filled in automatically.
In this example we will be naming our project HydroTech.

![Screenshot 2022-02-10 134630](https://user-images.githubusercontent.com/92703672/153345442-431c087c-8b89-4e35-b892-26389a9db6f8.png)

<b>Step 5.</b> Click on Select a platform from repository in the above tab.

![image](https://user-images.githubusercontent.com/92703672/153346584-c189d982-cfe1-4447-90d3-7887b6ee0b2b.png)

<b>Step 6.</b> Select xcvc1902_fixed and click next.

![Screenshot 2022-02-10 135037](https://user-images.githubusercontent.com/92703672/153346260-a003f7a3-014a-471c-88e6-75a61f722940.png)

<b>Step 7.</b> Next click on Create a new platform from hardware (XSA) in the above tab to select the wrapper.xsa file generated from Vivado. 

![image](https://user-images.githubusercontent.com/92703672/153348130-5818562f-e22f-4f91-887f-64c1cf49a614.png)

<b>Step 8.</b> We will be selecting HydroTech_wrapper for this example ( Select the wrapper that you generated from Vivado) , click next after selecting wrapper.

![Screenshot 2022-02-10 144856](https://user-images.githubusercontent.com/92703672/153353167-8367ec98-a3d7-4af7-89bd-60ba2e0cc88e.png)

<b>Step 9.</b> Select ps7_cortexa9_0 as your CPU, standalone OS and C++ for language and make sure to check generate boot components, then click next.

![Screenshot 2022-02-10 145238](https://user-images.githubusercontent.com/92703672/153353442-ff824b66-1026-4d69-9825-2211b11f6985.png)

<b>Step 10.</b> Choose empty application and hit finish.

![Screenshot 2022-02-10 145527](https://user-images.githubusercontent.com/92703672/153353680-71cfdfc3-dacc-4596-a990-9b1faafa10f6.png)


