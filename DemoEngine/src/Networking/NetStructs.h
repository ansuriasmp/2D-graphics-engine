// NetStructs.h
#pragma once

struct Vector2 {
    float x, y;
};

struct ClientPacket {
    int clientIndex;
    Vector2 position;
};

struct ClientData {
    int packetType = 0;
    int clientIndex;
};

struct NetworkEntity {
    int id;           // puedes usar index simple o UUID si lo tienes
    Vector2 position;
};

struct PhysicsData {
    int packetType = 1;
    int entityCount;
    NetworkEntity entities[64]; // soporta hasta 64 entidades, puedes hacer dinámico después
};

