#include <Socket.hh>
#include <raylib.h>
#include <stdio.h>

struct Player {
    static constexpr float s_radius = 50.f;
    static constexpr float s_speed  = 250.f;

    Socket socket;
    Color color;
    float posX;
    float posY;
    char nick[20];

    void update(float dt = GetFrameTime()) {
        if   (IsKeyDown(KEY_W) && posY > s_radius) {
            posY -= dt * s_speed;
        } if (IsKeyDown(KEY_S) && posY < GetRenderHeight() - s_radius) {
            posY += dt * s_speed;
        } if (IsKeyDown(KEY_D) && posX < GetRenderWidth() - s_radius) {
            posX += dt * s_speed;
        } if (IsKeyDown(KEY_A) && posX > s_radius) {
            posX -= dt * s_speed;
        }
    }

    void draw() {
        const int fontSize = 20;
        const int textWidthHalf = MeasureText(nick, fontSize) >> 1;

        DrawCircle((int)posX, (int)posY, s_radius, color);
        DrawText(nick, (int)posX - textWidthHalf, (int)posY + 70, fontSize, LIGHTGRAY);
    }
};

static Player player;

int main(void) {
    printf("Type your nick: ");
    scanf("%19s", player.nick);

    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(1280, 720, "Cramkap");
    SetWindowFocused();

    player.color = PINK;
    player.posX = GetRenderWidth()  >> 1;
    player.posY = GetRenderHeight() >> 1;

    while (!WindowShouldClose()) {
        player.update();

        BeginDrawing();
        ClearBackground(BLACK);
        {
            player.draw();
            DrawFPS(10, 10);
        }
        EndDrawing();
    }

    CloseWindow();
}