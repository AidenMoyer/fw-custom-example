// Base engine settings for the single-rotor Wankel genset, called from board_configuration.cpp.
// One chamber fires per shaft revolution, so the rotor is a 279 cc one-cylinder two-stroke, the same way rusEFI's
// own 13B preset (setRotary(), engine type 107) counts each rotor as a two-stroke cylinder. Everything here is a
// starting point for tuning in TunerStudio (firmware/README.md lists what to set there).
#include "pch.h"

void boardTuneDefaults() {
	engineConfiguration->cylindersCount = 1;
	engineConfiguration->firingOrder = FO_1;
	engineConfiguration->twoStroke = true;
	engineConfiguration->displacement = 0.279f;
	// 1/rev Hall with a small magnet: a short pulse once per shaft revolution
	engineConfiguration->trigger.type = trigger_type_e::TT_NARROW_SINGLE_TOOTH;
	engineConfiguration->ignitionMode = IM_ONE_COIL;
	engineConfiguration->injectionMode = IM_SEQUENTIAL;
	setAlgorithm(engine_load_mode_e::LM_SPEED_DENSITY);

	// Methanol, EV14 550 cc at 3 bar, returnless rail held at 3 bar by genset.lua
	engineConfiguration->stoichRatioPrimary = 6.4f;
	engineConfiguration->injector.flow = 550;

	// Primary speed limit: spark and fuel cut on the next event; the safety board trips at 9,500
	engineConfiguration->rpmHardLimit = 9000;
	engineConfiguration->cutFuelOnHardLimit = true;
	engineConfiguration->cutSparkOnHardLimit = true;

	// genset.lua cranks at 400-550 rpm and calls it running above 650
	engineConfiguration->cranking.rpm = 600;
}
