#ifndef CH_NTB_EEDURO_DELTA_TYPES_HPP
#define CH_NTB_EEDURO_DELTA_TYPES_HPP

#include <eeros/math/Matrix.hpp>
#include "constants.hpp"
#include <array>

namespace eeduro {
	namespace delta {
		using AxisVector = eeros::math::Matrix<nofAxis, 1>;
		using AxisSquareMatrix = eeros::math::Matrix<nofAxis, nofAxis>;
		using Array = std::array<double,nofAxis>;
		
	}
}

#endif /* CH_NTB_EEDURO_DELTA_TYPES_HPP */
