# Generate a particle

The type of particle, the iniatial coordinates, momentum, ... are defined in a file called **alert.gcard** accessible in `gemc/detectors/clas12/alert`. Here is an example  of such a file.

``` json
<gcard>
	
~	<!-- target. ALERT target -->
~	<detector name="../targets/target" factory="TEXT" variation="alert"/> 
~		
~	<!-- Implementation ATOF, ahdc -->
~	<detector name="atof/atof" factory="TEXT" variation="default"/>
~	<detector name="ahdc/ahdc" factory="TEXT" variation="default"/>
~	<detector name="./external_shell_nonActif/alertshell" factory="TEXT" variation="original"/>
~
~	<!-- He bag -->
~	<detector name="./He_bag/hebag" factory="TEXT" variation="original"/>
~
~	<!-- hall field -->
~	<option name="HALL_FIELD"  value="clas12-newSolenoid"/>
~
~	<!-- fields, precise mode -->
~	<option name="FIELD_PROPERTIES" value="TorusSymmetric,     2*mm, G4ClassicalRK4, linear"/>
~	<option name="FIELD_PROPERTIES" value="clas12-newSolenoid, 1*mm, G4ClassicalRK4, linear"/>
~
~	<option name="INTEGRATEDRAW" value="*"/>
~
~	<option name="PHYSICS" value="STD + FTFP_BERT"/>
~	<option name="SAVE_ALL_MOTHERS" value="0"/>
~	
	<!-- beam conditions -->
	<option name="BEAM_P"   value="proton, 160*MeV, 90.0*deg, 0*deg"/>
 	<option name="SPREAD_P"  value="90*MeV, 30.0*deg, 180*deg"/>
	
	<option name="BEAM_V"    value="(0, 0, 0)cm"/>
	<option name="SPREAD_V"  value="(0.0, 15.0)cm"/>

~	<!--  Run Number 11, picked up by digitization routines -->
~	<option name="RUNNO"    value="11" />
~	<option name="DIGITIZATION_VARIATION"  value="default" />
~	
~	<option name="OUTPUT" value="hipo, out.hipo"/>
~
</gcard>
```

The part of interest is called **beam conditions**. With this configuration file, the software will generate **proton** with momentum **\\(p\\) = 160 MeV** oriented along **\\(\theta\\) = 90°** and **\\(\phi\\) = 0°**. In fact, no ! Indeed, due to the option **SPREAD_P**, the real values are uniform draws : 

parameters | values
:---: | :---:
\\(p\\) | **160 MeV ± 90 MeV** 
\\(\theta\\) |**90° ± 30°**  
\\(\phi\\)  | **0° ± 180°**


This is the same logic for the position (BEAM_V and SPREAD_V). In other words :

``` json
<option name="BEAM_P"   value="type_of_particle, p*MeV, theta*deg, phi*deg"/>
<option name="SPREAD_P"  value="p*MeV, theta*deg, phi*deg"/>
<option name="BEAM_V"    value="(x,y,z)cm"/>
<option name="SPREAD_V"  value="(r=sqrt(x²+y²),z)cm"/>
```

Reference [GEMC options](https://gemc.jlab.org/gemc/html/documentation/options.html#gcard). The following plot shows the momentum distribution of ten thousands particle generated in GEMC with the above **alert.gcard**. We can see that all the parameters are centered in the values defined in *BEAM_P*. The widths of the distributions correspond to the value defined in *SPREAD_P*.

![mc_particle.png](./mc_particle.png)