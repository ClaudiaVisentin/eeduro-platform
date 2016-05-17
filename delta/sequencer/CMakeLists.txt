add_delta_sources (
                    MainSequence.cpp             MainSequenceFaulhaber.cpp 
                    SortSequence.cpp             MoveBlockSequence.cpp 
                    CalibrateSequence.cpp        DetectSequence.cpp
                  ) 


add_server_sources( server/MainSequence_server.cpp      server/MainSequenceFaulhaber_server.cpp 
                    server/SortSequence_server.cpp      server/MoveBlockSequence_server.cpp 
                    server/CalibrateSequence_server.cpp server/DetectSequence_server.cpp
                  ) 


add_client_sources( client/MainSequence_client.cpp      client/MainSequenceFaulhaber_client.cpp 
                    client/SortSequence_client.cpp      client/MoveBlockSequence_client.cpp 
                    client/CalibrateSequence_client.cpp client/DetectSequence_client.cpp
                  ) 