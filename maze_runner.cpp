#include <stdio.h>
#include <string.h>
#include <stack>
#include <iostream>
#include <sstream>
#include <fstream>
#include <unistd.h>

using namespace std;

string GAME_STATUS = "RUNNING";

// Representação de uma posição
struct pos_t {
	int i;
	int j;
};
//base de um labirinto
struct maze_data {
	string name;
	char** maze;
	int num_total_paths;
	int num_rows;
	int num_cols;
	pos_t initial_pos;
	pos_t exit_pos;
};


// Estrutura de dados contendo as próximas
// posicões a serem exploradas no labirinto
stack<pos_t> valid_positions;
/* Inserir elemento: 

	 pos_t pos;
	 pos.i = 1;
	 pos.j = 3;
	 valid_positions.push(pos)
 */
// Retornar o numero de elementos: 
//    valid_positions.size();
// 
// Retornar o elemento no topo: 
//  valid_positions.top(); 
// 
// Remover o primeiro elemento do vetor: 
//    valid_positions.pop();

/*❚█══█❚▬▬ι═══════>-----------------------------------------------FUNCOES AUXILIARES-----------------------------------------------<═══════ι▬▬❚█══█❚*/
bool isGameFinished(pos_t pos, maze_data mazeObj);
string generateBanner(string text, int horizontalSize);
string getCurrentDirectory();
string gerarBannerASCII() {
  string banner = R"(
 _______  _______  _______  _______    _______           _        _        _______  _______ 
(       )(  ___  )/ ___   )(  ____ \  (  ____ )|\     /|( (    /|( (    /|(  ____ \(  ____ )
| () () || (   ) |\/   )  || (    \/  | (    )|| )   ( ||  \  ( ||  \  ( || (    \/| (    )|
| || || || (___) |    /   )| (__      | (____)|| |   | ||   \ | ||   \ | || (__    | (____)|
| |(_)| ||  ___  |   /   / |  __)     |     __)| |   | || (\ \) || (\ \) ||  __)   |     __)
| |   | || (   ) |  /   /  | (        | (\ (   | |   | || | \   || | \   || (      | (\ (   
| )   ( || )   ( | /   (_/\| (____/\  | ) \ \__| (___) || )  \  || )  \  || (____/\| ) \ \__
|/     \||/     \|(_______/(_______/  |/   \__/(_______)|/    )_)|/    )_)(_______/|/   \__/
                                                                                            
)";

  return banner;
}
/*❚█══█❚▬▬ι═══════>-----------------------------------------------FUNCOES PRINCIPAIS-----------------------------------------------<═══════ι▬▬❚█══█❚*/
maze_data load_maze(string filePath, maze_data mazeObj);
void print_maze(maze_data mazeObj);
void walk(pos_t current_pos, maze_data current_maze);
void freeMaze(maze_data mazeObj);
/*❚█══█❚▬▬ι═══════>----------------------------------------------------MAIN--------------------------------------------------------<═══════ι▬▬❚█══█❚*/
int main() {

  	string banner = gerarBannerASCII();
  	cout << banner << endl;
	
	maze_data mazeObj;
	string fileName, path = "data/";
	cout << "Digite o nome do labirinto que deseja abrir\n" << endl;
	cin >> fileName;
	path = getCurrentDirectory() + "\\data\\" + fileName + ".txt";

	// carregar o labirinto com o nome do arquivo recebido como argumento
	mazeObj = load_maze(path, mazeObj);
	system("cls");
	mazeObj.name = fileName + " " + to_string(mazeObj.num_rows) + " x " + to_string(mazeObj.num_cols);

	// chamar a função de navegação
	walk(mazeObj.initial_pos, mazeObj);
	
	// Tratar o retorno (imprimir mensagem)
	cout << "Fim de jogo" << endl;
	freeMaze(mazeObj);
	
	return 0;
}


// Função que le o labirinto de um arquivo texto, carrega em 
// memória e retorna a posição inicial
maze_data load_maze(string filePath, maze_data mazeObj) {
	string dimensionsData;
	char c;

	// Abre o arquivo para leitura (fopen)
	ifstream file(filePath);
	if (!file.is_open()) {
		string msg = "Nao foi possivel encontrar o seguinte diretorio: " + filePath;
		throw runtime_error(msg);
	}

	// Le o numero de linhas e colunas (fscanf) 
	getline(file, dimensionsData);
	// e salva em num_rows e num_cols
	stringstream ss(dimensionsData);
  	ss >> mazeObj.num_rows >> mazeObj.num_cols;

	mazeObj.num_total_paths = mazeObj.num_rows * mazeObj.num_rows;

	// Aloca a matriz maze (malloc)
	mazeObj.maze = new char*[mazeObj.num_rows];
	for (int i = 0; i < mazeObj.num_rows; ++i) {
		mazeObj.maze[i] = new char[mazeObj.num_cols];
	}
	
    // Lendo e preenchendo a matriz com os caracteres do arquivo
	for (int i = 0; i < mazeObj.num_rows; ++i) {
		for (int j = 0; j < mazeObj.num_cols; ++j) {
			if (file.get(c) && c != '\n' && c != ' ') {
				if (c == 'e') {
					mazeObj.initial_pos.i = i;
					mazeObj.initial_pos.j = j;
				} else if (c == 's') {
					mazeObj.exit_pos.i = i;
					mazeObj.exit_pos.j = j;
				}
				mazeObj.maze[i][j] = c;
			} else {
				//Voltar o contador da coluna 1 unidade, pois este loop não contou
				--j;
			}
		}
	}

	// Fecha o arquivo
	file.close();
	return mazeObj;
}

string getCurrentDirectory() {
	char buffer[256];

	if (getcwd(buffer, sizeof(buffer)) != nullptr) {
		string currentDirectory(buffer);
		// return currentDirectory;
		return "C:\\Users\\Pichau\\UFMG\\2024_1\\ATR\\maze_runner-main\\maze_runner-main";
	} else {
		cout << "Erro ao obter o diretorio atual!" << endl;
	}
}

// Função que imprime o labirinto
void print_maze(maze_data mazeObj) {
	// Imprimir a régua horizontal superior
	printf("   ");
	int coluna = 0;
	for (int i = 0; i < mazeObj.num_cols; ++i) {
		if (coluna == 10) {
			coluna = 0;
		}
		printf("%d", coluna);
		coluna++;
	}
	printf("\n");

	// Imprimir cada linha do labirinto
	for (int i = 0; i < mazeObj.num_rows; ++i) {
	// Imprimir o número da linha
	printf("%2d ", i);

	// Imprimir cada coluna da linha
	for (int j = 0; j < mazeObj.num_cols; ++j) {
		printf("%c", mazeObj.maze[i][j]);
	}
	// Nova linha
	printf("\n");
	}
}


bool isGameFinished(pos_t pos, maze_data mazeObj) {
	bool hasAvaiblePath = false;

	if ((pos.i == mazeObj.exit_pos.i) && (pos.j == mazeObj.exit_pos.j)) {
		GAME_STATUS = "VOCE GANHOU UHUL!";
		return true;
	}

	for (int i = 0; i < mazeObj.num_rows; ++i) {
		for (int j = 0; j < mazeObj.num_cols; ++j) {
			if (mazeObj.maze[i][j] == 'x') {
				hasAvaiblePath = true;
			}
		}
	}

	if (!hasAvaiblePath) {
		GAME_STATUS = "VOCE EH UM PERDEDOR";
	}

	return !hasAvaiblePath;
}

string generateBanner(string text, int horizontalSize) {
  // Tamanho da string
  int texSize = text.length();

  // Ajusta o tamanho horizontal
  if (horizontalSize < texSize + 4) {
    horizontalSize = texSize + 4;
  }

  // Cria a string do banner
  string banner = "";

  // Borda superior
  banner += "+";
  for (int i = 0; i < horizontalSize - 2; i++) {
    banner += "-";
  }
  banner += "+";

  // Linha com texto
  banner += "\n| ";
  for (int i = 0; i < (horizontalSize - texSize - 4) / 2; i++) {
    banner += " ";
  }
  banner += text;
  for (int i = 0; i < (horizontalSize - texSize - 4) / 2 + (horizontalSize - texSize - 4) % 2; i++) {
    banner += " ";
  }
  banner += " |\n";

  // Borda inferior
  banner += "+";
  for (int i = 0; i < horizontalSize - 2; i++) {
    banner += "-";
  }
  banner += "+";

  return banner;
}

// Função responsável pela navegação.
// Recebe como entrada a posição initial e retorna um booleando indicando se a saída foi encontrada
void walk(pos_t current_pos, maze_data current_maze) {
	string banner = generateBanner(current_maze.name, current_maze.num_cols + 3);
	cout << banner << endl;
	print_maze(current_maze);
	pos_t next_pos;
	bool foundExit = (current_pos.i == mazeObj.exit_pos.i) && (current_pos.j == mazeObj.exit_pos.j);

	//Verifica deslocamento nas 4 direções
	for (i = 0; i <= 1; i++) {
		for (j = 0; j<= 1; j++) {
			pos_t possible_pos;
			possible_pos.i = current_pos.i + i;
			possible_pos.j = current_pos.j + j;

			bool valid_line_cordinate = (possible_pos.i >= 0) && (possible_pos.i < current_maze.num_rows);
			bool valid_col_cordinate = (new_col_pos.j >= 0) && (possible_pos.j < current_maze.num_cols);
			
			if (valid_line_cordinate && valid_col_cordinate && current_maze.maze[new_line_pos][new_col_pos] != '#' && current_maze.maze[new_line_pos][new_col_pos] != '.') {
				valid_positions.push(possible_pos);
			}
		}
	}

	if (foundExit) {
		GAME_STATUS = "VOCE GANHOU";
		return;
	} else if (possible_pos.empty() && !foundExit) {
		GAME_STATUS = "VOCE PERDEU FI";
		return;
	}
	
	next_pos = valid_positions.top();
	valid_positions.pop();

	//posicao anterior vira '.'
	current_maze.maze[current_pos.i][current_pos.j] = '.';
	//posicao atual vira 'o'
	current_maze.maze[next_pos.i][next_pos.j] = 'o';

	system("cls");
	walk(next_pos, current_maze);

}

void freeMaze(maze_data mazeObj) {
  for (int i = 0; i < mazeObj.num_rows; i++) {
    delete[] mazeObj.maze[i]; // Libera cada linha da matriz
  }
  delete[] mazeObj.maze; // Libera o ponteiro principal da matriz
}
