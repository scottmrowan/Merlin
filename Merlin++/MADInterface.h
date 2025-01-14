/*
 * Merlin++: C++ Class Library for Charged Particle Accelerator Simulations
 * Copyright (c) 2001-2018 The Merlin++ developers
 * This file is covered by the terms the GNU GPL version 2, or (at your option) any later version, see the file COPYING
 * This file is derived from software bearing the copyright notice in merlin4_copyright.txt
 */

#ifndef MADInterface_h
#define MADInterface_h 1

#include "merlin_config.h"
#include <iostream>
#include <string>
#include <map>
#include <set>
#include <memory>
#include <stack>
#include "AcceleratorModel.h"
#include "DataTable.h"

class AcceleratorModelConstructor;

/**
 *      Class used to construct a MERLIN model from a MAD optics
 *      output listing. The class now automatically  identifies
 *      the column parameters, and associates them with the
 *      constructed element types. If an element type is defined
 *      for which a required parameter is not present in the
 *      column headings, the parameter is set to zero and a
 *      warning is issued.
 */

class MADInterface
{
public:
	/**
	 *  Constructor taking the name of the MAD optics file, and
	 *  the momentum in GeV/c.
	 */
	MADInterface(const std::string& madFileName, double P0);

	/**
	 *  Constructor taking an input stream of a MAD optics file, and
	 *  the momentum in GeV/c.
	 */
	MADInterface(std::istream *in, double P0);

	/**
	 *  Destructor
	 */
	~MADInterface();

	AcceleratorComponent* ComponentTypeMap(std::string& type);

	/**
	 *   Causes the construction of an AcceleratorModel object
	 *   based on the MAD optics file.
	 */
	AcceleratorModel* ConstructModel();

	/**
	 *   Sets the log file stream to os.
	 */
	void SetLogFile(std::ostream& os);

	/**
	 *   Turns logging on.
	 */
	void SetLoggingOn();

	/**
	 *   Turns logging off.
	 */
	void SetLoggingOff();

	/**
	 * Function to return corresponding multipole string *
	 */
	std::string GetMutipoleType(DataTableRow& MADinputrow);

	/**
	 * Function to define all type overrides
	 */
	void TypeOverrides(DataTableRow& MADinputrow);

	/**
	 *   If true, all RFCavities will be forced to a length of
	 *   wavelength/2 + a Drift of remaining length (
	 *   //LHC MAD tfs table bugfix!!!
	 */
	void SetSingleCellRF(bool scrf)
	{
		single_cell_rf = scrf;
	}

	/**
	 *   If true, all LINE constructs in the MAD optics output
	 *   are constructed in the model. If false, only those
	 *   prefixed X_, where X is M, S, or G are constructed.
	 */
	void HonourMadStructure(bool flg);

	/**
	 *   If true, a flat lattice model in constructed, with no
	 *   nested frames.
	 */
	void ConstructFlatLattice(bool flg);

	/**
	 *   Components of type madType are ignored during
	 *   construction if their length is zero.
	 */
	void IgnoreZeroLengthType(const string& madType);

	/**
	 *   If scaleSR == true, then the magnetic fields of the
	 *   magnets are scaled to compensate beam energy losses due
	 *   to synchrotron radiation (default = false.) Note that in
	 *   this case, the beam energy is the initial energy.
	 *
	 *   This assumes that paritcle is an electron and that energy = momentum
	 */
	void ScaleForSynchRad(bool scaleSR);

	/**
	 *   Treats the mad type typestr as a drift.
	 */
	void TreatTypeAsDrift(const std::string& typestr);

	void AppendModel(const std::string& fname, double pref);
	AcceleratorModel* GetModel();
	AcceleratorModelConstructor* GetModelConstructor();

	void ConstructNewFrame(const std::string& name);
	void EndFrame(const std::string& name);

	double GetMomentum()
	{
		return momentum;
	}

	void SetMomentum(double newmomentum)
	{
		momentum = newmomentum;
	}

	[[deprecated("Use GetMomentum()")]] double GetEnergy()
	{
		return momentum;
	}

	[[deprecated("Use SetMomentum()")]] void SetEnergy(double newenergy)
	{
		momentum = newenergy;
	}

	bool GetSynchRadFlag()
	{
		return inc_sr;
	}

	double momentum;
	bool inc_sr = false;
	bool flatLattice = false;
	double z = 0;   ///Distance along the lattice
	bool single_cell_rf = false;

protected:
	void init();
	std::string filename;
	std::istream* ifs; // madx input stream (may point to infile or be passsed in)
	std::unique_ptr<std::ifstream> infile; // madx input if reading from file
	std::ostream* log;

	bool logFlag = false;
	bool honMadStructs = false;
	bool appendFlag = false;

	std::set<std::string> zeroLengths;
	std::set<std::string> driftTypes;

	AcceleratorModelConstructor* modelconstr = nullptr;
	AcceleratorComponent* currentcomponent;
	std::stack<std::string> frameStack;
};

inline void MADInterface::SetLogFile(std::ostream& os)
{
	log = &os;
}

inline void MADInterface::SetLoggingOn()
{
	logFlag = true;
}

inline void MADInterface::SetLoggingOff()
{
	logFlag = false;
}

inline void MADInterface::HonourMadStructure(bool flg)
{
	honMadStructs = flg;
}

inline void MADInterface::ConstructFlatLattice(bool flg)
{
	flatLattice = flg;
}

inline void MADInterface::ScaleForSynchRad(bool scaleSR)
{
	inc_sr = scaleSR;
}

typedef std::vector<AcceleratorComponent*> (*getTypeFunc)(DataTableRow& MADinputrow, double brho);

class TypeFactory
{
public:
	static std::map<std::string, getTypeFunc> componentTypes;
	std::vector<AcceleratorComponent*> GetInstance(DataTableRow& MADinputrow, double brho);

};

class TypeFactoryInit
{
	static TypeFactoryInit init;
public:
	TypeFactoryInit();
};

class DriftComponent: public AcceleratorComponent
{
public:
	static std::vector<AcceleratorComponent*> GetInstance(DataTableRow& MADinputrow, double brho);
};

class RBendComponent: public AcceleratorComponent
{
public:
	static std::vector<AcceleratorComponent*> GetInstance(DataTableRow& MADinputrow, double brho);
};

class SBendComponent: public AcceleratorComponent
{
public:
	static std::vector<AcceleratorComponent*> GetInstance(DataTableRow& MADinputrow, double brho);
};

class QuadrupoleComponent: public AcceleratorComponent
{
public:
	static std::vector<AcceleratorComponent*> GetInstance(DataTableRow& MADinputrow, double brho);
};

class SkewQuadrupoleComponent: public AcceleratorComponent
{
public:
	static std::vector<AcceleratorComponent*> GetInstance(DataTableRow& MADinputrow, double brho);
};

class SextupoleComponent: public AcceleratorComponent
{
public:
	static std::vector<AcceleratorComponent*> GetInstance(DataTableRow& MADinputrow, double brho);
};

class SkewSextupoleComponent: public AcceleratorComponent
{
public:
	static std::vector<AcceleratorComponent*> GetInstance(DataTableRow& MADinputrow, double brho);
};

class OctupoleComponent: public AcceleratorComponent
{
public:
	static std::vector<AcceleratorComponent*> GetInstance(DataTableRow& MADinputrow, double brho);
};

class YCorComponent: public AcceleratorComponent
{
public:
	static std::vector<AcceleratorComponent*> GetInstance(DataTableRow& MADinputrow, double brho);
};

class XCorComponent: public AcceleratorComponent
{
public:
	static std::vector<AcceleratorComponent*> GetInstance(DataTableRow& MADinputrow, double brho);
};

class VKickerComponent: public AcceleratorComponent
{
public:
	static std::vector<AcceleratorComponent*> GetInstance(DataTableRow& MADinputrow, double brho);
};

class HKickerComponent: public AcceleratorComponent
{
public:
	static std::vector<AcceleratorComponent*> GetInstance(DataTableRow& MADinputrow, double brho);
};

class SolenoidComponent: public AcceleratorComponent
{
public:
	static std::vector<AcceleratorComponent*> GetInstance(DataTableRow& MADinputrow, double brho);
};

class RFCavityComponent: public AcceleratorComponent
{
public:
	static std::vector<AcceleratorComponent*> GetInstance(DataTableRow& MADinputrow, double brho);
};

class RFCavityComponentSingleCell: public AcceleratorComponent
{
public:
	static std::vector<AcceleratorComponent*> GetInstance(DataTableRow& MADinputrow, double brho);
};

class CollimatorComponent: public AcceleratorComponent
{
public:
	static std::vector<AcceleratorComponent*> GetInstance(DataTableRow& MADinputrow, double brho);
};

class CrabMarkerComponent: public AcceleratorComponent
{
public:
	static std::vector<AcceleratorComponent*> GetInstance(DataTableRow& MADinputrow, double brho);
};

class CrabRFComponent: public AcceleratorComponent
{
public:
	static std::vector<AcceleratorComponent*> GetInstance(DataTableRow& MADinputrow, double brho);
};

class HELComponent: public AcceleratorComponent
{
public:
	static std::vector<AcceleratorComponent*> GetInstance(DataTableRow& MADinputrow, double brho);
};

class MonitorComponent: public AcceleratorComponent
{
public:
	static std::vector<AcceleratorComponent*> GetInstance(DataTableRow& MADinputrow, double brho);
};

class MarkerComponent: public AcceleratorComponent
{
public:
	static std::vector<AcceleratorComponent*> GetInstance(DataTableRow& MADinputrow, double brho);
};

#endif
