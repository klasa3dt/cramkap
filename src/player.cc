#include <player.hh>

bool Player::update(Player* frend, float dt) {
    bool move = false;
        
    float destX = posX;
    float destY = posY;

    if (IsKeyDown(KEY_W) && posY > s_width / 2) {
        destY -= dt * s_speed;
        move = true;
    }
        
    if (IsKeyDown(KEY_S) && posY < GetRenderHeight() - s_height / 2) {
        destY += dt * s_speed;
        move = true;
    }
        
    if (IsKeyDown(KEY_D) && posX < GetRenderWidth() - s_width / 2) {
        destX += dt * s_speed;
        move = true;
    }
        
    if (IsKeyDown(KEY_A) && posX > s_height / 2) {
        destX -= dt * s_speed;
        move = true;
    }

    Rectangle playerRectX = {destX, posY, s_width, s_height};
    Rectangle playerRectY = {posX, destY, s_width, s_height};
    Rectangle frendRect = {frend->posX, frend->posY, s_width, s_height};
    bool collisionX = CheckCollisionRecs(playerRectX, frendRect);
    bool collisionY = CheckCollisionRecs(playerRectY, frendRect);

    if (!collisionX) {
        posX = destX;
    }

    if (!collisionY) {
        posY = destY;
    }


    Rectangle playerRect = {posX, posY, s_width, s_height};
    bool collision = CheckCollisionRecs(playerRect, frendRect);
    if (collision) {
        float diffX = frend->posX - posX;
        float diffY = frend->posY - posY;

        Vector2 diffNormalized = Vector2Normalize({diffX, diffY});

        posX -= diffNormalized.x;
        posY -= diffNormalized.y;
    }
        
    return move;
}

void Player::draw() {
    const int fontSize = 20;
    const int textWidthHalf = MeasureText(nick, fontSize) >> 1;

    DrawRectangle((int)posX - s_width / 2, (int)posY - s_height / 2, (int)s_width, (int)s_height, color);
    DrawText(nick, (int)posX - textWidthHalf, (int)posY + 30, fontSize, LIGHTGRAY);
}

void LocalPlayer::receiveData(Player* frend) {
    while (CanReceive(&socket)) {
        DataToSend data;
        Connection conn;
        Receive(&socket, &conn, &data, sizeof(data));

    switch (data.type) {
        case DATA_TYPE_CONNECTION:
            printf("\nJakis cwel sie dodal do gry %hhu.%hhu.%hhu.%hhu\n\n",
                conn.ipaddress[0],
                conn.ipaddress[1],
                conn.ipaddress[2],
                conn.ipaddress[3]);
            frend->color = data.playerConnection.color;
            memcpy(frend->nick, data.playerConnection.nick, sizeof(frend->nick));
            printf ("%s", data.playerConnection.nick);
            break;
        case DATA_TYPE_PLAYER_DATA:
            frend->posX = data.playerData.posX;
            frend->posY = data.playerData.posY;
            break;
        }
    }
}

void LocalPlayer::connect(Player* frend, Connection conn) {
    DataToSend data;
    data.type = DATA_TYPE_CONNECTION;
    data.playerConnection.color = this->color;
    memcpy(data.playerConnection.nick, this->nick, sizeof(this->nick));

    printf ("%hhu.%hhu.%hhu.%hhu\n", conn.ipaddress[0], conn.ipaddress[1], conn.ipaddress[2], conn.ipaddress[3]);
        
    do {
        Send(&this->socket, &conn, &data, sizeof(DataToSend));
    } while (!CanReceive(&socket));

    receiveData(frend);
}

void LocalPlayer::sendData(Connection conn) {
    DataToSend data;
    data.type = DATA_TYPE_PLAYER_DATA;
    data.playerData = {.posX = this->posX, .posY = this->posY};
    Send(&this->socket, &conn, &data, sizeof(DataToSend));
}

