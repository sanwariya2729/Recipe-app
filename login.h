#include <iostream>
#include <string>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <memory>
using namespace std;
using namespace sql;


bool authenticate_user(shared_ptr<Connection> conn,int& user_id){
	string username;
	string passwd;
	
	cout<<"Enter username:";
	cin>>username;
	cout<<"Enter password:";
	cin>>passwd;
	
	try{
		unique_ptr<PreparedStatement> pst(conn->prepareStatement(" select * from users where user_name= ? and password=?"));
		pst->setString(1,username);
		pst->setString(2, passwd);
		
		unique_ptr<ResultSet> res(pst->executeQuery());
		if(res->next()){
			cout<<"login successful. welcome "<<res->getString("user_name")<<"!!\n";
			user_id=res->getInt("user_id");
			return true;
		}
		else{
			cout<<"invalid credentials!!\n check your username and password or create another user"<<endl;
			return false;
		}
		}
		catch(SQLException& e){
			cerr<<"ERROR:"<<e.what()<<endl;
			return false;
		}
		
}


void register_user(shared_ptr<Connection> conn){
	string user_name;
	string passwd;
	string email;
	cout<<"Enter username(less than 25 characters):"; cin>>user_name;
	cout<<"Enter email(less than 50 characters):"; cin >> email;
	cout<<"Enter password:"; cin>> passwd;
	
	
	try{
		unique_ptr<PreparedStatement> pst(conn->prepareStatement("insert into users (user_name,email,password) values (?,?,?)"));
		pst->setString(1,user_name);
		pst->setString(2,email);
		pst->setString(3,passwd);
		
		pst->executeUpdate();
		cout<<"user created successfully!!"<<endl;
	
	}catch(SQLException& e){
		cerr<<"ERROR:"<<e.what()<<endl;
	}
}






























