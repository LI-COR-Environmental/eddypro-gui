# EddyPro procedure to validate code with LI-COR cpplint

## Update the local cpplint with the current LI-COR cpplint
    `$ ./update-cpplint.sh`

## Create a dir for the version under test
    `$ mkdir ep-6.0.0`

## Copy script get-eddypro-new-files.sh and run it inside the created dir
    `$ ./get-eddypro-new-files.sh`

## Run cpplint driver script
    `$ python cpplint_eddypro.py`
