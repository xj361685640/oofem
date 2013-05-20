/*
 *
 *                 #####    #####   ######  ######  ###   ###
 *               ##   ##  ##   ##  ##      ##      ## ### ##
 *              ##   ##  ##   ##  ####    ####    ##  #  ##
 *             ##   ##  ##   ##  ##      ##      ##     ##
 *            ##   ##  ##   ##  ##      ##      ##     ##
 *            #####    #####   ##      ######  ##     ##
 *
 *
 *             OOFEM : Object Oriented Finite Element Code
 *
 *               Copyright (C) 1993 - 2013   Borek Patzak
 *
 *
 *
 *       Czech Technical University, Faculty of Civil Engineering,
 *   Department of Structural Mechanics, 166 29 Prague, Czech Republic
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef isolinearelasticmaterial_h
#define isolinearelasticmaterial_h

#include "linearelasticmaterial.h"
#include "dictionary.h"
#include "floatarray.h"
#include "floatmatrix.h"

///@name Input fields for IsotropicLinearElasticMaterial
//@{
#define _IFT_IsotropicLinearElasticMaterial_Name "isole"
#define _IFT_IsotropicLinearElasticMaterial_e "e"
#define _IFT_IsotropicLinearElasticMaterial_n "n"
#define _IFT_IsotropicLinearElasticMaterial_talpha "talpha"
//@}

namespace oofem {
class GaussPoint;

/**
 * Class implementing isotropic linear elastic material.
 * This class implements an isotropic linear elastic material in a finite
 * element problem. A material
 * is an attribute of a domain. It is usually also attribute of many elements.
 *
 * Tasks:
 * - Returning standard material stiffness matrix for 3d-case.
 *   according to current state determined by using data stored
 *   in Gausspoint.
 * - methods give2dPlaneStressMtrx, givePlaneStrainMtrx, give1dStressMtrx are
 *   introduced since form of this matrices is well known, and for
 *   faster response mainly in linear elastic problems.
 * - Returning a material property (method 'give'). Only for non-standard elements.
 * - Returning real stress state vector(tensor) at gauss point for 3d - case.
 */
class IsotropicLinearElasticMaterial : public LinearElasticMaterial
{
protected:
    /// Young's modulus.
    double E;
    /// Poisson's ratio.
    double nu;
    /// Shear modulus.
    double G;

public:
    /**
     * Creates a new IsotropicLinearElasticMaterial class instance
     * with given number belonging to domain d.
     * @param n material model number in domain
     * @param d domain which receiver belongs to
     */
    IsotropicLinearElasticMaterial(int n, Domain *d) : LinearElasticMaterial(n, d) { }
    /**
     * Creates a new IsotropicLinearElasticMaterial class instance
     * with given number belonging to domain d.
     * @param n Material model number in domain.
     * @param d Domain which receiver belongs to.
     * @param E Young modulus.
     * @param nu Poisson ratio.
     */
    IsotropicLinearElasticMaterial(int n, Domain *d, double E, double nu);
    /// Destructor.
    virtual ~IsotropicLinearElasticMaterial() { }

    virtual void giveCharacteristicMatrix(FloatMatrix &answer,
                                  MatResponseForm form,
                                  MatResponseMode mode,
                                  GaussPoint *gp,
                                  TimeStep *atTime);

    /**
     * Returns a vector of coefficients of thermal dilatation in direction
     * of each material principal (local) axis.
     * @param answer Vector of thermal dilatation coefficients.
     * @param gp Integration point.
     * @param tStep Time step (most models are able to respond only when atTime is current time step).
     */
    virtual void giveThermalDilatationVector(FloatArray &answer, GaussPoint *gp, TimeStep *tStep);

    // identification and auxiliary functions
    virtual int hasMaterialModeCapability(MaterialMode mode);
    virtual const char *giveClassName() const { return "IsotropicLinearElasticMaterial"; }
    virtual classType giveClassID() const { return IsotropicLinearElasticMaterialClass; }
    virtual const char *giveInputRecordName() const { return _IFT_IsotropicLinearElasticMaterial_Name; }
    /**
     * Initializes receiver according to object description stored in input record.
     * The E modulus (keyword "E"), Poisson ratio ("nu") and coefficient of thermal dilatation
     * alpha ("talpha") are read. The parent class instanciateFrom method is called.
     */
    virtual IRResultType initializeFrom(InputRecord *ir);
    virtual void giveInputRecord(DynamicInputRecord &input);

    virtual double give(int aProperty, GaussPoint *gp);

    /// Returns Young's modulus.
    double giveYoungsModulus() { return E; }

    /// Returns Poisson's ratio.
    double givePoissonsRatio() { return nu; }

    /// Returns the shear elastic modulus @f$ G = \frac{E}{2(1+\nu)} @f$.
    double giveShearModulus() { return G; }

    /// Returns the bulk elastic modulus @f$ K = \frac{E}{3(1-2\nu)} @f$.
    double giveBulkModulus() { return E / ( 3. * ( 1. - 2. * nu ) ); }

    virtual void give3dMaterialStiffnessMatrix(FloatMatrix & answer,
                                       MatResponseForm, MatResponseMode,
                                       GaussPoint * gp,
                                       TimeStep * atTime);

    /**
     * Creates new copy of associated status (StructuralMaterialStatus class )
     * and inserts it into given integration point.
     * @param gp Integration point where newly created status will be stored.
     * @return reference to new status.
     */
    virtual MaterialStatus *CreateStatus(GaussPoint *gp) const;

    /**
     * Computes bulk modulus from given Young's modulus and Poisson's ratio.
     * @param young Young's modulus (@f$ E @f$).
     * @param nu Poisson's ratio (@f$ \nu @f$).
     * @return Bulk modulus (@f$ K = E/(3*(1-2*nu) @f$).
     */
    static double computeBulkModulusFromYoungAndPoisson(double young, double nu)
    {
        return young / ( 3. * ( 1. - 2. * nu ) );
    }

    /**
     * Computes shear modulus from given Young's modulus and Poisson's ratio.
     * @param young Young's modulus (@f$ E @f$).
     * @param nu Poisson's ratio (@f$ \nu @f$).
     * @return Shear modulus (@f$ G = \frac{E}{2 (1+\nu)} @f$).
     */
    static double computeShearModulusFromYoungAndPoisson(double young, double nu)
    {
        return young / ( 2. * ( 1. + nu ) );
    }

protected:
    virtual void givePlaneStressStiffMtrx(FloatMatrix & answer,
                                  MatResponseForm, MatResponseMode, GaussPoint * gp,
                                  TimeStep * atTime);

    virtual void givePlaneStrainStiffMtrx(FloatMatrix & answer,
                                  MatResponseForm, MatResponseMode, GaussPoint * gp,
                                  TimeStep * atTime);

    virtual void give1dStressStiffMtrx(FloatMatrix & answer,
                               MatResponseForm, MatResponseMode, GaussPoint * gp,
                               TimeStep * atTime);

    virtual void give2dBeamStiffMtrx(FloatMatrix &answer,
                             MatResponseForm form, MatResponseMode rMode,
                             GaussPoint *gp,
                             TimeStep *tStep);

    virtual void give3dBeamStiffMtrx(FloatMatrix &answer,
                             MatResponseForm form, MatResponseMode rMode,
                             GaussPoint *gp,
                             TimeStep *tStep);

    friend class CrossSection;
};
} // end namespace oofem
#endif // isolinearelasticmaterial_h
