# Zybo Z7-20 software download instructions
The set of instructions below provides a complete walkthrough of the software that is required to be downloaded before being able to automate your very own vertical farming rack using Vitis IDE and Vivado.

This notebook will specifically be using Xilinx Vitis IDE 2019.2 and Vivado 2019.2 as an example, please make sure that the version of the Vitis IDE and Vivado is matching otherwise there may be many software version errors in the near future. (Other versions of Vitis IDE and Vivado has not been tested) 

(Try at your own risk) If you already have other versions of Vivado/Vitis IDE downloaded on your computer and do not wish to uninstall those, please take to have the same version for both Vitis IDE and Vivado.

However, if you wish to follow step by step of this walkthrough, uninstalling those version through either the control panel or the uninstall.exe for Xilinx first would be advised.

Note that the required hard disk space is 100 GB, although the actual disk space taken for Vitis IDE and Vivado is only around 30-40 GB.

![zybo-1](https://user-images.githubusercontent.com/92703672/152474145-e952bbc7-79f2-438c-bbbd-bf88e2810784.png)

## Steps
<b>Step 1.</b> Click this link and fill in your Xilinx account details to download the Xilinx Unified 2019.2 installer and run the setup once it is done downloading:   https://www.xilinx.com/member/forms/download/xef.html?filename=Xilinx_Unified_2019.2_1106_2127_Win64.exe

<b>Step 2.</b> Installer will prompt you to get the latest version of of the Xilinx Design Tools ignore this, click continue and next.

![Screenshot 2022-02-04 131741](https://user-images.githubusercontent.com/92703672/152480266-c5e59b91-bcba-4f24-aa1b-35e931e4857d.png)

<b>Step 3.</b> Fill in Xilinx account details then check the Download and install now and click next.

![Screenshot 2022-02-04 140409](https://user-images.githubusercontent.com/92703672/152480649-0b276c4c-669e-4cd2-9d64-6ee474175e18.png)

<b>Step 4.</b> Accept the lisence agreements and click next.

![Screenshot 2022-02-04 141024](https://user-images.githubusercontent.com/92703672/152481302-65cc8277-d695-4d6f-ad26-5dced022cea7.png)

<b>Step 5.</b> Check Vitis and click next. (We will be installing Vitis IDE first, then followed by Vivado next in step 9)

![Screenshot 2022-02-04 141848](https://user-images.githubusercontent.com/92703672/152481873-3ae9a89b-8878-4e3a-9a8b-60a7c07ad32d.png)

<b>Step 6.</b> Make sure to have all this checked below and click next

![Screenshot 2022-02-04 142036](https://user-images.githubusercontent.com/92703672/152482019-3b0b2514-890b-4ff1-9a8a-84d33c6da089.png)

<b>Step 7.</b> Choose anywhere for your destination directory or just leave it default. Click Next and Yes if prompted. Take note of the directory as we will be adding files manually into the folder later on. 

(You may be required to clear some disk space before downloading, else an error like this will appear)

![Screenshot 2022-02-04 142603](https://user-images.githubusercontent.com/92703672/152482696-ec67d068-d9ff-41af-92d4-39ec72b0b165.png)

<b>Step 8.</b> Proceed with the Vitis installation. (This will take some time)

<b>Step 9.</b> Once done with the Vitis installation, repeat steps 2 - 4 and check Vivado this time, then click next.

![Screenshot 2022-02-04 143109](https://user-images.githubusercontent.com/92703672/152483098-9ae61468-cd1f-4ac8-9920-d3ae9427055d.png)

<b>Step 10.</b> Check the Vivado HL Systems Edition and click next.

![Screenshot 2022-02-04 143325](https://user-images.githubusercontent.com/92703672/152483271-27ea90c3-dde5-4223-88e6-4370677b1bb5.png)

<b>Step 11.</b> Make sure to have all this checked below and click next.

![Screenshot 2022-02-04 143610](https://user-images.githubusercontent.com/92703672/152483588-159f9e3a-73f4-4e40-92a9-e6d5b05907b7.png)

<b>Step 12.</b> Choose the same directory as before or you could just leave it as default if you have done so before. Click Next and Yes if prompted. (Also required to have enough disk space or same error as step 7 before will occur)

<b>Step 13.</b> Proceed with the Vivado installation. (This will take some time)

<b>Step 14.</b> After downloading both Vitis IDE and Vivado, download Vivado boards master which contains the board files for the Zybo Z7-20 and extract it to a folder of your choice:https://github.com/Digilent/vivado-boards/archive/master.zip

<b>Step 15.</b> Move to the location of the extracted file and Navigate to the board files directory, for me it would be under downloads as I extracted it to the same folder e.g. C:\Users\User\Downloads\vivado-boards-master\vivado-boards-master\new\board_files 

![Screenshot 2022-02-04 145437](https://user-images.githubusercontent.com/92703672/152485535-93a7b29c-57c3-4c95-9aad-576ed26d8e4b.png)

<b>Step 16.</b> Copy these 3 files

![image](https://user-images.githubusercontent.com/92703672/152485934-1f08077b-5eda-4885-95a2-c29564797c00.png)

<b>Step 17.</b> Locate the directory where Vitis and Vivado was downloaded into e.g. C:\Xilinx\Vivado\2019.2\data\boards\board_files (Mentioned in step 7 and 12) and paste the 3 files copied into here. Your board files folder should contain the 3 files that were copied from step 16.

![Screenshot 2022-02-04 150522](https://user-images.githubusercontent.com/92703672/152486728-fe129f4b-aa69-4276-87a7-dfe5400689bd.png)

<b>Step 18.</b> Download the Vivado library master which contains the IP for the Pmod sensors and ZYNQ processor that we will be using and extract it:        https://github.com/Digilent/vivado-library/archive/master.zip (Remember the location of this folder as we will be using it in Vivado)

<b>Step 19.</b> Finished!!! :)

![Screenshot 2022-02-04 123401](https://user-images.githubusercontent.com/92703672/152496084-fa89e1fa-c3e7-428c-97dc-c77c50f59f6f.png)
