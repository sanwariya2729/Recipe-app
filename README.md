# RECIPE DATABASE

## Description
	-It is a recipe manager where you can establish connection to a remote database.
	-It is used C++ to connect to the the database and has a basic UI which was implemented using wxWidegets library
	-Using this we can fetch information through various methods such as name of the recipe, favourites , reviews etc 
	
	how it works
		-You first log in as a valid user or if your not a user then you can create a user .
		-after you login, you can check out recipes using a search bar and perform search according to various parameters.
		-if you have a new reipe, you can add it to the database or you give give review to any existing recipes
		-you can also mark them as favourite and can review them later.

## To compile, run:-
	-make clean && make

## To run:-
	./recipeapp
	
## Technologies

- **C++** 
- **wxWidgets** - To build the GUI
- **MySQL Connector/C++** -to interact with mysql using c++

Install the required packages:-


```bash
sudo apt install libwxgtk3.2-dev 
sudo apt install libmysqlcppconn-dev
sudo apt install mysql-server


	
