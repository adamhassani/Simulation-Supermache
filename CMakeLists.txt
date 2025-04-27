cmake_minimum_required(VERSION 3.30)
project(Simulation_Supermache C)

set(CMAKE_C_STANDARD 11)

include_directories(.)
include_directories(caisse)
include_directories(client)
include_directories(employe)
include_directories(manager)
include_directories(producteur)
include_directories(queue)
include_directories(rayon)
include_directories(superviseur)

add_executable(Simulation_Supermache
    caisse/caisse.c
    caisse/caisse.h
    client/client.c
    client/client.h
    employe/employe.c
    employe/employe.h
    manager/manager.c
    manager/manager.h
    producteur/producteur.c
    producteur/producteur.h
    queue/queue.c
    queue/queue.h
    rayon/rayon.c
    rayon/rayon.h
    superviseur/superviseur.c
    superviseur/superviseur.h
    main.c)
