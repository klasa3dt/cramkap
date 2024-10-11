#define MAX_NICK_SIZE 20
#include <raylib.h>
#include <stdio.h>
#include <string.h>
#include <raymath.h>
#include <Socket.hh>

enum DataType {
    DATA_TYPE_CONNECTION,
    DATA_TYPE_PLAYER_DATA
};

struct PlayerConnection {
    Color color;
    char nick[MAX_NICK_SIZE];
};

struct PlayerData {
    float posX;
    float posY;
};

struct DataToSend {
    DataType type;
    union {
        PlayerConnection playerConnection;
        PlayerData playerData;
    };
};

struct Player {
    
    static constexpr float s_height = 50.f;
    static constexpr float s_width  = 50.f;
    static constexpr float s_speed  = 250.f;

    Color color;
    float posX;
    float posY;
    char nick[MAX_NICK_SIZE];

    bool update(Player* frend, float dt = GetFrameTime());
    void draw();
};

struct LocalPlayer : Player {
    Socket socket;

    void receiveData(Player* frend);
    void connect(Player* frend, Connection conn);
    void sendData(Connection conn);
};
