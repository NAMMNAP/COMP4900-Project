# Motivation

AUV's or submarines have to predict GPS data since a signal can't be reached underwater. The way this is done is taking sensor data and updating an internal GPS estimation.

# Objective

The High-level goal of this project is to measure how accurately a QNX system can estimate GPS location in a AUV. To do this we will need to:

-   Create Sensor Simulation
-   Internal AUV GPS Estimation
-   Report Generation
-   Communication between the the AUV and the sensor simulation

## Sensor Simulation

For sensor simulation we will need to simulate the sensors on the AUV. The data will be sent to the AUV which will then be used to estimate the GPS locaiton of the AUV.

The implmentation will look like:

something something something (**TODO**)

## Internal AUV GPS Estimation

For the AUV GPS estimation we will need to use the data from the sensors to estimate the GPS location of the AUV. The AUV will record it's estimated GPS location.

We will use an algorithm to estimate the GPS location which when implemented looks like this:

something something something (**TODO**)

## Report Generation

For the report generation we will need to generate a report based on the data collected from the AUV. The report will include the estimated GPS location of the AUV and the actual GPS location of the AUV. The report will also include a comparison between the estimated GPS location and the actual GPS location.

The report genration system is implemented here:

something something something (**TODO**)

## Communication Module

This is just a module for the communication between the AUV and the sensor simulation. Like an API basically. (**TODO**)

# File Structure

src/ - contains all the source code for the project
sensor_sim/ - contains the sensor simulation code
communication_module/ - contains the communication module code
report_generator/ - contains the report generation code
AUV_sim/ - contains the AUV simulation code
