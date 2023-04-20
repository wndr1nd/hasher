#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <filesystem>
#include <mutex>

class Ihash
{
public:
	virtual void reset() = 0;
	virtual std::string getHash() = 0;
	virtual void add(const void* data, size_t numBytes) = 0;
	virtual std::string getName() = 0;

    virtual void start_hashing(std::string file_path, std::string result, std::string relative_path)
	{
        mut.lock();

		ss.open(file_path, std::ios_base::binary);
		out.open(result, std::ofstream::app);

        char ch;
		std::string res;

        while (ss.get(ch)) {

			res += ch;
		}

		this->add(res.c_str(), res.size());
        out << this->getHash() + "  " + relative_path + "\n";
        out.flush();
		this->reset();

		ss.close();
		out.close();
        mut.unlock();

	}

	virtual bool isEqual(std::string path_to_hashfile)
	{
		if (hash_vector.empty())
		{
			return false;
		}
		bool read_flag = true;
		bool result_flag = true;

		ss.open(path_to_hashfile);
        char ch;
		std::string res;

		while (ss.get(ch)) {

			if (ch == ' ')
			{
				read_flag = false;
			}

			else if (ch == '\n')
			{
				auto it = std::find(hash_vector.begin(), hash_vector.end(), res);
				if (it != hash_vector.end())
				{
					hash_vector.erase(it);
				}
				else
				{
					result_flag = false;
				}

				read_flag = true;
				res = "";
			}

			else if (read_flag)
			{
				res += ch;
			}
		}


		ss.close();
		return result_flag;
	}


	virtual void check_hashing(std::string path_to_file)
	{
        mut.lock();
		if (hash_vector.capacity() <= 1000)
		{
			hash_vector.resize(4096);
		}
		ss.open(path_to_file, std::ios_base::binary);
        char ch;
        std::string res;


        while (ss.get(ch)) {
            res += ch;
		}

		this->add(res.c_str(), res.size());
		hash_vector.push_back(this->getHash());
		this->reset();
		ss.close();
        mut.unlock();
	}

    std::mutex mut;
    std::ifstream ss;
    std::ofstream out;
	std::vector<std::string> hash_vector;
};
