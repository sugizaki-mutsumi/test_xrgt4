//
// Modidfed by Mutsumi S. 
//

// ExXRTG4DetectorConstruction.hh
//
// Giuseppe Vacanti (cosine Science & Computing BV)
// October 14, 2009
//
// This file is part of the XRTG4 Geant4 extension.

//     The XRTG4 Geant4 extension is free software: you can
//     redistribute it and/or modify it under the terms of the GNU
//     General Public License as published by the Free Software
//     Foundation, either version 2 of the License, or (at your
//     option) any later version.

//     XRTG4 Geant4 extension is distributed in the hope that it will
//     be useful, but WITHOUT ANY WARRANTY; without even the implied
//     warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//     PURPOSE.  See the GNU General Public License for more details.

//     You should have received a copy of the GNU General Public
//     License along with XRTG4 Geant4 extension.  If not, see
//     <http://www.gnu.org/licenses/>.
//
#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include <random>
#include "G4SystemOfUnits.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4Color.hh"
#include "G4VisAttributes.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4UnionSolid.hh"

#include "G4AssemblyVolume.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

#include "ExXRTG4DetectorConstruction.hh"

#include "G4VPhysicalVolume.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4XraySpecularReflectingSurface.hh"



ExXRTG4DetectorConstruction::ExXRTG4DetectorConstruction(G4XraySpecularReflectingSurface * property)
  : G4VUserDetectorConstruction(), xray_surface_property(property) {}

ExXRTG4DetectorConstruction::~ExXRTG4DetectorConstruction() {}

G4VPhysicalVolume* ExXRTG4DetectorConstruction::Construct() {

//G4NistManager* nist = G4NistManager::Instance();


/*
  Reflaction index data 
  text data format
  Energy[eV]  beta  delta
  
  get data from https://henke.lbl.gov/optical_constants/getdb2.html
  and swap 2nd and 3rd rows.

  sample files
  si.dat 
  ir.dat
  al.dat
*/

  G4Material *vacuum = new G4Material("Vacuum", 1.0 , 1.01*g/mole, 1.0E-25*g/cm3, kStateGas, 2.73*kelvin, 3.0E-18*pascal );
  G4Material *silicon = new G4Material("Si", 14, 28.0855*g/mole, 2.329*g/cm3);
  G4Material *ir = new G4Material("Ir", 77., 192.22*g/mole,22.42*g/cm3);
  //G4Material *al = new G4Material("Al", 13., 26.98*g/mole,2.70*g/cm3);


  //---------world volume-------------
  //G4Box* solidWorld = new G4Box("World", 1.5*m, 1.5*m, 1.5*m);  // full size  3 x 3 x 3 m
  G4Box* solidWorld = new G4Box("World", 1.*m, 1.*m, 1.*m);  // full size  2 x 2 x 2 m
  G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, vacuum, "logicWorld"); 
  G4VPhysicalVolume* physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "physWorld", 0, false, 0, 0);


  //---------------- MPO geometry ------------
  /// curvature radius 600 mm
  /// focal length = 300 mm
  const G4double frad = 600*mm; 
  const G4double flen = frad/2.0;
  const G4ThreeVector vfrz = G4ThreeVector(0,0,frad);


  /// pore outer size = 24.0 um (half size 12.0 um)
  /// pore inner size = 20.0 um (half size 10.0 um)
  /// pore XY pitch = 20.0 + 6.0 = 26.0 um 
  const G4double poreXYpitch   = 26.0*um;
  const G4double poreAngleStep = atan(poreXYpitch/2./frad)*2.;

  const G4double dxout = 12.0*um;
  const G4double dxin  = 10.0*um;
  const G4double dzth  = 1.25/2*mm; 
  
  G4double dx1, dx2, dy1, dy2, dz;

  dz = dzth;
  dx2 = dxout;
  dx1 = dx2*(frad-2*dz)/frad;
  dy2 = dx2;
  dy1 = dx1;
  G4VSolid* poreOuterTrd = new G4Trd("poreOuterTrd", dx1, dx2, dy1, dy2, dz);

  dz  = dzth*1.1;
  dx2 = dxin;
  dx1 = dx2*(frad-2*dz)/frad;
  dy2 = dx2;
  dy1 = dx1;
  G4VSolid* poreInnerTrd = new G4Trd("poreInnderTrd", dx1, dx2, dy1, dy2, dz);

  G4VSolid* poreSolid = new G4SubtractionSolid("poreSolid", poreOuterTrd, poreInnerTrd, 0, G4ThreeVector(0,0,0));

  G4LogicalVolume* poreLogic= new G4LogicalVolume(poreSolid, ir, "poreLogic");

  new G4LogicalSkinSurface("TargetSurface", poreLogic, xray_surface_property);




  G4Colour colorBrown(0.7, 0.4, 0.1);
  G4VisAttributes* copperVisAttributes = new G4VisAttributes(colorBrown);
  poreLogic ->SetVisAttributes(copperVisAttributes);
 
  /// MPO size 26.um * 1540 = 40.04 mm (40.04 mm x 40.04 mm) 
  //const G4int nxpole = 1540;   // 40.40 mm
  //const G4int nypole = 1540;   // 40.40 mm
  const G4int nxpole = 154;  // 4.004 mm
  const G4int nypole = 154;  // 4.004 mm

  //const G4int nxpole = 154*5;  // visiuallizetion limit
  //const G4int nypole = 154*5;  

  
  for (G4int iy=0; iy<=nypole; iy++){
    G4double yangle = poreAngleStep*(iy-nypole/2);
    G4RotationMatrix Ry = G4RotationMatrix();
    Ry.rotateY(yangle);

    //for (G4int ix=-nxpole; ix<=nxpole; ix++){
    for (G4int ix=0; ix<=nxpole; ix++){
      G4double xangle = poreAngleStep*(ix-nxpole/2);
      G4RotationMatrix Rx = G4RotationMatrix();
      Rx.rotateX(xangle);

      G4RotationMatrix Rm = Ry*Rx;
      G4ThreeVector   Ta = Rm*vfrz;
      G4String poreName = "Pore_ix" + std::to_string(ix) + "_iy" + std::to_string(iy);
      G4int copyNum = iy*10000+ix;
      //new G4PVPlacement( G4Transform3D(Rm, Ta), poreLogic, poreName, logicWorld, false, copyNum, true);
      new G4PVPlacement( G4Transform3D(Rm, Ta), poreLogic, poreName, logicWorld, false, copyNum, false);
    }
  }
  


  //Imager box
  G4double imagerthink = 2.0*mm;
  //G4Box* imagerBox = new G4Box("imagerBox", 20.*cm, 20.*cm, imagerthink/2.0);   // full size 40 cm x 40 cm x 2 mm 
  G4Box* imagerBox = new G4Box("imagerBox", 10.*cm, 10.*cm, imagerthink/2.0);   // full size 20 cm x 20 cm x 2 mm 
  G4LogicalVolume* imagerLogicVolume = new G4LogicalVolume(imagerBox, silicon, "imagerLogicVolume", 0,0,0);
  new G4PVPlacement(0, G4ThreeVector(0, 0, flen-imagerthink/2.0), imagerLogicVolume, "imager", logicWorld, false, 0,0);
  
  return physWorld;

}
