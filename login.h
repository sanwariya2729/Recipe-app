#include <iostream>
#include <string>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <memory>
using namespace std;
using namespace sql;


bool validateUser(shared_ptr<Connection> conn,string username, string passwd){
	try{
		unique_ptr<PreparedStatement> pst(conn->prepareStatement(" select * from users where username= ? and password=?"));
		pst->setString(1,username);
		pst->setString(2, passwd);
		
		unique_ptr<ResultSet> res(pst->executeQuery());
		if(res->next()){
			cout<<"login successful. welcome "<<res->getString("username")<<"!!\n";
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


bool createUser(shared_ptr<Connection> conn,string username,string passwd,string email){
	try{
		unique_ptr<PreparedStatement> pst(conn->prepareStatement("insert into users (username,email,password) values (?,?,?)"));
		pst->setString(1,username);
		pst->setString(2,email);
		pst->setString(3,passwd);
		
		pst->executeUpdate();
		cout<<"user created successfully!!"<<endl;
		return true;
	}catch(SQLException& e){
		cerr<<"ERROR:"<<e.what()<<endl;
		exit(1);
	}
}






























