#include <iostream>
#include <vector>
#include <filesystem>
#include <sstream>
#include "Ihash.h"
#include "md5.h"
#include "sha256.h"
#include "sha1.h"
#include "crc32.h"
#include <algorithm>
#include <set>
#include <string>
#include <thread>
#include <ctime>


#ifdef WIN32
std::string separ = "\\";
#endif

#ifdef __unix__
std::string separ = "/";
#endif

#ifndef nox11
#include "mainwindow.h"
#include <QCoreApplication>
QVector<MainWindow *> window_vec; //вектор для окна
#endif

std::vector<std::string> file_path; //вектор для путей к файлам
std::vector<std::thread> threads; //вектор для потоков
std::vector<Ihash*> vec; //алгоритмы

void check_hash(std::string par, std::string par2, std::string curPath, int quantity);
void check_file(std::string par, std::string par2, std::string curPath);
void fill_vec(std::vector<Ihash*>& vector);
void clear_thread();

#ifndef nox11
void main_win(MainWindow *window)
{
    window_vec.append(window);
}
#endif


//получение всех путей
void get_path(std::vector<std::string> &paths, std::string curr_path, std::string param = "", std::string param2 = "")
{
    for (auto& elem : std::filesystem::directory_iterator(curr_path))
    {
        if (!std::filesystem::is_directory(elem))
        {
            paths.push_back(elem.path().string());

        }
    }

    if (param == "rec" || param2 == "rec")
    {
        for (auto& elem : std::filesystem::directory_iterator(curr_path))
        {
            if (std::filesystem::is_directory(elem))
            {
                paths.push_back(elem.path().string());
                get_path(paths, elem.path().string(), param, param2);
            }
        }
    }

}


void clear_all()
{
    file_path.clear();
    vec.clear();
}


void start_hash(std::string dir1, std::vector<Ihash*> &gui_vec, std::string param1 = "", std::string param2 = "", int quantity_threads = 1)
{
    int quantity;
    std::string base_path = dir1;
    std::string rel_path; //относительный путь к файлу
    if (quantity_threads == 0)
    {
        quantity = std::thread::hardware_concurrency();
    }
    else
    {
        quantity = quantity_threads;
    }

    int num_threads = 0;

    if (!gui_vec.empty())
    {
        for (size_t i = 0; i < gui_vec.size(); ++i)
        {
            vec.resize(gui_vec.size());
            vec[i] = gui_vec[i];
        }
    }

    else if (vec.empty())
    {
        fill_vec(vec);
    }

    get_path(file_path, dir1, param1, param2);

    for (auto& file : file_path)
    {
        if (std::filesystem::is_directory(file))
        {
            if (param2 == "sep" || param1 == "sep")
            {
                clear_thread();
                dir1 = file;
            }
            continue;
        }
        else if (file.ends_with("hash.md5") || file.ends_with("hash.sha256") || file.ends_with("hash.crc32")
            || file.ends_with("hash.sha1")) continue;

        rel_path = std::filesystem::relative(file, base_path).string();


#ifndef nox11
if (!window_vec.isEmpty())
{
    window_vec[0]->set_log(rel_path);
    QCoreApplication::processEvents(); //обработка событий, нужна для отображения информации
}
#endif

    if (param1 != "silent" && param2 != "silent")
    {
        std::cout << rel_path << std::endl;
    }

    if (num_threads <= quantity && quantity != 1)
    {
        std::thread th([&](){ //без временных переменных иногда записывается каша
            std::string tmp_3 = dir1 + separ;
            std::string tmp_1 = file;
            std::string tmp_2 = std::filesystem::relative(file, base_path).string();
            for (auto& elem : vec)
            {
                elem->start_hashing(tmp_1, tmp_3 + elem->getName(), tmp_2);
            }
        });
        ++num_threads;
        threads.push_back(std::move(th));

        if (num_threads >= quantity)
        {
            clear_thread();
            num_threads = 0;
        }
    }

    else
    {
        for (auto& elem : vec)
        {
            elem->start_hashing(file, dir1 + separ + elem->getName(), rel_path);
        }
    }


  }

    clear_thread();
    clear_all();
}


void check_argv(std::string arg)
{
    if (arg == "md5" || arg == "--md5")
    {
        vec.push_back(new MD5);
    }
    if (arg == "sha256" || arg == "--sha256")
    {
        vec.push_back(new SHA256);
    }
    if (arg == "crc32" || arg == "--crc32")
    {
        vec.push_back(new CRC32);
    }
    if (arg == "sha1" || arg == "--sha1")
    {
        vec.push_back(new SHA1);
    }
}


void delete_hash(std::string path)
{
    get_path(file_path, path, "rec");

    for (auto& file: file_path)
    {
        if (file.ends_with("hash.md5") || file.ends_with("hash.sha256") || file.ends_with("hash.crc32")
                    || file.ends_with("hash.sha1"))
        {
            std::filesystem::remove(file);
        }
    }
    clear_all();

}

//обработка параметров и вызов функций
void cmd_handler(std::string param, std::string param2, std::string command, std::string path)
{
    if (command == "clear")
    {
        std::string ans;
        std::cout << "Do you really want to delete?" << std::endl;
        std::cin >> ans;
        if (ans == "yes" || ans == "y")
        {
            delete_hash(path);
            std::cout << "clear" << std::endl;
        }
        else
        {
            return;
        }

    }

    std::string arg1;
    std::string arg2;
    int num_thread = 1;

    if (param == "--recursive" || param2 == "--recursive")
    {
        arg1 = "rec";
    }
    if (param == "--separate-file" || param2 == "--separate-file")
    {
        arg2 = "sep";
    }
    if (param == "--silent" || param2 == "--silent")
    {
        if (arg1 != "")
        {
            arg1 = "silent";
        }
        else
        {
            arg2 = "silent";
        }
    }

    if (param.starts_with("-j") || param2.starts_with("-j"))
    {
        std::string tmp_string;

        if (param.starts_with("-j"))
        {
            std::copy(param.begin() + 2, param.end(), tmp_string.begin());
        }
        else
        {
            std::copy(param2.begin() + 2, param2.end(), tmp_string.begin());
        }

        try {
            num_thread = std::stoi(tmp_string);
            std::cout << "THREADS: " << num_thread  << std::endl;
        }
        catch(std::invalid_argument& e) {
                std::cout << "Caught Invalid Argument Exception\n";
            }
    }

    if (command == "check_hash" && path != "")
    {
        check_hash(arg1, arg2, path, num_thread);
    }
    else if (command == "check_files" && path != "")
    {
        check_file(arg1, arg2, path);
    }


    check_argv(param);
    check_argv(param2);


    if (command == "write_hash" && path != "")
    {
        std::vector<Ihash*> tmp_vec;
        start_hash(path, tmp_vec, arg1, arg2, num_thread);
    }



}


void fill_vec(std::vector<Ihash*>& vector)
{
    vector.push_back(new MD5);
    vector.push_back(new SHA256);
    vector.push_back(new SHA1);
    vector.push_back(new CRC32);
}


//заполнение вектора out путями к файлам hash.<алгоритм>, если передан currHash то он заполняется классами алгоритмов
void prepare_check(std::vector<std::string>& out, std::vector<std::string>& paths, std::set<Ihash*>* currHash = nullptr)
{
    for (auto& elem : paths)
    {
        for (auto& item : vec)
        {
            if (elem.ends_with(item->getName()))
            {
                out.push_back(elem);
                if (currHash != nullptr)
                {
                    currHash->insert(item);
                }
            }
        }
    }
}


void clear_thread()
{
    for (auto& thread:threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
    threads.clear();
}


void check_hash(std::string par, std::string par2, std::string curPath, int quantity_threads = 1)
{
    int quantity;
    std::string base_path = curPath;
    std::string rel_path;

    if (quantity_threads == 0)
    {
        quantity = std::thread::hardware_concurrency();
    }
    else
    {
        quantity = quantity_threads;
    }

    fill_vec(vec);
    get_path(file_path, curPath, par, par2);

    bool result;
    std::vector<std::string> check_files;
    std::vector<std::string> errors;
    std::set<Ihash *> currHash;
    int num_threads = 0;

    prepare_check(check_files, file_path, &currHash);
    for (auto& elem : file_path)
    {
        if (elem.ends_with("hash.md5") || elem.ends_with("hash.sha256") || elem.ends_with("hash.crc32")
            || elem.ends_with("hash.sha1") || std::filesystem::is_directory(elem)) continue;

        rel_path = std::filesystem::relative(elem, base_path).string();

#ifndef nox11
if (!window_vec.isEmpty())
{
    window_vec[0]->set_log(rel_path);
    QCoreApplication::processEvents();
}
#endif
        if (par != "silent" && par2 != "silent")
        {
            std::cout << rel_path << std::endl;
        }

        if (num_threads <= quantity && quantity != 1)
        {
            std::thread th([&](){
                for (auto& item : currHash)
                {
                    item->check_hashing(elem);
                }
            });

            threads.push_back(std::move(th));
            ++num_threads;

            if (num_threads >= quantity)
            {
                clear_thread();
                num_threads = 0;
            }

        }
        else
        {
            for (auto& item : currHash)
            {
                item->check_hashing(elem);
            }
        }

    }

    if (!threads.empty())
    {
        clear_thread();
    }


    for (auto& elem : check_files)
    {
        for (auto& item : currHash)
        {
            if (elem.ends_with(item->getName()))
            {
                result = item->isEqual(elem);
            }
        }

        if (!result)
        {
            errors.push_back(elem);
        }
    }


    if (errors.empty())
    {
        std::cout << "no errors" << std::endl;
#ifndef nox11
if (!window_vec.isEmpty())
{
    window_vec[0]->set_log("no errors");
    QCoreApplication::processEvents();
}
#endif
    }
    else
    {
        for (auto& elem : errors)
        {
            std::cout << "errors in " + elem << std::endl;
#ifndef nox11
if (!window_vec.isEmpty())
{
    window_vec[0]->set_log("errors in " + elem);
    QCoreApplication::processEvents();
}
#endif
        }
    }

    clear_all();
}


//открывает файлы hash.<алгоритм> и заполняет вектор files_vector именно названиями файлов
void check_available(std::string path_to_file, std::vector<std::string> &files_vector)
{
    std::ifstream ss;
    bool flag = false;

    ss.open(path_to_file);

   char ch;
   int count_space = 0;
   std::string res;

   while (ss.get(ch))
   {
       if (ch == ' ')
       {
           flag = true;
           ++count_space;
           if (count_space > 2)
           {
              res += ch;
           }
       }
       else if (ch == '\n')
       {
           files_vector.push_back(res);
           res.clear();
           flag = false;
           count_space = 0;
       }

       else if (flag)
       {
           res += ch;
       }
   }

   ss.close();
}


void check_file(std::string par, std::string par2, std::string curPath)
{
    std::string base_path = curPath;
    std::string rel_path;

    fill_vec(vec);
    file_path.push_back(curPath);
    get_path(file_path, curPath, par, par2);


    std::vector<std::string> current_dir;
    std::vector<std::string> result_vector;
    std::vector<std::string> hash_files;

    std::vector<std::string> errors;

    bool hash_flag;


    for (auto &elem:file_path)
    {
        if (!std::filesystem::is_directory(elem))
        {
            continue;
        }
        hash_flag = true;
        get_path(current_dir, elem);
        prepare_check(hash_files, current_dir);
        for (auto& item: current_dir)
        {
            if (item.ends_with("hash.md5") || item.ends_with("hash.sha256") || item.ends_with("hash.crc32")
                || item.ends_with("hash.sha1") || std::filesystem::is_directory(item)) continue;

            rel_path = std::filesystem::relative(item, base_path).string();

#ifndef nox11
if (!window_vec.isEmpty())
{
    window_vec[0]->set_log(rel_path);
    QCoreApplication::processEvents();
}
#endif
            if (par != "silent" && par2 != "silent")
            {
                std::cout << rel_path << std::endl;
            }

            for (auto& hashfile: hash_files)
            {

            if (hash_flag)
            {
                check_available(hashfile, result_vector);
            }


            auto it = std::find_if(result_vector.begin(), result_vector.end(), [&item](std::string st){
             return item.ends_with(st);
        });

            if (it == result_vector.end())
            {

                errors.push_back(item);
                break;

            }

        }
        hash_flag = false;

       }
        result_vector.clear();
        current_dir.clear();
        hash_files.clear();
    }

    if (errors.empty())
    {
        std::cout << "no errors" << std::endl;
#ifndef nox11
if (!window_vec.isEmpty())
{
    window_vec[0]->set_log("no errors");
}
#endif
    }
    else
    {
        for (auto& elem: errors)
        {
#ifndef nox11
if (!window_vec.isEmpty())
{
    window_vec[0]->set_log("error in " + elem);
}
#endif
            std::cout << "error in " + elem << std::endl;
        }
    }
    clear_all();
}
