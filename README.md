# Background
This project aims at implementing an intelligent human resources management system which combines the RFID and face recognition method. The proposed system will have a camera to capture the faces of people and a RFID reader to check the ID numbers. If both of the verifying processes return a “Pass” signal, then a successful entrance signal is generated. The concept diagram of the system is described in below figure.

<p align="center">
    <image src="./images/system_overview.png" width="50%">
    <div align="center">System overview</div>
</p>

Demo link: https://www.youtube.com/watch?v=MhpE4pdadtc

The system is the combination of three main components
* Camera
* RFID reader
* Computer


The main target is to build a recognition system which has a RFID reader for checking the user identity number and a biometric system that use an HD camera for recognising the user’s face. 

At the first step, the RFID reader reads the user’s RFID tag to get user ID, this ID numbers are then converted into binary data and sent to the computer via RS232 interface. 

In the next step, the computer searches out through its database for the received user number. Once the identity number is authenticated, the computer activates the camera to capture the user’s face. Then, the user’s images are sent back to the computer via USB interface for face detection and face recognition process. 

With built in OpenCV library, this user’s face is carefully analysed by an algorithm called “EigenFaces”. The verification process is completed by confirm the user identity result on general user interface.
