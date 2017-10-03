/*
Niniejszy program jest wolnym oprogramowaniem; mo¿esz go
rozprowadzaæ dalej i / lub modyfikowaæ na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundacjê Wolnego
Oprogramowania - wed³ug wersji 2 tej Licencji lub(wed³ug twojego
wyboru) którejœ z póŸniejszych wersji.

Niniejszy program rozpowszechniany jest z nadziej¹, i¿ bêdzie on
u¿yteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyœlnej
gwarancji PRZYDATNOŒCI HANDLOWEJ albo PRZYDATNOŒCI DO OKREŒLONYCH
ZASTOSOWAÑ.W celu uzyskania bli¿szych informacji siêgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnoœci¹ wraz z niniejszym programem otrzyma³eœ te¿ egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeœli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <conio.h>
#include <iostream>

#include "sphere.h"
#include "constants.h"
#include "lodepng.h"
#include "myCube.h"
#include "board.h"
#include "model2.h"

float speed = 0; // [radiany/s]
float l_high = 30.0f;
float l_angle = 10.0f;
int speed_x = 0, speed_y = 0;

Models::Sphere mySphere(0.5f, 18, 18);
Model2 stone, sun;
const int levels = 4;
const int s_plansza = 20; //rozmiar planszy
int plansza[levels][s_plansza][s_plansza];

int poziom = 0;
//skrz_str *skrzynka;
int character_x, character_y; // pozycja bohatera (ball)
int tempruch = 0, ruch = 0;
int reakcja1 = 0, reakcja2 = 0, reakcja3 = 0;
int restart = 0;
void key_callback(GLFWwindow* window, int key,
	int scancode, int action, int mods) {
	reakcja1 = reakcja2 = 1;
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) { character_y = speed_y; character_x = speed_x; speed_x = 1; };
		if (key == GLFW_KEY_RIGHT) { character_y = speed_y; character_x = speed_x; speed_x = -1; };
		if (key == GLFW_KEY_UP) { character_y = speed_y; character_x = speed_x; speed_y = -1; };
		if (key == GLFW_KEY_DOWN) { character_y = speed_y; character_x = speed_x; speed_y = 1; };
		if (key == GLFW_KEY_R) { restart = 1; }
		if (key == GLFW_KEY_A)speed = -3.14;
		if (key == GLFW_KEY_D)speed = +3.14;
		if (key == GLFW_KEY_W && l_high>10.0f)l_high -= 1.0f;
		if (key == GLFW_KEY_S && l_high<40.0f)l_high += 1.0f;
		if (key == GLFW_KEY_Q && l_angle >0.0f)l_angle -= 1.0f;
		if (key == GLFW_KEY_E && l_angle <20.0f)l_angle += 1.0f;
	}

	if (action == GLFW_RELEASE) {
		speed = 0;
		speed_x = 0;
		speed_y = 0;
	}
}

//Procedura obs³ugi b³êdów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}
GLuint tex0, tex1, tex2, tex3, tex4, tex5, tex6;

GLuint readTexture(char* filename) {

	GLuint	tex;
	glActiveTexture(GL_TEXTURE0);
	//Wczytanie do pamiêci komputera
	std::vector<unsigned char>image;   //Alokuj wektor do wczytania obrazka
	unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
	//Wczytaj obrazek
	unsigned error = lodepng::decode(image, width, height, filename);
	//Import do pamiêci karty graficznej
	glGenTextures(1, &tex); //Zainicjuj jeden uchwyt
	glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
	//Wczytaj obrazek do pamiêci KG skojarzonej z uchwytem
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	return tex;
}


//Procedura inicjuj¹ca
void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który nale¿y wykonaæ raz, na pocz¹tku programu************
	glClearColor(0, 0, 0, 1); //Czyœæ ekran na czarno
	glEnable(GL_LIGHTING); //W³¹cz tryb cieniowania
	
	glEnable(GL_DEPTH_TEST); //W³¹cz u¿ywanie Z-Bufora
	glEnable(GL_COLOR_MATERIAL); //glColor3d ma modyfikowaæ w³asnoœci materia³u

	glShadeModel(GL_SMOOTH);
	tex0 = readTexture("textures/wall.png");
	tex1 = readTexture("textures/platform.png"); 
	tex2 = readTexture("textures/grass.png");
	tex3 = readTexture("textures/main_character.png");
	tex4 = readTexture("textures/box.png");
	tex5 = readTexture("textures/stone.png");
	tex6 = readTexture("textures/sun.png");
	glfwSetKeyCallback(window, key_callback);

}
int poczatek = 1;
int *a, *b;
int skrzynka_id, s_id;
int skrzynia_x, skrzynia_y; // pozycja skrzyni
int **tabx = new int *[levels], **taby = new int *[levels];

// Tworzenie gry
int tmpa, tmpb;
int tmpsk_x, tmpsk_y;
int suma = 0;
int *ilzal;
int *ilosc;
int *restarta, *restartb, **restartska = new int *[levels], **restartskb = new int *[levels];
int **tabska = new int *[levels], **tabskb = new int *[levels];
int sprawdz(int x, int y, int ilosc){
	for (int i = 0; i<ilosc; i++)
	{
		if ((tabska[poziom][i] == x) && (tabskb[poziom][i] == y))
		{
			return i;
		}
	}
	return -1;
}
int tmp = 0;
char *nazw4;
int sum = 0;

int poziomm = 1;
//Procedura rysuj¹ca zawartoœæ sceny
void drawScene(GLFWwindow* window, float angle) {
	//************Tutaj umieszczaj kod rysuj¹cy obraz******************l
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_TEXTURE_2D);
	float swiatlo_otoczenia[] = { 0.9f, 0.9f, 0.9f,0.9f };
	float swiatlo_rozproszone[] = { 0.2f, 0.2f, 0.6f, 1.0f };
	float swiatlo_odbite[] = { 0.2f, 0.2f, 0.2f, 0.2f };
	float swiatlo_pozycja[] = { 3.0f, 3.0f, 5.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, swiatlo_otoczenia);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, swiatlo_rozproszone);
	glLightfv(GL_LIGHT0, GL_SPECULAR, swiatlo_odbite);
	glLightfv(GL_LIGHT0, GL_POSITION, swiatlo_pozycja);

	float material[] = { 0.0, 0.0, 0.0, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material);
	float material2[] = { 0.7, 0.5, 0.5, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material2);
	float material3[] = { 0.5, 0.5, 0.5, 1.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material3);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Wykonaj czyszczenie bufora kolorów

	std::ifstream plik;

	glm::mat4 V = glm::lookAt( //Wylicz macierz widoku
		glm::vec3(0.0f, l_angle, l_high),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f));

	glm::mat4 P = glm::perspective(50 * PI / 180, 1.0f, 1.0f, 50.0f); //Wylicz macierz rzutowania

	 //Za³aduj macierz rzutowania do OpenGL
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(P));

	//PrzejdŸ w tryb pracy z macierz¹ Model-Widok
	glMatrixMode(GL_MODELVIEW);

	//Wylicz macierz obrotu o k¹t angle wokó³ osi (0,0,1)
	glm::mat4 M = glm::mat4(1.0f);
	M = glm::rotate(M, angle, glm::vec3(0, 0, 1));
	glLoadMatrixf(glm::value_ptr(V*M)); //Za³aduj wyliczon¹ macierz do OpenGL

	//Narysuj model
	glBindTexture(GL_TEXTURE_2D, tex2);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	//glEnableClientState(GL_COLOR_ARRAY); //W³¹cz u¿ywanie tablicy kolorów
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//glColorPointer(3, GL_FLOAT, 0, myCubeColors1); //Wska¿ tablicê kolorów
	glVertexPointer(3, GL_FLOAT, 0, boardVertices);
	glTexCoordPointer(2, GL_FLOAT, 0, boardTex);
	glNormalPointer(GL_FLOAT, 0, geomNormals);
	glDrawArrays(GL_QUADS, 0, boardCount); //Wykonaj rysowanie
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	//-------------------------------------------------------------//

	if (poziomm == 1) {
		
		stone.loadOBJ("stone.obj", stone.Vertices, stone.Textures, stone.NormalVertices, &stone.numVertices, &stone.numTextures, &stone.normalNumVertices);
		sun.loadOBJ("sphere.obj", sun.Vertices, sun.Textures, sun.NormalVertices, &sun.numVertices, &sun.numTextures, &sun.normalNumVertices);
		poziomm = 0;
	}
	if (poczatek == 1) {
		restarta = new int[levels], restartb = new int[levels];
		ilosc = new int[levels];
		a = new int[levels], b = new int[levels];
		for (int i = 0; i < levels; i++)
			ilosc[i] = 0;
		for (int s = 0; s < levels; s++) {
			switch (s) {
			case 0: nazw4 = "levels/poziom1.txt"; break;
			case 1: nazw4 = "levels/poziom2.txt"; break;
			case 2: nazw4 = "levels/poziom3.txt"; break;
			case 3: nazw4 = "levels/poziom4.txt"; break;
			}
			plik.open(nazw4);
			for (int i = 0; i < s_plansza; i++) {
				for (int j = 0; j < s_plansza; j++)
					plik >> plansza[s][i][j];
			}
			plik.close();
		}

		// Wstawianie ruchomych obiektow
		for (int s = 0; s < levels; s++) {
			for (int i = 0; i < s_plansza; i++)
			{
				for (int j = 0; j < s_plansza; j++)
				{
					if (plansza[s][i][j] == 3)
						sum++;
				}
			}
			ilosc[s] = sum;
			sum = 0;
		}
		for (int i = 0; i < levels; i++) {
			tabx[i] = new int[ilosc[i]];
			taby[i] = new int[ilosc[i]];
			restartska[i] = new int[ilosc[i]];
			restartskb[i] = new int[ilosc[i]];
			tabska[i] = new int[ilosc[i]];
			tabskb[i] = new int[ilosc[i]];
		}


		for (int s = 0;s<levels;s++) {
			for (int i = 0; i < s_plansza; i++)
			{
				for (int j = 0; j < s_plansza; j++)
				{
					if (plansza[s][i][j] == 4)
					{
						restarta[s] = a[s] = i;
						restartb[s] = b[s] = j;
					}
					if (plansza[s][i][j] == 3)
					{
						restartska[s][tmp] = tabska[s][tmp] = j;
						restartskb[s][tmp] = tabskb[s][tmp] = i;
						tmp++;
					}
				}
			}
			tmp = 0;
		}

		poczatek = 0;
	}
	else {
		suma = 0;
		tmpa = a[poziom];
		tmpb = b[poziom];
		ilzal = new int[ilosc[poziom]];
		for (int i = 0; i < ilosc[poziom]; i++)
			ilzal[i] = 0;
		if (reakcja1 == 1) {
			a[poziom] = a[poziom] + speed_y;
			b[poziom] = b[poziom] + speed_x;
			reakcja1 = 0;
		}

		if (plansza[poziom][a[poziom]][b[poziom]] == 1 || plansza[poziom][a[poziom]][b[poziom]] == 0)
		{
			a[poziom] = tmpa;
			b[poziom] = tmpb;
			speed_x = character_x;
			speed_y = character_y;
		}
		tmpsk_x = skrzynia_x;
		tmpsk_y = skrzynia_y;

		skrzynka_id = sprawdz(b[poziom], a[poziom], ilosc[poziom]);

		skrzynia_y = b[poziom] + speed_x;
		skrzynia_x = a[poziom] + speed_y;

		if (skrzynka_id != -1)
		{
			if (plansza[poziom][skrzynia_x][skrzynia_y] == 1 || plansza[poziom][skrzynia_x][skrzynia_y] == 0)
			{
				skrzynia_x = tmpsk_x;
				skrzynia_y = tmpsk_y;
				a[poziom] = tmpa;
				b[poziom] = tmpb;
			}
			else if (sprawdz(skrzynia_y, skrzynia_x, ilosc[poziom]) != -1) {
				s_id = sprawdz(skrzynia_y, skrzynia_x, ilosc[poziom]);
				skrzynia_x = tmpsk_x;
				skrzynia_y = tmpsk_y;
				a[poziom] = tmpa;
				b[poziom] = tmpb;
			}
			else {
				tabska[poziom][skrzynka_id] = skrzynia_y;
				tabskb[poziom][skrzynka_id] = skrzynia_x;
			}
		}

		// rysowanie postaci gracza
		glm::mat4 W = glm::mat4(1.0f);
		W = glm::translate(W, glm::vec3(-10 + b[poziom] * 1, -10 + a[poziom] * 1, 0.5f));
		glLoadMatrixf(glm::value_ptr(V*M*W));
		glBindTexture(GL_TEXTURE_2D, tex3);
		mySphere.drawSolid();

		// rysowanie boxów do przeniesienia
		for (int i = 0; i < ilosc[poziom]; i++)
		{
			glm::mat4 W = glm::mat4(1.0f);
			W = glm::translate(W, glm::vec3(-10 + tabska[poziom][i] * 1, -10 + tabskb[poziom][i] * 1, 0.5f));
			W = glm::scale(W, glm::vec3(0.8f, 0.8f, 1.0f));
			glLoadMatrixf(glm::value_ptr(V*M*W));
			glBindTexture(GL_TEXTURE_2D, tex4);
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glVertexPointer(3, GL_FLOAT, 0, myCubeVertices);
			glTexCoordPointer(2, GL_FLOAT, 0, geomTexCoords);
			glDrawArrays(GL_QUADS, 0, myCubeVertexCount); //Wykonaj rysowanie*/
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);

			if (plansza[poziom][tabskb[poziom][i]][tabska[poziom][i]] == 2){ ilzal[i] = 1;}
			else ilzal[i] = 0;
		}
		// --------------------------------------------------


		for (int i = 0; i < ilosc[poziom]; i++)
			suma += ilzal[i];
		if (suma == ilosc[poziom])
		{
			suma = 0;
			poziom++;
		}
		if (restart == 1)
		{
			a[poziom] = restarta[poziom];
			b[poziom] = restartb[poziom];
			for (int i = 0; i < ilosc[poziom]; i++)
			{
				tabska[poziom][i] = restartska[poziom][i];
				tabskb[poziom][i] = restartskb[poziom][i];
			}
			suma = 0;
			restart = 0;
		}

		// ----------------------------------------------------
		// rysowanie reszty elementów

		for (int i = 0; i < s_plansza; i++) {
			for (int j = 0; j < s_plansza; j++) {
				if (plansza[poziom][i][j] == 1) { // skrzynki blokuj¹ce

					glm::mat4 W = glm::mat4(1.0f);
					W = glm::translate(W, glm::vec3(-10 + j * 1, -10 + i * 1, 0.5f));
					glLoadMatrixf(glm::value_ptr(V*M*W));
					glBindTexture(GL_TEXTURE_2D, tex0);
					glEnableClientState(GL_VERTEX_ARRAY);
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glEnableClientState(GL_NORMAL_ARRAY);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

					glVertexPointer(3, GL_FLOAT, 0, myCubeVertices);
					glNormalPointer(GL_FLOAT, 0, normals);
					glTexCoordPointer(2, GL_FLOAT, 0, geomTexCoords);
					glDrawArrays(GL_QUADS, 0, myCubeVertexCount); //Wykonaj rysowanie
					glDisableClientState(GL_VERTEX_ARRAY);
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
					glDisableClientState(GL_NORMAL_ARRAY);
				}
				if (plansza[poziom][i][j] == 2) { // podest

					glm::mat4 W = glm::mat4(1.0f);
					W = glm::translate(W, glm::vec3(-10 + j * 1, -10 + i * 1, 0.1f));
					W = glm::scale(W, glm::vec3(1.0f, 1.0f, 0.2f));
					glLoadMatrixf(glm::value_ptr(V*M*W));
					glBindTexture(GL_TEXTURE_2D, tex1);
					glEnableClientState(GL_VERTEX_ARRAY);
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glEnableClientState(GL_NORMAL_ARRAY);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

					glVertexPointer(3, GL_FLOAT, 0, myCubeVertices);
					glTexCoordPointer(2, GL_FLOAT, 0, geomTexCoords);
					glDrawArrays(GL_QUADS, 0, myCubeVertexCount); //Wykonaj rysowanie
					glDisableClientState(GL_VERTEX_ARRAY);
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
					glDisableClientState(GL_NORMAL_ARRAY);
				}

			}
		}
		//-----------------------------------------------------------------------------
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		//rysowanie kamienia
		glBindTexture(GL_TEXTURE_2D, tex5);
		glm::mat4 M2 = glm::mat4(1.0f);
		M2 = glm::translate(M2, glm::vec3(7, 7, 0.5f));
		M2 = glm::scale(M2, glm::vec3(2.0f, 2.0f, 1.0f));
		glLoadMatrixf(glm::value_ptr(V*M2*M));
		glVertexPointer(3, GL_FLOAT, 0, stone.Vertices);
		glTexCoordPointer(2, GL_FLOAT, 0, stone.Textures);
		glNormalPointer(GL_FLOAT, sizeof(float) * 3, stone.NormalVertices);
		glDrawArrays(GL_TRIANGLES, 0, stone.numVertices);
		//rysowanie s³oñca
		glBindTexture(GL_TEXTURE_2D, tex6);
		glm::mat4 M3 = glm::mat4(1.0f);
		M3 = glm::translate(M3, glm::vec3(-10, -10, 7));
		M3 = glm::scale(M3, glm::vec3(4.0f, 4.0f, 4.0f));
		glLoadMatrixf(glm::value_ptr(V*M3*M));
		glVertexPointer(3, GL_FLOAT, 0, sun.Vertices);
		glTexCoordPointer(2, GL_FLOAT, 0, sun.Textures);
		glNormalPointer(GL_FLOAT, sizeof(float) * 3, sun.NormalVertices);
		glDrawArrays(GL_TRIANGLES, 0, sun.numVertices);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		//---------------------------------------------------------------------------
	}
	glDisableClientState(GL_VERTEX_ARRAY); //Wy³¹cz u¿ywanie tablicy wierzcho³ków
	glDisableClientState(GL_COLOR_ARRAY); //Wy³¹cz u¿ywanie tablicy kolorów
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	//Przerzuæ tylny bufor na przedni
	glfwSwapBuffers(window);
}

int main(void)
{
	GLFWwindow* window; //WskaŸnik na obiekt reprezentuj¹cy okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurê obs³ugi b³êdów

	if (!glfwInit()) { //Zainicjuj bibliotekê GLFW
		fprintf(stderr, "Nie mo¿na zainicjowaæ GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(1024, 1024, "OpenGL", NULL, NULL);  //Utwórz okno 1024x1024 o tytule "OpenGL"

	if (!window) //Je¿eli okna nie uda³o siê utworzyæ, to zamknij program
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje siê aktywny i polecenia OpenGL bêd¹ dotyczyæ w³aœnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekê GLEW
		fprintf(stderr, "Nie mo¿na zainicjowaæ GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjuj¹ce

	float angle = 0; //K¹t obrotu
	glfwSetTime(0); //Wyzeruj licznik czasu

					//G³ówna pêtla
	while (!glfwWindowShouldClose(window)) //Tak d³ugo jak okno nie powinno zostaæ zamkniête
	{
		angle += speed*glfwGetTime(); //Zwiêksz k¹t o prêdkoœæ k¹tow¹ razy czas jaki up³yn¹³ od poprzedniej klatki
		glfwSetTime(0); //Wyzeruj licznik czasu
		drawScene(window, angle); //Wykonaj procedurê rysuj¹c¹
		glfwPollEvents(); //Wykonaj procedury callback w zaleznoœci od zdarzeñ jakie zasz³y.
	}
	glDeleteTextures(1, &tex0);
	glDeleteTextures(1, &tex1);
	glDeleteTextures(1, &tex2);
	glDeleteTextures(1, &tex3);
	glDeleteTextures(1, &tex4);
	glDeleteTextures(1, &tex5);
	glDeleteTextures(1, &tex6);
	glfwDestroyWindow(window); //Usuñ kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajête przez GLFW
	exit(EXIT_SUCCESS);
}


