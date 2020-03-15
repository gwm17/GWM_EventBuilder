/*

  Functions for the calculation of the kinematic shift of the FP
  for the SESPS @ FSU.


>>>  Get_Mass(Z, A) returns the mass (in amu) of an atom (Z, A).
     ***this requires a text file mass_info.txt w/ tabulated mass
     values.


>>>  Delta_Z(int...) returns the shift of the FP in the z-direction in
     cm. A negative (<0) delta-z is defined as a shift towards the
     magnet.
     ***NOTE: this uses Get_Mass(...), hence requires a tabulated
     mass file.

     Arguments: Delta_Z(int ZT, int AT, int ZP, int AP, int ZE, int AE,
	                double EP, double angle, double B),
        where Z,A are atomic number and mass number for each particle,
        EP is the KE of the projectile (i.e. beam energy in MeV),
        angle is the spectrograph angle (in degrees),
        B is the field in Gauss.


>>>  Delta_Z(double...) does the same, only takes in masses (in amu)
     explicitly, rather than (Z, A) for each particle (e.g. if you
     don't have a text file of tabulated masses).
     ***Make sure these are NUCLEAR masses, w/ electrons taken out.

     Arguments: Delta_Z(double MT, double MP, double ME, double MR,
   	                double EP, int ZE, double angle, double B),
        where NUCLEAR masses M are in amu, and the rest are the same
        as above. ***NOTE the required input of the ejectile Z for
	charge determination.


>>>  Wire_Dist() returns the distance (in cm) between the wires for
     the calculation of relative weights between FP1 and FP2.

  //format: T(P,E)R
  //   i.e., T = target,
  //         P = projectile,
  //         E = ejectile,
  //         R = residual;
  //expects angle in degs, B in G, masses and KE in MeV

  KGH -- Jul19

*/

#ifndef FP_KINEMATICS
#define FP_KINEMATICS

#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

double Get_Mass(int iZ, int iA);


//requires (Z,A) for T, P, and E, as well as energy of P,
// spectrograph angle of interest, and field value
double Delta_Z(int ZT, int AT, int ZP, int AP, int ZE, int AE,
	       double EP, double angle, double B); 


//requires masses (in amu) for all particles, as well as energy of P,
// Z of E, spectrograph angle of interest, and rho of interest
//***make sure this is NUCLEAR mass, w/ electrons taken out
double Delta_Z(double MT, double MP, double ME, double MR,
	       double EP, int ZE, double angle, double B);

double Wire_Dist();

#endif
