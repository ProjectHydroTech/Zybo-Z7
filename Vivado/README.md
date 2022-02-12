# Vivado walkthrough

The set of instructions below provides a simple startup walkthrough of the Vivado Software, to be able to start customizing your very own block diagram.
This notebook will be using Vivado 2019.2 with the Zybo Z7-20 as an example.

Example of the Wrapper, constraint files and xsa file has already been provided. You can download the wrapper.xsa file here and skip to step 52 if you do not wish to generate your own wrapper.xsa file and import it straight to Vitis IDE to start programming. https://github.com/ProjectHydroTech/Zybo-Z7/blob/main/Vivado/HydroTech_wrapper.xsa

Please note to go through the Installation Guide before reading this notebook. https://github.com/ProjectHydroTech/Zybo-Z7-20/blob/main/Installation/Installation%20Guide.md

![image](https://user-images.githubusercontent.com/92703672/153527757-0fe820e0-6294-40d0-8b32-bf39c5c1b162.png)

![WhatsApp Image 2022-02-11 at 11 43 43 AM](https://user-images.githubusercontent.com/92703672/153534563-1e064a1d-674a-4b50-9a84-0efaea574ef3.jpeg)


<b>Step 1.</b> Search Vivado 2019.2 in your search bar and run it.

![Screenshot 2022-02-11 101514](https://user-images.githubusercontent.com/92703672/153527965-0f3ebf8e-0921-46c5-9af5-bcbf1d29344a.png)

<b>Step 2.</b> This is the Start Up screen for new users in Vivado. Click on Create project under Quick Start.

![Screenshot 2022-02-11 101652](https://user-images.githubusercontent.com/92703672/153528059-5d4dd0c9-f48f-473d-b855-4cb0317e11c4.png)

<b>Step 3.</b> You should see this after clicking create project, click next.

![Screenshot 2022-02-11 101903](https://user-images.githubusercontent.com/92703672/153528274-37b42f61-3796-4a8e-a253-6fb046afb8c6.png)

<b>Step 4.</b> Fill in your project name and project location and check the create project subdirectory box, click next.
( Remember the location of this folder, as we will be needing it later )

![Screenshot 2022-02-11 102239](https://user-images.githubusercontent.com/92703672/153528425-7c181291-0cc8-4676-86ec-b6c919ac7015.png)

<b>Step 5.</b> Check RTL Project and DO not specify sources at this time, then click next. ( We will be manually specifying sources later )

![Screenshot 2022-02-11 102715](https://user-images.githubusercontent.com/92703672/153528720-5ad4a0d5-f060-42bc-a7a5-3d0e211a3ee2.png)

<b>Step 6.</b> After arriving at this page, click on boards at the top left corner to specify the board that we will be using.

![Screenshot 2022-02-11 103105](https://user-images.githubusercontent.com/92703672/153528987-043a16b7-951f-496d-bd2f-42a8118de921.png)

<b>Step 7.</b> If board files were added correctly from the installation guide, search for zybo and there should be 3 matches, select Zybo Z7-20 and click next.

![Screenshot 2022-02-11 103255](https://user-images.githubusercontent.com/92703672/153529243-5a87a5d8-aa28-4a89-a980-4305d5c88b13.png)

<b>Step 8.</b> Click finish to create the project.

![Screenshot 2022-02-11 103623](https://user-images.githubusercontent.com/92703672/153529357-8c095cd1-287c-4a2d-9eab-56c5794a8739.png)

<b>Step 9.</b> This will be the default screen after creating a new project.

![Screenshot 2022-02-11 103911](https://user-images.githubusercontent.com/92703672/153529570-2135bd8a-09a3-46a4-be4c-2b07b1d00d36.png)

<b>Step 10.</b> Navigate to the settings under project manager at the left panel and click on Repository under the IP section.

![Screenshot 2022-02-11 104354](https://user-images.githubusercontent.com/92703672/153529954-73a3ccd2-f4e0-4280-a815-82230db9f5cc.png)

<b>Step 11.</b> Click on the + sign to add files and select the vivado library master file mentioned in step 18 of the Installation guide linked above.
There is another vivado library master file inside the vivado library master file, select the file that contains the if and ip files shown in the image below.

![Screenshot 2022-02-11 104709](https://user-images.githubusercontent.com/92703672/153530193-2b9d5f2f-e4b8-4ab6-b53b-75c91e8c9e06.png)

<b>Step 12.</b> You should see this after selecting the correct file, click OK.

![Screenshot 2022-02-11 105105](https://user-images.githubusercontent.com/92703672/153530525-91d3ec43-4dee-4162-90af-a48762e3d066.png)

<b>Step 13.</b> After adding the right repository, make sure to apply and click ok to save changes.

![Screenshot 2022-02-11 105313](https://user-images.githubusercontent.com/92703672/153530666-660756c1-bc88-4625-8883-69f184a463f4.png)

<b>Step 14.</b> Download the zip constraint files for the Zybo Z7-20 here. ( https://github.com/Digilent/digilent-xdc )

![Screenshot 2022-02-11 110109](https://user-images.githubusercontent.com/92703672/153531343-6008c87f-b9f5-4305-8f88-995431f0138c.png)

<b>Step 15.</b> Click on the green button at the top right, download zip file and extract it. ( Remember the location of this file as we will be needing it )

![Screenshot 2022-02-11 110440](https://user-images.githubusercontent.com/92703672/153531698-14e61781-4966-4611-a480-28a93a35f611.png)

<b>Step 16.</b> Navigate to Add Sources under project manager and select add or create constraints and click next.

![Screenshot 2022-02-11 110540](https://user-images.githubusercontent.com/92703672/153531753-94492aba-e167-442a-972b-27ccb3af1f15.png)

<b>Step 17.</b> Click on add files and navigate to the location of the extracted constraint files and select Zybo-Z7-Master.xdc, click ok and finish.

![Screenshot 2022-02-11 110838](https://user-images.githubusercontent.com/92703672/153532037-169f1e1d-1d8e-4c40-b132-5c133b12085e.png)

<b>Step 18.</b> If constraint files have been added, you should be able to view it under sources.

![image](https://user-images.githubusercontent.com/92703672/153532119-9278f168-efeb-4ca7-bf1c-81800b83f18f.png)

<b>Step 19.</b> Click on create block diagram under IP INTEGRATOR at the left panel and fill in your design name, click OK.

![Screenshot 2022-02-11 111202](https://user-images.githubusercontent.com/92703672/153532211-ef93a2fa-7ecb-4251-88ee-310ce185dc82.png)

<b>Step 20.</b> Click on the + sign to start creating your block diagram and add all the required IPs. 
We will be using the ZYNQ processing system, 1x PMOD AD1, 1x PMOD ALS, 1x PMOD ESP32, 1x PMOD HYGRO, 1x PMOD AQS as an example.

![image](https://user-images.githubusercontent.com/92703672/153535174-0090e5dc-edc8-4a1b-97e2-b4d89847cd4d.png)

![image](https://user-images.githubusercontent.com/92703672/153534827-14d7d113-a14f-4163-8b07-8a02383f6b12.png)

![image](https://user-images.githubusercontent.com/92703672/153534964-cd2f0c00-473f-4cd2-a87c-0c415d1b92e0.png)

![image](https://user-images.githubusercontent.com/92703672/153534979-fc773a80-a008-4d53-a806-ede0b63c60e4.png)

![image](https://user-images.githubusercontent.com/92703672/153535143-cf320fed-5501-4d1d-8665-e188801244de.png)

![image](https://user-images.githubusercontent.com/92703672/153535406-ef5cb171-eb1e-4de6-a400-807a5c1b1f69.png)

<b>Step 21.</b> After adding all the IPs, regenerate the layout (refresh button) and there should be 6 blocks.

![Screenshot 2022-02-11 115751](https://user-images.githubusercontent.com/92703672/153535489-011c4573-2bab-434a-a9a5-1761c2c7ffb5.png)

<b>Step 22.</b> Double click the ZYNQ7 processing system and navigate to Clock configuration, then select PL Fabric Clocks

![Screenshot 2022-02-11 145214](https://user-images.githubusercontent.com/92703672/153548812-268e6430-bcb2-4fb9-ab02-37048b6e5245.png)

<b>Step 23.</b> Add FCLK_CLK1 and change the requested frequency to 100 MHz, then click OK.

![Screenshot 2022-02-11 145512](https://user-images.githubusercontent.com/92703672/153548942-21899464-fb05-4500-ab23-4e801deaf799.png)

<b>Step 24.</b> Run Block Automation and leave everything as default then click OK.

![image](https://user-images.githubusercontent.com/92703672/153535663-136ce1d0-a4b6-4491-9d99-cb28b1580d83.png)

![Screenshot 2022-02-11 144240](https://user-images.githubusercontent.com/92703672/153547841-2fddbee7-9187-4566-bf7d-2221a180010a.png)

<b>Step 25.</b> Next, Run Connection Automation

![image](https://user-images.githubusercontent.com/92703672/153547946-e50be920-6dbf-4c1a-96c8-0d09f27fda87.png)

<b>Step 26.</b> Select all automation

![Screenshot 2022-02-11 144520](https://user-images.githubusercontent.com/92703672/153548046-7295c9cc-3133-40b1-82ba-90f88535d8a9.png)

<b>Step 27.</b> Change both the clock sources for AXI_LITE_GPIO and AXI_LITE_UART under PmodESP32_0 from Auto to 100MHZ, then click OK to run the connection automation.

![Screenshot 2022-02-11 145741](https://user-images.githubusercontent.com/92703672/153549274-6d4e1181-19b2-4d86-bb4a-2b5e2ec347e8.png)

![Screenshot 2022-02-11 145820](https://user-images.githubusercontent.com/92703672/153549278-6b24ec4a-fa58-4e12-802d-82b0336b2704.png)

<b>Step 28.</b> After running, regenerate your layout again and your block diagram should look like this.

![Screenshot 2022-02-11 150044](https://user-images.githubusercontent.com/92703672/153549615-3752961f-d79c-4d1d-bfb7-6403eade86c0.png)

<b>Step 29.</b> After adding our ZYNQ proccessing system and Pmod sensors, next will be assigning the Pmod sensors to the respective ports on the Zybo. (JB, JC, JD, JE, JF)


<b>Step 30.</b> Click on board to the left of your block diagram and scroll to Pmod.

![image](https://user-images.githubusercontent.com/92703672/153551515-d509af60-55c2-49dd-abee-94b3a300d46f.png)

<b>Step 31.</b> Select Connector JA and select Pmod_Out under Pmod_ESP32 and click OK.

![Screenshot 2022-02-11 152450](https://user-images.githubusercontent.com/92703672/153552110-05ac3f92-18cf-4c1e-82bf-7bece7b8157c.png)

<b>Step 32.</b> PmodESP32 should have an external interface ja.

![image](https://user-images.githubusercontent.com/92703672/153552300-de0c7afd-c573-481b-a1bb-5c9a39e2c455.png)

<b>Step 33.</b> Right click an empty area on your block diagram and click on create interface port to create external interfaces to the Zybo.

![Screenshot 2022-02-11 153513](https://user-images.githubusercontent.com/92703672/153553252-5b3cdda2-2932-4e08-8571-6ca867e7099d.png)

<b>Step 34.</b> We will be creating Pmod interfaces for the Pmod sensors, name your interface and remember this name as we will be using it later ( Caps sensitive ), then make 
sure mode is MASTER, search for Pmod and choose the Pmod interface and click OK.

![Screenshot 2022-02-11 155016](https://user-images.githubusercontent.com/92703672/153554951-fb7f5979-8ecf-486a-86cc-380d34fe1a99.png)

<b>Step 35.</b> Do the same for Pmod HYGRO, Pmod AQS, Pmod AD1. You should see 4 Pmod interfaces in your block diagram.

![image](https://user-images.githubusercontent.com/92703672/153555137-5d0aa4a4-1162-40d3-9e35-6bc943fea9bb.png)

<b>Step 36.</b> Next hover over the interfaces and you should be able to see pen icon, match the interfaces to the respective Pmods by dragging the pen to the sensor block.

![image](https://user-images.githubusercontent.com/92703672/153556112-b51fb87d-c12a-4cc5-8ad4-9f2be9d4f607.png)

<b>Step 37.</b> Run connection automation should appear, run it again and click OK.

![Screenshot 2022-02-11 160658](https://user-images.githubusercontent.com/92703672/153556825-84734e7b-9256-4de8-880d-07546b1cf7e0.png)

Finished block diagram!

![Screenshot 2022-02-12 190459](https://user-images.githubusercontent.com/92703672/153708810-ca0c265f-b8cf-4845-a607-4d72cb6ce34a.png)

<b>Step 38.</b> Go to design sources under Sources, right click your block diagram and create HDL wrapper, select let vivado manage wrapper and auto-update, then click OK.

![Screenshot 2022-02-11 160819](https://user-images.githubusercontent.com/92703672/153556982-c5346782-5c25-452b-aef2-a265683e7c73.png)

<b>Step 39.</b> Some warnings should appear, go ahead and ignore them and click OK.

![image](https://user-images.githubusercontent.com/92703672/153557232-55a2dab9-de27-4dc0-a9e0-921ad6d5a33c.png)

<b>Step 40.</b> After generating wrapper for your block diagram, double click your wrapper under design sources to open it up, you should see your wrapper.v file, we will be editing some things in here to avoid some potential errors when generating bitstream.

![Screenshot 2022-02-11 161237](https://user-images.githubusercontent.com/92703672/153557472-d3d67bbe-b65f-4f2f-85a7-f4b4c1750424.png)

<b>Step 41.</b> The step here may require you to troubleshoot by a few times by yourself. Scroll down the wrapper and look for all the Pmods ONLY and delete lines that contain
pin7, pin8, pin9 and pin10. For example this section with the Pmod AD1.

![image](https://user-images.githubusercontent.com/92703672/153559070-3b5b405f-6d35-4d39-946e-77897ed25810.png)

<b>Step 42.</b> After deleting should look like this. Do it for all the Pmods ONLY. ( See step 43 too )

![image](https://user-images.githubusercontent.com/92703672/153559274-f0e414d0-28f7-4d3c-9487-78dc2d247dfa.png)

<b>Step 43.</b> Go ahead and click save when you are done editing the wrapper. Then open up Zybo-Z7-Master.xdc under constraints in Sources.

![Screenshot 2022-02-11 171129](https://user-images.githubusercontent.com/92703672/153564801-5164315c-1298-4725-a345-51be4860d53a.png)

<b>Step 44.</b> Select everything from Pmod header JB to Pmod header JD and uncomment it. This will basically tell the Zybo which ports we will be needed.

![image](https://user-images.githubusercontent.com/92703672/153565117-19fa53e1-6f86-4a39-8839-b70e3cbd24dd.png)

<b>Step 45.</b> We will be configuring the constraint files based on the ports that will be needing, for this example we will be placing the Pmod ALS in port JB ( top row ),
Pmod Hygro in port JC ( top row ), Pmod AQS in port JC ( bottom row ) and lastly the Pmod AD1 in pord JD ( top row ).

Remember the name of the Pmod interface from step 34.

e.g. Change the get_ports { jb[0] } to get_ports { Pmod_ALS_pin1_io } ( get_ports { interface name_pin1_io } )

![image](https://user-images.githubusercontent.com/92703672/153566558-4cd85d75-4cb8-4e6d-ac4f-e33e6d28f9fb.png)

![image](https://user-images.githubusercontent.com/92703672/153566597-dad72b66-42a0-42d5-acb6-0dec5c016a5e.png)

![image](https://user-images.githubusercontent.com/92703672/153566628-c122e758-eb51-494e-970c-0c979de9c7ec.png)

<b>Step 46.</b> Go ahead and click save, then proceed to generate bitstream under program and debug at the left panel. ( This will take some time )

<b>Step 47.</b> Some warnings (orange) will appear during implementation, however if they are not critical errors (red), they can mostly be ignored.

![image](https://user-images.githubusercontent.com/92703672/153562502-dfde654c-ad84-40f7-854b-99ba7abb2864.png)

<b>Step 48.</b> This will show if bitstream generated successfully. None of this is needed for this example, click cancel to proceed.

![image](https://user-images.githubusercontent.com/92703672/153570044-24b1631a-878e-4e8e-9525-5bbca4ba6e62.png)

<b>Step 49.</b> Now we will be generating our xsa file to bring over to Vitis to start some programming. Navigate to file > export > export hardware.

![Screenshot 2022-02-11 175213](https://user-images.githubusercontent.com/92703672/153570853-4755869d-ed7c-4356-8ab2-ee69452e260a.png)

<b>Step 50.</b> Check include bitstream and OK. ( Take note of where you will be exporting the file to )

![image](https://user-images.githubusercontent.com/92703672/153571222-982fbc0a-1012-4d2f-8118-c97a7eb83702.png)

<b>Step 51.</b> Navigate to the location of the exported file and copy the .xsa file. e.g. 

![image](https://user-images.githubusercontent.com/92703672/153571774-39073d5f-c16c-4d81-bc65-7d9984cdbcee.png)

<b>Step 52.</b> Next navigate to where your xilinx folder and navigate to fixed_hwplatforms e.g. C:\Xilinx\Vitis\2019.2\data\embeddedsw\lib\fixed_hwplatforms and paste the copied file into here. It should now contain your wrapper file.

![image](https://user-images.githubusercontent.com/92703672/153572890-8febf5b0-6bb5-43d3-bee8-f7a5ccedefee.png)

<b>Step 53.</b> Done! Now we can proceed to code in Vitis IDE. https://github.com/ProjectHydroTech/Zybo-Z7/tree/main/Vitis
