#include "Linked_list.h"
#include "aes.h"

Node::Node(): index(0), next(nullptr){}

Linked_list::Linked_list(): head(nullptr){}

Linked_list::~Linked_list(){
	while (head){
		Node* to_delete = head;
		head = head->next;
		delete to_delete;
	}
	head = nullptr;
}

void Linked_list::add(const Region& reg, const Locus& loc){
	Node* new_node = new Node();
	new_node->next = nullptr;
	new_node->reg = reg;
	new_node->reg.loc = loc;
	if (!head){
		new_node->index = 1;
		head = new_node;
	}else{
		Node* current = head;
		while (current->next){
			current = current->next;
		}
		new_node->index = current->index + 1;
		current->next = new_node;
	}
}

void Linked_list::read_for_the_first_time(const std::string& filename1){
	std::ifstream file(filename1);
        if (!file.is_open()){
                throw std::runtime_error("Coulnt open file for reading");       
        }
        std::string line;
        while (std::getline(file, line)){
                if (line.empty()){
                        continue;
                }
                Region reg;
                Locus loc;
                reg.name = line;
                std::getline(file, line);
                if (line.empty())
                        break;
                std::istringstream s(line);
                s >> reg.number >> reg.long_lat >> loc.date >> loc.time >> loc.temp >> loc.humidity 
                        >> loc.direction >> loc.speed_wind >> loc.type_precipitation >> loc.precipitation 
                        >> loc.radiation;
                reg.loc = loc;
		add(reg, loc);
        }
        file.close();
}

void Linked_list::read_from_file(const std::string& filename) {
	try {
        	Myaes aes;
        	std::ifstream file(filename, std::ios::binary);
        	if (!file) {
            		throw std::runtime_error("File does not exist or cannot be opened: " + filename);
        	}
        	while (head) {
            		Node* delete_elem = head;
            		head = head->next;
            		delete delete_elem;
        	}
        	head = nullptr;
        	while (file) {
            		size_t size = 0;
            		file.read(reinterpret_cast<char*>(&size), sizeof(size));
            		if (file.gcount() != sizeof(size)) 
				break;
            		std::vector<char> enc_data(size);
            		file.read(enc_data.data(), size);
            		if (file.gcount() != static_cast<std::streamsize>(size)) 
				break;
            		std::string data = aes.decrypt(std::string(enc_data.begin(), enc_data.end()));
            		std::istringstream dataStream(data);
            		Region reg;
            		Locus loc;            
            		std::getline(dataStream, reg.number);
            		std::getline(dataStream, reg.name);
            		std::getline(dataStream, reg.long_lat);
            		std::getline(dataStream, loc.date);
            		std::getline(dataStream, loc.time);
            		std::getline(dataStream, loc.temp);
            		std::getline(dataStream, loc.humidity);
            		std::getline(dataStream, loc.direction);
            		std::getline(dataStream, loc.speed_wind);
            		std::getline(dataStream, loc.type_precipitation);
            		std::getline(dataStream, loc.precipitation);
            		std::getline(dataStream, loc.radiation);
            
            		reg.loc = loc;
            		add(reg, loc);
        	}
        	file.close();
    	} catch (const std::exception& e) {
        	std::cerr << "Error reading file: " << e.what() << std::endl;
        	throw;
    	}
}

std::vector<std::string> Linked_list::get_all_regions(){
	Node* current = head;
	std::vector<std::string> reg_temp;
	while (current){
		if (std::find(reg_temp.begin(), reg_temp.end(), current->reg.name) == reg_temp.end())
			reg_temp.push_back(current->reg.name);
		current = current->next;
	}
	return reg_temp; 
}

void Linked_list::traversal(){
	std::vector<std::string> all_reg = get_all_regions();
        std::vector<std::string> flag;
	int* months = new int[12];
	int* measures = new int[12];
	std::cout << "+-----------+-----------------------------+-------------------+------------+----------+--------"
                << "--+-------+--------------------+--------------+--------+-----------+" << std::endl;
        std::cout << "| Point No. | Region Name                 | Coordinates       | Date       | Time     | Temp(°C) | Humid.|"
                << " Wind Dir.| Wind Sp.| Precip Type  | Precip | Radiation |" << std::endl;
	std::map<int,std::string> change_months = { 
		{1, "January"}, {2, "February"}, {3, "March"}, {4, "April"}, 
		{5, "May"}, {6, "June"}, {7, "July"}, {8, "August"},
		{9, "September"}, {10, "October"}, {11, "Novermber"}, {12, "December"} };
	for (const auto& reg: all_reg){
		Node* current = head;
		double number_measures = 0;
		double average = 0;
        	for (int i=0;i<12;++i){
                	months[i] = 0;
			measures[i] = 0;
        	}
		while (current){
			if ((std::find(flag.begin(), flag.end(), current->reg.name)== flag.end()) && current->reg.name == reg){
				std::cout << "+-----------+-----------------------------+-------------------+------------+-"
					<< "---------+----------+-------+--------------------+--------------+--------+-----------+" << std::endl;
				std::string str = "| Region's name: " + current->reg.name;
				int line = 163 - str.length();
				std::cout << str << std::setw(line) << std::right << "|"<< std::endl;
				flag.push_back(current->reg.name);
				std::cout << "+-----------+-----------------------------+-------------------+------------+-"
                                        << "---------+----------+-------+--------------------+--------------+--------+-----------+" << std::endl;
                	}
                	if (current->reg.name == reg){
				print_table_row(current->reg);
				int month = get_month(current->reg.loc.date);
				months[month] += conversion(current->reg.loc.temp);
				measures[month] += 1;
                	}
                	current = current->next;
 	      	}
		for (int i=0;i<12; ++i){
			if (months[i]!=0){
				std::string str = "| Average temp in " + change_months[i] + ": " + std::to_string(months[i]/measures[i]);
				int line = 163 - str.length();
				std::cout << str << std::setw(line) << std::right << "|" << std::endl;
			}
		}
	}
	std::cout << "+-----------+-----------------------------+-------------------+------------+-"
                  << "---------+----------+-------+--------------------+--------------+--------+-----------+" << std::endl;
	delete[] months;
	delete[] measures;
}


int Linked_list::get_month(const std::string& b){
	if (b.length() < 10){
		throw std::invalid_argument("Date string too short");
	}
	std::string month_str = b.substr(5, 2);
    	if (!std::isdigit(month_str[0]) || !std::isdigit(month_str[1]))
        	throw std::invalid_argument("Invalid month format");
    	int month = std::stoi(month_str);
    	if (month < 1 || month > 12) {
        	throw std::invalid_argument("Month out of range");
    	}
	return month;
}

void Linked_list::traversal_opt(){
	Node* current = head;
	print_table_up();
	while (current){
		print_table_row(current->reg);
		current = current->next;
	}
	print_table_down();
}

double Linked_list::conversion(const std::string& b){
	try{
		return std::stod(b);	
	} catch (std::exception& e){
		return 0.0;
	}
}

int Linked_list::maximum(const int& a, const int& b){
	if (a > b)
		return a;
	return b;
}

void Linked_list::max_precipitation(){
	std::vector<std::string> all_reg = get_all_regions();
	std::cout << "+" << std::string(161,'-') << "+" << std::endl;
	std::vector<int> precipit_by_reg(all_reg.size(), 0);
	for (int i=0; i<all_reg.size(); ++i){
		Node* current = head;
		while (current){
			if (all_reg[i] == current->reg.name)
				precipit_by_reg[i] += conversion(current->reg.loc.precipitation);
			current = current->next;
		}
	}
	int max_val = 0;
	for (const auto& reg: precipit_by_reg)
		max_val = maximum(max_val, reg);
	auto it = std::find(precipit_by_reg.begin(), precipit_by_reg.end(), max_val);
	if (it != precipit_by_reg.end()){
		int index_max = std::distance(precipit_by_reg.begin(), it);

		std::cout << "| The name of the region with the max amount of precipitation: "<< all_reg[index_max] 
			<< std::setw(87) << std::right << " |"<< std::endl;
		std::cout << "+" << std::string(161,'-') << "+" << std::endl;
		std::cout << "| Amount of precipitation: " << max_val 
			 << std::setw(134) << std::right << " |"<<std::endl;
	}else{
		throw std::invalid_argument("No region found with precipitation data");
	}
	std::cout << "+" << std::string(161,'-') << "+" << std::endl;
}

void Linked_list::find_direct_region(const std::string& dir_reg){
	std::vector<std::string> all_points;
	Node* current = head;
	print_table_up();
	while (current){
		if (current->reg.name==dir_reg && std::find(all_points.begin(), all_points.end(), current->reg.number)==all_points.end())
			all_points.push_back(current->reg.number);
		current = current->next;
	}
	int count = 0;
	for (auto& point: all_points){
		Node* current = head;
		while (current){
			if (current->reg.number == point){
				print_table_row(current->reg);
				++count;
			}
			current = current->next;
				
		}
	}
	std::cout << count << " rows in set" << std::endl;
        std::cout << std::endl;
}

std::string Linked_list::all_nodes_string(){
	std::ostringstream string;
	Node* current = head;
	while (current){
		string << current->reg.name << "\n";
        	string << current->reg.number << " " << current->reg.long_lat << " " 
            		<< current->reg.loc.date << " " << current->reg.loc.time << " "
            		<< current->reg.loc.temp << " " << current->reg.loc.humidity << " "
            		<< current->reg.loc.direction << " " << current->reg.loc.speed_wind << " "
            		<< current->reg.loc.type_precipitation << " " << current->reg.loc.precipitation << " "
            		<< current->reg.loc.radiation << "\n\n";
        	current = current->next;
	}
	return string.str();


}

void Linked_list::string_to_nodes(const std::string& data){
	while (head){
		Node* delete_elem = head;
		head = head->next;
		delete delete_elem;
	}
	head = nullptr;
	std::istringstream string(data);
	std::string line;
	while (std::getline(string, line)){
		if (line.empty())
			continue;
		Region reg;
		Locus loc;
		reg.name = line;
		if (!std::getline(string, line)) 
			break;
        	std::istringstream s(line);
        	s >> reg.number >> reg.long_lat >> loc.date >> loc.time >> loc.temp >> loc.humidity
          		>> loc.direction >> loc.speed_wind >> loc.type_precipitation >> loc.precipitation
          		>> loc.radiation;

        reg.loc = loc;
        add(reg, loc);
	}
}


void Linked_list::write_to_file(const std::string& filename){
	try{
        	Myaes aes;
        	std::ofstream file(filename, std::ios::binary);
        	if (!file.is_open())
            		throw std::runtime_error("Couldn't open file for writing"); 
        	Node* current = head;
        	while (current) {
            		std::ostringstream nodeData;
            		nodeData << current->reg.number << "\n"
                     		<< current->reg.name << "\n"
                     		<< current->reg.long_lat << "\n"
                     		<< current->reg.loc.date << "\n"
                     		<< current->reg.loc.time << "\n"
                     		<< current->reg.loc.temp << "\n"
                     		<< current->reg.loc.humidity << "\n"
                     		<< current->reg.loc.direction << "\n"
                     		<< current->reg.loc.speed_wind << "\n"
                     		<< current->reg.loc.type_precipitation << "\n"
                     		<< current->reg.loc.precipitation << "\n"
                     		<< current->reg.loc.radiation << "\n";
            		std::string data = nodeData.str();
            		std::string enc_data = aes.encrypt(data);
            		size_t size = enc_data.size();
            		file.write(reinterpret_cast<const char*>(&size), sizeof(size));
            		file.write(enc_data.data(), size);
            		current = current->next;
        	}
        	file.close();
	}catch (const std::exception& e){
        	std::cerr << "Failed to write to file: " << e.what() << std::endl;
        	throw;
    	}
}


bool check_reg(const std::string& region){
	bool flag = false;
	std::vector<std::string> v={"Moscow Oblast", "Saint Petersburg City", "Republic of Adygea", "Altai Republic", "Altai Krai", 
		"Amur Oblast", "Arkhangelsk Oblast", "Astrakhan Oblast", "Republic of Bashkortostan", "Belgorod Oblast", "Bryansk Oblast", 
		"Republic of Buryatia", "Chechen Republic", "Chelyabinsk Oblast", "Chukotka Autonomous Okrug", "Chuvash Republic", 
		"Republic of Crimea", "Republic of Dagestan", "Republic of Ingushetia", "Irkutsk Oblast", "Ivanovo Oblast", 
		"Jewish Autonomous Oblast", "Kabardino-Balkarian Republic", "Kaliningrad Oblast", "Republic of Kalmykia", "Kaluga Oblast", 
		"Kamchatka Krai", "Karachay-Cherkess Republic", "Republic of Karelia", "Kemerovo Oblast", "Khabarovsk Krai", 
		"Republic of Khakassia", "Khanty-Mansi Autonomous Okrug", "Kirov Oblast", "Komi Republic", "Kostroma Oblast", 
		"Krasnodar Krai", "Krasnoyarsk Krai", "Kurgan Oblast", "Kursk Oblast", "Leningrad Oblast", "Lipetsk Oblast", 
		"Magadan Oblast", "Mari El Republic", "Republic of Mordovia", "Moscow City", "Murmansk Oblast", 
		"Nenets Autonomous Okrug", "Nizhny Novgorod Oblast", "Novgorod Oblast", "Novosibirsk Oblast", "Omsk Oblast", 
		"Orenburg Oblast", "Oryol Oblast", "Penza Oblast", "Perm Krai", "Primorsky Krai", "Pskov Oblast", "Rostov Oblast", 
		"Ryazan Oblast", "Sakha Republic (Yakutia)", "Sakhalin Oblast", "Samara Oblast", "Saratov Oblast", 
		"Republic of North Ossetia-Alania", "Smolensk Oblast", "Stavropol Krai", "Sverdlovsk Oblast", "Tambov Oblast", 
		"Republic of Tatarstan", "Tomsk Oblast", "Tula Oblast", "Tver Oblast", "Tyumen Oblast", "Udmurt Republic", 
		"Ulyanovsk Oblast", "Vladimir Oblast", "Volgograd Oblast", "Vologda Oblast", "Voronezh Oblast", 
		"Yamalo-Nenets Autonomous Okrug", "Yaroslavl Oblast", "Zabaykalsky Krai"};
	for (int i=0;i<v.size();++i){
		if (region == v[i])
			flag = true;
	}
	return flag;
}

bool check_number(const std::string& a){
	std::regex word("\\w{2}-\\d\\d\\d");
	std::smatch match;	
	return std::regex_match(a, match, word);
}

bool check_lat(const std::string& a){
	std::regex word("\\d{3}.\\d{4}-\\d{3}.\\d{4}");
	std::smatch match;
	return std::regex_match(a, match, word);
}

bool check_date(const std::string& a){
	std::regex word("\\d{4}-\\d{2}-\\d{2}");
	std::smatch match;
	return std::regex_match(a, match, word);
}

bool check_time(const std::string& a){
	std::regex word("\\d{2}:\\d{2}:\\d{2}");
	std::smatch match;
	return std::regex_match(a, match, word);
}

bool check_temp(const std::string& a){
	std::regex word(".\\d+");
	std::smatch match;
	return std::regex_match(a, match, word);
}

bool check_hum(const std::string& a){
	std::regex word("\\d+");
	std::smatch match;
	return std::regex_match(a, match, word);
}

bool check_dir(const std::string& a){
	std::regex word("\\w+");
	std::smatch match;
	return std::regex_match(a, match, word);
}

bool check_speed(const std::string& a){
	std::regex word("\\d+");
	std::smatch match;
	return std::regex_match(a, match, word);
}

bool check_type_prec(const std::string& a){
	std::regex word("\\w+");
	std::smatch match;
	return std::regex_match(a, match, word);
}

bool check_prec(const std::string& a){
	std::regex word("\\d+");
	std::smatch match;
	return std::regex_match(a, match, word);
}

bool check_rad(const std::string& a){
	std::regex word("\\d+");
	std::smatch match;
	return std::regex_match(a, match, word);
}

void Linked_list::add_extra(){
	std::string ans;
	Region reg;
	Locus loc;
	std::string number;
	do{
		std::cin.clear();
		bool flag_name = false;
		while (!flag_name){
			std::cout << "Enter the region's name: ";
                	std::cin.ignore();
                	std::getline(std::cin, reg.name);
                	flag_name = check_reg(reg.name);
			if (!flag_name)
				std::cout << "Invalid input in the name's field. Try again." << std::endl;
		}
		bool flag_number = false;
		while (!flag_number){
			std::cout << "Enter the meteorogical point's number (XX-ddd): ";
			std::cin >> reg.number;
			flag_number = check_number(reg.number);
			if (!flag_number)
				std::cout << "Invalid input in the number's field. Try again." << std::endl;
		}
		bool flag_coord = false;
		while (!flag_coord){
			std::cout << "Enter the region's coordinates (longtitue-latitude, xxx.xxxx-yyy.yyyy): ";
			std::cin >> reg.long_lat;
			flag_coord = check_lat(reg.long_lat);
			if (!flag_coord)
				std::cout << "Invalid input in the coordinates' field. Try again." << std::endl;
		}
		bool flag_meas = false;
		while (!flag_meas){
			std::cout << "Enter the date of the measurement (YYYY-MM-DD): ";
			std::cin >> reg.loc.date;
			flag_meas = check_date(reg.loc.date);
			if (!flag_meas)
				std::cout << "Invalid input in the date's field. Try again." << std::endl;
		}
		bool flag_time = false;
		while (!flag_time){
			std::cout << "Enter the time of the measurement (Hour:Minutes:Seconds): ";
        		std::cin >> reg.loc.time;
			flag_time = check_time(reg.loc.time);
			if (!flag_time)
				std::cout << "Invalid input in the time's field. Try again." << std::endl;
		}
		bool flag_temp = false;
		while (!flag_temp){
			std::cout << "Enter a temperature (Digit): ";
			std::cin >> reg.loc.temp;
			flag_temp = check_temp(reg.loc.temp);
			if (!flag_temp)
				std::cout << "Invalid input in the temperature's field. Try again." << std::endl;
		}
		bool flag_hum = false;
		while (!flag_hum){
			std::cout << "Enter a humidity (Digit): ";
        		std::cin >> reg.loc.humidity;
			flag_hum = check_hum(reg.loc.humidity);
			if (!flag_hum)
				std::cout << "Invalid input in the humidity's field. Try again." << std::endl;
		}
		bool flag_dir = false;
		while (!flag_dir){
			std::cout << "Enter the wind's direction (North-West): ";
        		std::cin >> reg.loc.direction;
			flag_dir = check_dir(reg.loc.direction);
			if (!flag_dir)
				std::cout << "Invalid input in the wind's direction field. Try again." << std::endl;
		}
		bool flag_speed = false;
		while (!flag_speed){
			std::cout << "Enter the wind's speed (Digit): ";
        		std::cin >> reg.loc.speed_wind;
			flag_speed = check_speed(reg.loc.speed_wind);
			if (!flag_speed)
				std::cout << "Invalid input in the wind's speed field. Try again." << std::endl;
		}
		bool flag_prec = false;
		while (!flag_prec){
			std::cout << "Enter a type of precipitation (Foggy): ";
        		std::cin >> reg.loc.type_precipitation;
			flag_prec = check_type_prec(reg.loc.type_precipitation);
			if (!flag_prec)
				std::cout << "Invalid input in the type of precipitation's field. Try again." << std::endl;
		}
		bool flag_am_prec = false;
		while (!flag_am_prec){
			std::cout << "Enter an amount of precipitation (Digit): ";
			std::cin >> reg.loc.precipitation;
			flag_am_prec = check_prec(reg.loc.precipitation);
			if (!flag_am_prec)
				std::cout << "Invalid input in the amount of precipitation's field. Try again." << std::endl;
		}
		bool flag_rad = false;
		while (!flag_rad){
			std::cout << "Enter a radiation (Digit): ";
                	std::cin >> reg.loc.radiation;
			flag_rad = check_rad(reg.loc.radiation);
			if (!flag_rad)
				std::cout << "Invalid input in the radiation's field. Try again." << std::endl;
		}
		std::cout << "If you want to continue enter yes, otherwise no: ";
                std::cin >> ans;
		add(reg,reg.loc);
	}while(ans!="no");
}

void Linked_list::delete_node_by_number(const std::string& b){
	if (!head)
		return;
	Node* current = head;
	if (head->reg.number == b){
		Node* temp = head;
		head = head->next;
		delete temp;
		return;
	}
	while (current->next){
		if (current->next->reg.number == b){
			Node* temp = current->next;
			current->next = current->next->next;
			delete temp;
			return;
		} else
			current = current->next;
	}	
}

void Linked_list::delete_node_by_num_date_time(const std::string& a, const std::string& b, const std::string& c){
        Node* current = head;
        if (head->reg.number == a && head->reg.loc.date == b && head->reg.loc.time == c){
		Node* temp = head;
                head = head->next;
		delete temp;
        }
        while (current->next){
                if (current->next->reg.number == a && current->next->reg.loc.date == b && current->next->reg.loc.time == c){
                        Node* temp = current->next;
                        current->next = current->next->next;
                        delete temp;
                } else
                        current = current->next;
        }
}

void Linked_list::print_table_row(const Region& reg) {
    	std::cout << "| " << std::setw(9) << std::left << reg.number << " | "
              	<< std::setw(27) << std::left << reg.name << " | "
              	<< std::setw(17) << std::left << reg.long_lat << " | "
              	<< std::setw(10) << std::left << reg.loc.date << " | "
              	<< std::setw(4)  << std::left << reg.loc.time << " | "
              	<< std::setw(8) << std::left << reg.loc.temp << " | "
              	<< std::setw(5) << std::left << reg.loc.humidity << " | "
              	<< std::setw(8) << std::left << reg.loc.direction << " | "
              	<< std::setw(7) << std::left << reg.loc.speed_wind << " | "
              	<< std::setw(12) << std::left << reg.loc.type_precipitation << " | "
		<< std::setw(6) << std::left << reg.loc.precipitation << " | "
              	<< std::setw(9) << std::left << reg.loc.radiation << " |" << std::endl;
	std::cout << "+-----------+-----------------------------+-------------------+------------+----------+--------"
                << "--+-------+--------------------+--------------+--------+-----------+" << std::endl;
}

void Linked_list::print_table_up(){
	std::cout << "+-----------+-----------------------------+-------------------+------------+----------+--------"
		<< "--+-------+--------------------+--------------+--------+-----------+" << std::endl;
	std::cout << "| Point No. | Region Name                 | Coordinates       | Date       | Time     | Temp(°C) | Humid.|"
		<< " Wind Dir.| Wind Sp.| Precip Type  | Precip | Radiation |" << std::endl;
	std::cout << "+-----------+-----------------------------+-------------------+------------+----------+--------"
                << "--+-------+--------------------+--------------+--------+-----------+" << std::endl;
}


void Linked_list::print_table_down(){
	Node* current2 = head;
	int count = 0;
	while (current2) {
		++count;
        	current2 = current2->next;
    	}
    	std::cout << count << " rows in set" << std::endl;	
	std::cout << std::endl;
}

void Linked_list::to_change(){
	std::cout << std::string(168,'=') << std::endl;
	std::cout << "| 0 - Stop changing program" << std::setw(27) << std::right << "|"<<std::endl;
	std::cout << "| 1 - Change the point's region" << std::setw(23) << std::right << "|" << std::endl;
	std::cout << "| 2 - Change the point's number" << std::setw(23) << std::right << "|" <<std::endl;
	std::cout << "| 3 - Change the point's coordinates" << std::setw(18) << std::right << "|" << std::endl;
	std::cout << "| 4 - Change the point's date" << std::setw(25) << std::right << "|" << std::endl;
	std::cout << "| 5 - Change the point's time" << std::setw(25) << std::right << "|" << std::endl;
	std::cout << "| 6 - Change the point's temperature" << std::setw(18) << std::right << "|" << std::endl;
	std::cout << "| 7 - Change the point's humidity" << std::setw(21) << std::right << "|" << std::endl;
	std::cout << "| 8 - Change the point's wind direction" << std::setw(15) << std::right << "|" << std::endl;
	std::cout << "| 9 - Change the point's wind speed" << std::setw(19) << std::right << "|" << std::endl;
	std::cout << "| 10 - Change the point's type of precipitation" << std::setw(7) << std::right << "|" << std::endl;
	std::cout << "| 11 - Change the point's amount of precipitation" << std::setw(5) << std::right << "|" << std::endl;
	std::cout << "| 12 - Change the point's level of radiation" << std::setw(10) << std::right << "|" << std::endl;
	std::cout << std::string(168,'=') << std::endl;
}

std::pair<std::string, std::string> Linked_list::get_reg(){
	std::pair<std::string, std::string> get_reg("0","0");
	bool flag_reg_first = false;
	bool flag_reg_second = false;
	while (!flag_reg_first){
		std::cout << "Enter the point's region to change: " << std::endl;
        	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        	std::getline(std::cin, get_reg.first);
		flag_reg_first = check_reg(get_reg.first);
		if (!flag_reg_first)
			std::cout << "Wrong input in the old region's field. Try again." << std::endl;
	}
	while (!flag_reg_second){
        	std::cout << "Enter the new point's region:" << std::endl;
        	std::getline(std::cin, get_reg.second);
		flag_reg_second = check_reg(get_reg.second);
		if (!flag_reg_second)
			std::cout << "Wrong input in the new region's filed. Try again." << std::endl;
	}
	return get_reg;
}

void Linked_list::change_inf(){
	std::pair<std::string, std::string> region("0","0");
	std::pair<std::string, std::string> number("0","0");
	std::pair<std::string, std::string> coordinates("0","0");
	std::pair<std::string, std::string> date("0","0");
	std::pair<std::string, std::string> time("0","0");
	std::pair<std::string, std::string> temperature("0","0");
	std::pair<std::string, std::string> humidity("0","0");
	std::pair<std::string, std::string> wind_direction("0","0");
	std::pair<std::string, std::string> wind_speed("0","0");
	std::string reg_str;
	std::string num_str;
	bool flag_reg = false;
	bool flag_num = false;
	bool flag_num_first = false;
        bool flag_num_second = false;
	bool coord_old = false;
        bool coord_new = false;
	bool flag_new_date = false;
        bool flag_old_date = false;
	bool flag_old_time = false;
        bool flag_new_time = false;
	bool flag_old_temp = false;
        bool flag_new_temp = false;
	bool flag_old_hum = false;
        bool flag_new_hum = false;
	bool flag_old_dir = false;
        bool flag_new_dir = false;
	bool flag_old_speed = false;
        bool flag_new_speed = false;
	bool flag_old_type = false;
        bool flag_new_type = false;
	bool flag_old_am = false;
        bool flag_new_am = false;
	bool flag_old_rad = false;
        bool flag_new_rad = false;
	std::pair<std::string, std::string> type_of_prec("0","0");
	std::pair<std::string, std::string> am_of_prec("0","0");
	std::pair<std::string, std::string> level_rad("0","0");
	int k;
	do{	
		std::cout << std::string(168,'=') << std::endl;
		std::cout << "YOU MUST INPUT THE REGION'S NAME TO CHANGE, EVEN IF YOU AREN'T GOING TO CHANGE IT!!!!" << std::endl;
		to_change();
		std::cout << std::string(168,'=') << std::endl;
		std::cin >> k;
		switch (k){
			case 0:
				break;
			case 1:
				region = get_reg();
				break;
			case 2:
				flag_reg = false;
				flag_num_first = false;
				flag_num_second = false;
				while (!flag_reg){
					std::cout << "Enter the point's region for changing the number:" << std::endl;
        				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        				std::getline(std::cin, reg_str);
					flag_reg = check_reg(reg_str);
					if (!flag_reg)
						std::cout << "Wrong input in the region's field. Try again." << std::endl;
				}
				while (!flag_num_first){
					std::cout << "Enter the point's number to change: ";
					std::cin >> number.first;
					num_str = number.first;
					flag_num_first = check_number(num_str);
					if (!flag_num_first)
						std::cout << "Wrong input in the old number's field. Try again." << std::endl;
				}
				while (!flag_num_second){
					std::cout << "Enter a new point's number: ";
					std::cin >> number.second;
					flag_num_second = check_number(number.second);
					if (!flag_num_second)
						std::cout << "Wrong input in the new number's field. Try again." << std::endl;
				}
                                break;
			case 3:
				flag_reg = false;
				flag_num = false;
				coord_old = false;
				coord_new = false;
				while (!flag_reg){
					std::cout << "Enter the point's region for changing the coordinates:" << std::endl;
                                	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                	std::getline(std::cin, reg_str);
					flag_reg = check_reg(reg_str);
					if (!flag_reg)
						std::cout << "Wrong input in the region's field. Try again." << std::endl;
				}
				while (!flag_num){
					std::cout << "Enter the point's number for changing the coordinates: " << std::endl;
					std::cin >> num_str;
					flag_num = check_number(num_str);
					if (!flag_num)
						std::cout << "Wrong input in the number's field. Try again." << std::endl;
				}
				while (!coord_old){
					std::cout << "Enter the point's coordinates to change: ";
					std::cin >> coordinates.first;
					coord_old = check_lat(coordinates.first);
					if (!coord_old)
						std::cout << "Wrong input in the old coordinates' field. Try again." << std::endl;
				}
				while (!coord_new){
                                	std::cout << "Enter the new point's coordinates: ";
                                	std::cin >> coordinates.second;
					coord_new = check_lat(coordinates.second);
					if (!coord_new)
						std::cout << "Wrong input in the new coordinates' field. Try again." << std::endl;
				}
                                break;
			case 4:
				flag_reg = false;
				flag_num = false;
				flag_new_date = false;
				flag_old_date = false;
				while (!flag_reg){
					std::cout << "Enter the point's region for changing the number:" << std::endl;
                                	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                	std::getline(std::cin, reg_str);
					flag_reg = check_reg(reg_str);
					if (!flag_reg)
						std::cout << "Wrong input in the region's field. Try again." << std::endl;
				}
				while (!flag_num){
					std::cout << "Enter the point's number for changing the date: " << std::endl;
					std::cin >> num_str;
					flag_num = check_number(num_str);
					if (!flag_num)
						std::cout << "Wrong input in the number's field. Try again." << std::endl;
				}
				while (!flag_old_date){
					std::cout << "Enter the point's date to change: ";
                                	std::cin >> date.first;
					flag_old_date = check_date(date.first);
					if (!flag_old_date)
						std::cout << "Wrong input in the old date's field. Try again." << std::endl;
				}
				while (!flag_new_date){
                                	std::cout << "Enter the new point's date: ";
                                	std::cin >> date.second;
					flag_new_date = check_date(date.second);
					if (!flag_new_date)
						std::cout << "Wrong input in the new date's field. Try again." << std::endl;
				}
                                break;
			case 5:
				flag_reg = false;
				flag_num = false;
				flag_old_time = false;
				flag_new_time = false;
				while (!flag_reg){
					std::cout << "Enter the point's region for changing the number:" << std::endl;
                                	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                	std::getline(std::cin, reg_str);
					flag_reg = check_reg(reg_str);
					if (!flag_reg)
						std::cout << "Wrong input in the region's field. Try again." << std::endl;
				}
				while (!flag_num){
					std::cout << "Enter the point's number for chaning the time: ";
					std::cin >> num_str;
					flag_num = check_number(num_str);
					if (!flag_num)
						std::cout << "Wrong input in the number's field. Try again." << std::endl;
				}
				while (!flag_old_time){
					std::cout << "Enter the point's time to change: ";
                                	std::cin >> time.first;
					flag_old_time = check_time(time.first);
					if (!flag_old_time)
						std::cout << "Wrong input in the old time's field. Try again." << std::endl;
				}
				while (!flag_new_time){
                                	std::cout << "Enter the new point's time: ";
                                	std::cin >> time.second;
					flag_new_time = check_time(time.second);
					if (!flag_new_time)
						std::cout << "Wrong input in the new time's field. Try again." << std::endl;
				}
                                break;
			case 6:
				flag_reg = false;
				flag_num = false;
				flag_old_temp = false;
				flag_new_temp = false;
				while (!flag_reg){
					std::cout << "Enter the point's region for changing the number:" << std::endl;
                                	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                	std::getline(std::cin, reg_str);
					flag_reg = check_reg(reg_str);
					if (!flag_reg)
						std::cout << "Wrong input in the region's field. Try again." << std::endl;
				}
				while (!flag_num){
					std::cout << "Enter the point's number for changing the temperature: ";
                                	std::cin >> num_str;
					flag_num = check_number(num_str);
					if (!flag_num)
						std::cout << "Wrong input in the number's field. Try again." << std::endl;
				}
				while (!flag_old_temp){
					std::cout << "Enter the point's temperature to change: ";
                                	std::cin >> temperature.first;
					flag_old_temp = check_temp(temperature.first);
					if (!flag_old_temp)
						std::cout << "Wrong input in the old temperature's field. Try again." << std::endl;
				}
				while (!flag_new_temp){
                                	std::cout << "Enter the new point's temperature: ";
                                	std::cin >> temperature.second;
					flag_new_temp = check_temp(temperature.second);
					if (!flag_new_temp)
						std::cout << "Wrong input in the new temperature's field. Try again." << std::endl;
				}
                                break;
			case 7:
				flag_reg = false;
				flag_num = false;
				flag_old_hum = false;
				flag_new_hum = false;
				while (!flag_reg){
					std::cout << "Enter the point's region for changing the humidity:" << std::endl;
                                	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                	std::getline(std::cin, reg_str);
					flag_reg = check_reg(reg_str);
					if (!flag_reg)
						std::cout << "Wrong input in the region's field. Try again." << std::endl;
				}
				while (!flag_num){
					std::cout << "Enter the point's number for changing the humidity: ";
					std::cin >> num_str;
					flag_num = check_number(num_str);
					if (!flag_num)
						std::cout << "Wrong input in the number's field. Try again." << std::endl;
				}
				while (!flag_old_hum){
					std::cout << "Enter the point's humidity to change: ";
                                	std::cin >> humidity.first;
					flag_old_hum = check_hum(humidity.first);
					if (!flag_old_hum)
						std::cout << "Wrong input in the old humidity's field. Try again." << std::endl;
				}
				while (!flag_new_hum){
                                	std::cout << "Enter the new point's humidity: ";
                                	std::cin >> humidity.second;
					flag_new_hum = check_hum(humidity.second);
					if (!flag_new_hum)
						std::cout << "Wrong input in the new humidity's field. Try again." << std::endl;
				}
                                break;
			case 8:
				flag_reg = false;
				flag_num = false;
				flag_old_dir = false;
				flag_new_dir = false;
				while (!flag_reg){
					std::cout << "Enter the point's region for changing the number:" << std::endl;
                                	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                	std::getline(std::cin, reg_str);
					flag_reg = check_reg(reg_str);
					if (!flag_reg)
						std::cout << "Wrong input in the region's field. Try again." << std::endl;
				}
				while (!flag_num){
					std::cout << "Enter the point's number for changing the direction of wind: ";
					std::cin >> num_str;
					flag_num = check_number(num_str);
					if (!flag_num)
						std::cout << "Wrong input in the number's field. Try again." << std::endl;
				}
				while (!flag_old_dir){
					std::cout << "Enter the point's direction of wind to change: ";
                                	std::cin >> wind_direction.first;
					flag_old_dir = check_dir(wind_direction.first);
					if (!flag_old_dir)
						std::cout << "Wrong input in the old wind's direction field. Try again." << std::endl;
				}
				while (!flag_new_dir){
                                	std::cout << "Enter the new point's wind direction: ";
                                	std::cin >> wind_direction.second;
					flag_new_dir = check_dir(wind_direction.second);
					if (!flag_new_dir)
						std::cout << "Wrong input in the new wind's direction field. Try again." << std::endl;
				}
                                break;
			case 9:
				flag_reg = false;
				flag_num = false;
				flag_old_speed = false;
				flag_new_speed = false;
				while (!flag_reg){
					std::cout << "Enter the point's region for changing the number:" << std::endl;
                                	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                	std::getline(std::cin, reg_str);
					flag_reg = check_reg(reg_str);
					if (!flag_reg)
						std::cout << "Wrong input in the region's field. Try again." << std::endl;
				}
				while (!flag_num){
					std::cout << "Enter the point's number for changing the speed of wind: ";
					std::cin >> num_str;
					flag_num = check_number(num_str);
					if (!flag_num)
						std::cout << "Wrong input in the number's field. Try again." << std::endl;
				}
				while (!flag_old_speed){
					std::cout << "Enter the point's speed of wind to change: ";
                                	std::cin >> wind_speed.first;
					flag_old_speed = check_speed(wind_speed.first);
					if (!flag_old_speed)
						std::cout << "Wrong input in the old wind's speed field. Try again." << std::endl;
				}
				while (!flag_new_speed){
                                	std::cout << "Enter the new point's name: ";
                                	std::cin >> wind_speed.second;
					flag_new_speed = check_speed(wind_speed.second);
					if (!flag_new_speed)
						std::cout << "Wrong input in the new wind's speed field. Try again." << std::endl;
				}
                                break;
			case 10:
				flag_reg = false;
				flag_num = false;
				flag_old_type = false;
				flag_new_type = false;
				while (!flag_reg){
					std::cout << "Enter the point's region for changing the number:" << std::endl;
                                	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                	std::getline(std::cin, reg_str);
					flag_reg = check_reg(reg_str);
					if (!flag_reg)
						std::cout << "Wrong input in the region's field. Try again." << std::endl;
				}
				while (!flag_num){
					std::cout << "Enter the point's region for changing the number: ";
					std::cin >> num_str;
					flag_num = check_number(num_str);
					if (!flag_num)
						std::cout << "Wrong input in the number's field. Try again." << std::endl;
				}
				while (!flag_old_type){
					std::cout << "Enter the point's type of precipitation to change: ";
                                	std::cin >> type_of_prec.first;
					flag_old_type = check_type_prec(type_of_prec.first);
					if (!flag_old_type)
						std::cout << "Wrong input in the old type of precipitation's field. Try again." << std::endl;
				}
				while (!flag_new_type){
                                	std::cout << "Enter the new point's type of precipitation: ";
                                	std::cin >> type_of_prec.second;
					flag_new_type = check_type_prec(type_of_prec.second);
					if (!flag_new_type)
						std::cout << "Wrong input in the new type of precipitation's field. Try again." << std::endl;
				}
                                break;
			case 11:
				flag_reg = false;
				flag_num = false;
				flag_old_am = false;
				flag_new_am = false;
				while (!flag_reg){
					std::cout << "Enter the point's region for changing the number:" << std::endl;
                                	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                	std::getline(std::cin, reg_str);
					flag_reg = check_reg(reg_str);
					if (!flag_reg)
						std::cout << "Wrong input in the region's field. Try again." << std::endl;
				}
				while (!flag_num){
					std::cout << "Enter the point's number for changing the amount of precipitation: ";
					std::cin >> num_str;
					flag_num = check_number(num_str);
					if (!flag_num)
						std::cout << "Wrong input in the number's field. Try again." << std::endl;
				}
				while (!flag_old_am){
					std::cout << "Enter the point's amount of precipitation to change: ";
                                	std::cin >> am_of_prec.first;
					flag_old_am = check_prec(am_of_prec.first);
					if (!flag_old_am)
						std::cout << "Wrong input in the amount of precipitation's field. Try again." << std::endl;
				}
				while (!flag_new_am){
                                	std::cout << "Enter the new point's amount of precipitation: ";
                                	std::cin >> am_of_prec.second;
					flag_new_am = check_prec(am_of_prec.second);
					if (!flag_new_am)
						std::cout << "Wrong input in the amount of precipitation's field. Try again." << std::endl;
				}
                                break;
			case 12:
				flag_reg = false;
				flag_num = false;
				flag_old_rad = false;
				flag_new_rad = false;
				while (!flag_reg){
					std::cout << "Enter the point's region for changing the number:" << std::endl;
                                	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                	std::getline(std::cin, reg_str);
					flag_reg = check_reg(reg_str);
					if (!flag_reg)
						std::cout << "Wrong input in the region's field. Try again." << std::endl;
				}
				while (!flag_num){
					std::cout << "Enter the point's number for changing the level of radiation: ";
					std::cin >> num_str;
					flag_num = check_number(num_str);
					if (!flag_num)
						std::cout << "Wrong input in the number's field. Try again." << std::endl;
				}
				while (!flag_old_rad){
					std::cout << "Enter the point's level of radiation to change: ";
					std::cin >> level_rad.first;
					flag_old_rad = check_rad(level_rad.first);
					if (!flag_old_rad)
						std::cout << "Wrong input in the old radiation's level field. Try again." << std::endl;
				}
				while (!flag_new_rad){
                                	std::cout << "Enter the new point's level of radiation: ";
                                	std::cin >> level_rad.second;
					flag_new_rad = check_rad(level_rad.second);
					if (!flag_new_rad)
						std::cout << "Wrong input in the new radiation's level field. Try again." << std::endl;
				}
                                break;
			default:
				std::cout << "Invalid selection - Please input 0 to 12 only." << std::endl;
                                break;
		}
	}while(k!=0);
	Node* current = head;
	if ((!region.first.empty() && !region.second.empty()) or !reg_str.empty()){
		while (current){
			if (current->reg.name == region.first or current->reg.name == reg_str){
				if (current->reg.number == number.first)
					current->reg.number = number.second;
				if (current->reg.long_lat == coordinates.first)
					current->reg.long_lat = coordinates.second;
				if (current->reg.loc.date == date.first)
					current->reg.loc.date = date.second;
				if (current->reg.loc.time == time.first)
					current->reg.loc.time = time.second;
				if (current->reg.loc.temp == temperature.first)
					current->reg.loc.temp = temperature.second;
				if (current->reg.loc.humidity == humidity.first)
					current->reg.loc.humidity = humidity.second;
				if (current->reg.loc.direction == wind_direction.first)
					current->reg.loc.direction = wind_direction.second;
				if (current->reg.loc.speed_wind == wind_speed.first)
					current->reg.loc.speed_wind = wind_speed.second;
				if (current->reg.loc.type_precipitation == type_of_prec.first)
					current->reg.loc.type_precipitation = type_of_prec.second;
				if (current->reg.loc.precipitation == am_of_prec.first)
					current->reg.loc.precipitation = am_of_prec.second;
				if (current->reg.loc.radiation == level_rad.first)
					current->reg.loc.radiation = level_rad.second;
			}
			current = current->next;
		}
	}else{
		std::cout << "Invalid argument - the region can't be empty!" << std::endl;
	}
}		

std::ostream& operator<<(std::ostream& ostr, const Linked_list& t){
	Node* current = t.head;
	while (current){
		ostr << current->reg << std::endl;
		ostr << current->reg.loc << std::endl;
		current = current->next;
	}
	return ostr;
}

std::ostream& operator<<(std::ostream& ostr, const Region& reg){
	ostr << "Region number: "<< reg.number << "\tRegion name: " << reg.name << "\tRegion coordinates: " << 
		reg.long_lat << std::endl;
	return ostr;
}

std::ostream& operator<<(std::ostream& ostr, const Locus& loc){
	ostr << "Date: " << loc.date << "\tTime: " << loc.time << "\tTemperature: " << loc.temp << std::endl;
        ostr << "Humidity: " << loc.humidity << "\t\tWind direction: " << loc.direction << std::endl;
        ostr << "Speed of wind: " << loc.speed_wind << "\tType of precipitation: " << loc.type_precipitation << std::endl;
        ostr << "Precipitation: " << loc.precipitation << "\tRadiation: " << loc.radiation << std::endl;
	return ostr;

}

