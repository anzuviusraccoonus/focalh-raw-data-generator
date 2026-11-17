# focalh-raw-data-generator
Small utility program to generate dummy data for testing / debugging purposes within O2 for FoCal-H

## Dependencies

- ALICE O2
- ROOT (provided through the O2 environment)

## Setup

Enter local O2 environment, e.g. with alienv:
    
    alienv enter O2/latest

then clone this repository and run the macro with ROOT:

    root -l -q generateRawData.C+
