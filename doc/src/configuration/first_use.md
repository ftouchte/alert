# First use

This section concerns people that are not familiar with the software. For others, it is a prerequisite to expect the same results as mine. It is based on the [official ALERT software wiki](https://clasweb.jlab.org/wiki/index.php/ALERT_Software_and_Simulations/GEMC_coatjava_setup_on_farm).

## Preamble

If you have the correct right access, we recommend to create a new work space following these commande line :

``` shell
cd /work/clas12/users/
```

``` shell
mkdir yourjlabusername
```

``` shell
cd yourjlabusername 
```

## Setup "default" CLAS12 software modules

Run these command lines :

``` shell
source /group/clas12/packages/setup.csh
module load clas12
module switch ccdb/1.99.2
module load gemc
module switch ccdb/1.99.1
```

This set of commands can be added into your .login file in your home directory.