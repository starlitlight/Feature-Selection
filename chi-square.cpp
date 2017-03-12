#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <set>
#include <cmath>
#include <functional>   // std::plus
#include "csv_parser.hpp"

using namespace std;

	
vector<vector<string> >rawdata; //The original data
vector<string> rowdata; //data in each row from the original data

vector<vector<string> > chi_table_data;
vector<string> chi_table;

vector<string> decision;
int version = 1;
int feature_delete = 0;

double op (double i, double j) { return i+j; }

int main(int argc, char* argv[]) {

	char *ifname, *ofname, *conf, *pvalue;
	//Check the value of argc. If not enough parameters have been passed, inform user and exit.
	if (argc != 7) { 
		cout << "Usage is -in <infile> -out <outdir> -p <pvalue>\n"; //inform the user of how to use the program
		cin.get();
		exit(0);
	} else { //if we got enough parameters...
		for (int i = 1; i < argc; i++) {
			if (argv[i] == string("-in")) {
				ifname = argv[i + 1];
				i++;
			} else if (argv[i] == string("-out")) {
				ofname = argv[i + 1];
				i++;
			} else if (argv[i] == string("-p")) {
				pvalue = argv[i + 1];
				i++;
			} else {
				cout << " Not enough or invalid arguments, please try again.\n";
				exit(0);
			}
		}
	}


	cout << "Loading set ................................" << ifname << endl;
	//cout << "The out file name is " << ofname << endl;
	cout << "Set P value is less than ..................." << pvalue << endl;

	csv_parser csv(ifname);

	//Get the header from the file
	string header_line = csv.get_line(1); 

	//Get the number of feature including decision
	int feature_num = csv.fields(header_line);

	vector<string> header_fields;
	for (auto i = 1; i <= feature_num; i++)
	{
		header_fields.push_back(csv.get_value(1,i));
	}

	int rows = csv.total_lines() - 1;//1 for header, 1 for /n
	int cols = header_fields.size();
	
	cout << "Applying settings...........................Done" << endl;
	cout << "Lading Dataset..............................Done" << endl;
	cout << "          Featues:                   " << (cols - 1) << endl; 
	cout << "          Objects:                   " << (rows - 1) << endl;
	cout << "Featues Selection...........................Done" << endl;
	//cout << "Delete Features................................." << endl;
	//cout << "Writing Dataset.............................Done" << endl;


	for (auto i = 1; i <= rows; i++) 
	{
		for (auto j = 1; j <= cols-1; j++) 
		{
			rowdata.push_back(csv.get_value(i,j));
		}
		decision.push_back(csv.get_value(i,cols));
		rawdata.push_back(rowdata);
		rowdata.clear();
	}

	

	//import the chi square table
	csv_parser csv2("chi_table.csv");
	string chi_header_line = csv2.get_line(1); //Get the header from the file
	int plevel = csv2.fields(chi_header_line);
	
	vector<string> chi_table_header;
	vector<vector<string> > chi_table_data;
	vector<string> chi_table;

	for (auto i = 1; i <= plevel; i++)
	{
		chi_table_header.push_back(csv2.get_value(1,i));
	}

	// cout << "chi_table number of row is " << csv2.total_lines()-1<< endl; //11
	// cout << "chi_table number of column is " << chi_table_header.size() << endl; //8

	for (auto i = 1; i <= csv2.total_lines() - 1; i++) 
	{
		for (auto j = 1; j <= chi_table_header.size(); j++) 
		{
			chi_table.push_back(csv2.get_value(i,j));
		}
		chi_table_data.push_back(chi_table);
		chi_table.clear();
	}


	////////////////////////////////////////////////////////////////////////
	ofstream logfile;
	logfile.open("log.txt", std::ofstream::out | std::ofstream::trunc);
	logfile.close();

	//////////////////////////////////////////////////////////////////////////////
	
	vector<string> first_column; //get the value from the first feature
	vector<string> second_column; //get the value from the second feature

	set<string> first_parameters;  //Params in the first feature 
	set<string> second_parameters; //Params in the second feature

	vector<double> ob_col; //Column of observed data
	vector<double> ob_row; //Row of oberved data

	vector<double> observe; //Observed data
	vector<double> expect;  //Expected data
	vector<double> chi_value;

	//Parameter initialization
	int j = 1;
	double count = 0;
	double total_row = 0;
	double total_col = 0;
	double value=0;
	double chi_square = 0;

	for (int k = 0; k < j; k++)
	{
		for (j = k+1; j < rawdata[0].size(); j++)
		{
			//cout << "j value is " << j << endl;
			for (int i = 1; i < rawdata.size(); i++)
			{
				first_column.push_back(rawdata[i][k]);
		 		second_column.push_back(rawdata[i][j]);
		 	}

		 	first_parameters.insert(first_column.begin(), first_column.end());
		 	second_parameters.insert(second_column.begin(), second_column.end());

			// for (set<string>::iterator first = first_parameters.begin(); first != first_parameters.end(); first++)
			// {
			// 	cout << "The first parameters " << *first << endl;
			// }

			// for (set<string>::iterator second = second_parameters.begin(); second != second_parameters.end(); second++)
			// {
			// 	cout << "The second parameters " << *second << endl;
			// }

	 		for (set<string>::iterator it2=second_parameters.begin(); it2!=second_parameters.end(); it2++)
			{
				for (set<string>::iterator it1=first_parameters.begin(); it1!=first_parameters.end(); it1++)
				{
					for (int i = 1; i < rawdata.size(); i++)
	 				{
						if (rawdata[i][k] == *it1 && rawdata[i][j] == *it2)
						{
							count++;
						}
					}
					observe.push_back(count);

					//cout << "count value is " << count << endl;
					count = 0;
				}
			}

			// for (vector<double>::iterator ob = observe.begin(); ob != observe.end(); ob++)
			// {
			// 	cout << "The observe is " << *ob << endl;
			// }

			for (int j = 0; j < first_parameters.size(); ++j)
			{
				for(int i = j; i<observe.size(); i=i+first_parameters.size())
			 	{
			 		total_col = total_col + observe.at(i);
			 	}
			 	ob_col.push_back(total_col);
			 	total_col=0;
		 	}

		 	for (int j = 0; j <observe.size(); j=j+first_parameters.size())
		 	{		 	
			 	for(int i = j; i<first_parameters.size()+j; ++i)
			 	{
			 		total_row = total_row + observe.at(i);
			 	}
			 	ob_row.push_back(total_row);
			 	total_row=0;
		 	}

		 // 	for (vector<double>::iterator ob = ob_col.begin(); ob != ob_col.end(); ob++)
			// {
			// 	cout << "The col is " << *ob << endl;
			// }	
		 	
		 // 	for (vector<double>::iterator ob = ob_row.begin(); ob != ob_row.end(); ob++)
			// {
			// 	cout << "The row is " << *ob << endl;
			// }

		 	for (vector<double>::iterator row=ob_row.begin(); row!=ob_row.end(); row++)
			{
				for (vector<double>::iterator col=ob_col.begin(); col!=ob_col.end(); col++)
				{
					value = (*row) * (*col) / (rows-1);
					expect.push_back(value);
				}
			}

			// for (vector<double>::iterator ob = expect.begin(); ob != expect.end(); ob++)
			// {
			// 	cout << "The expected data is " << *ob << endl;
			// }

			vector<double>::iterator ex=expect.begin();
			for (vector<double>::iterator ob=observe.begin(); ob!=observe.end(); ob++)
			{
				if(*ex != 0 ){
					chi_value.push_back((*ob-*ex) * (*ob-*ex) / (*ex));	
				}else{
					chi_value.push_back(0);
				}
				*ex++;
			}

			// for (vector<double>::iterator chi = chi_value.begin(); chi != chi_value.end(); chi++)
			// {
			// 	cout << "The chi square value is " << *chi << endl;
			// }

			for (vector<double>::iterator chi = chi_value.begin(); chi != chi_value.end(); chi++)
			{
				chi_square = chi_square + (*chi);
			}

		 	//cout << "the total chi_square value is " << chi_square << endl;


		 	//////////////////////////////////////////////
		 	//calculate the value in reference table
		 	int x = 0;
			int y = 0;
			while(chi_table_data[0][y] != pvalue)
			{
				y++;
			}
			x = (first_parameters.size()-1)*(second_parameters.size()-1);
		

			// cout << "x valeu is " << x << endl;
			// cout << "y valeu is " << y << endl;
			//cout << "value in table is " << chi_table_data[x][y] << endl;

			if(chi_square < stod(chi_table_data[x][y]))	
			{
				feature_delete ++;
				// cout << "They have strong relationship, delete the feature"<< endl;
				// cout << "***********************************************" << endl << endl;
				logfile.open("log.txt", iostream::app);
				logfile << "Compared with " << rawdata[0][k]  <<" and " << rawdata[0][j] << endl;
				logfile << "Delete " << rawdata[0][j] << endl;
				logfile.close();

				//Delete the column
				for (int i = 0; i < rawdata.size(); ++i)
				{
					if(rawdata[i].size() > j)
					{
						rawdata[i].erase(rawdata[i].begin() + j);
					}
				}
				j--;
			}
			else{
				//	cout << "Then have weak relationship, keep the feature" << endl<< endl;
			}

		 	chi_square = 0;
		 	ob_row.clear();
		 	ob_col.clear();
			observe.clear();
			expect.clear();
			chi_value.clear();
			first_parameters.clear();
			second_parameters.clear();
		}
	}

	vector<string>::iterator dl = decision.begin();
	for (int i = 0; i < rawdata.size(); ++i)
	{
		rawdata[i].push_back(*dl);
		dl++;
	}

	ofstream ofile(ofname + to_string(version) + ".csv");

	for (int i = 0; i < rawdata.size(); ++i)
	{
		for (vector<string>::iterator it = rawdata[i].begin(); it!=rawdata[i].end(); it++)
		{
			if (it==rawdata[i].end()-1)
			{
				ofile << *it;
			}else{
				ofile << *it << ",";	
			}
		}
		ofile << "\n";
	}
	ofile.close();

	cout << "   Featues Reduce:                   " << feature_delete << endl; 
	cout << "          Objects:                   " << (rows - 1) << endl;
	cout << "Writing Dataset.............................Done" << endl;
	return 0;
}