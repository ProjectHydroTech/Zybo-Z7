# Generating SD card image

This notebook will demo how to generate SD card image for the Zybo Z7-20

Please remember to set the boot mode to SD card if you wish to boot up the Zybo Z7-20 with your SD card image.

Before proceeding on with this notebook, please be sure to stand by a SD card that has been formatted. If not you may go ahead to reformat the SD card. (Caution! This will clear all data in your SD card.) https://sd-memory-card-formatter.en.softonic.com/download?utm_source=SEM&utm_medium=paid&utm_campaign=EN_UK_DSA&gclid=CjwKCAiA9aKQBhBREiwAyGP5lb7nmkmznUxZHMG-uG2vtOPUnhzdi3uHJnaWlrbNMnsAXlk2QinGchoC2lsQAvD_BwE

## Steps

<b>Step 1.</b> Select Zynq Architecture and Import from existing BIF file, navigate to bif file under the default workspace for your Vitis IDE projects, copy the location of the file and use it for both Import and Output BIF file path.

e.g. C:/Users/User/Workspace/HydroTechSP_system/_ide/bootimage/HydroTechSP_system.bif

![image](https://user-images.githubusercontent.com/92703672/153809625-f38b55a1-0252-48ea-83b5-72a5b843ed64.png)

<b>Step 2.</b> Output format set at BIN and choose output path of your boot image, or you could use the default bootimage folder in your default Vitis workspace. 
e.g. C:/Users/User/Workspace/HydroTechSP_system/_ide/bootimage/BOOT.bin

<b>Step 3.</b> Add your wrapper partition and select bootloader partition type and select the filepath that contains the fsbl.elf file. e.g. C:/Users/User/Workspace/HydroTech_wrapper/export/HydroTech_wrapper/sw/HydroTech_wrapper/boot/fsbl.elf

![image](https://user-images.githubusercontent.com/92703672/153811042-cbef8978-cc02-4614-adce-406a8c439fea.png)

![image](https://user-images.githubusercontent.com/92703672/153810774-8de663e4-8f49-4e24-b8b2-cfa74091313e.png)

<b>Step 4.</b> Add your bitstream partition and select datafile partition type and select the filepath that contains your wrapper.bit file. e.g. C:/Users/User/Workspace/HydroTechSP/_ide/bitstream/HydroTech_wrapper.bit

![image](https://user-images.githubusercontent.com/92703672/153810739-17b5c7b6-7bf7-4ee0-8c66-8ea358fe7f7c.png)

<b>Step 5.</b> Add your debug partition and select create another datafile partition type and select the filepath that contains your elf file. e.g. C:/Users/User/Workspace/HydroTechSP/Debug/HydroTechSP.elf

![image](https://user-images.githubusercontent.com/92703672/153810909-f6d3b6c8-2b06-49f4-9942-703da1f36d62.png)

<b>Step 6.</b> You should have 3 different boot image partitions.

![image](https://user-images.githubusercontent.com/92703672/153811193-e0aa4a4d-b2aa-4b99-92f4-85989ca7b6ad.png)

<b>Step 7.</b> Next you may proceed to Create your SD card Image 

![image](https://user-images.githubusercontent.com/92703672/153811359-7ab5593b-5c26-4833-9459-09c7cd62e77c.png)

<b>Step 8.</b> Head to the output path folder and copy these 3 files. e.g. C:/Users/User/Workspace/HydroTechSP_system/_ide/bootimage

![image](https://user-images.githubusercontent.com/92703672/153811506-f4a7634e-7a15-454e-8629-ae23310e3fcb.png)

<b>Step 9.</b> Paste these 3 files in your SD card and you are ready to boot up the Zybo Z7-20 with your own SD card image.
