/*
 * Merlin++: C++ Class Library for Charged Particle Accelerator Simulations
 * Copyright (c) 2001-2018 The Merlin++ developers
 * This file is covered by the terms the GNU GPL version 2, or (at your option) any later version, see the file COPYING
 * This file is derived from software bearing the copyright notice in merlin4_copyright.txt
 */

#ifndef SynchRadParticleProcess_h
#define SynchRadParticleProcess_h 1

#include "merlin_config.h"

#include "ParticleBunchProcess.h"
#include "MultipoleField.h"

// Global functions for photon spectrum generation

/**
 * Generator from Helmut Burkhardt (CERN)
 * (default generator, see also CERN-LEP-Note 632)
 */
double HBSpectrumGen(double u);

/**
 * Generator from Andy Wolski (LBL).
 * Faster than HBSpectrumGen but not as accurate.
 */
double AWSpectrumGen(double u);

/**
 *	Models the effects of synchrotron radiation in dipoles
 *	and (optionally) quadrupoles. The default behaviour is
 *	to include only the classical (average) effects. Calling
 *	GeneratPhotons(true) models the quantum effects due to
 *	random photon generation. The photon spectrum used can
 *	be changed by a call to SetPhotonGenerator(double
 *	(*)(double u)). The default spectrum (dipole radiation)
 *	has been provided by H. Burkhardt (CERN-LEP-Note 632).
 *
 *	The number of equally spaced steps to take through a
 *	component can be specified (default = 1). The effect of
 *	the energy loss on the particles can be specified in two
 *	ways: first (default), only the dp/p are adjusted, and
 *	the reference energy (momentum) of the bunch is left
 *	unchanged; alternatively, the reference energy is adjusted to
 *	reflect the mean energy loss, and the dp/p then reflect
 *	the additional change with respect to the mean.
 */

namespace ParticleTracking
{

class SynchRadParticleProcess: public ParticleBunchProcess
{
public:

	typedef double (*PhotonGenerator)(double);

	/**
	 *	Constructor taking the process priority (>=0), the
	 *	number of equal steps to take through a component
	 *	(nstep). If the flag q==true, then quantum excitation is
	 *	modelled.
	 */
	SynchRadParticleProcess(int prio, bool q = false);

	/**
	 *	Sets the current accelerator component. If component is
	 *	a SectorBend, then the process becomes active.
	 */
	virtual void SetCurrentComponent(AcceleratorComponent& component);

	/**
	 *	Preform the process for the specified step ds.
	 */
	virtual void DoProcess(double ds);

	/**
	 *	Returns the current maximum step length for this process.
	 *	@return Current maximum step length of process
	 */
	virtual double GetMaxAllowedStepSize() const;

	/**
	 *	Include radiation effects in Quadrupoles and Skew
	 *	Quadrupoles.
	 */
	void IncludeQuadRadiation(bool quadsr);

	/**
	 *	Sets the function to be called to generate the photon
	 *	spectrum.
	 */
	static void SetPhotonGenerator(PhotonGenerator pg)
	{
		pgen = pg;
	}

	/**
	 *	Sets the minimum number of equal steps to take through
	 *	the component.
	 */
	void SetNumComponentSteps(int n);

	/**
	 *	Sets the maximum step length to take through
	 *	the component.
	 */
	void SetMaxComponentStepSize(double ds_max);

	/**
	 *	If gp==true, include quantum excitation effects via
	 *	photon generation.
	 */
	void GeneratePhotons(bool gp);

	/**
	 *	If flg==true, the reference energy (momentum) of the
	 *	ParticleBunch is adjusted to the mean of the particle
	 *	energies. If false, then only the dp/p are adjusted.
	 */
	void AdjustBunchReferenceEnergy(bool flg)
	{
		adjustEref = flg;
	}

	/**
	 *  If flg==true, changes to phase space variables are
	 *  calculated appropriately for symplectic variables,
	 *  i.e. xp and yp are the momenta canonical to
	 *  the co-ordinates x and y.
	 *  If flg==false, changes to phase space variables are
	 *  calculated appropriately for xp and yp being the angles
	 *  of the particle trajectory to the reference trajectory.
	 */
	static void UseSymplecticVariables(bool flg)
	{
		sympVars = flg;
	}

	// Data Members for Associations

	/**
	 *	Pointer to the function which generates the random
	 *	photon spectrum. The function should have the form
	 *
	 *	double (*)(double u)
	 *
	 *	where u is the critical photon energy. Returns the
	 *	energy of the photon in GeV.
	 */

	static PhotonGenerator pgen;

	static bool sympVars;   /// Flag to indicate whether tracking uses symplectic variables

	double PHOTCONST1, PHOTCONST2, ParticleMassMeV;

protected:
private:
	// Data Members for Class Attributes

	int ns;

	PhotonGenerator quantum;

	bool incQ;

	bool adjustEref;

private:
	// Data Members for Class Attributes

	/**
	 *	The rms momentum addition per unit length.
	 */
	double ds2;

	/**
	 *	The mean energy loss per unit length.
	 */
	double ds1;

	double dL;

	int nk1;

	double intS;

	// Data Members for Associations

	MultipoleField* currentField;

	double dsMax;

	// Copy prevention
	SynchRadParticleProcess(const SynchRadParticleProcess& rhs);
	SynchRadParticleProcess& operator=(const SynchRadParticleProcess& rhs);
};

} // end namespace ParticleTracking
#endif
