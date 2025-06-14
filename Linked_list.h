#ifndef __NODE_H__
#define __NODE_H__

#include <iostream>
#include <string>
#include <regex>
#include <cstddef>
#include <sstream>
#include <fstream>
#include <variant>
#include <vector>
#include <limits>
#include <iomanip>
#include <algorithm>
#include <map>
#include <utility>

struct Locus{
	std::string date;
	std::string time;
	std::string temp;
	std::string humidity;
	std::string direction;
	std::string speed_wind;
	std::string type_precipitation;
	std::string precipitation;
	std::string radiation;
	friend std::ostream& operator<<(std::ostream& ostr, const Locus& loc);

};

struct Region{
	std::string number;
	std::string name;
	std::string long_lat;
	Locus loc;
	friend std::ostream& operator<<(std::ostream& ostr, const Region& reg);
};

struct Node{
	Region reg;
	int index;
	Node* next;
	
	Node();
};

class Linked_list{
	public:
		Linked_list();
		~Linked_list();
		void read_from_file(const std::string& filename);
		std::vector<std::string> get_all_regions();
		void traversal();
		void traversal_opt();
		void group_by_month();
		void add(const Region& reg, const Locus& loc);
		void average_temp();
		int get_month(const std::string& b);
		int maximum(const int& a, const int& b);
		void max_precipitation();
		void write_to_file(const std::string& filename);
		double conversion(const std::string& b);
		void find_direct_region(const std::string& b);
		void add_extra();
		void read_for_the_first_time(const std::string& filename1);
		std::string all_nodes_string();
		void string_to_nodes(const std::string& data);
		void delete_node_by_number(const std::string& b);
		void print_table_down();
		void print_table_row(const Region& reg);
		void print_table_up();
		void change_inf();
		void to_change();
		std::pair<std::string, std::string> get_reg();
		void delete_node_by_num_date_time(const std::string& a, const std::string& b, const std::string& c);
		friend std::ostream& operator<<(std::ostream& ostr, const Linked_list& t);
	private:
		Node* head;
};

#endif
