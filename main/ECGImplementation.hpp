/*
 * ECGImplementation.hpp
 *
 *  Created on: 22 mrt. 2018
 *      Author: casper.rebholz
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_err.h"
#include "esp_log.h"

#include "driver/gpio.h"
#include "driver/adc.h"

#include "Sensor.hpp"

class ECGImplementation: public Sensor{
public:

	ECGImplementation();

   /*!
	 * \brief ecgImplementation DataSize method
	 * \return Datasize of SensorRead method in bytes
	 *
	 * This method returns the datasize of a single
	 * sensorread. This value is given in bytes.
	 *
	 * For the ecg, is is a static value of 4 bytes.
	 */
	int DataSize() override;

   /*!
	 * \brief ECG SensorRead method
	 * \return Pointer to an unsigned short array with data
	 *
	 * This method reads GPIO36 and returns an unsigned
	 * short pointer with the ADC value and an incrementing
	 * sample number.
	 */
   unsigned short* SensorRead() override;

	/*!
	 * \brief ECGImplementation Sleep method
	 *
	 * This method will put the ECG in sleep
	 * mode.
	 */
	void Sleep() override;

private:
   int kernel_size = 5;
   int sum = 0;
   int i = 0;
   int j = 0;
   int sample_number = 0;
   int sample_value[5] = {0};
   int sample_value_filtered = 0; //evt float van maken
   unsigned short ECGData[4];
   int sine [1000] = {
		   //1
		   2048,2151,2255,2358,2460,2561,2660,2759,
		   2855,2949,3041,3131,3217,3301,3381,3458,
		   3532,3601,3667,3728,3785,3838,3886,3929,
		   3968,4001,4030,4053,4071,4084,4092,4095,
		   4092,4084,4071,4053,4030,4001,3968,3929,
		   3886,3838,3785,3728,3667,3601,3532,3458,
		   3381,3301,3217,3131,3041,2949,2855,2759,
		   2660,2561,2460,2358,2255,2151,2048,1944,
		   1840,1737,1635,1534,1435,1336,1240,1146,
		   1054,964,878,794,714,637,563,494,
		   428,367,310,257,209,166,127,94,
		   65,42,24,11,3,0,3,11,
		   24,42,65,94,127,166,209,257,
		   310,367,428,494,563,637,714,794,
		   878,964,1054,1146,1240,1336,1435,1534,
		   1635,1737,1840,1944,2048,
		   //2
		   2048,2151,2255,2358,2460,2561,2660,2759,
		   2855,2949,3041,3131,3217,3301,3381,3458,
		   3532,3601,3667,3728,3785,3838,3886,3929,
		   3968,4001,4030,4053,4071,4084,4092,4095,
		   4092,4084,4071,4053,4030,4001,3968,3929,
		   3886,3838,3785,3728,3667,3601,3532,3458,
		   3381,3301,3217,3131,3041,2949,2855,2759,
		   2660,2561,2460,2358,2255,2151,2048,1944,
		   1840,1737,1635,1534,1435,1336,1240,1146,
		   1054,964,878,794,714,637,563,494,
		   428,367,310,257,209,166,127,94,
		   65,42,24,11,3,0,3,11,
		   24,42,65,94,127,166,209,257,
		   310,367,428,494,563,637,714,794,
		   878,964,1054,1146,1240,1336,1435,1534,
		   1635,1737,1840,1944,2048,
		   //3
		   2048,2210,2372,2531,2688,2840,2988,3129,
		   3264,3391,3510,3619,3718,3807,3885,3951,
		   4005,4046,4075,4091,4095,4085,4062,4027,
		   3979,3919,3847,3764,3670,3565,3451,3329,
		   3198,3059,2915,2765,2610,2452,2291,2129,
		   1966,1804,1643,1485,1330,1180,1036,897,
		   766,644,530,425,331,248,176,116,
		   68,33,10,0,4,20,49,90,
		   144,210,288,377,476,585,704,831,
		   966,1107,1255,1407,1564,1723,1885,2048,
		   //4
		   2048,2151,2255,2358,2460,2561,2660,2759,
		   2855,2949,3041,3131,3217,3301,3381,3458,
		   3532,3601,3667,3728,3785,3838,3886,3929,
		   3968,4001,4030,4053,4071,4084,4092,4095,
		   4092,4084,4071,4053,4030,4001,3968,3929,
		   3886,3838,3785,3728,3667,3601,3532,3458,
		   3381,3301,3217,3131,3041,2949,2855,2759,
		   2660,2561,2460,2358,2255,2151,2048,1944,
		   1840,1737,1635,1534,1435,1336,1240,1146,
		   1054,964,878,794,714,637,563,494,
		   428,367,310,257,209,166,127,94,
		   65,42,24,11,3,0,3,11,
		   24,42,65,94,127,166,209,257,
		   310,367,428,494,563,637,714,794,
		   878,964,1054,1146,1240,1336,1435,1534,
		   1635,1737,1840,1944,2048,
		   //5
		   2048,2140,2232,2324,2416,2506,2596,2685,
		   2772,2858,2942,3024,3104,3183,3258,3332,
		   3403,3471,3536,3598,3657,3712,3764,3813,
		   3858,3899,3937,3971,4000,4026,4048,4066,
		   4079,4089,4094,4095,4092,4084,4073,4057,
		   4038,4014,3986,3954,3919,3879,3836,3789,
		   3739,3685,3627,3567,3503,3437,3368,3295,
		   3221,3144,3065,2983,2900,2815,2728,2640,
		   2551,2461,2370,2278,2186,2094,2001,1909,
		   1817,1725,1634,1544,1455,1367,1280,1195,
		   1112,1030,951,874,800,727,658,592,
		   528,468,410,356,306,259,216,176,
		   141,109,81,57,38,22,11,3,
		   0,1,6,16,29,47,69,95,
		   124,158,196,237,282,331,383,438,
		   497,559,624,692,763,837,912,991,
		   1071,1153,1237,1323,1410,1499,1589,1679,
		   1771,1863,1955,2048,
		   //6
		   2048,2140,2232,2324,2416,2506,2596,2685,
		   2772,2858,2942,3024,3104,3183,3258,3332,
		   3403,3471,3536,3598,3657,3712,3764,3813,
		   3858,3899,3937,3971,4000,4026,4048,4066,
		   4079,4089,4094,4095,4092,4084,4073,4057,
		   4038,4014,3986,3954,3919,3879,3836,3789,
		   3739,3685,3627,3567,3503,3437,3368,3295,
		   3221,3144,3065,2983,2900,2815,2728,2640,
		   2551,2461,2370,2278,2186,2094,2001,1909,
		   1817,1725,1634,1544,1455,1367,1280,1195,
		   1112,1030,951,874,800,727,658,592,
		   528,468,410,356,306,259,216,176,
		   141,109,81,57,38,22,11,3,
		   0,1,6,16,29,47,69,95,
		   124,158,196,237,282,331,383,438,
		   497,559,624,692,763,837,912,991,
		   1071,1153,1237,1323,1410,1499,1589,1679,
		   1771,1863,1955,2048,
		   //7
		   2048,2140,2232,2324,2416,2506,2596,2685,
		   2772,2858,2942,3024,3104,3183,3258,3332,
		   3403,3471,3536,3598,3657,3712,3764,3813,
		   3858,3899,3937,3971,4000,4026,4048,4066,
		   4079,4089,4094,4095,4092,4084,4073,4057,
		   4038,4014,3986,3954,3919,3879,3836,3789,
		   3739,3685,3627,3567,3503,3437,3368,3295,
		   3221,3144,3065,2983,2900,2815,2728,2640,
		   2551,2461,2370,2278,2186,2094,2001,1909,
		   1817,1725,1634,1544,1455,1367,1280,1195,
		   1112,1030,951,874,800,727,658,592,
		   528,468,410,356,306,259,216,176,
		   141,109,81,57,38,22,11,3,
		   0,1,6,16,29,47,69,95,
		   124,158,196,237,282,331,383,438,
		   497,559,624,692,763,837,912,991,
		   1071,1153,1237,1323,1410,1499,1589,1679,
		   1771,1863,1955,2048,
		   //8
		   2048,2151,2255,2358,2460,2561,2660,2759,
		   2855,2949,3041,3131,3217,3301,3381,3458,
		   3532,3601,3667,3728,3785,3838,3886,3929,
		   3968,4001,4030,4053,4071,4084,4092,4095,
		   4092,4084,4071,4053,4030,4001,3968,3929,
		   3886,3838,3785,3728,3667,3601,3532,3458,
		   3381,3301,3217,3131,3041,2949,2855,2759,
		   2660,2561,2460,2358,2255,2151,2048,1944,
		   1840,1737,1635,1534,1435,1336,1240,1146,
		   1054,964,878,794,714,637,563,494,
		   428,367,310,257,209,166,127,94,
		   65,42,24,11,3,0,3,11,
		   24,42,65,94,127,166,209,257,
		   310,367,428,494,563,637,714,794,
		   878,964,1054,1146,1240,1336,1435,1534,
		   1635,1737,1840,1944,2048};
};





