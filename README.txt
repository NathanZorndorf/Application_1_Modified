To use this example please import it into your workspace.

Import procedure:
================
1. Run CCS
2. Go to the "Project" menu and select "Import Existing CCS/CCE Eclipse project"
3. In the import wizard choose "select archive file" and browse to the zip file this readme is in.
4. There should now be a "C5515 Audio Filter" project listed.  Clikc Finish to import this project into your workspace.

The project contains a target configuration file for the C5515 eZdsp USB Stick.  You can
click on the bug button in CCS to build this project and launch the debugger.

Notes:
-----
1. This project depends on v4.3.6 or later of the C55x compiler.  If you don't have this compiler you can obtain
it by checking for updates within CCS (Help->Software Updates->Find and Install). 
2. The target configuration file and project assume that you have C5515 device support installed into your CCS version. If
you installed the CCS that came with the eZdsp USB stick then you have this support.  Alternatively you can update your CCS
to v4.1.3 or later.
3. If you do not have the ccsv4\emulation\boards\usbstk5515_v1\gel\usbstk5515.gel file you can get it from the Spectrum
Digital website http://support.spectrumdigital.com/boards/usbstk5515/


