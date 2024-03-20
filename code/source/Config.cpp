#include <fstream>
#include <iostream>

#include "../../lib/include/json.hpp"

#include "../include/Config.h"

void Config::readConfig() {
	try {
		using json = nlohmann::json;
		auto currentPath = std::filesystem::current_path();
		std::string exePath = currentPath.generic_string();
		std::string localPath = exePath + "/config.json";
		std::string externalPath = "../config.json";
		std::ifstream localFile(localPath);
		std::ifstream externalFile(externalPath);
		std::string configPath = "";
		if (localFile) {
			configPath = localPath;
			printf("Local config file found\n");
		}
		else if (externalFile) {
			configPath = externalPath;
			printf("No local config, but external config file found\n");
		}
		else {
			printf("No config file\n");
			exit(3);
		}
		std::ifstream configFile(configPath);
		json processedFile{ json::parse(configFile) };
		//


		Processes::inParallel = uint32_t{ processedFile["processes"]["in parallel"] };
		if (Processes::inParallel == 0) {
			printf("0 processes, computation is not possible\n");
			exit(1);
		}


		Directory::project = std::string{ processedFile["path to project"] };


		Geometry::size = Vec3(
			double{ processedFile["geometry"]["size"]["x"] },
			double{ processedFile["geometry"]["size"]["y"] },
			double{ processedFile["geometry"]["size"]["z"] }
		);
		Geometry::step = Vec3(
			double{ processedFile["geometry"]["step"]["x"] },
			double{ processedFile["geometry"]["step"]["y"] },
			double{ processedFile["geometry"]["step"]["z"] }
		);
		Geometry::stepRev = Vec3(
			1 / Geometry::step.x,
			1 / Geometry::step.y,
			1 / Geometry::step.z
		);
		Geometry::stepCoeff = 0.5 * Geometry::stepRev.x * Geometry::stepRev.x;
		Geometry::powderThickness = double{ processedFile["geometry"]["powder thickness"] };
		Geometry::surfaceArea = Geometry::step.x * Geometry::step.y;
		Geometry::buffer = IntVec3(
			int32_t{ processedFile["geometry"]["buffer zone, elems"] },
			int32_t{ processedFile["geometry"]["buffer zone, elems"] },
			0
		);
		Geometry::mirrorXAxis = bool{ processedFile["geometry"]["mirror x-axis"] };
		Geometry::mirrorYAxis = bool{ processedFile["geometry"]["mirror y-axis"] };


		Time::start = double{ processedFile["time"]["start"] };
		Time::step = double{ processedFile["time"]["step"] };
		Time::end = Time::step * double{ processedFile["time"]["iterations"] };


		Log::desiredEntries = uint32_t{ processedFile["logging"]["log entries"] };


		Temperature::air = double{ processedFile["temperatures"]["air"] };
		Temperature::air4 = Temperature::air * Temperature::air * Temperature::air * Temperature::air;
		Temperature::initial = double{ processedFile["temperatures"]["initial"] };
		Temperature::melting = double{ processedFile["temperatures"]["melting"] };
		Temperature::cutOff = double{ processedFile["temperatures"]["cut off"] };


		Mass::Rho::solid = double{ processedFile["mass"]["rho"]["solid"] };
		Mass::Rho::packing = double{ processedFile["mass"]["rho"]["packing"] };
		Mass::Rho::packingRev = 1 / Mass::Rho::packing;
		Mass::Rho::liquid = double{ processedFile["mass"]["rho"]["liquid"] };
		Mass::solid = Geometry::step.x * Geometry::step.y * Geometry::step.z * Mass::Rho::solid;
		Mass::liquid = Geometry::step.x * Geometry::step.y * Geometry::step.z * Mass::Rho::liquid;
		Mass::powder = Mass::solid * Mass::Rho::packing;

		Misc::sigmoidConst = 1 / (1 + exp(-12.0 * Mass::Rho::packing + 6.0));
		Misc::sigmoidConstRev = 1 / Misc::sigmoidConst;

		Energy::Solid::C = double{ processedFile["energy"]["solid"]["C"] };
		Energy::Solid::KA = double{ processedFile["energy"]["solid"]["KA"] };
		Energy::Solid::KB = double{ processedFile["energy"]["solid"]["KB"] };
		Energy::Solid::mc = Mass::solid * Energy::Solid::C;
		Energy::Solid::mcRev = 1 / Energy::Solid::mc;
		Energy::Liquid::C = double{ processedFile["energy"]["liquid"]["C"] };
		Energy::Liquid::KA = double{ processedFile["energy"]["liquid"]["KA"] };
		Energy::Liquid::KB = double{ processedFile["energy"]["liquid"]["KB"] };
		Energy::Liquid::mc = Mass::liquid * Energy::Liquid::C;
		Energy::Liquid::mcRev = 1 / Energy::Liquid::mc;
		Energy::Powder::C = Energy::Solid::C;
		Energy::Powder::mc = Mass::powder * Energy::Powder::C;
		Energy::Powder::mcRev = 1 / Energy::Powder::mc;
		Energy::Enthalpy::fusion = double{ processedFile["energy"]["enthalpy"]["fusion"] };
		Energy::Enthalpy::minusRegular = Mass::solid * Energy::Solid::C * Temperature::melting;
		Energy::Enthalpy::plusRegular = Energy::Enthalpy::minusRegular + Mass::solid * Energy::Enthalpy::fusion;
		Energy::Enthalpy::minusPowder = Mass::powder * Energy::Powder::C * Temperature::melting;
		Energy::Enthalpy::plusPowder = Energy::Enthalpy::minusPowder + Mass::powder * Energy::Enthalpy::fusion;


		Radiation::stefanBoltzmannConst = 5.67e-8;
		Radiation::emmisivity = double{ processedFile["radiation"]["emmisivity"] };
		Radiation::fluxConst = Radiation::stefanBoltzmannConst * Radiation::emmisivity;


		Laser::vec = Geometry::size.dot(Vec3(
			double{ processedFile["laser"]["loc"]["relative x"] },
			double{ processedFile["laser"]["loc"]["relative y"] },
			double{ processedFile["laser"]["loc"]["relative z"] }
		));
		Laser::vel = Vec3(
			double{ processedFile["laser"]["vel"]["x"] },
			double{ processedFile["laser"]["vel"]["y"] },
			double{ processedFile["laser"]["vel"]["z"] }
		);
		Laser::radius = double{ processedFile["laser"]["radius"] };
		Laser::power = double{ processedFile["laser"]["power"] };
		Laser::state = bool{ processedFile["laser"]["state"] };


		uint32_t xRes = (uint32_t)round(Geometry::size.x / Geometry::step.x);
		uint32_t yRes = (uint32_t)round(Geometry::size.y / Geometry::step.y);
		uint32_t zRes = (uint32_t)round(Geometry::size.z / Geometry::step.z);
		uint32_t coolingNodes = xRes * yRes + 2 * xRes * zRes + 2 * yRes * zRes;
		Misc::coolingPowerPerNode = Laser::power / coolingNodes;
	}
	catch (...) {
		std::cout << "Error while parsing config file" << std::endl;
		exit(5);
	}
}


uint32_t    Config::Processes::inParallel = 0;
std::string Config::Directory::project = "null";
Vec3        Config::Geometry::size = Vec3();
Vec3        Config::Geometry::step = Vec3();
Vec3        Config::Geometry::stepRev = Vec3();
double      Config::Geometry::stepCoeff = 0.0;
double      Config::Geometry::powderThickness = 0.0;
double      Config::Geometry::surfaceArea = 0.0;
IntVec3     Config::Geometry::buffer = IntVec3();
bool        Config::Geometry::mirrorXAxis = false;
bool        Config::Geometry::mirrorYAxis = false;
double      Config::Time::start = 0.0;
double      Config::Time::step = 0.0;
double      Config::Time::end = 0.0;
uint32_t    Config::Log::desiredEntries = 0;
double      Config::Temperature::air = 0.0;
double      Config::Temperature::air4 = 0.0;
double      Config::Temperature::initial = 0.0;
double      Config::Temperature::melting = 0.0;
double      Config::Temperature::cutOff = 0.0;
double      Config::Mass::Rho::solid = 0.0;
double      Config::Mass::Rho::packing = 0.0;
double      Config::Mass::Rho::packingRev = 0.0;
double      Config::Mass::Rho::liquid = 0.0;
double      Config::Mass::solid = 0.0;
double      Config::Mass::liquid = 0.0;
double      Config::Mass::powder = 0.0;
double      Config::Energy::Solid::C = 0.0;
double      Config::Energy::Solid::KA = 0.0;
double      Config::Energy::Solid::KB = 0.0;
double      Config::Energy::Solid::mc = 0.0;
double      Config::Energy::Solid::mcRev = 0.0;
double      Config::Energy::Liquid::C = 0.0;
double      Config::Energy::Liquid::KA = 0.0;
double      Config::Energy::Liquid::KB = 0.0;
double      Config::Energy::Liquid::mc = 0.0;
double      Config::Energy::Liquid::mcRev = 0.0;
double      Config::Energy::Powder::C = 0.0;
double      Config::Energy::Powder::mc = 0.0;
double      Config::Energy::Powder::mcRev = 0.0;
double      Config::Energy::Enthalpy::fusion = 0.0;
double      Config::Energy::Enthalpy::minusRegular = 0.0;
double      Config::Energy::Enthalpy::plusRegular = 0.0;
double      Config::Energy::Enthalpy::minusPowder = 0.0;
double      Config::Energy::Enthalpy::plusPowder = 0.0;
double      Config::Radiation::stefanBoltzmannConst = 0.0;
double      Config::Radiation::emmisivity = 0.0;
double      Config::Radiation::fluxConst = 0.0;
Vec3        Config::Laser::vec = Vec3();
Vec3        Config::Laser::vel = Vec3();
double      Config::Laser::radius = 0.0;
double      Config::Laser::power = 0.0;
bool        Config::Laser::state = false;
double      Config::Misc::sigmoidConst = 0.0;
double      Config::Misc::sigmoidConstRev = 0.0;
double      Config::Misc::coolingPowerPerNode = 0.0;