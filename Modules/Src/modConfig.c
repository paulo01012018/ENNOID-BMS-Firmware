#include "modConfig.h"

modConfigGeneralConfigStructTypedef modConfigGeneralConfig;

modConfigGeneralConfigStructTypedef* modConfigInit(void) {
	driverSWStorageManagerConfigStructSize = (sizeof(modConfigGeneralConfigStructTypedef)/sizeof(uint16_t)); // Calculate the space needed for the config struct in EEPROM
	return &modConfigGeneralConfig;
};

bool modConfigStoreAndLoadDefaultConfig(void) {
	bool returnVal = false;
	if(driverSWStorageManagerConfigEmpty) {
		returnVal = modConfigStoreDefaultConfig();
	}
	
	modConfigLoadConfig();
	return returnVal;
};

bool modConfigStoreConfig(void) {
	return driverSWStorageManagerStoreStruct(&modConfigGeneralConfig,STORAGE_CONFIG);
	// TODO_EEPROM
};

bool modConfigLoadConfig(void) {
	return driverSWStorageManagerGetStruct(&modConfigGeneralConfig,STORAGE_CONFIG);
};

bool modConfigStoreDefaultConfig(void) {
	// VALUES WILL ONLY AUTIMATICALLY UPDATE AFTER FLASH ERASE!
	modConfigGeneralConfigStructTypedef defaultConfig;
  modConfigLoadDefaultConfig(&defaultConfig);
	
	driverSWStorageManagerConfigEmpty = false;
	return driverSWStorageManagerStoreStruct(&defaultConfig,STORAGE_CONFIG);
}

void modconfigHardwareLimitsApply(modConfigGeneralConfigStructTypedef *configLocation) {
	configLocation->maxSimultaneousDischargingCells = MIN(configLocation->maxSimultaneousDischargingCells,HW_LIM_CELL_BALANCE_MAX_SIMULTANEOUS_DISCHARGE);
	configLocation->cellMonitorICCount              = MIN(configLocation->cellMonitorICCount             ,HW_LIM_CELL_MONITOR_IC_COUNT);
	configLocation->cellMonitorType                 = HW_LIM_CELL_MONITOR_IC_TYPE;
	
	if(configLocation->notUsedTimeout)
		configLocation->notUsedTimeout                = MAX(configLocation->notUsedTimeout                 ,HW_LIM_MIN_NOT_USED_DELAY);
	
	// Check for feasable cell count config
	configLocation->noOfCellsPerModule = configLocation->noOfCellsSeries/configLocation->cellMonitorICCount;
	
	if((configLocation->noOfCellsPerModule*configLocation->cellMonitorICCount) != configLocation->noOfCellsSeries){
		configLocation->noOfCellsSeries    = 12;
		configLocation->cellMonitorICCount = 1;
		configLocation->noOfCellsPerModule = 1;
	}
}

void modConfigLoadDefaultConfig(modConfigGeneralConfigStructTypedef *configLocation) {
	#ifdef ENNOID
	configLocation->noOfCellsSeries														     = 12;											// Total number of cells in series in the battery pack
	configLocation->noOfCellsParallel                              = 10;                      // Number of cells in parallel
	configLocation->noOfCellsPerModule                             = 12;                      // Number of cell levels monitored per LTC68XX
	configLocation->noOfParallelModules                       	   = 1;                     	// Number of parallel modules
	configLocation->batteryCapacity														     = 22.00f;									// XXAh battery
	configLocation->cellHardUnderVoltage											     = 2.30f;										// Worst case X.XXV as lowest cell voltage
	configLocation->cellHardOverVoltage												     = 4.35f;										// Worst case X.XXV as highest cell voltage
	configLocation->cellLCSoftUnderVoltage											   = 2.70f;										// Low current lowest cell voltage X.XXV.
  configLocation->cellHCSoftUnderVoltage                         = 3.25f;                   // High current lowest cell voltage X.XXV.
	configLocation->cellSoftOverVoltage												     = 4.20f;										// Normal highest cell voltage X.XXV.
	configLocation->cellBalanceDifferenceThreshold                 = 0.01f;										// Start balancing @ XmV difference, stop if below.
	configLocation->cellBalanceStart													     = 3.80f;										// Start balancing above X.XXV.
	configLocation->cellThrottleUpperStart										     = 0.03f;										// Upper range of cell voltage for charge throttling.
	configLocation->cellThrottleLowerStart										     = 0.20f;									  // Lower range of cell voltage for discharge throttling.
	configLocation->cellThrottleUpperMargin										     = 0.01f;										// Margin of throttle from upper soft limits.
	configLocation->cellThrottleLowerMargin										     = 0.50f;									  // Margin of throttle from lower soft limits.	
	configLocation->packVoltageDataSource                          = sourcePackVoltageISL28022_2_0X40_LVBatteryIn; // Packvoltage source.
	configLocation->packCurrentDataSource                          = sourcePackCurrentLowCurrentShunt; // The pack current is the same as the current through the low current shunt
	configLocation->buzzerSignalSource                             = buzzerSourceOff;         // Stores what source shoud be taken to trigger
	configLocation->buzzerSignalType                               = buzzerSignalTypeOn;      // Stores what sound pattern should be made
	configLocation->buzzerSingalPersistant                         = true;                    // Stores whether the buzzer should stay on after triggering
  configLocation->shuntLCFactor                                  = -0.004494f;              // Shunt factor low current
	configLocation->shuntLCOffset                                  = 0;                       // Shunt offset low current
  configLocation->shuntHCFactor	                                 = 0.001f;                  // Shunt factor high current
	configLocation->shuntHCOffset                                  = -4;                      // Shunt offset high current
	configLocation->voltageLCFactor	                               = 1.0f;                  // Voltage factor low current
	configLocation->voltageLCOffset                                = 0;                      // Voltage offset low current
	configLocation->throttleChargeIncreaseRate                     = 1;                       // Percentage charge throttle increase rate per 100ms (cell voltage loop time)  
	configLocation->throttleDisChargeIncreaseRate                  = 2;                       // Percentage discharge throttle increase rate per 100ms (cell voltage loop time)  	
	configLocation->cellBalanceUpdateInterval									     = 4*1000;									// Keep calculated resistors enabled for this amount of time in miliseconds.
	configLocation->maxSimultaneousDischargingCells						     = 5;												// Allow a maximum of X cells simultinous discharging trough bleeding resistors.
	configLocation->timeoutDischargeRetry											     = 4*1000;									// Wait for X seconds before retrying to enable load.
	configLocation->hysteresisDischarge 											     = 0.02f;										// Lowest cell should rise XXmV before output is re enabled.
	configLocation->timeoutChargeRetry												     = 30*1000;									// Wait for XX seconds before retrying to enable charger.
	configLocation->hysteresisCharge													     = 0.01f;										// Highest cell should lower XXmV before charger is re enabled.
	configLocation->timeoutChargeCompleted										     = 30*60*1000;							// Wait for XX minutes before setting charge state to charged.
	configLocation->timeoutChargingCompletedMinimalMismatch 	     = 6*1000;									// If cell mismatch is under threshold and (charging is not allowed) wait this delay time to set "charged" state.
	configLocation->maxMismatchThreshold											     = 0.020f;									// If mismatch is under this threshold for timeoutChargingCompletedMinimalMismatch determin fully charged.
	configLocation->chargerEnabledThreshold										     = 0.5f;										// If charge current > X.XA stay in charging mode and dont power off.
	configLocation->timeoutChargerDisconnected								     = 2000;										// Wait for X seconds to respond to charger disconnect.
	configLocation->minimalPrechargePercentage								     = 0.80f;										// output should be at a minimal of 80% of input voltage.
	configLocation->timeoutLCPreCharge												     = 3000;											// Precharge error timeout, allow 300ms pre-charge time before declaring load error.
	configLocation->maxAllowedCurrent													     = 120.0f;									// Allow max XXXA trough BMS.
	configLocation->allowedTempBattDischargingMax                  = 75.0f;                   // Max battery temperature where discharging is still allowed
	configLocation->allowedTempBattDischargingMin                  = 0.0f;                  // Min battery temperature where discharging is still allowed
	configLocation->allowedTempBattChargingMax                     = 50.0f;                   // Max battery temperature where charging is still allowed
	configLocation->allowedTempBattChargingMin                     = 0.0f;                    // Min battery temperature where charging is still allowed
	configLocation->allowedTempBMSMax                              = 80.0f;                   // Max BMS operational temperature
	configLocation->allowedTempBMSMin                              = 0.0f;                  // Min BMS operational temperature
	configLocation->displayTimeoutBatteryDead									     = 5000;										// Show battery dead symbol X seconds before going to powerdown in cell voltage error state.
	configLocation->displayTimeoutBatteryError								     = 2000;										// Show error symbol for X seconds before going to powerdown in general error state.
	configLocation->displayTimeoutBatteryErrorPreCharge				     = 10000;										// Show pre charge error for XX seconds.
	configLocation->displayTimeoutSplashScreen								     = 1000;										// Display / INIT splash screen time.
	configLocation->maxUnderAndOverVoltageErrorCount 					     = 5;												// Max count of hard cell voltage errors.
	configLocation->notUsedCurrentThreshold										     = 1.0f;										// If abs(packcurrent) < X.XA consider pack as not used.
	configLocation->notUsedTimeout														     = 20*60*1000;							// If pack is not used for longer than XX minutes disable bms.
	configLocation->stateOfChargeStoreInterval								     = 60*1000;									// Interval in ms to store state of charge information.
	configLocation->stateOfChargeMethod                            = socCoulomb;              // Use coulomb counting for SoC calculation
	configLocation->CANID																			     = 10;											// CAN ID for CAN communication.
	configLocation->CANIDStyle                                     = CANIDStyleVESC;          // CAN ID default Style.
	configLocation->emitStatusOverCAN                              = false;                   // Send status over can.
	configLocation->emitStatusProtocol                             = canEmitProtocolDieBieEngineering; // Can emit protocol set to MG style for backwards compatibility
	configLocation->waterSensorEnableMask                          = 0;                       // Turn all sensors are off
	configLocation->waterSensorThreshold                           = 20.0f;                   // Declare water detected when one of the sensors go above this value
	configLocation->tempEnableMaskBMS                              = 0x0008;									// Bitwise select what sensor to enable for the BMS (internal sensors).
	configLocation->tempEnableMaskBattery                          = 0x0001;									// Bitwise select what sensor to enable for the battery (external sensors).
  configLocation->LCUseDischarge                                 = enabled;                    // Enable or disable the solid state output
	configLocation->LCUsePrecharge                                 = persistent;                    		// Use precharge before enabling main output
	configLocation->NTCTopResistor[modConfigNTCGroupLTCExt]        = 100000;                  // NTC Pullup resistor value
	configLocation->NTCTopResistor[modConfigNTCGroupMasterPCB]     = 100000;                  // NTC Pullup resistor value
	configLocation->NTC25DegResistance[modConfigNTCGroupLTCExt]    = 100000;                  // NTC resistance at 25 degree
	configLocation->NTC25DegResistance[modConfigNTCGroupMasterPCB] = 100000;                  // NTC resistance at 25 degree
	configLocation->NTCBetaFactor[modConfigNTCGroupLTCExt]         = 3590;                    // NTC Beta factor
	configLocation->NTCBetaFactor[modConfigNTCGroupMasterPCB]      = 3590;                    // NTC Beta factor
	configLocation->allowChargingDuringDischarge                   = true;                    // Allow the battery to be charged in normal mode
	configLocation->allowForceOn                                   = false;                   // Allow the BMS to be forced ON by long actuation of the power button
	configLocation->pulseToggleButton                              = true;                    // Select either pulse or toggle power button
	configLocation->togglePowerModeDirectHCDelay                   = true;                    // Select either direct power state control or HC output control with delayed turn off.
	configLocation->useCANSafetyInput                              = false;                   // Use the safety input status from CAN
	configLocation->useCANDelayedPowerDown                         = false;                   // Use delayed power down
	configLocation->cellMonitorType                                = CELL_MON_LTC6804_1;      // Use the new cell voltage monitor
	configLocation->cellMonitorICCount                             = 1;                       // Only one slave IC
	configLocation->externalEnableOperationalState                 = opStateExtNormal;        // Go to normal enable mode
	configLocation->powerDownDelay                                 = 3000;                    // Wait only minimal to turn off
	configLocation->canBusSpeed                                    = canSpeedBaud500k;        // 500k CAN baud
	configLocation->chargeEnableOperationalState                   = opStateChargingModeCharging;// Go to charging mode when a charger is connected
	configLocation->DCDCEnableInverted                             = false;                   // De not invert the DCDC enable signal, DCDC converter is not present on the ESK8 BMS
	configLocation->DCDCTargetVoltage                              = 12.0f;                   // Target voltage for the DCDC converter
	
	// Slave / HiAmp Config
	configLocation->NTCTopResistor[modConfigNTCGroupHiAmpExt]      = 100000;                  // NTC Pullup resistor value
	configLocation->NTCTopResistor[modConfigNTCGroupHiAmpPCB]      = 100000;                  // NTC Pullup resistor value
	configLocation->NTCTopResistor[modConfigNTCGroupHiAmpAUX]      = 100000;                  // NTC Pullup resistor value
	configLocation->NTC25DegResistance[modConfigNTCGroupHiAmpExt]  = 100000;                  // NTC resistance at 25 degree
	configLocation->NTC25DegResistance[modConfigNTCGroupHiAmpPCB]  = 100000;                  // NTC resistance at 25 degree
	configLocation->NTC25DegResistance[modConfigNTCGroupHiAmpAUX]  = 100000;                  // NTC resistance at 25 degree	
	configLocation->NTCBetaFactor[modConfigNTCGroupHiAmpExt]       = 4390;                    // NTC Beta factor
	configLocation->NTCBetaFactor[modConfigNTCGroupHiAmpPCB]       = 4100;                    // NTC Beta factor
	configLocation->NTCBetaFactor[modConfigNTCGroupHiAmpAUX]       = 4390;                    // NTC Beta factor
	configLocation->HCUseRelay                                     = false;                   // Enable or disable the relay output, when false will also disable HC pre charge.
	configLocation->HCUsePrecharge                                 = false;                    // choice whether to precharge or not, will only work when HCUseRelay = true.
	configLocation->HCUseLoadDetect                                = false;                   // Use voltage drop based load detect on high current load.
	configLocation->HCLoadDetectThreshold                          = 2000;                    // When precharging takes longer then this assume that a load is present
	configLocation->HCLoadVoltageDataSource                        = sourceLoadHCVoltageNone; // select the HC load voltage source.
	configLocation->HCLoadCurrentDataSource                        = sourceLoadHCCurrentNone; // Select the HC load current source.
	configLocation->timeoutHCPreCharge													   = 300;											// Precharge error timeout, allow xxxms pre-charge time before declaring load error.
	configLocation->timeoutHCPreChargeRetryInterval						     = 20000;										// When pre charge failes wait this long in ms
	configLocation->timeoutHCRelayOverlap											     = 1000;										// When precharge succeeds enable both relay and precharge combined for this time, then go to relay only.
#endif

#ifdef DSBTSolar
	configLocation->noOfCellsSeries														     = 12;											// Total number of cells in series in the battery pack
	configLocation->noOfCellsParallel                              = 16;                      // For the efoil battery this will always be this
	configLocation->noOfCellsPerModule                             = 1;                       // Number of cell levels monitored per LTC68XX
	configLocation->noOfParallelModules                       	   = 1;                     	// Number of parallel modules
	configLocation->batteryCapacity														     = 40.00f;									// XXAh battery
	configLocation->cellHardUnderVoltage											     = 2.30f;										// Worst case X.XXV as lowest cell voltage
	configLocation->cellHardOverVoltage												     = 4.20f;										// Worst case X.XXV as highest cell voltage
	configLocation->cellLCSoftUnderVoltage											   = 2.90f;										// Low current lowest cell voltage X.XXV.
  configLocation->cellHCSoftUnderVoltage                         = 3.00f;                   // High current lowest cell voltage X.XXV.
	configLocation->cellSoftOverVoltage												     = 4.15f;										// Normal highest cell voltage X.XXV.
	configLocation->cellBalanceDifferenceThreshold                 = 0.005f;									// Start balancing @ XmV difference, stop if below.
	configLocation->cellBalanceStart													     = 3.90f;										// Start balancing above X.XXV.
	configLocation->cellThrottleUpperStart										     = 0.02f;										// Upper range of cell voltage for charge throttling.
	configLocation->cellThrottleLowerStart										     = 0.20f;									  // Lower range of cell voltage for discharge throttling.
	configLocation->cellThrottleUpperMargin										     = 0.01f;										// Margin of throttle from upper soft limits.
	configLocation->cellThrottleLowerMargin										     = 0.20f;									  // Margin of throttle from lower soft limits.
	configLocation->packVoltageDataSource                          = sourcePackVoltageISL28022_2_0X40_LVBatteryIn; // Packvoltage source.
	configLocation->packCurrentDataSource                          = sourcePackCurrentHighCurrentShunt;  // The pack current is the same as the current through the high current shunt
	configLocation->buzzerSignalSource                             = buzzerSourceWater;       // Stores what source shoud be taken to trigger
	configLocation->buzzerSignalType                               = buzzerSignalTypePulse1000_4; // Stores what sound pattern should be made
	configLocation->buzzerSingalPersistant                         = true;                    // Stores whether the buzzer should stay on after triggering
  configLocation->shuntLCFactor                                  = -0.0052f;                // Shunt factor low current
	configLocation->shuntLCOffset                                  = 0;                       // Shunt offset low current
  configLocation->shuntHCFactor	                                 = -0.0269f;                 // Shunt factor high current
	configLocation->shuntHCOffset                                  = -2;                      // Shunt offset high current
	configLocation->voltageLCFactor	                               = 1.0f;                  // Voltage factor low current
	configLocation->voltageLCOffset                                = 0;                      // Voltage offset low current
	configLocation->throttleChargeIncreaseRate                     = 2;                       // Percentage charge throttle increase rate per 100ms (cell voltage loop time)  
	configLocation->throttleDisChargeIncreaseRate                  = 10;                      // Percentage discharge throttle increase rate per 100ms (cell voltage loop time)  	
	configLocation->cellBalanceUpdateInterval									     = 4*1000;									// Keep calculated resistors enabled for this amount of time in miliseconds.
	configLocation->maxSimultaneousDischargingCells						     = 5;												// Allow a maximum of X cells simultinous discharging trough bleeding resistors.
	configLocation->timeoutDischargeRetry											     = 4*1000;									// Wait for X seconds before retrying to enable load.
	configLocation->hysteresisDischarge 											     = 0.02f;										// Lowest cell should rise XXmV before output is re enabled.
	configLocation->timeoutChargeRetry												     = 30*1000;									// Wait for XX seconds before retrying to enable charger.
	configLocation->hysteresisCharge													     = 0.01f;										// Highest cell should lower XXmV before charger is re enabled.
	configLocation->timeoutChargeCompleted										     = 30*60*1000;							// Wait for XX minutes before setting charge state to charged.
	configLocation->timeoutChargingCompletedMinimalMismatch 	     = 6*1000;									// If cell mismatch is under threshold and (charging is not allowed) wait this delay time to set "charged" state.
	configLocation->maxMismatchThreshold											     = 0.020f;									// If mismatch is under this threshold for timeoutChargingCompletedMinimalMismatch determin fully charged.
	configLocation->chargerEnabledThreshold										     = 0.5f;										// If charge current > X.XA stay in charging mode and dont power off.
	configLocation->timeoutChargerDisconnected								     = 2000;										// Wait for X seconds to respond to charger disconnect.
	configLocation->minimalPrechargePercentage								     = 0.80f;										// output should be at a minimal of 80% of input voltage.
	configLocation->timeoutLCPreCharge												     = 300;											// Precharge error timeout, allow 300ms pre-charge time before declaring load error.
	configLocation->maxAllowedCurrent													     = 120.0f;									// Allow max XXXA trough BMS.
	configLocation->allowedTempBattDischargingMax                  = 75.0f;                   // Max battery temperature where discharging is still allowed
	configLocation->allowedTempBattDischargingMin                  = -10.0f;                  // Min battery temperature where discharging is still allowed
	configLocation->allowedTempBattChargingMax                     = 55.0f;                   // Max battery temperature where charging is still allowed
	configLocation->allowedTempBattChargingMin                     = 0.0f;                    // Min battery temperature where charging is still allowed
	configLocation->allowedTempBMSMax                              = 90.0f;                   // Max BMS operational temperature
	configLocation->allowedTempBMSMin                              = -20.0f;                  // Min BMS operational temperature
	configLocation->displayTimeoutBatteryDead									     = 5000;										// Show battery dead symbol X seconds before going to powerdown in cell voltage error state.
	configLocation->displayTimeoutBatteryError								     = 2000;										// Show error symbol for X seconds before going to powerdown in general error state.
	configLocation->displayTimeoutBatteryErrorPreCharge				     = 10000;										// Show pre charge error for XX seconds.
	configLocation->displayTimeoutSplashScreen								     = 1000;										// Display / INIT splash screen time.
	configLocation->maxUnderAndOverVoltageErrorCount 					     = 5;												// Max count of hard cell voltage errors.
	configLocation->notUsedCurrentThreshold										     = 0.5f;										// If abs(packcurrent) < X.XA consider pack as not used.
	configLocation->notUsedTimeout														     = 60*60*1000;							// If pack is not used for longer than XX minutes disable bms.
	configLocation->stateOfChargeStoreInterval								     = 60*1000;									// Interval in ms to store state of charge information.
	configLocation->stateOfChargeMethod                            = socCoulomb;              // Use coulomb counting for SoC calculation
	configLocation->CANID																			     = 10;											// CAN ID for CAN communication.
	configLocation->CANIDStyle                                     = CANIDStyleFoiler;        // CAN ID default Style
	configLocation->emitStatusOverCAN                              = true;                    // Send status over can
	configLocation->emitStatusProtocol                             = canEmitProtocolMGElectronics; // Can emit protocol set to MG style for backwards compatibility
	configLocation->waterSensorEnableMask                          = 240;                     // Turn all sensors are off // 1008
	configLocation->waterSensorThreshold                           = 10.0f;                   // Declare water detected when one of the sensors go above this value
	configLocation->tempEnableMaskBMS                              = 0x1C08;									// Bitwise select what sensor to enable for the BMS (internal sensors).
	configLocation->tempEnableMaskBattery                          = 0x0001;									// Bitwise select what sensor to enable for the battery (external sensors). this was the same as above
  configLocation->LCUseDischarge                                 = enabled;                    // Enable or disable the solid state output
	configLocation->LCUsePrecharge                                 = enabled;                    // Use precharge before enabling main output
	configLocation->NTCTopResistor[modConfigNTCGroupLTCExt]        = 100000;                  // NTC Pullup resistor value
	configLocation->NTCTopResistor[modConfigNTCGroupMasterPCB]     = 100000;                  // NTC Pullup resistor value
	configLocation->NTC25DegResistance[modConfigNTCGroupLTCExt]    = 100000;                  // NTC resistance at 25 degree
	configLocation->NTC25DegResistance[modConfigNTCGroupMasterPCB] = 100000;                  // NTC resistance at 25 degree
	configLocation->NTCBetaFactor[modConfigNTCGroupLTCExt]         = 4100;                    // NTC Beta factor
	configLocation->NTCBetaFactor[modConfigNTCGroupMasterPCB]      = 4100;                    // NTC Beta factor
	configLocation->allowChargingDuringDischarge                   = true;                    // Allow the battery to be charged in normal mode
	configLocation->allowForceOn                                   = false;                   // Allow the BMS to be forced ON by long actuation of the power button
	configLocation->pulseToggleButton                              = true;                    // Select either pulse or toggle power button
	configLocation->togglePowerModeDirectHCDelay                   = true;                    // Select either direct power state control or HC output control with delayed turn off.
	configLocation->useCANSafetyInput                              = false;                    // Use the safety input status from CAN
	configLocation->useCANDelayedPowerDown                         = false;                    // Use delayed power down
	configLocation->cellMonitorType                                = CELL_MON_LTC6804_1;      // Use the new cell voltage monitor
	configLocation->cellMonitorICCount                             = 1;                       // Only one slave IC
	configLocation->externalEnableOperationalState                 = opStateExtNormal;        // Go to normal enable mode
	configLocation->powerDownDelay                                 = 1000;                    // Wait only minimal to turn off
	configLocation->canBusSpeed                                    = canSpeedBaud125k;        // 500k CAN baud
	configLocation->chargeEnableOperationalState                   = opStateChargingModeNormal;// Go to normal mode when a charger is connected
	configLocation->DCDCEnableInverted                             = true;                    // Invert the DCDC enable signal
	configLocation->DCDCTargetVoltage                              = 12.0f;                   // Target voltage for the DCDC converter
	
	// Slave / HiAmp Config
	configLocation->NTCTopResistor[modConfigNTCGroupHiAmpExt]      = 100000;                  // NTC Pullup resistor value
	configLocation->NTCTopResistor[modConfigNTCGroupHiAmpPCB]      = 100000;                  // NTC Pullup resistor value
	configLocation->NTCTopResistor[modConfigNTCGroupHiAmpAUX]      = 100000;                  // NTC Pullup resistor value
	configLocation->NTC25DegResistance[modConfigNTCGroupHiAmpExt]  = 100000;                  // NTC resistance at 25 degree
	configLocation->NTC25DegResistance[modConfigNTCGroupHiAmpPCB]  = 100000;                  // NTC resistance at 25 degree
	configLocation->NTC25DegResistance[modConfigNTCGroupHiAmpAUX]  = 100000;                  // NTC resistance at 25 degree	
	configLocation->NTCBetaFactor[modConfigNTCGroupHiAmpExt]       = 4390;                    // NTC Beta factor
	configLocation->NTCBetaFactor[modConfigNTCGroupHiAmpPCB]       = 4100;                    // NTC Beta factor
	configLocation->NTCBetaFactor[modConfigNTCGroupHiAmpAUX]       = 4390;                    // NTC Beta factor	
	configLocation->HCUseRelay                                     = true;                    // Enable or disable the relay output, when false will also disable HC pre charge.
	configLocation->HCUsePrecharge                                 = true;                    // choice whether to precharge or not, will only work when HCUseRelay = true.
	configLocation->HCUseLoadDetect                                = false;                    // Use voltage drop based load detect on high current load.
	configLocation->HCLoadDetectThreshold                          = 300 ;                    // When precharging takes longer then this assume that a load is present
	configLocation->HCLoadVoltageDataSource                        = sourceLoadHCVoltageISL28022_1_0X44_LVLoadOutput; // select the HC load voltage source.
	configLocation->HCLoadCurrentDataSource                        = sourceLoadHCCurrentISL28022_1_0X44_LVHCShunt; // Select the HC load current source.
	configLocation->timeoutHCPreCharge													   = 1500;										// Precharge error timeout, allow xxxms pre-charge time before declaring load error.
	configLocation->timeoutHCPreChargeRetryInterval						     = 20000;										// When pre charge failes wait this long in ms
	configLocation->timeoutHCRelayOverlap											     = 1000;										// When precharge succeeds enable both relay and precharge combined for this time, then go to relay only.
#endif
	
#ifdef TDHVSolar
	configLocation->noOfCellsSeries														     = 35;											// Total number of cells in series in the battery pack
	configLocation->noOfCellsParallel                              = 12;                      // For the efoil battery this will always be this
	configLocation->noOfCellsPerModule                             = 7;                       // Number of cell levels monitored per LTC68XX
	configLocation->noOfParallelModules                       	   = 1;                     	// Number of parallel modules
	configLocation->batteryCapacity														     = 40.00f;									// XXAh battery
	configLocation->cellHardUnderVoltage											     = 2.30f;										// Worst case X.XXV as lowest cell voltage
	configLocation->cellHardOverVoltage												     = 4.20f;										// Worst case X.XXV as highest cell voltage
	configLocation->cellLCSoftUnderVoltage											   = 2.90f;										// Low current lowest cell voltage X.XXV.
  configLocation->cellHCSoftUnderVoltage                         = 3.00f;                   // High current lowest cell voltage X.XXV.
	configLocation->cellSoftOverVoltage												     = 4.15f;										// Normal highest cell voltage X.XXV.
	configLocation->cellBalanceDifferenceThreshold                 = 0.005f;									// Start balancing @ XmV difference, stop if below.
	configLocation->cellBalanceStart													     = 3.90f;										// Start balancing above X.XXV.
	configLocation->cellThrottleUpperStart										     = 0.02f;										// Upper range of cell voltage for charge throttling.
	configLocation->cellThrottleLowerStart										     = 0.20f;									  // Lower range of cell voltage for discharge throttling.
	configLocation->cellThrottleUpperMargin										     = 0.01f;										// Margin of throttle from upper soft limits.
	configLocation->cellThrottleLowerMargin										     = 0.20f;									  // Margin of throttle from lower soft limits.
	configLocation->packVoltageDataSource                          = sourcePackVoltageADS1015_AN01_HVBatteryIn; // Packvoltage source.
	configLocation->packCurrentDataSource                          = sourcePackCurrentNone;   // The pack current is the same as the current through the high current shunt
	configLocation->buzzerSignalSource                             = buzzerSourceOff;         // Stores what source shoud be taken to trigger
	configLocation->buzzerSignalType                               = buzzerSignalTypeOff;     // Stores what sound pattern should be made
	configLocation->buzzerSingalPersistant                         = true;                    // Stores whether the buzzer should stay on after triggering
  configLocation->shuntLCFactor                                  = -0.0052f;                // Shunt factor low current
	configLocation->shuntLCOffset                                  = 0;                       // Shunt offset low current
  configLocation->shuntHCFactor	                                 = -0.0269f;                 // Shunt factor high current
	configLocation->shuntHCOffset                                  = -2;                      // Shunt offset high current
	configLocation->voltageLCFactor	                               = 1.0f;                  // Voltage factor low current
	configLocation->voltageLCOffset                                = 0;                      // Voltage offset low current
	configLocation->throttleChargeIncreaseRate                     = 2;                       // Percentage charge throttle increase rate per 100ms (cell voltage loop time)  
	configLocation->throttleDisChargeIncreaseRate                  = 10;                      // Percentage discharge throttle increase rate per 100ms (cell voltage loop time)  	
	configLocation->cellBalanceUpdateInterval									     = 4*1000;									// Keep calculated resistors enabled for this amount of time in miliseconds.
	configLocation->maxSimultaneousDischargingCells						     = 5;												// Allow a maximum of X cells simultinous discharging trough bleeding resistors.
	configLocation->timeoutDischargeRetry											     = 4*1000;									// Wait for X seconds before retrying to enable load.
	configLocation->hysteresisDischarge 											     = 0.02f;										// Lowest cell should rise XXmV before output is re enabled.
	configLocation->timeoutChargeRetry												     = 30*1000;									// Wait for XX seconds before retrying to enable charger.
	configLocation->hysteresisCharge													     = 0.01f;										// Highest cell should lower XXmV before charger is re enabled.
	configLocation->timeoutChargeCompleted										     = 30*60*1000;							// Wait for XX minutes before setting charge state to charged.
	configLocation->timeoutChargingCompletedMinimalMismatch 	     = 6*1000;									// If cell mismatch is under threshold and (charging is not allowed) wait this delay time to set "charged" state.
	configLocation->maxMismatchThreshold											     = 0.020f;									// If mismatch is under this threshold for timeoutChargingCompletedMinimalMismatch determin fully charged.
	configLocation->chargerEnabledThreshold										     = 0.5f;										// If charge current > X.XA stay in charging mode and dont power off.
	configLocation->timeoutChargerDisconnected								     = 2000;										// Wait for X seconds to respond to charger disconnect.
	configLocation->minimalPrechargePercentage								     = 0.80f;										// output should be at a minimal of 80% of input voltage.
	configLocation->timeoutLCPreCharge												     = 300;											// Precharge error timeout, allow 300ms pre-charge time before declaring load error.
	configLocation->maxAllowedCurrent													     = 120.0f;									// Allow max XXXA trough BMS.
	configLocation->allowedTempBattDischargingMax                  = 75.0f;                   // Max battery temperature where discharging is still allowed
	configLocation->allowedTempBattDischargingMin                  = -10.0f;                  // Min battery temperature where discharging is still allowed
	configLocation->allowedTempBattChargingMax                     = 55.0f;                   // Max battery temperature where charging is still allowed
	configLocation->allowedTempBattChargingMin                     = 0.0f;                    // Min battery temperature where charging is still allowed
	configLocation->allowedTempBMSMax                              = 90.0f;                   // Max BMS operational temperature
	configLocation->allowedTempBMSMin                              = -20.0f;                  // Min BMS operational temperature
	configLocation->displayTimeoutBatteryDead									     = 5000;										// Show battery dead symbol X seconds before going to powerdown in cell voltage error state.
	configLocation->displayTimeoutBatteryError								     = 2000;										// Show error symbol for X seconds before going to powerdown in general error state.
	configLocation->displayTimeoutBatteryErrorPreCharge				     = 10000;										// Show pre charge error for XX seconds.
	configLocation->displayTimeoutSplashScreen								     = 1000;										// Display / INIT splash screen time.
	configLocation->maxUnderAndOverVoltageErrorCount 					     = 5;												// Max count of hard cell voltage errors.
	configLocation->notUsedCurrentThreshold										     = 0.5f;										// If abs(packcurrent) < X.XA consider pack as not used.
	configLocation->notUsedTimeout														     = 0;							          // If pack is not used for longer than XX minutes disable bms. 0 -> not used
	configLocation->stateOfChargeStoreInterval								     = 60*1000;									// Interval in ms to store state of charge information.
	configLocation->stateOfChargeMethod                            = socCoulomb;              // Use coulomb counting for SoC calculation
	configLocation->CANID																			     = 10;											// CAN ID for CAN communication.
	configLocation->CANIDStyle                                     = CANIDStyleVESC;          // CAN ID default Style
	configLocation->emitStatusOverCAN                              = true;                    // Send status over can
	configLocation->emitStatusProtocol                             = canEmitProtocolDieBieEngineering; // Can emit protocol set to MG style for backwards compatibility
	configLocation->waterSensorEnableMask                          = 0x0000;                  // Turn all sensors are off // 1008
	configLocation->waterSensorThreshold                           = 10.0f;                   // Declare water detected when one of the sensors go above this value
	configLocation->tempEnableMaskBMS                              = 0x00F0;									// Bitwise select what sensor to enable for the BMS (internal sensors).
	configLocation->tempEnableMaskBattery                          = 0x0001;									// Bitwise select what sensor to enable for the battery (external sensors). this was the same as above
  configLocation->LCUseDischarge                                 = enabled;                   // Enable or disable the solid state output
	configLocation->LCUsePrecharge                                 = enabled;                   // Use precharge before enabling main output
	configLocation->NTCTopResistor[modConfigNTCGroupLTCExt]        = 100000;                  // NTC Pullup resistor value
	configLocation->NTCTopResistor[modConfigNTCGroupMasterPCB]     = 100000;                  // NTC Pullup resistor value
	configLocation->NTC25DegResistance[modConfigNTCGroupLTCExt]    = 100000;                  // NTC resistance at 25 degree
	configLocation->NTC25DegResistance[modConfigNTCGroupMasterPCB] = 100000;                  // NTC resistance at 25 degree
	configLocation->NTCBetaFactor[modConfigNTCGroupLTCExt]         = 4100;                    // NTC Beta factor
	configLocation->NTCBetaFactor[modConfigNTCGroupMasterPCB]      = 4100;                    // NTC Beta factor
	configLocation->allowChargingDuringDischarge                   = true;                    // Allow the battery to be charged in normal mode
	configLocation->allowForceOn                                   = false;                   // Allow the BMS to be forced ON by long actuation of the power button
	configLocation->pulseToggleButton                              = false;                   // Select either pulse or toggle power button
	configLocation->togglePowerModeDirectHCDelay                   = true;                    // Select either direct power state control or HC output control with delayed turn off.
	configLocation->useCANSafetyInput                              = false;                   // Use the safety input status from CAN
	configLocation->useCANDelayedPowerDown                         = false;                   // Use delayed power down
	configLocation->cellMonitorType                                = CELL_MON_LTC6804_1;      // Use the new cell voltage monitor
	configLocation->cellMonitorICCount                             = 5;                       // Only one slave IC
	configLocation->externalEnableOperationalState                 = opStateExtNormal;        // Go to normal enable mode
	configLocation->powerDownDelay                                 = 1000;                    // Wait only minimal to turn off
	configLocation->canBusSpeed                                    = canSpeedBaud500k;        // 500k CAN baud
	configLocation->chargeEnableOperationalState                   = opStateChargingModeNormal;// Go to normal mode when a charger is connected
	configLocation->DCDCEnableInverted                             = true;                    // Invert the DCDC enable signal
	configLocation->DCDCTargetVoltage                              = 24.0f;                   // Target voltage for the DCDC converter
	
	// Slave / HiAmp Config
	configLocation->NTCTopResistor[modConfigNTCGroupHiAmpExt]      = 100000;                  // NTC Pullup resistor value
	configLocation->NTCTopResistor[modConfigNTCGroupHiAmpPCB]      = 100000;                  // NTC Pullup resistor value
	configLocation->NTCTopResistor[modConfigNTCGroupHiAmpAUX]      = 100000;                  // NTC Pullup resistor value
	configLocation->noOfParallelModules                       	   = 1;                     	// Number of parallel modules
	configLocation->NTC25DegResistance[modConfigNTCGroupHiAmpExt]  = 100000;                  // NTC resistance at 25 degree
	configLocation->NTC25DegResistance[modConfigNTCGroupHiAmpPCB]  = 100000;                  // NTC resistance at 25 degree
	configLocation->NTC25DegResistance[modConfigNTCGroupHiAmpAUX]  = 100000;                  // NTC resistance at 25 degree	
	configLocation->NTCBetaFactor[modConfigNTCGroupHiAmpExt]       = 4390;                    // NTC Beta factor
	configLocation->NTCBetaFactor[modConfigNTCGroupHiAmpPCB]       = 4100;                    // NTC Beta factor
	configLocation->NTCBetaFactor[modConfigNTCGroupHiAmpAUX]       = 4390;                    // NTC Beta factor	
	configLocation->HCUseRelay                                     = true;                    // Enable or disable the relay output, when false will also disable HC pre charge.
	configLocation->HCUsePrecharge                                 = true;                    // choice whether to precharge or not, will only work when HCUseRelay = true.
	configLocation->HCUseLoadDetect                                = false;                    // Use voltage drop based load detect on high current load.
	configLocation->HCLoadDetectThreshold                          = 300 ;                    // When precharging takes longer then this assume that a load is present
	configLocation->HCLoadVoltageDataSource                        = sourceLoadHCVoltageADS1015_AN23_HVLoadOut; // select the HC load voltage source.
	configLocation->HCLoadCurrentDataSource                        = sourceLoadHCCurrentNone; // Select the HC load current source.
	configLocation->timeoutHCPreCharge													   = 5000;										// Precharge error timeout, allow xxxms pre-charge time before declaring load error.
	configLocation->timeoutHCPreChargeRetryInterval						     = 20000;										// When pre charge failes wait this long in ms
	configLocation->timeoutHCRelayOverlap											     = 1000;										// When precharge succeeds enable both relay and precharge combined for this time, then go to relay only.
#endif
	
#ifdef EFoilV2
	configLocation->noOfCellsSeries														     = 12;											// Total number of cells in series in the battery pack
	configLocation->noOfCellsParallel                              = 16;                      // For the efoil battery this will always be this
	configLocation->noOfCellsPerModule                             = 1;                       // Number of cell levels monitored per LTC68XX
	configLocation->noOfParallelModules                       	   = 1;                     	// Number of parallel modules
	configLocation->batteryCapacity														     = 40.00f;									// XXAh battery
	configLocation->cellHardUnderVoltage											     = 2.30f;										// Worst case X.XXV as lowest cell voltage
	configLocation->cellHardOverVoltage												     = 4.20f;										// Worst case X.XXV as highest cell voltage
	configLocation->cellLCSoftUnderVoltage											   = 2.90f;										// Low current lowest cell voltage X.XXV.
  configLocation->cellHCSoftUnderVoltage                         = 3.00f;                   // High current lowest cell voltage X.XXV.
	configLocation->cellSoftOverVoltage												     = 4.15f;										// Normal highest cell voltage X.XXV.
	configLocation->cellBalanceDifferenceThreshold                 = 0.005f;									// Start balancing @ XmV difference, stop if below.
	configLocation->cellBalanceStart													     = 3.90f;										// Start balancing above X.XXV.
	configLocation->cellThrottleUpperStart										     = 0.02f;										// Upper range of cell voltage for charge throttling.
	configLocation->cellThrottleLowerStart										     = 0.20f;									  // Lower range of cell voltage for discharge throttling.
	configLocation->cellThrottleUpperMargin										     = 0.01f;										// Margin of throttle from upper soft limits.
	configLocation->cellThrottleLowerMargin										     = 0.20f;									  // Margin of throttle from lower soft limits.
	configLocation->packVoltageDataSource                          = sourcePackVoltageISL28022_2_0X40_LVBatteryIn; // Packvoltage source.
	configLocation->packCurrentDataSource                          = sourcePackCurrentHighCurrentShunt;  // The pack current is the same as the current through the high current shunt
	configLocation->buzzerSignalSource                             = buzzerSourceWater;       // Stores what source shoud be taken to trigger
	configLocation->buzzerSignalType                               = buzzerSignalTypePulse1000_4; // Stores what sound pattern should be made
	configLocation->buzzerSingalPersistant                         = true;                    // Stores whether the buzzer should stay on after triggering
  configLocation->shuntLCFactor                                  = -0.0052f;                // Shunt factor low current
	configLocation->shuntLCOffset                                  = 0;                       // Shunt offset low current
  configLocation->shuntHCFactor	                                 = -0.0269f;                 // Shunt factor high current
	configLocation->shuntHCOffset                                  = -2;                      // Shunt offset high current
	configLocation->voltageLCFactor	                               = 1.0f;                  // Voltage factor low current
	configLocation->voltageLCOffset                                = 0;                      // Voltage offset low current
	configLocation->throttleChargeIncreaseRate                     = 2;                       // Percentage charge throttle increase rate per 100ms (cell voltage loop time)  
	configLocation->throttleDisChargeIncreaseRate                  = 10;                      // Percentage discharge throttle increase rate per 100ms (cell voltage loop time)  	
	configLocation->cellBalanceUpdateInterval									     = 4*1000;									// Keep calculated resistors enabled for this amount of time in miliseconds.
	configLocation->maxSimultaneousDischargingCells						     = 5;												// Allow a maximum of X cells simultinous discharging trough bleeding resistors.
	configLocation->timeoutDischargeRetry											     = 4*1000;									// Wait for X seconds before retrying to enable load.
	configLocation->hysteresisDischarge 											     = 0.02f;										// Lowest cell should rise XXmV before output is re enabled.
	configLocation->timeoutChargeRetry												     = 30*1000;									// Wait for XX seconds before retrying to enable charger.
	configLocation->hysteresisCharge													     = 0.01f;										// Highest cell should lower XXmV before charger is re enabled.
	configLocation->timeoutChargeCompleted										     = 30*60*1000;							// Wait for XX minutes before setting charge state to charged.
	configLocation->timeoutChargingCompletedMinimalMismatch 	     = 6*1000;									// If cell mismatch is under threshold and (charging is not allowed) wait this delay time to set "charged" state.
	configLocation->maxMismatchThreshold											     = 0.020f;									// If mismatch is under this threshold for timeoutChargingCompletedMinimalMismatch determin fully charged.
	configLocation->chargerEnabledThreshold										     = 0.5f;										// If charge current > X.XA stay in charging mode and dont power off.
	configLocation->timeoutChargerDisconnected								     = 2000;										// Wait for X seconds to respond to charger disconnect.
	configLocation->minimalPrechargePercentage								     = 0.80f;										// output should be at a minimal of 80% of input voltage.
	configLocation->timeoutLCPreCharge												     = 300;											// Precharge error timeout, allow 300ms pre-charge time before declaring load error.
	configLocation->maxAllowedCurrent													     = 120.0f;									// Allow max XXXA trough BMS.
	configLocation->allowedTempBattDischargingMax                  = 75.0f;                   // Max battery temperature where discharging is still allowed
	configLocation->allowedTempBattDischargingMin                  = -10.0f;                  // Min battery temperature where discharging is still allowed
	configLocation->allowedTempBattChargingMax                     = 55.0f;                   // Max battery temperature where charging is still allowed
	configLocation->allowedTempBattChargingMin                     = 0.0f;                    // Min battery temperature where charging is still allowed
	configLocation->allowedTempBMSMax                              = 90.0f;                   // Max BMS operational temperature
	configLocation->allowedTempBMSMin                              = -20.0f;                  // Min BMS operational temperature
	configLocation->displayTimeoutBatteryDead									     = 5000;										// Show battery dead symbol X seconds before going to powerdown in cell voltage error state.
	configLocation->displayTimeoutBatteryError								     = 2000;										// Show error symbol for X seconds before going to powerdown in general error state.
	configLocation->displayTimeoutBatteryErrorPreCharge				     = 10000;										// Show pre charge error for XX seconds.
	configLocation->displayTimeoutSplashScreen								     = 1000;										// Display / INIT splash screen time.
	configLocation->maxUnderAndOverVoltageErrorCount 					     = 5;												// Max count of hard cell voltage errors.
	configLocation->notUsedCurrentThreshold										     = 0.5f;										// If abs(packcurrent) < X.XA consider pack as not used.
	configLocation->notUsedTimeout														     = 60*60*1000;							// If pack is not used for longer than XX minutes disable bms.
	configLocation->stateOfChargeStoreInterval								     = 60*1000;									// Interval in ms to store state of charge information.
	configLocation->stateOfChargeMethod                            = socCoulomb;              // Use coulomb counting for SoC calculation
	configLocation->CANID																			     = 10;											// CAN ID for CAN communication.
	configLocation->CANIDStyle                                     = CANIDStyleFoiler;        // CAN ID default Style
	configLocation->emitStatusOverCAN                              = true;                    // Send status over can
	configLocation->emitStatusProtocol                             = canEmitProtocolDieBieEngineering; // Can emit protocol set to MG style for backwards compatibility
	configLocation->waterSensorEnableMask                          = 240;                     // Turn all sensors are off // 1008
	configLocation->waterSensorThreshold                           = 10.0f;                   // Declare water detected when one of the sensors go above this value
	configLocation->tempEnableMaskBMS                              = 0x3C08;									// Bitwise select what sensor to enable for the BMS (internal sensors).
	configLocation->tempEnableMaskBattery                          = 0x0001;									// Bitwise select what sensor to enable for the battery (external sensors). this was the same as above
  configLocation->LCUseDischarge                                 = enabled;                    // Enable or disable the solid state output
	configLocation->LCUsePrecharge                                 = enabled;                    // Use precharge before enabling main output
	configLocation->NTCTopResistor[modConfigNTCGroupLTCExt]        = 100000;                  // NTC Pullup resistor value
	configLocation->NTCTopResistor[modConfigNTCGroupMasterPCB]     = 100000;                  // NTC Pullup resistor value
	configLocation->NTC25DegResistance[modConfigNTCGroupLTCExt]    = 100000;                  // NTC resistance at 25 degree
	configLocation->NTC25DegResistance[modConfigNTCGroupMasterPCB] = 100000;                  // NTC resistance at 25 degree
	configLocation->NTCBetaFactor[modConfigNTCGroupLTCExt]         = 4100;                    // NTC Beta factor
	configLocation->NTCBetaFactor[modConfigNTCGroupMasterPCB]      = 4100;                    // NTC Beta factor
	configLocation->allowChargingDuringDischarge                   = true;                    // Allow the battery to be charged in normal mode
	configLocation->allowForceOn                                   = false;                   // Allow the BMS to be forced ON by long actuation of the power button
	configLocation->pulseToggleButton                              = true;                    // Select either pulse or toggle power button
	configLocation->togglePowerModeDirectHCDelay                   = true;                    // Select either direct power state control or HC output control with delayed turn off.
	configLocation->useCANSafetyInput                              = true;                    // Use the safety input status from CAN
	configLocation->useCANDelayedPowerDown                         = true;                    // Use delayed power down
	configLocation->cellMonitorType                                = CELL_MON_LTC6804_1;      // Use the new cell voltage monitor
	configLocation->cellMonitorICCount                             = 1;                       // Only one slave IC
	configLocation->externalEnableOperationalState                 = opStateExtNormal;        // Go to normal enable mode
	configLocation->powerDownDelay                                 = 1000;                    // Wait only minimal to turn off
	configLocation->canBusSpeed                                    = canSpeedBaud500k;                // 500k CAN baud
	configLocation->chargeEnableOperationalState                   = opStateChargingModeNormal;// Go to normal mode when a charger is connected
	configLocation->DCDCEnableInverted                             = true;                    // Invert the DCDC enable signal
	configLocation->DCDCTargetVoltage                              = 12.0f;                   // Target voltage for the DCDC converter
	
	// Slave / HiAmp Config
	configLocation->NTCTopResistor[modConfigNTCGroupHiAmpExt]      = 100000;                  // NTC Pullup resistor value
	configLocation->NTCTopResistor[modConfigNTCGroupHiAmpPCB]      = 100000;                  // NTC Pullup resistor value
	configLocation->NTCTopResistor[modConfigNTCGroupHiAmpAUX]      = 100000;                  // NTC Pullup resistor value
	configLocation->NTC25DegResistance[modConfigNTCGroupHiAmpExt]  = 100000;                  // NTC resistance at 25 degree
	configLocation->NTC25DegResistance[modConfigNTCGroupHiAmpPCB]  = 100000;                  // NTC resistance at 25 degree
	configLocation->NTC25DegResistance[modConfigNTCGroupHiAmpAUX]  = 100000;                  // NTC resistance at 25 degree	
	configLocation->NTCBetaFactor[modConfigNTCGroupHiAmpExt]       = 4390;                    // NTC Beta factor
	configLocation->NTCBetaFactor[modConfigNTCGroupHiAmpPCB]       = 4100;                    // NTC Beta factor
	configLocation->NTCBetaFactor[modConfigNTCGroupHiAmpAUX]       = 4390;                    // NTC Beta factor	
	configLocation->HCUseRelay                                     = true;                    // Enable or disable the relay output, when false will also disable HC pre charge.
	configLocation->HCUsePrecharge                                 = true;                    // choice whether to precharge or not, will only work when HCUseRelay = true.
	configLocation->HCUseLoadDetect                                = true;                    // Use voltage drop based load detect on high current load.
	configLocation->HCLoadDetectThreshold                          = 300 ;                    // When precharging takes longer then this assume that a load is present
	configLocation->HCLoadVoltageDataSource                        = sourceLoadHCVoltageISL28022_1_0X44_LVLoadOutput; // select the HC load voltage source.
	configLocation->HCLoadCurrentDataSource                        = sourceLoadHCCurrentISL28022_1_0X44_LVHCShunt; // Select the HC load current source.
	configLocation->timeoutHCPreCharge													   = 1500;										// Precharge error timeout, allow xxxms pre-charge time before declaring load error.
	configLocation->timeoutHCPreChargeRetryInterval						     = 20000;										// When pre charge failes wait this long in ms
	configLocation->timeoutHCRelayOverlap											     = 1000;										// When precharge succeeds enable both relay and precharge combined for this time, then go to relay only.
#endif
	
#ifdef EFoilV1
	configLocation->noOfCellsSeries														     = 12;											// Total number of cells in series in the battery pack
	configLocation->noOfCellsParallel                              = 16;                      // For the efoil battery this will always be this
	configLocation->noOfCellsPerModule                             = 1;                       // Number of cell levels monitored per LTC68XX
	configLocation->noOfParallelModules                       	   = 1;                     	// Number of parallel modules
	configLocation->batteryCapacity														     = 40.00f;									// XXAh battery
	configLocation->cellHardUnderVoltage											     = 2.30f;										// Worst case X.XXV as lowest cell voltage
	configLocation->cellHardOverVoltage												     = 4.25f;										// Worst case X.XXV as highest cell voltage
	configLocation->cellLCSoftUnderVoltage											   = 2.90f;										// Low current lowest cell voltage X.XXV.
  configLocation->cellHCSoftUnderVoltage                         = 3.25f;                   // High current lowest cell voltage X.XXV.
	configLocation->cellSoftOverVoltage												     = 4.15f;										// Normal highest cell voltage X.XXV.
	configLocation->cellBalanceDifferenceThreshold                 = 0.01f;										// Start balancing @ XmV difference, stop if below.
	configLocation->cellBalanceStart													     = 3.80f;										// Start balancing above X.XXV.
	configLocation->cellThrottleUpperStart										     = 0.03f;										// Upper range of cell voltage for charge throttling.
	configLocation->cellThrottleLowerStart										     = 0.20f;									  // Lower range of cell voltage for discharge throttling.
	configLocation->cellThrottleUpperMargin										     = 0.01f;										// Margin of throttle from upper soft limits.
	configLocation->cellThrottleLowerMargin										     = 0.50f;									  // Margin of throttle from lower soft limits.
	configLocation->packVoltageDataSource                          = sourcePackVoltageISL28022_2_0X40_LVBatteryIn; // Packvoltage source.
	configLocation->packCurrentDataSource                          = sourcePackCurrentLowPlusHighCurrentShunt; // The pack current is the same as the current through the high+low current shunt
	configLocation->buzzerSignalSource                             = buzzerSourceOff;         // Stores what source shoud be taken to trigger
	configLocation->buzzerSignalType                               = buzzerSignalTypeOn;      // Stores what sound pattern should be made
	configLocation->buzzerSingalPersistant                         = true;                    // Stores whether the buzzer should stay on after triggering
  configLocation->shuntLCFactor                                  = -0.004494f;              // Shunt factor low current
	configLocation->shuntLCOffset                                  = 0;                       // Shunt offset low current
  configLocation->shuntHCFactor	                                 = -0.038f;                 // Shunt factor high current
	configLocation->shuntHCOffset                                  = 4;                       // Shunt offset high current
	configLocation->voltageLCFactor	                               = 1.0f;                  // Voltage factor low current
	configLocation->voltageLCOffset                                = 0;                      // Voltage offset low current
	configLocation->throttleChargeIncreaseRate                     = 1;                       // Percentage charge throttle increase rate per 100ms (cell voltage loop time)  
	configLocation->throttleDisChargeIncreaseRate                  = 2;                       // Percentage discharge throttle increase rate per 100ms (cell voltage loop time)  	
	configLocation->cellBalanceUpdateInterval									     = 4*1000;									// Keep calculated resistors enabled for this amount of time in miliseconds.
	configLocation->maxSimultaneousDischargingCells						     = 5;												// Allow a maximum of X cells simultinous discharging trough bleeding resistors.
	configLocation->timeoutDischargeRetry											     = 4*1000;									// Wait for X seconds before retrying to enable load.
	configLocation->hysteresisDischarge 											     = 0.02f;										// Lowest cell should rise XXmV before output is re enabled.
	configLocation->timeoutChargeRetry												     = 30*1000;									// Wait for XX seconds before retrying to enable charger.
	configLocation->hysteresisCharge													     = 0.01f;										// Highest cell should lower XXmV before charger is re enabled.
	configLocation->timeoutChargeCompleted										     = 30*60*1000;							// Wait for XX minutes before setting charge state to charged.
	configLocation->timeoutChargingCompletedMinimalMismatch 	     = 6*1000;									// If cell mismatch is under threshold and (charging is not allowed) wait this delay time to set "charged" state.
	configLocation->maxMismatchThreshold											     = 0.020f;									// If mismatch is under this threshold for timeoutChargingCompletedMinimalMismatch determin fully charged.
	configLocation->chargerEnabledThreshold										     = 0.5f;										// If charge current > X.XA stay in charging mode and dont power off.
	configLocation->timeoutChargerDisconnected								     = 2000;										// Wait for X seconds to respond to charger disconnect.
	configLocation->minimalPrechargePercentage								     = 0.80f;										// output should be at a minimal of 80% of input voltage.
	configLocation->timeoutLCPreCharge												     = 300;											// Precharge error timeout, allow 300ms pre-charge time before declaring load error.
	configLocation->maxAllowedCurrent													     = 120.0f;									// Allow max XXXA trough BMS.
	configLocation->allowedTempBattDischargingMax                  = 75.0f;                   // Max battery temperature where discharging is still allowed
	configLocation->allowedTempBattDischargingMin                  = -20.0f;                  // Min battery temperature where discharging is still allowed
	configLocation->allowedTempBattChargingMax                     = 50.0f;                   // Max battery temperature where charging is still allowed
	configLocation->allowedTempBattChargingMin                     = 0.0f;                    // Min battery temperature where charging is still allowed
	configLocation->allowedTempBMSMax                              = 80.0f;                   // Max BMS operational temperature
	configLocation->allowedTempBMSMin                              = -20.0f;                  // Min BMS operational temperature
	configLocation->displayTimeoutBatteryDead									     = 5000;										// Show battery dead symbol X seconds before going to powerdown in cell voltage error state.
	configLocation->displayTimeoutBatteryError								     = 2000;										// Show error symbol for X seconds before going to powerdown in general error state.
	configLocation->displayTimeoutBatteryErrorPreCharge				     = 10000;										// Show pre charge error for XX seconds.
	configLocation->displayTimeoutSplashScreen								     = 1000;										// Display / INIT splash screen time.
	configLocation->maxUnderAndOverVoltageErrorCount 					     = 5;												// Max count of hard cell voltage errors.
	configLocation->notUsedCurrentThreshold										     = 1.0f;										// If abs(packcurrent) < X.XA consider pack as not used.
	configLocation->notUsedTimeout														     = 60*60*1000;							// If pack is not used for longer than XX minutes disable bms.
	configLocation->stateOfChargeStoreInterval								     = 60*1000;									// Interval in ms to store state of charge information.
	configLocation->stateOfChargeMethod                            = socCoulomb;              // Use coulomb counting for SoC calculation
	configLocation->CANID																			     = 10;											// CAN ID for CAN communication.
	configLocation->CANIDStyle                                     = CANIDStyleFoiler;        // CAN ID default Style
	configLocation->emitStatusOverCAN                              = true;                    // Send status over can.
	configLocation->emitStatusProtocol                             = canEmitProtocolDieBieEngineering; // Can emit protocol set to MG style for backwards compatibility
	configLocation->waterSensorEnableMask                          = 0;                       // Turn all sensors are off
	configLocation->waterSensorThreshold                           = 20.0f;                   // Declare water detected when one of the sensors go above this value
	configLocation->tempEnableMaskBMS                              = 0x1C0F;									// Bitwise select what sensor to enable for the BMS (internal sensors).
	configLocation->tempEnableMaskBattery                          = 0x03F0;									// Bitwise select what sensor to enable for the battery (external sensors).
  configLocation->LCUseDischarge                                 = enabled;                    // Enable or disable the solid state output
	configLocation->LCUsePrecharge                                 = enabled;                    // Use precharge before enabling main output
	configLocation->NTCTopResistor[modConfigNTCGroupLTCExt]        = 100000;                  // NTC Pullup resistor value
	configLocation->NTCTopResistor[modConfigNTCGroupMasterPCB]     = 100000;                  // NTC Pullup resistor value
	configLocation->NTC25DegResistance[modConfigNTCGroupLTCExt]    = 100000;                  // NTC resistance at 25 degree
	configLocation->NTC25DegResistance[modConfigNTCGroupMasterPCB] = 100000;                  // NTC resistance at 25 degree
	configLocation->NTCBetaFactor[modConfigNTCGroupLTCExt]         = 4390;                    // NTC Beta factor
	configLocation->NTCBetaFactor[modConfigNTCGroupMasterPCB]      = 3590;                    // NTC Beta factor
	configLocation->allowChargingDuringDischarge                   = true;                    // Allow the battery to be charged in normal mode
	configLocation->allowForceOn                                   = false;                   // Allow the BMS to be forced ON by long actuation of the power button
	configLocation->pulseToggleButton                              = false;                   // Select either pulse or toggle power button
	configLocation->togglePowerModeDirectHCDelay                   = true;                    // Select either direct power state control or HC output control with delayed turn off.
	configLocation->useCANSafetyInput                              = true;                    // Use the safety input status from CAN
	configLocation->useCANDelayedPowerDown                         = true;                    // Use delayed power down
	configLocation->cellMonitorType                                = CELL_MON_LTC6803_2;      // Use the new cell voltage monitor
	configLocation->cellMonitorICCount                             = 1;                       // Only one slave IC
	configLocation->externalEnableOperationalState                 = opStateExtNormal;        // Go to normal enable mode
	configLocation->powerDownDelay                                 = 1000;                    // Wait only minimal to turn off
	configLocation->canBusSpeed                                    = canSpeedBaud500k;        // 500k CAN baud
	configLocation->chargeEnableOperationalState                   = opStateChargingModeCharging; // Go to charging mode when a charger is connected
	configLocation->DCDCEnableInverted                             = false;                   // De not invert the DCDC enable signal
	configLocation->DCDCTargetVoltage                              = 12.0f;                   // Target voltage for the DCDC converter
	
	// Slave / HiAmp Config
	configLocation->NTCTopResistor[modConfigNTCGroupHiAmpExt]      = 100000;                  // NTC Pullup resistor value
	configLocation->NTCTopResistor[modConfigNTCGroupHiAmpPCB]      = 100000;                  // NTC Pullup resistor value
	configLocation->NTCTopResistor[modConfigNTCGroupHiAmpAUX]      = 100000;                  // NTC Pullup resistor value
	configLocation->NTC25DegResistance[modConfigNTCGroupHiAmpExt]  = 100000;                  // NTC resistance at 25 degree
	configLocation->NTC25DegResistance[modConfigNTCGroupHiAmpPCB]  = 100000;                  // NTC resistance at 25 degree
	configLocation->NTC25DegResistance[modConfigNTCGroupHiAmpAUX]  = 100000;                  // NTC resistance at 25 degree	
	configLocation->NTCBetaFactor[modConfigNTCGroupHiAmpExt]       = 4390;                    // NTC Beta factor
	configLocation->NTCBetaFactor[modConfigNTCGroupHiAmpPCB]       = 4100;                    // NTC Beta factor
	configLocation->NTCBetaFactor[modConfigNTCGroupHiAmpAUX]       = 4390;                    // NTC Beta factor	
	configLocation->HCUseRelay                                     = false;                    // Enable or disable the relay output, when false will also disable HC pre charge.
	configLocation->HCUsePrecharge                                 = true;                    // choice whether to precharge or not, will only work when HCUseRelay = true.
	configLocation->HCUseLoadDetect                                = false;                   // Use voltage drop based load detect on high current load.
	configLocation->HCLoadDetectThreshold                          = 2000;                    // When precharging takes longer then this assume that a load is present
	configLocation->HCLoadVoltageDataSource                        = sourceLoadHCVoltageISL28022_1_0X44_LVLoadOutput; // select the HC load voltage source.
	configLocation->HCLoadCurrentDataSource                        = sourceLoadHCCurrentISL28022_1_0X44_LVHCShunt; // Select the HC load current source.
	configLocation->timeoutHCPreCharge													   = 300;											// Precharge error timeout, allow xxxms pre-charge time before declaring load error.
	configLocation->timeoutHCPreChargeRetryInterval						     = 20000;										// When pre charge failes wait this long in ms
	configLocation->timeoutHCRelayOverlap											     = 1000;										// When precharge succeeds enable both relay and precharge combined for this time, then go to relay only.
#endif

#ifdef ESK8
	configLocation->noOfCellsSeries														     = 12;											// Total number of cells in series in the battery pack
	configLocation->noOfCellsParallel                              = 4;                       // For the esk8 world this could be many things, set default to 4
	configLocation->noOfCellsPerModule                             = 1;                       // Number of cell levels monitored per LTC68XX
	configLocation->noOfParallelModules                       	   = 1;                     	// Number of parallel modules
	configLocation->batteryCapacity														     = 15.00f;									// XXAh battery
	configLocation->cellHardUnderVoltage											     = 2.30f;										// Worst case X.XXV as lowest cell voltage
	configLocation->cellHardOverVoltage												     = 4.35f;										// Worst case X.XXV as highest cell voltage
	configLocation->cellLCSoftUnderVoltage											   = 2.70f;										// Low current lowest cell voltage X.XXV.
  configLocation->cellHCSoftUnderVoltage                         = 3.25f;                   // High current lowest cell voltage X.XXV.
	configLocation->cellSoftOverVoltage												     = 4.20f;										// Normal highest cell voltage X.XXV.
	configLocation->cellBalanceDifferenceThreshold                 = 0.01f;										// Start balancing @ XmV difference, stop if below.
	configLocation->cellBalanceStart													     = 3.80f;										// Start balancing above X.XXV.
	configLocation->cellThrottleUpperStart										     = 0.03f;										// Upper range of cell voltage for charge throttling.
	configLocation->cellThrottleLowerStart										     = 0.20f;									  // Lower range of cell voltage for discharge throttling.
	configLocation->cellThrottleUpperMargin										     = 0.01f;										// Margin of throttle from upper soft limits.
	configLocation->cellThrottleLowerMargin										     = 0.50f;									  // Margin of throttle from lower soft limits.	
	configLocation->packVoltageDataSource                          = sourcePackVoltageISL28022_2_0X40_LVBatteryIn; // Packvoltage source.
	configLocation->packCurrentDataSource                          = sourcePackCurrentLowCurrentShunt; // The pack current is the same as the current through the low current shunt
	configLocation->buzzerSignalSource                             = buzzerSourceOff;         // Stores what source shoud be taken to trigger
	configLocation->buzzerSignalType                               = buzzerSignalTypeOn;      // Stores what sound pattern should be made
	configLocation->buzzerSingalPersistant                         = true;                    // Stores whether the buzzer should stay on after triggering
  configLocation->shuntLCFactor                                  = -0.004494f;              // Shunt factor low current
	configLocation->shuntLCOffset                                  = 0;                       // Shunt offset low current
  configLocation->shuntHCFactor	                                 = 0.001f;                  // Shunt factor high current
	configLocation->shuntHCOffset                                  = -4;                      // Shunt offset high current
	configLocation->voltageLCFactor	                               = 1.0f;                  // Voltage factor low current
	configLocation->voltageLCOffset                                = 0;                      // Voltage offset low current
	configLocation->throttleChargeIncreaseRate                     = 1;                       // Percentage charge throttle increase rate per 100ms (cell voltage loop time)  
	configLocation->throttleDisChargeIncreaseRate                  = 2;                       // Percentage discharge throttle increase rate per 100ms (cell voltage loop time)  	
	configLocation->cellBalanceUpdateInterval									     = 4*1000;									// Keep calculated resistors enabled for this amount of time in miliseconds.
	configLocation->maxSimultaneousDischargingCells						     = 5;												// Allow a maximum of X cells simultinous discharging trough bleeding resistors.
	configLocation->timeoutDischargeRetry											     = 4*1000;									// Wait for X seconds before retrying to enable load.
	configLocation->hysteresisDischarge 											     = 0.02f;										// Lowest cell should rise XXmV before output is re enabled.
	configLocation->timeoutChargeRetry												     = 30*1000;									// Wait for XX seconds before retrying to enable charger.
	configLocation->hysteresisCharge													     = 0.01f;										// Highest cell should lower XXmV before charger is re enabled.
	configLocation->timeoutChargeCompleted										     = 30*60*1000;							// Wait for XX minutes before setting charge state to charged.
	configLocation->timeoutChargingCompletedMinimalMismatch 	     = 6*1000;									// If cell mismatch is under threshold and (charging is not allowed) wait this delay time to set "charged" state.
	configLocation->maxMismatchThreshold											     = 0.020f;									// If mismatch is under this threshold for timeoutChargingCompletedMinimalMismatch determin fully charged.
	configLocation->chargerEnabledThreshold										     = 0.5f;										// If charge current > X.XA stay in charging mode and dont power off.
	configLocation->timeoutChargerDisconnected								     = 2000;										// Wait for X seconds to respond to charger disconnect.
	configLocation->minimalPrechargePercentage								     = 0.80f;										// output should be at a minimal of 80% of input voltage.
	configLocation->timeoutLCPreCharge												     = 300;											// Precharge error timeout, allow 300ms pre-charge time before declaring load error.
	configLocation->maxAllowedCurrent													     = 120.0f;									// Allow max XXXA trough BMS.
	configLocation->allowedTempBattDischargingMax                  = 75.0f;                   // Max battery temperature where discharging is still allowed
	configLocation->allowedTempBattDischargingMin                  = -20.0f;                  // Min battery temperature where discharging is still allowed
	configLocation->allowedTempBattChargingMax                     = 50.0f;                   // Max battery temperature where charging is still allowed
	configLocation->allowedTempBattChargingMin                     = 0.0f;                    // Min battery temperature where charging is still allowed
	configLocation->allowedTempBMSMax                              = 80.0f;                   // Max BMS operational temperature
	configLocation->allowedTempBMSMin                              = -20.0f;                  // Min BMS operational temperature
	configLocation->displayTimeoutBatteryDead									     = 5000;										// Show battery dead symbol X seconds before going to powerdown in cell voltage error state.
	configLocation->displayTimeoutBatteryError								     = 2000;										// Show error symbol for X seconds before going to powerdown in general error state.
	configLocation->displayTimeoutBatteryErrorPreCharge				     = 10000;										// Show pre charge error for XX seconds.
	configLocation->displayTimeoutSplashScreen								     = 1000;										// Display / INIT splash screen time.
	configLocation->maxUnderAndOverVoltageErrorCount 					     = 5;												// Max count of hard cell voltage errors.
	configLocation->notUsedCurrentThreshold										     = 1.0f;										// If abs(packcurrent) < X.XA consider pack as not used.
	configLocation->notUsedTimeout														     = 20*60*1000;							// If pack is not used for longer than XX minutes disable bms.
	configLocation->stateOfChargeStoreInterval								     = 60*1000;									// Interval in ms to store state of charge information.
	configLocation->stateOfChargeMethod                            = socCoulomb;              // Use coulomb counting for SoC calculation
	configLocation->CANID																			     = 10;											// CAN ID for CAN communication.
	configLocation->CANIDStyle                                     = CANIDStyleVESC;          // CAN ID default Style.
	configLocation->emitStatusOverCAN                              = false;                   // Send status over can.
	configLocation->emitStatusProtocol                             = canEmitProtocolDieBieEngineering; // Can emit protocol set to MG style for backwards compatibility
	configLocation->waterSensorEnableMask                          = 0;                       // Turn all sensors are off
	configLocation->waterSensorThreshold                           = 20.0f;                   // Declare water detected when one of the sensors go above this value
	configLocation->tempEnableMaskBMS                              = 0x0004;									// Bitwise select what sensor to enable for the BMS (internal sensors).
	configLocation->tempEnableMaskBattery                          = 0x0000;									// Bitwise select what sensor to enable for the battery (external sensors).
  configLocation->LCUseDischarge                                 = enabled;                    // Enable or disable the solid state output
	configLocation->LCUsePrecharge                                 = enabled;                    // Use precharge before enabling main output
	configLocation->NTCTopResistor[modConfigNTCGroupLTCExt]        = 100000;                  // NTC Pullup resistor value
	configLocation->NTCTopResistor[modConfigNTCGroupMasterPCB]     = 100000;                  // NTC Pullup resistor value
	configLocation->NTC25DegResistance[modConfigNTCGroupLTCExt]    = 100000;                  // NTC resistance at 25 degree
	configLocation->NTC25DegResistance[modConfigNTCGroupMasterPCB] = 100000;                  // NTC resistance at 25 degree
	configLocation->NTCBetaFactor[modConfigNTCGroupLTCExt]         = 4390;                    // NTC Beta factor
	configLocation->NTCBetaFactor[modConfigNTCGroupMasterPCB]      = 3590;                    // NTC Beta factor
	configLocation->allowChargingDuringDischarge                   = true;                    // Allow the battery to be charged in normal mode
	configLocation->allowForceOn                                   = false;                   // Allow the BMS to be forced ON by long actuation of the power button
	configLocation->pulseToggleButton                              = true;                    // Select either pulse or toggle power button
	configLocation->togglePowerModeDirectHCDelay                   = true;                    // Select either direct power state control or HC output control with delayed turn off.
	configLocation->useCANSafetyInput                              = false;                   // Use the safety input status from CAN
	configLocation->useCANDelayedPowerDown                         = false;                   // Use delayed power down
	configLocation->cellMonitorType                                = CELL_MON_LTC6803_2;      // Use the new cell voltage monitor
	configLocation->cellMonitorICCount                             = 1;                       // Only one slave IC
	configLocation->externalEnableOperationalState                 = opStateExtNormal;        // Go to normal enable mode
	configLocation->powerDownDelay                                 = 3000;                    // Wait only minimal to turn off
	configLocation->canBusSpeed                                    = canSpeedBaud500k;        // 500k CAN baud
	configLocation->chargeEnableOperationalState                   = opStateChargingModeCharging;// Go to charging mode when a charger is connected
	configLocation->DCDCEnableInverted                             = false;                   // De not invert the DCDC enable signal, DCDC converter is not present on the ESK8 BMS
	configLocation->DCDCTargetVoltage                              = 12.0f;                   // Target voltage for the DCDC converter
	
	// Slave / HiAmp Config
	configLocation->NTCTopResistor[modConfigNTCGroupHiAmpExt]      = 100000;                  // NTC Pullup resistor value
	configLocation->NTCTopResistor[modConfigNTCGroupHiAmpPCB]      = 100000;                  // NTC Pullup resistor value
	configLocation->NTCTopResistor[modConfigNTCGroupHiAmpAUX]      = 100000;                  // NTC Pullup resistor value
	configLocation->NTC25DegResistance[modConfigNTCGroupHiAmpExt]  = 100000;                  // NTC resistance at 25 degree
	configLocation->NTC25DegResistance[modConfigNTCGroupHiAmpPCB]  = 100000;                  // NTC resistance at 25 degree
	configLocation->NTC25DegResistance[modConfigNTCGroupHiAmpAUX]  = 100000;                  // NTC resistance at 25 degree	
	configLocation->NTCBetaFactor[modConfigNTCGroupHiAmpExt]       = 4390;                    // NTC Beta factor
	configLocation->NTCBetaFactor[modConfigNTCGroupHiAmpPCB]       = 4100;                    // NTC Beta factor
	configLocation->NTCBetaFactor[modConfigNTCGroupHiAmpAUX]       = 4390;                    // NTC Beta factor
	configLocation->HCUseRelay                                     = false;                   // Enable or disable the relay output, when false will also disable HC pre charge.
	configLocation->HCUsePrecharge                                 = true;                    // choice whether to precharge or not, will only work when HCUseRelay = true.
	configLocation->HCUseLoadDetect                                = false;                   // Use voltage drop based load detect on high current load.
	configLocation->HCLoadDetectThreshold                          = 2000;                    // When precharging takes longer then this assume that a load is present
	configLocation->HCLoadVoltageDataSource                        = sourceLoadHCVoltageNone; // select the HC load voltage source.
	configLocation->HCLoadCurrentDataSource                        = sourceLoadHCCurrentNone; // Select the HC load current source.
	configLocation->timeoutHCPreCharge													   = 300;											// Precharge error timeout, allow xxxms pre-charge time before declaring load error.
	configLocation->timeoutHCPreChargeRetryInterval						     = 20000;										// When pre charge failes wait this long in ms
	configLocation->timeoutHCRelayOverlap											     = 1000;										// When precharge succeeds enable both relay and precharge combined for this time, then go to relay only.
#endif
}

