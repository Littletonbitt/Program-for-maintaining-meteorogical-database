#include "Linked_list.h"
#include "aes.h"

int main(int argc, char** argv) {
	Linked_list l;
    	Myaes aes("abcdefghijklmnopqrstu0123456789q", "1234567890afdwst");
    	std::string filename = "test_data.bin";
	//std::string filename1 = "test_text.txt";
    	try{
		//l.read_for_the_first_time(filename1);
		//l.write_to_file(filename);
        	Linked_list l1;
        	l1.read_from_file(filename);
        	std::string out;
		std::string num6;
		std::string date7;
		std::string time7;
        	std::string name;
        	int m;
        	do {
			std::cout << std::string(168, '=') << std::endl;
            		std::cout << "1 - The list of monitoring points, grouped by region, with the average temperature for a certain month." << std::endl;
            		std::cout << "2 - The name of the region with the max amount of precipitation" << std::endl;
            		std::cout << "3 - The results of monitoring the meteorological situation in a certain region"
                        		 << "(for all points in the region)" << std::endl;
			std::cout << "_____________________________________________________________" << std::endl;
			std::cout << "|		Extra options:" << std::setw(31) << std::right << "|"<<std::endl;
			std::cout << "| 4 - Add another one point or measurement (add a node)" << std::setw(6) << std::right << "|" << std::endl;
			std::cout << "| 5 - Output full (clean) database without any calculations" << std::setw(2) << std::right << "|" << std::endl;
			std::cout << "| 6 - Removing an object by his point's number (all records)" << std::setw(1) << std::right << "|" << std::endl;
			std::cout << "| 7 - Removing an object by his (number && date && time)" << std::setw(5) << std::right << "|" << std::endl;
			std::cout << "| 8 - Changing information about the point" << std::setw(19) << std::right << "|" << std::endl;
			std::cout << "|___________________________________________________________|" << std::endl;
            		std::cout << "0 - Terminate program" << std::endl;
			std::cout << std::string(168, '=') << std::endl;
            		std::cout << "Choose a category: ";
            		std::cin >> m;
            		switch (m) {
                		case 1:
                    			l1.traversal();
					break;
                		case 2:
                    			l1.max_precipitation();
                    			break;
				case 3:
					std::cout << "Example of input: Moscow Oblast" << std::endl;
                    			std::cout << "Enter the region, which you want to find?" << std::endl;
                    			std::cin.ignore();
                    			std::getline(std::cin, name);
                    			l1.find_direct_region(name);
                    			break;
				case 4:
					l1.add_extra();
					break;
				case 5:
					l1.traversal_opt();
					break;
				case 6:
					std::cout << "Enter the point's number (NOT NAME): ";
					std::cin >> num6;
					l1.delete_node_by_number(num6);
					break;
				case 7:
					std::cout << "Enter the point's number (NOT NAME): ";
					std::cin >> num6;
					std::cout << "Enter the date of measurement: ";
					std::cin >> date7;
					std::cout << "Enter the time of measurement: ";
					std::cin >> time7;
					l1.delete_node_by_num_date_time(num6,date7,time7);
					break;
				case 8:
					l1.change_inf();
					break;
                		case 0:
                    			break;
                		default:
					std::cout << "Invalid selection - Please input 0 to 8 only." << std::endl;
                    			break;
            		}
            		if (m == 0) {
                		std::cout << "Terminating the program...." << std::endl;
                		break;
            		}
        	} while (m!=0);
		l1.write_to_file(filename);
    	} catch (const std::exception& e) {
        	std::cerr << "Error: " << e.what() << std::endl;
        	return 1;
    	}

    	return 0;
}
