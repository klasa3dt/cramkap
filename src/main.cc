#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
#include <player.hh>

static LocalPlayer player;
static Player frend;

static Connection selectionScreen() {
    int width;
    int height;
    char ip[16] = {};
    bool nameSelected = false;
    player.color = PINK;

    while (!WindowShouldClose()) {
        width  = GetRenderWidth();
        height = GetRenderHeight();

        float centerX = float(width >> 1);
        float centerY = float(height >> 1);
        
        const char* playerIp = TextFormat("Your ip: %s", GetMyIp());

        BeginDrawing();
        {
            ClearBackground(BLACK);
            if (!nameSelected) {
                GuiColorPicker({ centerX - 75, centerY + 150, 150, 150 }, "", &player.color);
                GuiDrawText("Type your nick", { centerX - 200, centerY - 100, 400, 50 }, 1, WHITE);
                GuiTextBox({ centerX - 200, centerY - 25, 400, 50 }, player.nick, MAX_NICK_SIZE, true);

                nameSelected = GuiButton({centerX - 200, centerY + 75, 400, 50}, "Select ip");
                
            }
            else {

                GuiDrawText("Type frend ip", { centerX - 200, centerY - 100, 400, 50 }, 1, WHITE);
                GuiTextBox({ centerX - 200, centerY - 25, 400, 50 }, ip, 16, true);
                
                if (GuiButton({centerX - 200, centerY + 75, 400, 50}, "Start")) {
                    EndDrawing();
                    break;
                }
            }
            DrawText(playerIp, width - MeasureText(playerIp, 20) - 10, 10, 20, WHITE);
            DrawFPS(10, 10);
        }
        EndDrawing();

    }
    
    int colorSum = player.color.r + player.color.g + player.color.b;
    if (colorSum < 127) {
        player.color.r += 50;
        player.color.g += 50;
        player.color.b += 50;
    }
    
    return ConvertToConnection(ip);
}

int main(void) {
    Connection conn;
    conn.port = DEFAULT_PORT;

    SetConfigFlags(FLAG_VSYNC_HINT);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "Cramkap");
    SetWindowFocused();
    GuiSetStyle(DEFAULT, TEXT_SIZE, 30);
    conn = selectionScreen();

    frend.color = GREEN;
    player.posX = GetRenderWidth()  >> 1;
    player.posY = GetRenderHeight() >> 1;
    CreateSocket(&player.socket);
    ListenPort(&player.socket);

    player.connect(&frend, conn);

    while (!WindowShouldClose()) {
        if (player.update(&frend)) {
            player.sendData(conn);
        }

        BeginDrawing();
        ClearBackground(BLACK);
            
        player.draw();
        frend.draw();
        DrawFPS(10, 10);
        
        EndDrawing();
            
        player.receiveData(&frend);
    }

    CloseWindow();
}
