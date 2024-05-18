#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <string>
#include <vector>
#include "../../lib/include/Vec.h"

class Config {
public:
	static void readConfig();
	static std::string configPath;

	struct Processes {
		static uint32_t count; // 0 - no processes created, 1 and more - amount of manually created processes
	};

	struct Directory {
		static std::string project;
	};

	struct Geometry {
		static Vec3I resolution;
		static Vec3 step;
		static Vec3 stepRev;
		static std::vector<std::vector<int32_t>> coarsen;
		static double stepCoeff;
		static double powderThickness;
		static double surfaceArea;
		static Vec3I buffer;
	};

	struct Time {
		static double start;
		static double step;
		static double end;
	};

	struct Log {
		static uint32_t desiredEntries;
	};

	struct Temperature {
		static double air;
		static double air4;
		static double initial;
		static double melting;
		static double cutOff;
	};

	struct Mass {
		struct Rho {
			static double solid;
			static double packing;
			static double packingRev;
			static double liquid;
		};
		static double solid;
		static double liquid;
		static double powder;
	};

	struct Energy {
		struct Solid {
			static double C;
			static double KA;
			static double KB;
			static double mc;
			static double mcRev;
		};
		struct Liquid {
			static double C;
			static double KA;
			static double KB;
			static double mc;
			static double mcRev;
		};
		struct Powder {
			static double C;
			static double mc;
			static double mcRev;
		};
		struct Enthalpy {
			static double fusion;
			static double minusRegular;
			static double plusRegular;
			static double minusPowder;
			static double plusPowder;
		};
	};

	struct Radiation {
		static double stefanBoltzmannConst;
		static double emmisivity;
		static double fluxConst;
	};

	struct Laser {
		static Vec3 vec;
		static Vec3 vel;
		static double radius;
		static double power;
		static double absorbtion;
		static double goUntill;
		static double sideStep;
		static bool state;
		static uint32_t tracks;
		static uint32_t layers;
	};

	struct Misc {
		static double sigmoidConst;
		static double sigmoidConstRev;
	};
};

#endif // !CONFIG_H