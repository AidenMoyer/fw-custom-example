// Genset ECU board (STM32F407VGT6, electronics/ecu.py) for rusEFI, laid out like rusefi/fw-custom-example.
// Pins follow electronics/ecu.py MCU_NETS; firmware/tools/build_firmware_files.py checks every assignment below
// against it, so edit both together.
#include "pch.h"
#include "board_overrides.h"

void setupBoardHardwareTestOverrides();

// The green LED (PD14) is the Lua status LED and the red LED (PD15) is rusEFI's critical-error LED (board.mk),
// so rusEFI's own comms/running/warning LEDs are not used.
Gpio getCommsLedPin() {
	return Gpio::Unassigned;
}

Gpio getRunningLedPin() {
	return Gpio::Unassigned;
}

Gpio getWarningLedPin() {
	return Gpio::Unassigned;
}

static void setGensetPins() {
	// 1/rev Hall, buffered on the safety board
	engineConfiguration->triggerInputPins[0] = Gpio::C6;
	engineConfiguration->triggerInputPins[1] = Gpio::Unassigned;

	// IGN1 drives the twin wasted-spark coil (leading + trailing plug together); IGN2 is the spare IGBT
	engineConfiguration->ignitionPins[0] = Gpio::E2;
	engineConfiguration->trailingCoilPins[0] = Gpio::E3;
	// INJ1 = EV14 550 cc; INJ2 (PD6) is a spare driver
	engineConfiguration->injectionPins[0] = Gpio::D7;

	// TLE9201SG: PWM + DIR + DIS, not two-wire mode
	engineConfiguration->etbFunctions[0] = DC_Throttle1;
	engineConfiguration->etb_use_two_wires = false;
	engineConfiguration->etbIo[0].controlPin = Gpio::E9;
	engineConfiguration->etbIo[0].directionPin1 = Gpio::E10;
	engineConfiguration->etbIo[0].directionPin2 = Gpio::Unassigned;
	engineConfiguration->etbIo[0].disablePin = Gpio::E12;

	// Analog inputs (STM32F4: EFI_ADC_0..7 = PA0..PA7, 8..9 = PB0..PB1, 10..15 = PC0..PC5)
	engineConfiguration->tps1_1AdcChannel = EFI_ADC_0;
	engineConfiguration->tps1_2AdcChannel = EFI_ADC_1;
	engineConfiguration->map.sensor.hwChannel = EFI_ADC_2;
	engineConfiguration->clt.adcChannel = EFI_ADC_3;
	engineConfiguration->iat.adcChannel = EFI_ADC_4;
	engineConfiguration->lowPressureFuel.hwChannel = EFI_ADC_6;
	engineConfiguration->afr.hwChannel = EFI_ADC_7;
	engineConfiguration->throttlePedalPositionAdcChannel = EFI_ADC_8;
	engineConfiguration->throttlePedalPositionSecondAdcChannel = EFI_ADC_14;
	engineConfiguration->vbattAdcChannel = EFI_ADC_11;

	// Lua analog inputs, read by genset.lua with getAuxAnalog(0..4)
	engineConfiguration->auxAnalogInputs[0] = EFI_ADC_9;   // HV+ tap
	engineConfiguration->auxAnalogInputs[1] = EFI_ADC_10;  // HV- tap
	engineConfiguration->auxAnalogInputs[2] = EFI_ADC_15;  // VREF from the safety board
	engineConfiguration->auxAnalogInputs[3] = EFI_ADC_12;  // 48 V starter battery
	engineConfiguration->auxAnalogInputs[4] = EFI_ADC_13;  // fuel pump current

	// Lua PWM outputs, driven by genset.lua with setPwmDuty(0..7)
	engineConfiguration->luaOutputPins[0] = Gpio::D13;  // heartbeat to the safety board
	engineConfiguration->luaOutputPins[1] = Gpio::D12;  // contactor request
	engineConfiguration->luaOutputPins[2] = Gpio::D11;  // starter enable
	engineConfiguration->luaOutputPins[3] = Gpio::B9;   // starter command, 1 kHz PWM -> 0.8-4.2 V
	engineConfiguration->luaOutputPins[4] = Gpio::E13;  // fuel pump
	engineConfiguration->luaOutputPins[5] = Gpio::E14;  // coolant pump
	engineConfiguration->luaOutputPins[6] = Gpio::B8;   // radiator fan
	engineConfiguration->luaOutputPins[7] = Gpio::D14;  // green status LED
	// Lua's own pump, fan and starter logic replaces rusEFI's
	engineConfiguration->fuelPumpPin = Gpio::Unassigned;
	engineConfiguration->mainRelayPin = Gpio::Unassigned;

	// 2 x MAX31855K on SPI2: EGT1 = hot-arc housing, EGT2 = charge outlet
	engineConfiguration->is_enabled_spi_2 = true;
	engineConfiguration->spi2sckPin = Gpio::B13;
	engineConfiguration->spi2misoPin = Gpio::B14;
	engineConfiguration->spi2mosiPin = Gpio::B15;
	engineConfiguration->max31855spiDevice = SPI_DEVICE_2;
	engineConfiguration->max31855_cs[0] = Gpio::B12;
	engineConfiguration->max31855_cs[1] = Gpio::D8;

	// micro-SD logging on SPI3
	engineConfiguration->is_enabled_spi_3 = true;
	engineConfiguration->spi3sckPin = Gpio::C10;
	engineConfiguration->spi3misoPin = Gpio::C11;
	engineConfiguration->spi3mosiPin = Gpio::C12;
	engineConfiguration->sdCardSpiDevice = SPI_DEVICE_3;
	engineConfiguration->sdCardCsPin = Gpio::D2;
	engineConfiguration->isSdCardEnabled = true;

	engineConfiguration->canTxPin = Gpio::D1;
	engineConfiguration->canRxPin = Gpio::D0;
}

static void setGensetAnalogScaling() {
	engineConfiguration->adcVcc = 3.3f;
	// 5 V sensors reach the ADC through 10k / 18k
	engineConfiguration->analogInputDividerCoefficient = (10.0f + 18.0f) / 18.0f;
	// 12 V through 47k / 10k
	engineConfiguration->vbattDividerCoeff = (47.0f + 10.0f) / 10.0f;
	// NTCs pulled up to VDDA through 2.49k
	engineConfiguration->clt.config.bias_resistor = 2490;
	engineConfiguration->iat.config.bias_resistor = 2490;

	// MPXH6115A on the board: 0.2 V = 15 kPa, 4.7 V = 115 kPa (Vs = 5 V)
	engineConfiguration->map.sensor.type = MT_CUSTOM;
	engineConfiguration->mapLowValueVoltage = 0.2f;
	engineConfiguration->map.sensor.lowValue = 15;
	engineConfiguration->mapHighValueVoltage = 4.7f;
	engineConfiguration->map.sensor.highValue = 115;

	// 0-100 psi gauge rail-pressure sensor, 0.5-4.5 V
	engineConfiguration->lowPressureFuel.v1 = 0.5f;
	engineConfiguration->lowPressureFuel.value1 = 0;
	engineConfiguration->lowPressureFuel.v2 = 4.5f;
	engineConfiguration->lowPressureFuel.value2 = 689.5f;

	// Fixed pedal dividers read as 0 %: 39k/10k and 82k/10k from 3.3 V, scaled by the divider coefficient above
	engineConfiguration->throttlePedalUpVoltage = 1.05f;
	engineConfiguration->throttlePedalWOTVoltage = 4.05f;
	engineConfiguration->throttlePedalSecondaryUpVoltage = 0.56f;
	engineConfiguration->throttlePedalSecondaryWOTVoltage = 3.56f;
}

// engine base settings, in default_tune.cpp
void boardTuneDefaults();

static void customBoardDefaultConfiguration() {
	setGensetPins();
	setGensetAnalogScaling();
	boardTuneDefaults();
}

void setup_custom_board_overrides() {
	setupBoardHardwareTestOverrides();
	custom_board_DefaultConfiguration = customBoardDefaultConfiguration;
}
