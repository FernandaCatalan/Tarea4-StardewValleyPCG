#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <unordered_map>

// Minerales
#define CUARZO 'Q'
#define ESMERALDA 'E'
#define DIAMANTE 'D'

struct Room {
    int x, y, width, height;
    Room(int x, int y, int w, int h) : x(x), y(y), width(w), height(h) {}
};

// Generador de mapas usando el algoritmo BSP
class BSPMapGenerator {
private:
    int mapWidth, mapHeight;
    std::vector<std::vector<char>> map;
    std::vector<Room> rooms;

    // Probabilidades de minerales (Q: 70%, E: 25%, D: 5%)
    std::unordered_map<char, int> mineralProb = {
        {CUARZO, 70},
        {ESMERALDA, 25},
        {DIAMANTE, 5}
    };

    // Divide el mapa en habitaciones usando el algoritmo BSP
    void split(int x, int y, int w, int h, int depth) {
        if (depth <= 0 || w < 10 || h < 10) {
            createRoom(x + 1, y + 1, w - 2, h - 2);
            return;
        }

        bool horizontal = (rand() % 2 == 0);
        if (w > h * 1.25) horizontal = false;
        else if (h > w * 1.25) horizontal = true;

        if (horizontal) {
            int splitY = y + (h / 2) + (rand() % 3 - 1);
            split(x, y, w, splitY - y, depth - 1);
            split(x, splitY, w, h - (splitY - y), depth - 1);
        } else {
            int splitX = x + (w / 2) + (rand() % 3 - 1);
            split(x, y, splitX - x, h, depth - 1);
            split(splitX, y, w - (splitX - x), h, depth - 1);
        }
    }

    // Crea una habitación en el mapa y la llena con el carácter de suelo ('.')
    void createRoom(int x, int y, int w, int h) {
        rooms.emplace_back(x, y, w, h);
        for (int i = x; i < x + w; ++i) {
            for (int j = y; j < y + h; ++j) {
                if (i < mapWidth && j < mapHeight) map[j][i] = '.';
            }
        }
    }

    // Conecta las habitaciones con túneles
    void connectRooms() {
        for (size_t i = 1; i < rooms.size(); ++i) {
            int prevX = rooms[i-1].x + rooms[i-1].width / 2;
            int prevY = rooms[i-1].y + rooms[i-1].height / 2;
            int currX = rooms[i].x + rooms[i].width / 2;
            int currY = rooms[i].y + rooms[i].height / 2;

            if (rand() % 2 == 0) {
                createTunnel(prevX, currX, prevY, true);  // Horizontal
                createTunnel(prevY, currY, currX, false); // Vertical
            } else {
                createTunnel(prevY, currY, prevX, false); // Vertical
                createTunnel(prevX, currX, currY, true);  // Horizontal
            }
        }
    }

    // Crea un túnel entre dos puntos
    void createTunnel(int start, int end, int constant, bool isHorizontal) {
        for (int pos = std::min(start, end); pos <= std::max(start, end); ++pos) {
            if (isHorizontal) {
                if (pos >= 0 && pos < mapWidth && constant >= 0 && constant < mapHeight) {
                    map[constant][pos] = '.';
                }
            } else {
                if (constant >= 0 && constant < mapWidth && pos >= 0 && pos < mapHeight) {
                    map[pos][constant] = '.';
                }
            }
        }
    }

    // Selecciona un mineral aleatorio basado en las probabilidades
    char getRandomMineral() {
        int randVal = rand() % 100;
        if (randVal < mineralProb[DIAMANTE]) return DIAMANTE;
        if (randVal < mineralProb[DIAMANTE] + mineralProb[ESMERALDA]) return ESMERALDA;
        return CUARZO;
    }

    // Añade minerales a las habitaciones
    void addMinerals() {
        for (const auto& room : rooms) {
            int mineralCells = (room.width * room.height) * 0.2;  // Cantidad de minerales por habitación
            for (int i = 0; i < mineralCells; ++i) {
                int rx = room.x + rand() % room.width;
                int ry = room.y + rand() % room.height;
                if (rx < mapWidth && ry < mapHeight && map[ry][rx] == '.') {
                    map[ry][rx] = getRandomMineral();
                }
            }
        }
    }

public:
    BSPMapGenerator(int width, int height) : mapWidth(width), mapHeight(height) {
        map.resize(height, std::vector<char>(width, '#'));
        srand(time(nullptr));
    }

    // Genera el mapa completo y maxDepth controla la profundidad máxima de división del BSP
    void generateMap(int maxDepth) {
        split(0, 0, mapWidth, mapHeight, maxDepth);
        connectRooms();
        addMinerals();
    }

    // Imprime el mapa en la consola
    void printMap() {
        for (const auto& row : map) {
            for (char cell : row) {
                std::cout << cell << ' ';
            }
            std::cout << '\n';
        }
    }
};

int main() {
    // Define las dimensiones del mapa
    // Ajustables dentro del código, para cambiar el tamaño del mapa
    const int WIDTH = 50;
    const int HEIGHT = 30;
    const int DEPTH = 4;

    // Inicializa el generador de mapas y genera el mapa
    BSPMapGenerator generator(WIDTH, HEIGHT);
    generator.generateMap(DEPTH);
    generator.printMap();

    return 0;
}