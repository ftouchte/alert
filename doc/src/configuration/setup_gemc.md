# Setup GEMC

In your work space (`/work/clas12/users/yourjlabusername`), create a new repository named `gemc`

``` shell
mkdir gemc
cd gemc
```
## GEMC source

Now, clone my version of GEMC :

``` shell 
git clone git@github.com:ftouchte/gemc_source.git source
```

N.B: The repository has been renamed `source`. 

If you don't have a ssh key, you will get an error. You can learn how to create one and add this in your GitHub account. Then you can re-use the same command. If you don't want to create a ssh key, you can use :

``` shell
git clone https://github.com/ftouchte/gemc_source.git source
```

Now go inside the directory and compile gemc.

``` shell
cd source
scons -j4
```

An executabe called **gemc** has been created. 

<!-- **N.B:** At the date of June 26 2024, this repository contains ROOT codes. To avoid error, you have to use a version of ROOT based on C++14. To do so, you can download a pre-compile version of ROOT followinf these [instructions](https://root.cern/install/#download-a-pre-compiled-binary-distribution). -->

<!-- You have to correct right access, you just source this version :

``` shell
source /w/hallb-scshelf2102/clas12/users/touchte/root/bin/thisroot.csh
``` -->

## GEMC detector

Go back in your working space (`/work/clas12/users/yourjlabusername/gemc`) and clone this repository:

``` shell
git clone git@github.com:mpaolone/detectors.git detectors
```
or 

``` shell
git clone https://github.com/mpaolone/detectors.git detectors
```

Now go inside the directory 

``` shell
cd detectors/clas12/alert
```

and copy and paste the following lines in the terminal (line by line)

``` shell
cd ../targets
./targets.pl config.dat
cd ../alert/ahdc
./ahdc.pl config.dat
cd ../atof
./atof.pl config.dat
cd ../external_shell_nonActif
./alertshell.pl config.dat
cd ../He_bag
./hebag.pl config.dat
cd ../ 
```

<!-- ``` shell
cd ../targets
./targets.pl config.dat
cd ../alert/ahdc
run-groovy factory.groovy --variation default --runnumber 11
run-groovy factory.groovy --variation rga_fall2018 --runnumber 11
./ahdc.pl config.dat
cd ../atof
run-groovy factory.groovy --variation default --runnumber 11
run-groovy factory.groovy --variation rga_fall2018 --runnumber 11
./atof.pl config.dat
cd ../external_shell_nonActif
./alertshell.pl config.dat
cd ../He_bag
./hebag.pl config.dat
cd ../ 
``` -->