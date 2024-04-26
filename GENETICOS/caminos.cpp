#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;

const int NUM_CIUDADES = 5;

struct Ciudad {
    int x, y;
};

double distancia(const Ciudad& ciudad1, const Ciudad& ciudad2) {
    int dx = ciudad1.x - ciudad2.x;
    int dy = ciudad1.y - ciudad2.y;
    return sqrt(dx * dx + dy * dy);
}

struct Individuo {
    vector<int> recorrido;
    double fitness;

    Individuo(const vector<Ciudad>& ciudades) {
        recorrido.resize(NUM_CIUDADES);
        for (int i = 0; i < NUM_CIUDADES; ++i) {
            recorrido[i] = i;
        }
        random_shuffle(recorrido.begin() + 1, recorrido.end());
        fitness = calcularFitness(ciudades);
    }

    double calcularFitness(const vector<Ciudad>& ciudades) {
        double dist = 0.0;
        for (int i = 0; i < NUM_CIUDADES - 1; ++i) {
            dist += distancia(ciudades[recorrido[i]], ciudades[recorrido[i + 1]]);
        }
        dist += distancia(ciudades[recorrido[NUM_CIUDADES - 1]], ciudades[recorrido[0]]);
        return 1.0 / dist;
    }
};

Individuo cruzar(const Individuo& padre1, const Individuo& padre2, const vector<Ciudad>& ciudades) {
    int punto1 = rand() % NUM_CIUDADES;
    int punto2 = rand() % NUM_CIUDADES;
    if (punto1 > punto2) {
        swap(punto1, punto2);
    }

    vector<int> hijo(NUM_CIUDADES, -1);

    for (int i = punto1; i <= punto2; ++i) {
        hijo[i] = padre1.recorrido[i];
    }

    int j = 0;
    for (int i = 0; i < NUM_CIUDADES; ++i) {
        if (j == punto1) {
            j = punto2 + 1;
        }
        if (find(hijo.begin(), hijo.end(), padre2.recorrido[i]) == hijo.end()) {
            hijo[j++] = padre2.recorrido[i];
        }
    }

    Individuo hijoIndividuo(ciudades);
    hijoIndividuo.recorrido = hijo;

    return hijoIndividuo;
}

void mutar(Individuo& individuo, const vector<Ciudad>& ciudades) {
    int indice1 = rand() % NUM_CIUDADES;
    int indice2 = rand() % NUM_CIUDADES;
    swap(individuo.recorrido[indice1], individuo.recorrido[indice2]);
    individuo.fitness = individuo.calcularFitness(ciudades);
}

pair<Individuo, Individuo> seleccionarPadres(const vector<Individuo>& poblacion) {
    int indice1 = rand() % poblacion.size();
    int indice2 = rand() % poblacion.size();
    if (poblacion[indice1].fitness > poblacion[indice2].fitness) {
        return make_pair(poblacion[indice1], poblacion[indice2]);
    }
    else {
        return make_pair(poblacion[indice2], poblacion[indice1]);
    }
}

Individuo ejecutarAlgoritmoGenetico(int poblacionSize, int generaciones, const vector<Ciudad>& ciudades, vector<double>& mejoresFitness) {
    srand(static_cast<unsigned int>(time(nullptr)));

    const int TAM_POBLACION = poblacionSize;
    const double PROBABILIDAD_MUTACION = 10;

    vector<Individuo> poblacion(TAM_POBLACION, Individuo(ciudades));

    Individuo mejorIndividuo(ciudades);
    double mejorFitness = mejorIndividuo.fitness;

    for (int generacion = 0; generacion < generaciones; ++generacion) {
        vector<Individuo> nuevaPoblacion;
        while (nuevaPoblacion.size() < TAM_POBLACION) {
            pair<Individuo, Individuo> padres = seleccionarPadres(poblacion);
            Individuo hijo = cruzar(padres.first, padres.second, ciudades);
            // Aplicar mutación con probabilidad PROBABILIDAD_MUTACION
            if ((rand() / static_cast<double>(RAND_MAX)) < PROBABILIDAD_MUTACION) {
                mutar(hijo, ciudades);
            }
            nuevaPoblacion.push_back(hijo);

            if (hijo.fitness > mejorFitness) {
                mejorIndividuo = hijo;
                mejorFitness = hijo.fitness;
            }
        }
        poblacion = nuevaPoblacion;

        mejoresFitness.push_back(1.0 / mejorFitness);
    }

    cout << "Mejor recorrido encontrado: ";
    for (int i = 0; i < NUM_CIUDADES; ++i) {
        cout << mejorIndividuo.recorrido[i] << " ";
    }
    cout << "| Fitness: " << 1.0 / mejorFitness << endl;

    return mejorIndividuo;
}

int randInt(int min, int max) {
    return rand() % (max - min + 1) + min;
}

// Función para dibujar una flecha
void drawArrow(RenderWindow& window, Vector2f p, Vector2f q, Color color, float arrowSize = 20.0f) {
    // Calcular la dirección y la longitud de la flecha
    Vector2f direction = q - p;
    float length = sqrt(direction.x * direction.x + direction.y * direction.y);
    direction /= length; // Normalizar la dirección

    // Calcular la posición de la punta de la flecha
    Vector2f arrowTip = q - direction * arrowSize;

    // Calcular las posiciones de los dos puntos de la base de la flecha
    Vector2f arrowBase1 = arrowTip + Vector2f(direction.y, -direction.x) * (arrowSize / 2.0f);
    Vector2f arrowBase2 = arrowTip - Vector2f(direction.y, -direction.x) * (arrowSize / 2.0f);

    // Dibujar la línea principal de la flecha
    Vertex line[] = { Vertex(p, color), Vertex(arrowTip, color) };
    window.draw(line, 2, Lines);

    // Dibujar la punta de la flecha
    Vertex arrowHead[] = { Vertex(arrowBase1, color), Vertex(q, color), Vertex(arrowBase2, color) };
    window.draw(arrowHead, 3, LinesStrip);
}

int main() {
    int poblacionSize = 28;
    int generaciones = 100;
    //srand(static_cast<unsigned int>(time(nullptr)));
    vector<Ciudad> ciudades(NUM_CIUDADES);
    for (int i = 0; i < NUM_CIUDADES; ++i) {
        ciudades[i].x = randInt(100, 500); // Ajuste de posición en el rango de la ventana
        ciudades[i].y = randInt(100, 500); // Ajuste de posición en el rango de la ventana
    }

    vector<double> mejoresFitness;

    Individuo mejorRecorrido = ejecutarAlgoritmoGenetico(poblacionSize, generaciones, ciudades, mejoresFitness);

    cout << "Mejor fitness encontrado: " << 1.0 / mejorRecorrido.fitness << endl;

    RenderWindow window(VideoMode(600, 600), "TSP");
    RenderWindow fitnessWindow(VideoMode(700, 700), "Fitness Evolution");
    Event event;
    window.setFramerateLimit(30);
    fitnessWindow.setFramerateLimit(30);

    // Cálculo del valor máximo y mínimo de mejoresFitness
    double maxFitness = mejoresFitness[0];
    double minFitness = mejoresFitness[0];
    for (double fitness : mejoresFitness) {
        if (fitness > maxFitness) {
            maxFitness = fitness;
        }
        if (fitness < minFitness) {
            minFitness = fitness;
        }
    }

    // Cálculo de la escala para el gráfico de fitness
    double tope = mejoresFitness[0];
    double div = static_cast<double>(500 / (maxFitness - minFitness));
    double divi = static_cast<double>(500 / (generaciones - 1));

    // Evento de cierre mejorado
    while (window.isOpen() || fitnessWindow.isOpen())
    {
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed) {
                window.close();
                fitnessWindow.close();
            }
        }

        window.clear();

        for (int i = 0; i < NUM_CIUDADES; i++) {
            for (int j = 0; j < NUM_CIUDADES; j++) {
                if (i != j) {
                    Vertex line[] =
                    {
                        Vertex(Vector2f(ciudades[i].x, ciudades[i].y), Color::White),
                        Vertex(Vector2f(ciudades[j].x, ciudades[j].y), Color::White)
                    };
                    window.draw(line, 2, Lines);
                }
            }
        }

        // Dibuja las ciudades
        for (int i = 0; i < NUM_CIUDADES; i++) {
            CircleShape cir(5);
            if (i == mejorRecorrido.recorrido[0]) {
                cir.setFillColor(Color::Blue); // Punto de inicio en azul
            } else if (i == mejorRecorrido.recorrido[NUM_CIUDADES - 1]) {
                cir.setFillColor(Color::Yellow); // Punto final en amarillo
            } else {
                cir.setFillColor(Color::Red); // Resto de puntos en rojo
            }
            cir.setPosition(static_cast<float>(ciudades[i].x), static_cast<float>(ciudades[i].y));
            window.draw(cir);
        }


        // Dibuja la mejor ruta del mejor recorrido encontrado en verde con flechas
        for (int i = 0; i < NUM_CIUDADES; i++) {
            int ciudadActual = mejorRecorrido.recorrido[i];
            int ciudadSiguiente = mejorRecorrido.recorrido[(i + 1) % NUM_CIUDADES];
            drawArrow(window, Vector2f(ciudades[ciudadActual].x, ciudades[ciudadActual].y), Vector2f(ciudades[ciudadSiguiente].x, ciudades[ciudadSiguiente].y), Color::Green);
        }

        window.display();

        // Dibuja el gráfico de fitness
        fitnessWindow.clear();
        VertexArray ejeX(Lines, 2);
        ejeX[0].position = Vector2f(100, 100);
        ejeX[1].position = Vector2f(100, 600);
        fitnessWindow.draw(ejeX);

        VertexArray ejeY(Lines, 2);
        ejeY[0].position = Vector2f(600, 600);
        ejeY[1].position = Vector2f(100, 600);
        fitnessWindow.draw(ejeY);

        // Añadir etiquetas para los ejes
        Text text;
        Font font;
        font.loadFromFile("arial.ttf"); // Asegúrate de tener este archivo de fuente en tu directorio
        text.setFont(font);
        text.setCharacterSize(14);
        text.setFillColor(Color::White);

        // Etiqueta para el eje Y (Fitness)
        text.setPosition(50, 50);
        text.setString("Fitness");
        fitnessWindow.draw(text);

        // Etiqueta para el eje X (Generaciones)
        text.setPosition(650, 620);
        text.setString("Generaciones");
        fitnessWindow.draw(text);

        for (int i = 0; i < generaciones - 1; i += 1) {
            VertexArray line(Lines, 2);
            line[1].position = Vector2f(100 + (i * divi), 600 - mejoresFitness[i] * div);
            line[0].position = Vector2f(100 + ((i + 1) * divi), 600 - mejoresFitness[i + 1] * div);
            fitnessWindow.draw(line);
        }

        // Añadir una leyenda para la línea de la gráfica de fitness
        text.setPosition(650, 50);
        text.setString("Mejor Fitness");
        fitnessWindow.draw(text);

        RectangleShape lineLegend(Vector2f(50, 2));
        lineLegend.setPosition(650, 70);
        lineLegend.setFillColor(Color::Black);
        fitnessWindow.draw(lineLegend);

        fitnessWindow.display();
    }

    return 0;
}
