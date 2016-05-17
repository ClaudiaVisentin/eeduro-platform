add_delta_sources ( 
                    ControlSystem.cpp    
                    Kinematic.cpp      Jacobian.cpp           DirectKinematic.cpp  
                    Jacobi.cpp         MotorModel.cpp         PathPlanner.cpp    
                    Inertia.cpp        Velocity2Position.cpp
                   )
                   
add_client_sources(
                    client/ControlSystem_client.cpp   client/ClientData.cpp 
                    Kinematic.cpp                     Jacobian.cpp             DirectKinematic.cpp 
                    Jacobi.cpp                        MotorModel.cpp           PathPlanner.cpp 
                    Inertia.cpp                       Velocity2Position.cpp
                  )  
                    
add_server_sources(
                    server/ControlSystem_server.cpp   server/ServerData.cpp 
                    Kinematic.cpp                     Jacobian.cpp             DirectKinematic.cpp 
                    Jacobi.cpp                        MotorModel.cpp           PathPlanner.cpp 
                    Inertia.cpp                       Velocity2Position.cpp
                  )
