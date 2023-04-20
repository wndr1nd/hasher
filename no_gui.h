
#ifndef NO_GUI_H
#define NO_GUI_H

#ifndef nox11
#include "mainwindow.h"
void main_win(MainWindow *window);
#endif

#include <vector>
#include "Ihash.h"
void delete_hash(std::string path);
void get_path(std::vector<std::string> &paths, std::string curr_path, std::string param, std::string param2);
void start_hash(std::string dir1, std::vector<Ihash*> &gui_vec, std::string param1 = "", std::string param2 = "", int threads = 1);
void fill_vec(std::vector<Ihash*>& vector);
void check_hash(std::string par, std::string par2, std::string curPath, int quantity_threads = 1);
void cmd_handler(std::string param, std::string param2, std::string command, std::string path);
void check_file(std::string par, std::string par2, std::string curPath);
#endif // NO_GUI_H
