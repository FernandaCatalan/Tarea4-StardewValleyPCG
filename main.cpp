#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <unordered_map>

// Minerales
#define CUARZO 'Q'
#define JADE 'J'
#define DIAMANTE 'D'
#define COBRE 'C'
#define HIERRO 'H'
#define ORO 'O'
#define PIEDRA 'P'
// Objetos en el mapa
#define ENEMIGO 'X'
#define ESCALERA 'E'
#define BARRIL 'B'
#define COFRE 'C' 

// Estructura que representa una habitación
struct Room {
    int x, y, width, height;
    Room(int x, int y, int w, int h) : x(x), y(y), width(w), height(h) {}
};

// Generador de mapas usando el algoritmo BSP
class BSPMapGenerator {
private:
    int mapWidth, mapHeight, nivel; // Dimensiones y nivel de la mina
    std::vector<std::vector<char>> map;
    std::vector<Room> rooms;

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

    // Selecciona un mineral aleatorio basado en el nivel de la mina
    char getRandomMineral() {
        int randVal = rand() % 100;
        if (randVal < 60) return CUARZO;
        if (nivel < 40 && randVal < 80) return COBRE;
        else if (nivel < 80 && randVal < 80) return HIERRO;
        else if (randVal < 80) return ORO;
        if (randVal < 90) return JADE;
        return DIAMANTE;
    }

    // Añade piedras en el 50% de las celdas del suelo de la habitación
    void addRocksToRoom(const Room& room) {
        for (int i = room.x; i < room.x + room.width; ++i) {
            for (int j = room.y; j < room.y + room.height; ++j) {
                if (map[j][i] == '.' && rand() % 100 < 50) {
                    map[j][i] = PIEDRA;
                }
            }
        }
    }

    // Añade minerales sobre piedras existentes
    void addMineralsToRoom(const Room& room) {
        int area = room.width * room.height;
        int numMinerales = area * 0.07;
        for (int i = 0; i < numMinerales; ++i) {
            int x = room.x + rand() % room.width;
            int y = room.y + rand() % room.height;
            if (map[y][x] == PIEDRA) map[y][x] = getRandomMineral();
        }
    }

    // Añade enemigos sobre roca o suelo
    void addEnemiesToRoom(const Room& room) {
        int area = room.width * room.height;
        int numEnemigos = area * 0.05;
        for (int i = 0; i < numEnemigos; ++i) {
            int x = room.x + rand() % room.width;
            int y = room.y + rand() % room.height;
            if (map[y][x] == PIEDRA || map[y][x] == '.') map[y][x] = ENEMIGO;
        }
    }

    // Añade barriles sobre suelo libre
    void addBarrelsToRoom(const Room& room) {
        int area = room.width * room.height;
        int numBarriles = area * 0.03;
        for (int i = 0; i < numBarriles; ++i) {
            int x = room.x + rand() % room.width;
            int y = room.y + rand() % room.height;
            if (map[y][x] == '.') map[y][x] = BARRIL;
        }
    }

    // Orquestador de decoración de cada habitación
    void addObjects() {
        for (const Room& room : rooms) {
            addRocksToRoom(room);
            addMineralsToRoom(room);
            addEnemiesToRoom(room);
            addBarrelsToRoom(room);
        }
    }

    // Piso especial: sin enemigos, sin rocas, con un cofre y una escalera
    void addTreasureRoom() {
        for (const Room& room : rooms) {
            for (int i = room.x; i < room.x + room.width; ++i) {
                for (int j = room.y; j < room.y + room.height; ++j) {
                    map[j][i] = '.'; // dejar todo limpio
                }
            }
            int x = room.x + rand() % room.width;
            int y = room.y + rand() % room.height;
            if (map[y][x] == '.') map[y][x] = COFRE; // agrega cofre
        }
        addStaircase();
    }

    // Añade una escalera al mapa
    void addStaircase() {
        for (int i = 0; i < 100; ++i) {
            const Room& room = rooms[rand() % rooms.size()];
            int x = room.x + rand() % room.width;
            int y = room.y + rand() % room.height;
            if (map[y][x] == '.' || map[y][x] == PIEDRA) {
                map[y][x] = ESCALERA;
                return;
            }
        }
    }

public:
    // Constructor que define dimensiones y el nivel de mina
    BSPMapGenerator(int width, int height, int nivelMina)
        : mapWidth(width), mapHeight(height), nivel(nivelMina) {
        map.resize(height, std::vector<char>(width, '#'));
        srand(nivel); // Semilla fija por nivel
    }

    // Genera el mapa completo
    void generateMap(int maxDepth) {
        split(0, 0, mapWidth, mapHeight, maxDepth);
        connectRooms();

        if (nivel % 10 == 0) {
            addTreasureRoom(); // piso especial
        } else {
            addObjects();      // piso normal
            addStaircase();
        }
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
    srand(time(nullptr)); // semilla aleatoria para que el nivel también cambie

    const int WIDTH = 40;
    const int HEIGHT = 20;
    const int DEPTH = 4;

    // Nivel de mina aleatorio entre 1 y 120
    int MINE_LEVEL = rand() % 120 + 1;
    std::cout << "Generando nivel: " << MINE_LEVEL << std::endl;

    BSPMapGenerator generator(WIDTH, HEIGHT, MINE_LEVEL);
    generator.generateMap(DEPTH);
    generator.printMap();

    return 0;
}