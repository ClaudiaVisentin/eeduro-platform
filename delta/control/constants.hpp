#ifndef CH_NTB_EEDURO_DELTA_CONSTANTS_HPP
#define CH_NTB_EEDURO_DELTA_CONSTANTS_HPP

namespace eeduro {
	namespace delta {
		// Math constants
		constexpr double pi = 3.14159265359;
		
		// General constants
		constexpr unsigned int nofAxis = 4;
		
		// Electrical and mechanical parameters
		constexpr double i1524 = 387283.0/5103.0;	// gear ratio for the 3 main axis
		constexpr double i0816 = 319333.0/2673.0;	// gear ratio for the 4th axis
		
		// Controller parameters
		constexpr double dt = 0.001; // [s]
		constexpr double D = 0.7;
		constexpr double w0 = 2 * pi * 1 / (2 * D * dt * 2 * pi);
		constexpr double kp = w0 / (2 * D);
		constexpr double kd = 2 * D * w0;
	}
}

#endif /* CH_NTB_EEDURO_DELTA_CONSTANTS_HPP */