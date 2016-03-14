#ifndef EEDURO_VELOCITY2POSITION_HPP_
#define EEDURO_VELOCITY2POSITION_HPP_

#include <string>
#include <thread>
#include <eeros/control/Output.hpp>
#include <eeros/control/Block1o.hpp>
#include <eeros/control/Input.hpp>
#include <eeros/core/System.hpp>
#include <eeros/hal/Joystick.hpp>
#include <eeros/math/Matrix.hpp>

namespace eeduro {

		class Velocity2Position: public eeros::control::Block {
		public:
			Velocity2Position();
			virtual ~Velocity2Position();
			
			virtual void run();
			virtual void setInitPos(eeros::math::Vector4 initPos);

			virtual eeros::control::Input<eeros::math::Vector4>& getMouseIn();
			virtual eeros::control::Input<eeros::math::Vector4>& getJoystickIn();
			virtual eeros::control::Output<eeros::math::Vector4>& getPositionOut();
			
			double xScale = 0.0001;
			double yScale = 0.0001;
			double zScale = 0.0001;
			double rScale = 0.002;
			double min_x = -0.03;
			double max_x = 0.045;
			double min_y = -0.03;
			double max_y = 0.03;
			double min_z = -0.053;
			double max_z = -0.015;
			double min_r = -2.8;
			double max_r = 2.8;
		
		protected:
			eeros::control::Input<eeros::math::Vector4> joytstickIn;
			eeros::control::Input<eeros::math::Vector4> mouseIn;
			eeros::control::Output<eeros::math::Vector4> positionOut;
			
			eeros::math::Matrix<4,4,double> axisScale;
			
			eeros::hal::Joystick j;
			std::thread* t;
		};
};

#endif /* EEDURO_VELOCITY2POSITION_HPP_ */
